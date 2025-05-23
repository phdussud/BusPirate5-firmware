#include "queue.h"
extern queue_t rx_fifo;
extern queue_t bin_rx_fifo;
void rx_fifo_init(void);
void rx_uart_init_irq(void);
void rx_usb_init(void);
void rx_from_rtt_terminal(void); // get terminal input from RTT until queue full or RTT input is empty

void rx_fifo_add(char* c);
bool rx_fifo_try_get(char* c);
void rx_fifo_get_blocking(char* c);
bool rx_fifo_try_peek(char* c);
void rx_fifo_peek_blocking(char* c);

void bin_rx_fifo_add(char* c);
void bin_rx_fifo_get_blocking(char* c);
void bin_rx_fifo_available_bytes(uint16_t* cnt);
bool bin_rx_fifo_try_get(char* c);
