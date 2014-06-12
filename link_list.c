#include "link_list.h"
#include "data_global.h"

linklist linkHead, linkTail;

linklist CreateEmptyLinklist ()
{
	linklist head;
	head = (linklist)malloc (sizeof (linknode));
	linkTail = head;
	head->next = NULL;
	return head;
}

int EmptyLinklist (linklist head)
{
	return NULL == head->next;
}

linklist GetLinknode (linklist head)
{
	if (1 == EmptyLinklist (head))	
	{
		return NULL;
	}
	linklist p = head->next;
	head->next = p->next;
	if (p->next == NULL)
		linkTail = head;
	return p;
}

int InsertLinknode (link_datatype data)
{
	linklist q = (linklist)malloc (sizeof (linknode));
	if (NULL == q)
	{
		printf ("InsertLinknode Error\n");
		return -1;
	}
	linkTail->next = q;
	linkTail = q;
	q->data = data;
	q->next = NULL;

	return 0;
}

