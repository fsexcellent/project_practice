#ifndef __LIST_QUEUE_H____
#define __LIST_QUEUE_H____
#include "data_global.h"

typedef struct msg_pack
{
	char msg_type;
	char text[27];
}link_datatype;

typedef struct _node_
{
	link_datatype data;
	struct _node_ *next;
}linknode, *linklist;

extern linklist CreateEmptyLinklist ();
extern int EmptyLinklist (linklist head);
extern linklist GetLinknode (linklist head);
extern int InsertLinknode (link_datatype data);

#endif
