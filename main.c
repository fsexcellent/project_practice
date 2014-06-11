#include <stdio.h>
#include <pthread.h>
#include <signal.h>
#include <sys/ipc.h>
#include <sys/msg.h>
#include <sys/sem.h>
#include <stdlib.h>
#include <pthread.h>

#include "data_global.h"
extern void *pthread_sqlite (void *);			//数据库线程
extern void *pthread_analysis (void *);			//数据解析线程
extern void *pthread_transfer (void *);	

extern void* pthread_led(void*);
extern void* pthread_msgq(void*);
extern void* pthread_camera(void*);
extern void* pthread_uart_snd(void*);
extern void* pthread_sms(void*);

extern void *pthread_refresh (void *);	//共享内存数据刷新线程


extern void create_table ();

pthread_t   id_led,
			id_msgq,
			id_camera,
			id_uart_snd,
			id_sms,
			
			id_sqlite,
			id_analysis,
			id_transfer,
			id_refresh;

void release_resource(int signo)
{


	pthread_mutex_destroy (&mutex_sms);
	pthread_mutex_destroy (&mutex_camera);
	pthread_mutex_destroy (&mutex_led);
	pthread_mutex_destroy (&mutex_chg_rcvnum);
	pthread_mutex_destroy (&mutex_chg_cntnum);
	pthread_mutex_destroy (&mutex_uart_snd);
	
	
	
	pthread_mutex_destroy (&mutex_analysis);
	pthread_mutex_destroy (&mutex_sqlite);
	pthread_mutex_destroy (&mutex_slinklist);
	pthread_mutex_destroy (&mutex_linklist);
	pthread_mutex_destroy (&mutex_global);
	pthread_mutex_destroy (&mutex_refresh);

	pthread_cond_destroy (&cond_sms);
	pthread_cond_destroy (&cond_camera);
	pthread_cond_destroy (&cond_led);
	pthread_cond_destroy (&cond_uart_snd);
	
	pthread_cond_destroy (&cond_analysis);
	pthread_cond_destroy (&cond_sqlite);
	pthread_cond_destroy (&cond_refresh);
//	pthread_mutex_destory(&mutex_led);

//	pthread_cond_destory(&cond_led);

	pthread_cancel(id_led);
	pthread_cancel(id_msgq);
	pthread_cancel(id_camera);
	pthread_cancel(id_uart_snd);
	pthread_cancel(id_sms);
	
	pthread_cancel (id_transfer);
	pthread_cancel (id_analysis);
	pthread_cancel (id_sqlite);
	pthread_cancel (id_refresh);

	msgctl(msgqid, IPC_RMID, NULL);
	shmctl (shmid, IPC_RMID, NULL);

	exit(0);
}

void setLimit (int sto_no, float temMAX, float temMIN, float humMAX, float humMIN, float illMAX, float illMIN)
{
	if (sto_no >= 0 && (sto_no <=STORAGE_NUM - 1))
	{
		all_info_RT.storage_no[sto_no].temperatureMAX = temMAX;
		all_info_RT.storage_no[sto_no].temperatureMIN = temMIN;
		all_info_RT.storage_no[sto_no].humidityMAX = humMAX;
		all_info_RT.storage_no[sto_no].humidityMIN = humMIN;
		all_info_RT.storage_no[sto_no].illuminationMAX = illMAX;
		all_info_RT.storage_no[sto_no].illuminationMIN = illMIN;
	}
}

int main(int argc, const char *argv[])
{
	create_table ();
	
	setLimit (0, 50, 5, 50, 10, 500, 10);
	setLimit (1, 50, 5, 50, 10, 500, 10);
	
	pthread_mutex_init(&mutex_led, NULL);
	pthread_mutex_init(&mutex_camera, NULL);
	pthread_mutex_init(&mutex_uart_snd, NULL);
	pthread_mutex_init(&mutex_chg_rcvnum, NULL);
	pthread_mutex_init(&mutex_chg_cntnum, NULL);
	pthread_mutex_init(&mutex_sms, NULL);
	
	pthread_mutex_init (&mutex_slinklist, NULL);
	pthread_mutex_init (&mutex_sqlite, NULL);
	pthread_mutex_init (&mutex_analysis, NULL);
	pthread_mutex_init (&mutex_global, NULL);
	pthread_mutex_init (&mutex_linklist, NULL);

	pthread_cond_init(&cond_led, NULL);
	pthread_cond_init(&cond_camera, NULL);
	pthread_cond_init(&cond_uart_snd, NULL);
	pthread_cond_init(&cond_sms, NULL);
	
	pthread_cond_init (&cond_sqlite, NULL);
	pthread_cond_init (&cond_analysis, NULL);
	pthread_cond_init (&cond_refresh, NULL);

	signal(SIGINT, release_resource);

	pthread_create (&id_sqlite, 0, pthread_sqlite, NULL);
	sleep (1);
	pthread_create (&id_analysis, 0, pthread_analysis, NULL);
	pthread_create(&id_msgq, NULL, pthread_msgq, NULL);
	sleep(1);
	pthread_create (&id_transfer, 0, pthread_transfer, NULL);
	sleep (1);
	
	pthread_create(&id_led, NULL, pthread_led, NULL);
	pthread_create(&id_camera, NULL, pthread_camera, NULL);
	pthread_create(&id_uart_snd, NULL, pthread_uart_snd, NULL);
	pthread_create(&id_sms, NULL, pthread_sms, NULL);

	pthread_create (&id_refresh, 0, pthread_refresh, NULL);
	
	
	pthread_join (id_sqlite, NULL);
	printf ("g1\n");
	pthread_join (id_analysis, NULL);
	printf ("g2\n");
	pthread_join (id_transfer, NULL);
	
	pthread_join(id_led, NULL);
	pthread_join(id_msgq, NULL);
	pthread_join(id_camera, NULL);
	pthread_join(id_uart_snd, NULL);
	pthread_join(id_sms, NULL);
	
	pthread_join (id_refresh, NULL);
	printf ("g11\n");

	
	

	return 0;
}
