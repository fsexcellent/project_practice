#include <stdio.h>
#include <stdlib.h>

#include "uart_snd_queue.h"

uart_snd_q* create_uart_snd_queue(void)
{
	uart_snd_node* head = (uart_snd_node*)malloc(sizeof(uart_snd_node));
	uart_snd_q* q = (uart_snd_q*)malloc(sizeof(uart_snd_q));

	head->next = NULL;
	q->front = q->rear = head;

	return q;
}

int is_empty_uart_snd_queue(uart_snd_q* q)
{
	return q->front == q->rear ? 1 : 0;
}

int enter_uart_snd_queue(uart_snd_q* q, unsigned char data)
{
	uart_snd_node* tmp = (uart_snd_node*)malloc(sizeof(uart_snd_node));

	tmp->cmd = data;
	tmp->next = NULL;

	q->rear->next = tmp;
	q->rear = tmp;

	return 0;
}

unsigned char delete_uart_snd_queue(uart_snd_q* q)
{
	uart_snd_node* tmp;

	tmp = q->front;
	q->front = tmp->next;
	free(tmp);

	return q->front->cmd;
}

#if 0
int main(int argc, const char *argv[])
{
	unsigned char a[] = {1,2,3,4,5,6};
	int i = 0;
	uart_snd_q* q = create_uart_snd_queue();

	for(i = 0; i < 6; i++)
	{
		enter_uart_snd_queue(q, a[i]);
	}

	while(!is_empty_uart_snd_queue(q))
	{
		printf("%d ", delete_uart_snd_queue(q));
	}
	printf("\n");
	
	return 0;
}
#endif
