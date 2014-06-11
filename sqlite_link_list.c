#include <string.h>
#include <stdlib.h>
#include <stdio.h>
#include <errno.h>
#include "sqlite_link_list.h"
#include "data_global.h"

extern slinklist slinkhead, slinktail;

slinklist sqlite_CreateEmptyLinklist ()
{
	slinklist head = NULL;
	head = (slinklist)malloc(sizeof(slinknode));
	if(head == NULL)
	{
		perror("malloc");	
		exit(-1);
	}
	bzero(head, sizeof(slinknode));
	head->next = NULL;
	slinktail = head;

	return head;
}

int sqlite_isemptyLinklist (slinklist h)
{
	return h->next == NULL;
}

slinklist sqlite_GetLinknode (slinklist h)
{
	if(sqlite_isemptyLinklist(h))
		return NULL;

	slinklist p = h->next;
	h->next = p->next;
	if(p->next == NULL)
		slinktail = h;

	return p;
}

int sqlite_InsertLinknode (int x, struct env_info_clien_addr y, 
								  int storageNum_l, int goodsKinds_l)
{
	slinklist new = (slinklist)malloc(sizeof(slinknode));
	if(new == NULL)
	{
		perror("malloc");
		return -1;
	}
	struct sqlite_operation sqlite_operation_t;
	switch(x & 0xff)
	{
	case 0x00:
		sqlite_operation_t.table_select_cmd = 0;
		sqlite_operation_t.env_operation_cmd = 0;
		break;
	case 0x10:
		sqlite_operation_t.table_select_cmd = 1;
		sqlite_operation_t.collectEnv_operation_cmd = 0;
		break;
	case 0x11:
		sqlite_operation_t.table_select_cmd = 1;
		sqlite_operation_t.collectEnv_operation_cmd = 1;
		break;
	case 0x12:
		sqlite_operation_t.table_select_cmd = 1;
		sqlite_operation_t.collectEnv_operation_cmd = 2;
		break;
	case 0x20:
		sqlite_operation_t.table_select_cmd = 2;
		sqlite_operation_t.goods_operation_cmd = 0;
		break;
	case 0x21:
		sqlite_operation_t.table_select_cmd = 2;
		sqlite_operation_t.goods_operation_cmd = 1;
		break;
	case 0x22:
		sqlite_operation_t.table_select_cmd = 2;
		sqlite_operation_t.goods_operation_cmd = 2;
		break;
	case 0x23:
		sqlite_operation_t.table_select_cmd = 2;
		sqlite_operation_t.goods_operation_cmd = 3;
		break;
	}
	slinktail->next = new;
	slinktail = new;
	new->data = sqlite_operation_t;
	new->data_link = y;
	new->storageNum = storageNum_l;
	new->goodsKinds = goodsKinds_l;
	new->next = NULL;

	return 0;
}
