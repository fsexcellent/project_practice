#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <strings.h>
#include <pthread.h>

#include <sys/types.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>

#include "uart_snd_queue.h"
#include "data_global.h"

extern uart_snd_q* uart_snd_qhead;
extern int sqlite_InsertLinknode (int operation, struct env_info_clien_addr y, 
								   int storageNum_l, int goodsKinds_l);
struct setEnv
{
	int temMAX;
	int temMIN;
	int humMAX;
	int humMIN;
	int illMAX;
	int illMIN;
};

void* pthread_msgq(void* arg)
{
	key_t key;
	msg_s msgbuf;
	struct setEnv new;
	int sto_no;

	char tmp[64]= {0};
	char ip[32] = {0};
	char gw[32] = {0};

	if((key = ftok("/dev", 'g')) < 0)
	{
		perror("msgq_ftok");
		return 1;
	}

	if((msgqid = msgget(key, IPC_CREAT | IPC_EXCL | 0666)) < 0)
	{
		if(errno == EEXIST)
		{
			msgqid = msgget(key,0666);
		}
		else
		{
		perror("msgget");
		return 1;
		}
	}

	printf("message queue OK!\n");

	uart_snd_qhead = create_uart_snd_queue();
	
	while(1)
	{
		bzero(&msgbuf, sizeof(msgbuf));
		if(msgrcv(msgqid, &msgbuf, MSGSIZE, MSGTYPE, 0) <= 0)
		{
			perror("msgrcv");
			return 0;
		}
		switch(msgbuf.mtype)
		{
		case MSG_LED:
			pthread_mutex_lock(&mutex_led);
			dev_led_cmd = msgbuf.text[0];
			printf("led msg: %d\n", dev_led_cmd);
			pthread_cond_signal(&cond_led);
			pthread_mutex_unlock(&mutex_led);
			break;
		case MSG_CAMERA:
			pthread_mutex_lock(&mutex_camera);
			dev_camera_cmd = msgbuf.text[0];
			pthread_cond_signal(&cond_camera);
			pthread_mutex_unlock(&mutex_camera);
			break;

		case MSG_UART_SND://给M0发送的数据
			printf("send message: %x\n", msgbuf.text[0]);
			pthread_mutex_lock(&mutex_uart_snd);
			enter_uart_snd_queue(uart_snd_qhead, msgbuf.text[0]);
			pthread_cond_signal(&cond_uart_snd);
			pthread_mutex_unlock(&mutex_uart_snd);
			break;
		case MSG_SETENV:
			memcpy (&new, msgbuf.text + 1, 24);
			sto_no = msgbuf.text[0] - '0' -1;
			printf ("sto_no = %d temMAX = %d, temMIN = %d, humMAX = %d, hunMIN = %d, illMAX = %d, illMIN = %d\n",
					sto_no, new.temMAX, new.temMIN, new.humMAX, new.humMIN, new.illMAX, new.illMIN);
			pthread_mutex_lock (&mutex_global);
			if (new.temMAX > 0 && new.temMAX > all_info_RT.storage_no[sto_no].temperatureMIN)
			{
				all_info_RT.storage_no[sto_no].temperatureMAX = new.temMAX;
			}
			if (new.temMIN > 0 && new.temMIN < all_info_RT.storage_no[sto_no].temperatureMAX)
			{
				all_info_RT.storage_no[sto_no].temperatureMIN = new.temMIN;
			}
			if (new.humMAX > 0 && new.humMAX > all_info_RT.storage_no[sto_no].humidityMIN)
			{
				all_info_RT.storage_no[sto_no].humidityMAX = new.humMAX;
			}
			if (new.humMIN > 0 && new.humMIN < all_info_RT.storage_no[sto_no].temperatureMAX)
			{
				all_info_RT.storage_no[sto_no].humidityMIN = new.humMIN;
			}
			if (new.illMAX > 0 && new.illMAX > all_info_RT.storage_no[sto_no].illuminationMIN)
			{
				all_info_RT.storage_no[sto_no].illuminationMAX = new.illMAX;
			}
			if (new.illMIN > 0 && new.illMIN < all_info_RT.storage_no[sto_no].illuminationMAX)
			{
				all_info_RT.storage_no[sto_no].illuminationMIN = new.illMIN;
			}
			pthread_mutex_lock (&mutex_slinklist);
			sqlite_InsertLinknode (ENV_UPDATE, all_info_RT, sto_no, 0);//0,0鍒嗗埆鏄粨搴撳彿鍜岃揣鐗╃绫诲彿
			pthread_mutex_unlock (&mutex_slinklist);
			pthread_cond_signal (&cond_sqlite);
			pthread_mutex_unlock (&mutex_global);
			pthread_cond_signal (&cond_refresh);
			break;
		case MSG_CHGNUM:
			pthread_mutex_lock(&mutex_chg_rcvnum);
			strncpy(receive_phone, msgbuf.text, PHONE_NUM_SIZE);
			receive_phone[PHONE_NUM_SIZE] = '\0';
			printf("receive_phone: %s\n", receive_phone);
			pthread_mutex_unlock(&mutex_chg_rcvnum);
			if(msgbuf.text[PHONE_NUM_SIZE] != '\0')
			{
				pthread_mutex_lock(&mutex_chg_cntnum);
				strncpy(center_phone, msgbuf.text + PHONE_NUM_SIZE, PHONE_NUM_SIZE);
				center_phone[PHONE_NUM_SIZE] = '\0';
				printf("center_phone: %s\n", center_phone);
				pthread_mutex_unlock(&mutex_chg_cntnum);
			}
			break;
		case MSG_WIFI:

			strcpy(tmp,msgbuf.text);
			int i = 0, j= 0;
			for(i = 0 ; tmp[i] != 'f'; i++)
			{
				ip[i] = tmp[i];	
			}
			ip[i] = '\0';
			printf("ip : %s\n",ip);
			i++;
			for(j=0 ; tmp[i] != '\0'; i++, j++)
			{
				gw[j] = tmp[i];
			}
			gw[j] = '\0';
			printf("gw : %s\n",gw);
			system("mkdir /var/run/wpa_supplicant -p");
			char ifcon[32] = "ifconfig wlan0 ";
			strcat(ifcon,ip);
			printf("string : %s\n",ifcon);
			system(ifcon);
			char gateway[32] = "route add default gw ";
			strcat(gateway,gw);
			system(gateway);
			printf("gw : %s\n",gateway);
			system("wpa_supplicant -B -iwlan0 -c /etc/wpa-psk-tkip.conf");
			printf("wifi is ok\n");
			break;
		}
	}
}
