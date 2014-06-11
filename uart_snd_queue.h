#ifndef __UART_SND_QUEUE__HHHHHHHHHHHHHHH
#define __UART_SND_QUEUE__HHHHHHHHHHHHHHH

typedef struct uart_node
{
	unsigned char cmd;
	struct uart_node* next;
}uart_snd_node;

typedef struct
{
	uart_snd_node* front;
	uart_snd_node* rear;
}uart_snd_q;

extern uart_snd_q* create_uart_snd_queue(void);
extern int is_empty_uart_snd_queue(uart_snd_q* q);
extern int enter_uart_snd_queue(uart_snd_q* q, unsigned char data);
extern unsigned char delete_uart_snd_queue(uart_snd_q* q);
	
#endif

