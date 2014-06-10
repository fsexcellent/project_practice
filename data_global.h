#ifndef __DATA_GLOBAL_HHHHHHHHHHHHHH__
#define __DATA_GLOBAL_HHHHHHHHHHHHHH__

#include <stdio.h>
#include <errno.h>
#include <fcntl.h>
#include <unistd.h>
#include <stdlib.h>
#include <string.h>
#include <signal.h>
#include <pthread.h>
#include <termios.h>
#include <syscall.h>
#include <sys/ipc.h>
#include <sys/shm.h>
#include <sys/sem.h>

#include <sys/msg.h>

#include <sys/stat.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <linux/fs.h>
#include <linux/ioctl.h>

#define DEV_LED			"/dev/new_led_device"
#define DEV_CAMERA		"/tmp/webcom"
//
#define DEV_GPRS		"/dev/ttyUSB0"
#define DEV_ZIGBEE		"/dev/ttyUSB1"

//消息队列类型
#define MSGTYPE				1L
//消息类型
#define MSG_LED				1L
#define MSG_CAMERA			2L
#define MSG_UART_SND		3L
#define MSG_SETENV			4L
#define MSG_CHGNUM			5L
#define MSG_WIFI			6L

#define MSG_M0				3L
#define MSG_BEEP			8L


#define MSG_BEEP_OFF		0
#define MSG_BEEP_ON			1

#define MSG_LED_ON			0
#define MSG_LED_OFF			1

#define MSG_SIZE				32
#define MSGSIZE			sizeof(msg_s)-sizeof(long)






#define MSG_CONNECT_SUCCESS 'Y'
//往M0发送的消息类型
#define MSG_M0_FAN_OFF			0x00
#define MSG_M0_FAN_ON1		0x01
#define MSG_M0_FAN_ON2		0x02
#define MSG_M0_FAN_ON3		0x03

#define MSG_M0_BEEP_OFF		0x10
#define MSG_M0_BEEP_ON			0x11
#define MSG_M0_BEEP_AU_OFF		0x12
#define MSG_M0_BEEP_AU_ON		0x13

#define MSG_M0_LED_OFF			0x20
#define MSG_M0_LED_ON			0x21

#define MSG_M0_SEG_ON			0x30
#define MSG_M0_SEG_OFF			0x3f



//数据库相关

#define SMS_TEM			1
#define SMS_HUM		2	
#define SMS_ILL			3	
#define SMS_BRE			4
#define SMS_STRNO1		0
#define SMS_STRNO2		1<<7

#define SMS_MSG_TEM1	0
#define SMS_MSG_BRE1	1
#define SMS_MSG_ILL1	2
#define SMS_MSG_HUM1	3
#define SMS_MSG_TEM2	4
#define SMS_MSG_BRE2	5
#define SMS_MSG_ILL2	6
#define SMS_MSG_HUM2	7




#define PHONE_NUM_SIZE	11




#define SQLITE_OPEN			"./www/warehouse.db"
//#define DEV_ZIGBEE 			"/dev/usbdev1.5"//串口
#define STORAGE_NUM 		5
#define ENV_UPDATE			0x00
#define ENV_GET				0x01
#define COLLECT_INSERTER		0x10
#define COLLECT_TIME_GET	0x11
#define COLLECT_CURRENT_GET 0x12
#define GOODS_ADD			0x20
#define GOODS_REDUCE		0x21
#define GOODS_GET			0x22
#define GOODS_NUM			10
#define GOODS_IN 			'I'
#define GOODS_OUT			'O'



typedef struct msg{
	long type;
	long mtype;
	char text[MSG_SIZE];
}msg_s;


struct storage_goods_info
{
	unsigned char goods_type;
	unsigned int goods_count;
};

struct getGoodsMsg
{
	unsigned char sto_no;
	unsigned char io;
	unsigned char goodsno;
	unsigned char goodsnum;
};

struct storage_info
{
	unsigned char storage_status;				// 0:open 1:close
	unsigned char led_status;
	unsigned char buzzer_status;
	unsigned char fan_status;	//风扇的开关状态
	unsigned char seg_status; //数码管的开关状态	
	signed char x;
	signed char y;
	signed char z;
	char samplingTime[20]; //获取仓库环境信息的时间
	float temperature;
	float temperatureMIN;
	float temperatureMAX;
	float humidity;
	float humidityMIN;
	float humidityMAX;
	float illumination;
	float illuminationMIN;
	float illuminationMAX;
	float battery;
	float adc;
	float adcMIN;
	struct storage_goods_info goods_info[GOODS_NUM];
};

struct env_info_clien_addr
{
	struct storage_info storage_no[STORAGE_NUM];	
};

/************************/
struct env_info_clien_addr env_info_clien_addr_s;//缓存环境信息
struct env_info_clien_addr all_info_RT;//全局环境变量


struct storage_info storage_RT[STORAGE_NUM];

struct sqlite_operation
{
	int table_select_cmd;
	int env_operation_cmd;
	int collectEnv_operation_cmd;
	int goods_operation_cmd;
};


//pthread_mutex_t mutex_analysis_chaek_env;
extern pthread_mutex_t mutex_led;
extern pthread_mutex_t mutex_camera;
extern pthread_mutex_t mutex_uart_snd;
extern pthread_mutex_t mutex_chg_rcvnum;
extern pthread_mutex_t mutex_chg_cntnum;
extern pthread_mutex_t mutex_sms;


extern pthread_mutex_t mutex_refresh;
extern pthread_mutex_t mutex_global;
extern pthread_mutex_t mutex_linklist;
extern pthread_mutex_t mutex_slinklist;
extern pthread_mutex_t mutex_sqlite;
extern pthread_mutex_t mutex_analysis;


/************************/
extern pthread_cond_t cond_led;
extern pthread_cond_t cond_camera;
extern pthread_cond_t cond_uart_snd;
extern pthread_cond_t cond_sms;

extern pthread_cond_t cond_sqlite;
extern pthread_cond_t cond_analysis;
extern pthread_cond_t cond_refresh;


extern int dev_led_fd;
extern int dev_camera_fd;
extern int dev_uart_fd;
extern int dev_sms_fd;

/************************/
//extern int dev_infrared_fd;
//extern int dev_buzzer_fd;

extern int shmid;
extern int semid;
extern int storageNum;
extern int goodsKinds;
/************************/

extern int msgqid;

extern unsigned char dev_led_cmd;
extern unsigned char dev_camera_cmd;
extern unsigned char dev_uart_snd_cmd;
extern unsigned char dev_sms_cmd;

extern char receive_phone[12];
extern char center_phone[12];

//extern unsigned char dev_infrared_cmd;
//extern unsigned char dev_buzzer_cmd;
#endif
