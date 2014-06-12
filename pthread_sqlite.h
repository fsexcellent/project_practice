#ifndef __SQLITE_TEST_H__
#define __SQLITE_TEST_H__

#include "data_global.h"
#include "sqlite_link_list.h"
#include "sqlite3.h"

#define DEBUG_SQLITE 0


int create_table (void);  //创建数据库表
int  init_table_env (void);  //初始化环境参数表

void sqlite_task(struct env_info_clien_addr env_info_clien_addr_t, struct sqlite_operation sqlite_operation_t, int storageNum_t, int goodsKinds_t);

int update_env (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t);  //更新整个环境参数
int get_env (struct env_info_clien_addr *env_info_clien_addr_t, int storageNum_t);  //获得整个环境参数
int insertCollect_env (struct env_info_clien_addr env_info_clien_addr_t, int storage_num);
int get_collect_env (char itime_t[], int storage_num_t);//获得Collect_env表参数
int get_current_env (int storage_num_t);
int insert_goods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);  //插入新的货物信息
int delet_goods (int storageNum_t, int goodsKinds_t);  //删除指定编号货物信息
int get_goods (int storageNum_t, int goods_kinds_t);  //获得指定编号货物信息
int get_goodsInfo (void);  //获得所有货物信息
int view_goods (int storageNum_t, int goodsKinds_t);  //查看仓库是否存在此类货物
int add_goods(struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);
int reduce_goods (struct env_info_clien_addr env_info_clien_addr_t, int storageNum_t, int goodsKinds_t);








#endif
	  
