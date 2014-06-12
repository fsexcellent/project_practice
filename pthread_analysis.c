
#include "data_global.h"
#include "link_list.h"
#include "sqlite_link_list.h"

#define THRESHOD  15//三轴临界值



struct getEnvMsg
{
	unsigned char sto_no;
	unsigned char tem[2];
	unsigned char hum[2];
	unsigned char x;
	unsigned char y;
	unsigned char z;
	unsigned int ill;
	unsigned int battery;
	unsigned int adc;
};

extern linklist linkHead;
extern linklist linkTail;
extern 	void sendMsgQueue (long type, unsigned char text);

char tri_alarm_status[STORAGE_NUM] = {0};
char tem_alarm_status[STORAGE_NUM] = {0};
char hum_alarm_status[STORAGE_NUM] = {0};
char ill_alarm_status[STORAGE_NUM] = {0};
char beep_status[STORAGE_NUM] = {0};

float dota_atof (char unitl)
{
	if (unitl > 100)
	{
		return unitl / 1000.0;
	}
	else if (unitl > 10)
	{
		return unitl / 100.0;
	}
	else
	{
		return unitl / 10.0;
	}
}
int dota_atoi (const char * cDecade)
{
	int result = 0;
	if (' ' != cDecade[0])
	{
		result = (cDecade[0] - 48) * 10;
	}
	result += cDecade[1] - 48;
	return result;
}

float dota_adc (unsigned int ratio)
{
	return ((ratio * 3.6) / 1024);
}


/* 
** 温度,湿度,光照状态是否都正常
*/
int storageAllgood (int sto_no)
{
	if ((tem_alarm_status[sto_no] + hum_alarm_status[sto_no] + ill_alarm_status[sto_no] + tri_alarm_status[sto_no]) == 0)
	{
		return 1;
	}
	return 0;
}



/* 得到货物的包 */
void getGoodsPackage (link_datatype *buf)
{
	struct getGoodsMsg pack;
	memcpy (&pack, buf->text, 16);
	int sto_no = pack.sto_no;

	pthread_mutex_lock (&mutex_global);
	struct storage_info current = all_info_RT.storage_no[sto_no-1];
	pthread_mutex_unlock (&mutex_global);

	current.storage_status = 1;
	current.goods_info[pack.goodsno].goods_type = pack.goodsno;
	if (pack.io == 'I')
	{
		current.goods_info[pack.goodsno].goods_count += pack.goodsnum;
	}else{
		current.goods_info[pack.goodsno].goods_count -= pack.goodsnum;
	}

	printf ("sto_no = %d, io = %c goods_type = %d, goods_num = %d\n", sto_no, pack.io, current.goods_info[pack.goodsno].goods_type, current.goods_info[pack.goodsno].goods_count);	

	pthread_mutex_lock (&mutex_global);
	all_info_RT.storage_no[sto_no-1] = current;
	pthread_mutex_lock (&mutex_slinklist);
	if (pack.io == 'I')
	{
		sqlite_InsertLinknode (GOODS_ADD, all_info_RT, sto_no-1, pack.goodsno);//0,0分别是仓库号和货物种类号
	}
	else if (pack.io == 'O')
	{
		sqlite_InsertLinknode (GOODS_REDUCE, all_info_RT, sto_no-1, pack.goodsno);//0,0分别是仓库号和货物种类号
	}
	pthread_mutex_unlock (&mutex_slinklist);
	pthread_mutex_unlock (&mutex_global);

	pthread_cond_signal (&cond_refresh);
	pthread_cond_signal (&cond_sqlite);
	return ;
}


/* 监测数据是否异常 */
int checkEnv (int sto_no, struct storage_info *p)
{
	char flag = 0;
	static char a8_beep_status = 0;

	printf("sto_no: %d\n", sto_no);
	if (0 == tem_alarm_status[sto_no-1])//温度警报标志正常,看看是否有不正常的温度
	{
		if (p->temperature > p->temperatureMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_ON3 | sto_no << 6);
			p->fan_status = 3;
			tem_alarm_status[sto_no-1] = 2;
			flag = 1;
		}
		else if (p->temperature < p->temperatureMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			tem_alarm_status[sto_no-1] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			if (sto_no == 1)
				dev_sms_cmd = SMS_TEM |SMS_STRNO1;
			else
				dev_sms_cmd = SMS_TEM |SMS_STRNO2;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no-1] == 0)
			{
				beep_status[sto_no-1] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}
		}
	}
	else //温度警报标志位不正常,那么看看当前的温度是否正常.如果正常则恢复警报标志位
	{
		if (p->temperature < p->temperatureMAX && p->temperature > p->temperatureMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			tem_alarm_status[sto_no-1] = 0;
			if (storageAllgood (sto_no-1))
			{
				beep_status[sto_no-1] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);
			}
		}
	}

	if (0 == hum_alarm_status[sto_no-1])//湿度,同上
	{
		if (p->humidity > p->humidityMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_ON3 | sto_no << 6);
			p->fan_status = 3;
			hum_alarm_status[sto_no-1] = 2;
			flag = 1;
		}
		else if (p->humidity < p->humidityMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			hum_alarm_status[sto_no-1] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			if(sto_no == 1)
				dev_sms_cmd = SMS_HUM|SMS_STRNO1;
			else
				dev_sms_cmd = SMS_HUM|SMS_STRNO2;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no-1] == 0)
			{
				beep_status[sto_no-1] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}

		}
	}
	else//湿度,同上
	{
		if (p->humidity < p->humidityMAX && p->humidity > p->humidityMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
			p->fan_status = 0;
			hum_alarm_status[sto_no-1] = 0;
			if (storageAllgood (sto_no-1))
			{
				beep_status[sto_no-1] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);

			}
		}
	}

	if (0 == ill_alarm_status[sto_no-1])//光照标志 ,同上
	{
		if (p->illumination > p->illuminationMAX)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_LED_OFF | sto_no << 6);
			p->led_status = 0;
			ill_alarm_status[sto_no-1] = 2;
			flag = 1;
		}
		else if (p->illumination < p->illuminationMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_M0, MSG_M0_LED_ON | sto_no << 6);
			p->led_status = 1;
			ill_alarm_status[sto_no-1] = 1;
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			if(sto_no == 1)
				dev_sms_cmd = SMS_ILL|SMS_STRNO1;
			else
				dev_sms_cmd = SMS_ILL|SMS_STRNO2;
		
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no-1] == 0)
			{
				beep_status[sto_no-1] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}
		}
	}
	else if (ill_alarm_status[sto_no-1])//光照,同上
	{
		if (p->illumination < p->illuminationMAX && p->illumination > p->illuminationMIN)
		{
			sendMsgQueue (MSG_LED, MSG_LED_OFF);
			sendMsgQueue (MSG_M0, MSG_M0_LED_OFF | sto_no << 6);
			p->led_status = 0;
			ill_alarm_status[sto_no-1] = 0;
			if (storageAllgood (sto_no-1))
			{
				beep_status[sto_no-1] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);
			}
		}
	}

	if (0 == tri_alarm_status[sto_no-1]) // 判断是否有人闯入
	{
		if (p->x > THRESHOD || p->y > THRESHOD || p->z >THRESHOD)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			sendMsgQueue (MSG_CAMERA, 1);
			//sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
		
			flag = 1;
		}
		else if (p->x < -THRESHOD || p->y < -THRESHOD || p->z < -THRESHOD)
		{
			sendMsgQueue (MSG_LED, MSG_LED_ON);
			//sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			flag = 1;
		}

		if (flag)
		{
			pthread_mutex_lock (&mutex_sms);
			if (sto_no == 1)
				dev_sms_cmd = SMS_BRE |SMS_STRNO1;
			else
				dev_sms_cmd = SMS_BRE |SMS_STRNO2;
			pthread_mutex_unlock (&mutex_sms);
			pthread_cond_signal (&cond_sms);
			flag = 0;
			if (beep_status[sto_no-1] == 0)
			{
				beep_status[sto_no-1] = 1;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_ON | sto_no << 6);
			}
			if (a8_beep_status == 0)
			{
				a8_beep_status = 1;
				sendMsgQueue (MSG_BEEP, MSG_M0_BEEP_ON);	
			}
		}
	}
	else //三轴警报标志位不正常,那么看看当前三轴是否正常.如果正常则恢复警报标志位
	{
		if (((p->x < THRESHOD) && (p->x > -THRESHOD)) && ((p->y < THRESHOD) && 
					(p->y > -THRESHOD)) && ((p->z < THRESHOD) && (p->z > -THRESHOD)))
		{
			sendMsgQueue (MSG_LED, MSG_LED_OFF);
			//sendMsgQueue (MSG_M0, MSG_M0_FAN_OFF | sto_no << 6);
		
			tri_alarm_status[sto_no-1] = 0;
			if (storageAllgood (sto_no-1))
			{
				beep_status[sto_no-1] = 0;
				sendMsgQueue (MSG_M0, MSG_M0_BEEP_OFF | sto_no << 6);
				a8_beep_status = 0;
				sendMsgQueue (MSG_BEEP, MSG_BEEP_OFF);
			}
		}
	}


	return 0;
}

/* 取得env包 */
void getEnvPackage (link_datatype *buf)
{
	struct getEnvMsg pack;
	memcpy (&pack, buf->text, 20);
	int sto_no = pack.sto_no;

	pthread_mutex_lock (&mutex_global);
	struct storage_info current = all_info_RT.storage_no[sto_no-1];
	pthread_mutex_unlock (&mutex_global);

	current.storage_status = 1;
	current.x = pack.x;
	current.y = pack.y;
	current.z = pack.z;
	current.temperature = pack.tem[0] + dota_atof (pack.tem[1]);
	current.humidity = pack.hum[0] + dota_atof (pack.hum[1]);
	current.illumination = pack.ill;
	current.battery = dota_adc (pack.battery);
	current.adc = dota_adc (pack.adc);

	printf ("no = %d tem = %0.2f hum = %0.2f ill = %0.2f battery = %0.2f adc = %0.2f x = %d y = %d z = %d\n", sto_no,
			current.temperature, current.humidity, current.illumination, current.battery, current.adc, current.x, current.y, current.z);

	checkEnv (sto_no, &current);	

	pthread_mutex_lock (&mutex_global);
	all_info_RT.storage_no[sto_no-1] = current;
	pthread_mutex_lock (&mutex_slinklist);
	sqlite_InsertLinknode (COLLECT_INSERTER, all_info_RT, sto_no-1, 0);//0,0分别是仓库号和货物种类号
	pthread_mutex_unlock (&mutex_slinklist);
	pthread_mutex_unlock (&mutex_global);

	pthread_cond_signal (&cond_refresh);
	pthread_cond_signal (&cond_sqlite);


	return ;
}

void *pthread_analysis (void *arg)
{
	linklist node;
	link_datatype buf;
	printf ("pthread_analysis is ok\n");
	while (1)
	{
		pthread_mutex_lock (&mutex_analysis);
		pthread_cond_wait (&cond_analysis, &mutex_analysis);
		pthread_mutex_unlock (&mutex_analysis);

//		printf ("wake pthread_analysis wake up\n");
		while (1)
		{
			pthread_mutex_lock (&mutex_linklist);
			if ((node = GetLinknode (linkHead)) == NULL)
			{
				pthread_mutex_unlock (&mutex_linklist);
				break;
			}
			buf = node->data;
			free (node);
			pthread_mutex_unlock (&mutex_linklist);

			if ('e' == buf.msg_type)
			{
				getEnvPackage (&buf);
			}
			else if ('r' == buf.msg_type)
			{
				getGoodsPackage (&buf);
			}
		}
	}
	return 0;
}
