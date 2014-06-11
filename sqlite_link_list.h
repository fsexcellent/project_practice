#ifndef __SQLITE_LIST_QUEUE_H____
#define __SQLITE_LIST_QUEUE_H____
#include "data_global.h"


typedef struct _snode_
{
	struct sqlite_operation data;
	struct env_info_clien_addr data_link;
	int storageNum;
	int goodsKinds;
	struct _snode_ *next;
}slinknode, *slinklist;

slinklist slinkhead ,slinktail ;

extern slinklist sqlite_CreateEmptyLinklist ();
extern int sqlite_EmptyLinklist (slinklist h);
extern slinklist sqlite_GetLinknode (slinklist h);
extern int sqlite_InsertLinknode (int x, struct env_info_clien_addr y, 
								  int storageNum_l, int goodsKinds_l);

#endif
