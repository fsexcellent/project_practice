//#include <sys/ipc.h>
//#include <sys/shm.h>
//#include <sys/sem.h>

#include "uart_snd_queue.h"
#include "sqlite_link_list.h"
#include "data_global.h"

/* pthread_cond_t cond_infrared;
pthread_cond_t cond_buzzer; */
pthread_cond_t cond_led;
pthread_cond_t cond_camera;
pthread_cond_t cond_uart_snd;
pthread_cond_t cond_sms;

pthread_cond_t cond_sqlite;
pthread_cond_t cond_analysis;
pthread_cond_t cond_refresh;


//pthread_mutex_t mutex_analysis_chaek_env;

pthread_mutex_t mutex_slinklist;
pthread_mutex_t mutex_sqlite;
pthread_mutex_t mutex_analysis;
pthread_mutex_t mutex_refresh;
pthread_mutex_t mutex_global;
pthread_mutex_t mutex_linklist;

pthread_mutex_t mutex_uart_snd;
pthread_mutex_t mutex_chg_rcvnum;
pthread_mutex_t mutex_chg_cntnum;
pthread_mutex_t mutex_led;
pthread_mutex_t mutex_camera;
pthread_mutex_t mutex_sms;


int storageNum;
int goodsKinds;
int dev_infrared_fd;
int dev_buzzer_fd;
int dev_sms_fd;
int shmid;
int semid;
/************************/

int dev_led_fd;
int dev_camera_fd;
int dev_uart_fd;
int dev_sms_fd;

int msgqid;
uart_snd_q* uart_snd_qhead;

unsigned char dev_led_cmd;		//LED命令
unsigned char dev_camera_cmd;	//摄像头命令
unsigned char dev_uart_snd_cmd; //串口发送命令
unsigned char dev_sms_cmd;    //短信命令

char receive_phone[12] = "18688370343";
char center_phone[12] = "13800100500";



void sendMsgQueue (long type, unsigned char text)
{
	struct msg msgbuf; 
	msgbuf.type = MSGTYPE;
	//msgbuf.sto_no = sto_no;
	msgbuf.mtype = type;
	msgbuf.text[0] = text;
	printf("analysis: %ld, %x\n", type, text);
	msgsnd (msgqid, &msgbuf, MSGSIZE, 0);
}
