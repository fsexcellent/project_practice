#include <sqlite3.h>
#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#include "data_global.h"
#include "sqlite_link_list.h"

#define TEMPERATURE_MAX   1//温度最值
#define TEMPERATURE_MIN   2
#define HUMIDITY_MAX      3//湿度最值
#define HUMIDITY_MIN      4
#define ILLUMINATION_MAX  5//光照最值
#define ILLUMINATION_MIN  6
#define TELEPHONE_NUM     7
/*
#define MODE              8
#define IFNOCICE          9
#define INTERVAL          10 
#define UPDATE_PERIOD     11
 */
#define USER_NAME         8
#define PASSWORD          9

float callback_val[6];//详见数据库env表项
char callback_str[20];

/*
**得到某一个环境(字段)的参数
*/

static int callback_getenv_one (void * para, int n_column, char ** column_value, char ** column_name )
{
	//int i;
#if DEBUG_SQLITE
	printf ("n_column = %d\n", n_column);
#endif
	
	callback_val[0] = (float)atof(column_value[0]);
#if DEBUG_SQLITE
			printf ("%s = %f\n", column_name[0], callback_val[0]);
#endif
		
	
#if DEBUG_SQLITE
	printf ("**********************\n");
#endif
	return 0;
}


/*
**回调函数，查询之后的信息都通过column_value[i] 和callback_str传出
*/
static int callback_getenv (void * para, int n_column, char ** column_value, char ** column_name )
{
	int i;
#if DEBUG_SQLITE
	printf ("n_column = %d\n", n_column);
#endif
	for (i=0; i<n_column; i++)
	{
		if (n_column-1 == i)
		{
			strcpy (callback_str, column_value[i]);
#if DEBUG_SQLITE
			printf ("%s = %s\n", column_name[i], callback_str);
#endif
		}
		else
		{
			callback_val[i] = (float)atof(column_value[i]);
#if DEBUG_SQLITE
			printf ("%s = %f\n", column_name[i], callback_val[i]);
#endif
		}
	}
#if DEBUG_SQLITE
	printf ("**********************\n");
#endif
	return 0;
}

static int callback_getenv_s (void * para, int n_column, char ** column_value, char ** column_name )
{
	strcpy (callback_str, column_value[0]);
#if DEBUG_SQLITE
	printf ("%s = %s\n", column_name[0], callback_str);
#endif
	return 0;
}
/*
**初始化环境表，
*/
int init_table_env (void)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	int i;

	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n",sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
		for (i=1; i<=2; i++)
		{
			sprintf (sql,"insert into env values('%d',25.4,10.5,55.8,10.8,\
				258,129,'12345678901',1,\
					2,3,4,'binea',123456);", i);
			recode = sqlite3_exec (db,sql,0,0,&err_msg);
			if (recode != SQLITE_OK)
			{
				printf ("Error:%s",err_msg);
				sqlite3_close (db);
				return 1;
			}
		}
		sqlite3_close (db);
	}
	return 0;
}


int create_table(void)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = NULL;
	int ret;

	ret = sqlite3_open(SQLITE_OPEN, &db);
	if(ret != SQLITE_OK)
	{
		printf("Can't open database:%s\n", SQLITE_OPEN);	
		sqlite3_close(db);
		return -1;
	}
	else
	{
		sprintf(sql, "create table if not exists env(dev_no int not null,\
									temperatureMax float not null,\
									temperatureMin float not null,\
									humidityMax float not null,\
									humidityMin float not null,\
									illuminationMax float not null,\
									illuminationMin float not null,\
									telephoneNum varchar(11) not null\
									mode int not null,\
									ifnocice int not null,\
									interval int not null,\
									updatePeriod int not null,\
									username varchar(20) not null,\
									password int not null);");	
		ret = sqlite3_exec(db, sql, 0, 0, &err_msg);
		if(ret != SQLITE_OK)
		{
			printf("Error:%s", err_msg);	
			sqlite3_close(db);
			return -1;
		}
		else
		{
			init_table_env();
		}
		
		sprintf(sql, "create table if not exists collect_env(dev_no int not null,\
			  							temperatureVal float not null,\
										humidityVal float not null,\
				   						illuminationVal float not null,\
				      					envtime varchar(10) not null);");
		ret = sqlite3_exec(db, sql, 0, 0, &err_msg);
		if(ret != SQLITE_OK)
		{
			printf("Error:%s", err_msg);	
			sqlite3_close(db);
			return -1;
		}
		
		sprintf(sql, "create table if not exists goods(dev_no int not null,\
										 goodsId int not null,\
										 goodsCount int not null,\
										 goodsTime varchar(10) not null);");
		ret = sqlite3_exec(db, sql, 0, 0, &err_msg);
		if(ret != SQLITE_OK)
		{
			printf("Error:%s", err_msg);	
			sqlite3_close(db);
			return -1;
		}
		
		sqlite3_close(db);
	}

	return 0;	
}	

int set_env(float val, int dev_no, int no)
{
	char sql[1024];
	switch (no)
	{
	case 1:
		sprintf(sql, "update env set temperatureMax = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	case 2:
		sprintf(sql, "update env set temperatureMin = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	case 3:
		sprintf(sql, "update env set humidityMax = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	case 4:
		sprintf(sql, "update env set humidityMin = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	case 5:
		sprintf(sql, "update env set illuminationMax = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	case 6:
		sprintf(sql, "update env set illuminationMin = '%f' where dev_no = '%d';", val, dev_no + 1);
		break;
	}

	sqlite3 *db;
	char *err_msg;
	int ret;
	ret = sqlite3_open(SQLITE_OPEN, &db);
	if(ret != SQLITE_OK)
	{
		printf("Can't open database: %s!\n", sqlite3_errmsg(db));	
		sqlite3_close(db);
		return -1;
	}
	ret = sqlite3_exec(db,sql,0,0,&err_msg);	
	if(ret != SQLITE_OK)
	{
		printf("Error:%s\n", err_msg);	
		sqlite3_close(db);
		return -1;
	}

	sqlite3_close(db);
	return 0;
}

int update_env(struct env_info_clien_addr env_info, int storageNum)	
{
	set_env(env_info.storage_no[storageNum].temperatureMAX,storageNum,TEMPERATURE_MAX);
	set_env(env_info.storage_no[storageNum].temperatureMIN,storageNum,TEMPERATURE_MIN);
	set_env(env_info.storage_no[storageNum].humidityMAX,storageNum,HUMIDITY_MAX);
	set_env(env_info.storage_no[storageNum].humidityMIN,storageNum,HUMIDITY_MIN);
	set_env(env_info.storage_no[storageNum].illuminationMAX,storageNum,ILLUMINATION_MAX);
	set_env(env_info.storage_no[storageNum].illuminationMIN,storageNum,ILLUMINATION_MIN);

	return 0;
}

int getenv_select(int storageNum, int no)
{
	char sql[1024];
	sqlite3 *db;
	char *err_msg = 0;
	int recode;

	switch (no)
	{
	case 1:
		sprintf (sql, "select temperatureMax from env where dev_no = '%d';", storageNum);
		break;
	case 2:
		sprintf (sql, "select temperatureMin from env where dev_no = '%d';", storageNum);
		break;
	case 3:
		sprintf (sql, "select humidityMax from env where dev_no = '%d';", storageNum);
		break;
	case 4:
		sprintf (sql, "select humidityMin from env where dev_no = '%d';", storageNum);
		break;
	case 5:
		sprintf (sql, "select illuminationMax from env where dev_no = '%d';", storageNum);
		break;
	case 6:
		sprintf (sql, "select illuminationMin from env where dev_no = '%d';", storageNum);
		break;
#if 0
	case 7:
		sprintf (sql, "select telephoneNum from env;");
		break;
	case 8:
		sprintf (sql, "select username from env;");
		break;
	case 9:
		sprintf (sql, "select password from env;");
		break;
	case 10:
		sprintf (sql, "select interval from env;");
		break;
	case 11:
		sprintf (sql, "select updatePeriod from env;");
		break;
	case 12:
		sprintf (sql, "select mode from env;");
		break;
	case 13:
		sprintf (sql, "select ifnocice from env;");
		break;
#endif
	}
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	if((7 == no) || (8 == no))
	{
		recode = sqlite3_exec(db, sql, callback_getenv_s, 0, &err_msg);
	}
	else
	{
		recode = sqlite3_exec(db, sql, callback_getenv_one, 0, &err_msg);
	}
	if (recode != SQLITE_OK)
	{
		printf ("Error:%s", err_msg);
		sqlite3_close(db);
		return 1;
	}
	sqlite3_close(db);
	return 0;
}

int get_env(struct env_info_clien_addr *env_info, int storageNum)	
{
	getenv_select(storageNum, TEMPERATURE_MAX);
	env_info->storage_no[storageNum].temperatureMAX = callback_val[0];
	all_info_RT.storage_no[storageNum].temperatureMAX = callback_val[0];	
	getenv_select(storageNum, TEMPERATURE_MIN);
	env_info->storage_no[storageNum].temperatureMIN = callback_val[0];
	all_info_RT.storage_no[storageNum].temperatureMIN = callback_val[0];
	getenv_select(storageNum, HUMIDITY_MAX);
	env_info->storage_no[storageNum].humidityMAX = callback_val[0];
	all_info_RT.storage_no[storageNum].humidityMAX = callback_val[0];
	getenv_select(storageNum, HUMIDITY_MIN);
	env_info->storage_no[storageNum].humidityMIN = callback_val[0];
	all_info_RT.storage_no[storageNum].humidityMIN = callback_val[0];
	getenv_select(storageNum, ILLUMINATION_MAX);
	env_info->storage_no[storageNum].illuminationMAX = callback_val[0];
	all_info_RT.storage_no[storageNum].illuminationMAX = callback_val[0];
	getenv_select(storageNum, ILLUMINATION_MIN);
	env_info->storage_no[storageNum].illuminationMIN = callback_val[0];
	all_info_RT.storage_no[storageNum].illuminationMIN = callback_val[0];

	return 0;
}

int insert_collect_env(struct env_info_clien_addr env_info, int storage_num)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	char sql[1024];
	sprintf(sql, "insert into collect_env values(%d,%f,%f,%f,'%s');",
			storage_num,
			env_info.storage_no[storage_num].temperature,
			env_info.storage_no[storage_num].humidity,
			env_info.storage_no[storage_num].illumination,
			env_info.storage_no[storage_num].samplingTime);
	recode = sqlite3_open(SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
	if (recode != SQLITE_OK)
	{
		printf ("Error:%s", err_msg);
		sqlite3_close (db);
		return 1;
	}
	sqlite3_close (db);
	return 0;
}

int	get_collect_env(char itime_t[], int storage_num)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from collect_env where envtime = '%s' and dev_no = %d;", itime_t, storage_num);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
	if (recode != SQLITE_OK)
	{
		printf ("Error:%s", err_msg);
		sqlite3_close (db);
		return 1;
	}
	sqlite3_close (db);

	env_info_clien_addr_s.storage_no[storage_num].temperature = callback_val[1];
	env_info_clien_addr_s.storage_no[storage_num].humidity = callback_val[2];
	env_info_clien_addr_s.storage_no[storage_num].illumination = callback_val[3];
	strcpy(env_info_clien_addr_s.storage_no[storage_num].samplingTime, callback_str);

	all_info_RT.storage_no[storage_num].temperature = callback_val[1];
	all_info_RT.storage_no[storage_num].humidity = callback_val[2];
	all_info_RT.storage_no[storage_num].illumination = callback_val[3];
	strcpy(all_info_RT.storage_no[storage_num].samplingTime, callback_str);
#if 1
	printf ("dev_no:%d  temperature = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].temperature);
	printf ("dev_no:%d  humidity = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].humidity);
	printf ("dev_no:%d  illumination = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].illumination);
	printf ("dev_no:%d  time = %s\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].samplingTime);
	printf ("-----------------------------\n");
#endif

	return 0;
}

int	get_current_env(int storage_num)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from collect_env where dev_no = %d order by envtime desc limit 1", storage_num);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}

			sqlite3_close (db);

		env_info_clien_addr_s.storage_no[storage_num].temperature = callback_val[1];
		env_info_clien_addr_s.storage_no[storage_num].humidity = callback_val[2];
		env_info_clien_addr_s.storage_no[storage_num].illumination = callback_val[3];
		strcpy(env_info_clien_addr_s.storage_no[storage_num].samplingTime, callback_str);

		all_info_RT.storage_no[storage_num].temperature = callback_val[1];
		all_info_RT.storage_no[storage_num].humidity = callback_val[2];
		all_info_RT.storage_no[storage_num].illumination = callback_val[3];
		strcpy(all_info_RT.storage_no[storage_num].samplingTime, callback_str);
		
#if 1
		printf ("dev_no:%d  temperature = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].temperature);
		printf ("dev_no:%d  humidity = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].humidity);
		printf ("dev_no:%d  illumination = %0.1f\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].illumination);
		printf ("dev_no:%d  time = %s\n", storage_num, env_info_clien_addr_s.storage_no[storage_num].samplingTime);
		printf ("-----------------------------\n");
#endif

	return 0;

}

int	insert_goods(struct env_info_clien_addr env_info, int storageNum, int goodskinds)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	char sql[1024];
	sprintf (sql, "insert into goods values(%d,%d,%d,'%s');",
				  storageNum,
				  env_info.storage_no[storageNum].goods_info[goodskinds].goods_type,
				  env_info.storage_no[storageNum].goods_info[goodskinds].goods_count,
			      env_info.storage_no[storageNum].samplingTime);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Insert goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}

	return 0;
}
int	add_goods(struct env_info_clien_addr env_info, int storageNum, int goodskinds)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode, addNum = 0;
	char sql[1024];
	addNum = env_info.storage_no[storageNum].goods_info[goodskinds].goods_count;

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum, goodskinds);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1; 
		}
		else
		{
#if DEBUG_SQLITE
			printf ("add select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count = (int)callback_val[2];
		printf ("!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!!%d, %d\n", env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type, env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count);
	}

	sprintf (sql, "update goods set goodsCount = '%d', goodsTime = '%s' where dev_no = '%d' and goodsId = '%d';",
//				  env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count,
				  addNum,
			      env_info_clien_addr_s.storage_no[storageNum].samplingTime,
				  storageNum,
				  env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("add goods OK!\n");
#endif
		printf ("~~~~~~~~~~~~~~%d, %d\n", env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type, env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count + addNum);
			sqlite3_close (db);
		}
	}

	return 0;
}

int view_goods (int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("viewGoods ok\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("view goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}

	return (int)callback_val[2];
}

int delete_goods (int storageNum_t, int goodsKinds_t)
{
#if DEBUG_SQLITE
	printf ("deleteGoods OK\n");
#endif
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "delete from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("delete goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}
	return 0;
}

int	reduce_goods(struct env_info_clien_addr env_info, int storageNum, int goodskinds)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode, addNum = 0;
	char sql[1024];
	addNum = env_info.storage_no[storageNum].goods_info[goodskinds].goods_count,

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum, goodskinds);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("Reduce select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count = (int)callback_val[2];
	}

	sprintf (sql, "update goods set goodsCount = '%d', goodsTime = '%s' where dev_no = '%d' and goodsId = '%d';",
//				  (env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_count - addNum),
				  addNum,
			      env_info_clien_addr_s.storage_no[storageNum].samplingTime,
				  storageNum,
				  env_info_clien_addr_s.storage_no[storageNum].goods_info[goodskinds].goods_type);
	recode = sqlite3_open (SQLITE_OPEN, &db);
	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, 0, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("reduce goods OK!\n");
#endif
			sqlite3_close (db);
		}
	}
	if (0 == view_goods(storageNum, goodskinds))
	{
		delete_goods (storageNum, goodskinds);
	}

	return 0;
}

int get_goods (int storageNum_t, int goodsKinds_t)
{
	sqlite3 *db;
	char *err_msg = 0;
	int recode;
	recode = sqlite3_open (SQLITE_OPEN, &db);
	char sql[1024];

	sprintf (sql, "select * from goods where dev_no = '%d' and goodsId = '%d';", storageNum_t, goodsKinds_t);

	if (recode != SQLITE_OK)
	{
		printf ("Can't Open Database:%s!\n", sqlite3_errmsg(db));
		sqlite3_close (db);
		return 1;
	}
	else
	{
#if DEBUG_SQLITE
		printf ("open OK!\n");
#endif
		recode = sqlite3_exec (db, sql, callback_getenv, 0, &err_msg);
		if (recode != SQLITE_OK)
		{
			printf ("Error:%s", err_msg);
			sqlite3_close (db);
			return 1;
		}
		else
		{
#if DEBUG_SQLITE
			printf ("select goods OK!\n");
#endif
			sqlite3_close (db);
		}
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];

		all_info_RT.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type = (int)callback_val[1];
		all_info_RT.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count = (int)callback_val[2];
#if DEBUG_SQLITE 
		printf ("goods_s.goods_type = %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_type);
		printf ("goods_s.goods_count = %d\n", env_info_clien_addr_s.storage_no[storageNum_t].goods_info[goodsKinds_t].goods_count);
		printf ("---------------------------------\n");
#endif
	}

	return 0;
}

int	get_allgoods()
{
	
#if DEBUG_SQLITE
	printf ("getAllGoods ok\n");
#endif

	return 0;
}

void sqlite_task(struct env_info_clien_addr env_info, struct sqlite_operation sqlite_op, int storageNum, int goodskinds)
{
	int storage_num;
	if(0 == sqlite_op.table_select_cmd)
	{
		if(0 == sqlite_op.env_operation_cmd)	
		{
			update_env(env_info, storageNum);	
		}
		else if(1 == sqlite_op.env_operation_cmd)
		{
			get_env(&env_info, storageNum);	
		}
	}
	else if(1 == sqlite_op.table_select_cmd)
	{
		if(0 == sqlite_op.collectEnv_operation_cmd)	
		{
			for(storage_num = 0; storage_num < STORAGE_NUM; storage_num++)	
			{
				insert_collect_env(env_info, storageNum);	
			}
		}
		else if(1 == sqlite_op.collectEnv_operation_cmd)	
		{
			for(storage_num = 0; storage_num < STORAGE_NUM; storage_num++)	
			{
				get_collect_env(env_info.storage_no[storage_num].samplingTime, storageNum);	
			}
		}
		else if(2 == sqlite_op.collectEnv_operation_cmd)	
		{
			for(storage_num = 0; storage_num < STORAGE_NUM; storage_num++)	
			{
				get_current_env(storage_num);	
			}
		}
	}
	else if(2 == sqlite_op.table_select_cmd)
	{
		if(0 == sqlite_op.goods_operation_cmd)	
		{
			if(0 == view_goods(storageNum, goodskinds))
			{
				insert_goods(env_info, storageNum, goodskinds);	
			}
			else
			{
				add_goods(env_info, storageNum, goodskinds);	
			}
		}
		else if(1 == sqlite_op.goods_operation_cmd)	
		{
			reduce_goods(env_info, storageNum, goodskinds);
		}
		else if(2 == sqlite_op.goods_operation_cmd)	
		{
			get_goods(storageNum, goodskinds);
		}
		else if(3 == sqlite_op.goods_operation_cmd)	
		{
			get_allgoods();
		}
	}
}

void *pthread_sqlite(void *arg)
{
	printf("pthread_sqlite is ok!\n");
	slinkhead = sqlite_CreateEmptyLinklist(); 
	slinklist buf = NULL;

	while(1)
	{
		pthread_mutex_lock(&mutex_slinklist);	
		if((buf = sqlite_GetLinknode(slinkhead)) == NULL)
		{
			pthread_mutex_unlock(&mutex_slinklist);
			break;
		}
		pthread_mutex_unlock(&mutex_slinklist);
		sqlite_task(buf->data_link, buf->data, buf->storageNum, buf->goodsKinds);
		free(buf);
	}

	return NULL;
}

