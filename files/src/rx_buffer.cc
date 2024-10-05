#include <stdio.h>
#include "rx_buffer.hh"

#define RX_BUFFER_WORDS RX_BUFFER_BYTES/8

uint64_t G_RX_BUFFER_DATA[RX_BUFFER_DEPTH][RX_BUFFER_WORDS];
unsigned G_RX_BUFFER_HEAD;
unsigned G_RX_BUFFER_TAIL;

void rx_buffer_init(int verbose){
  G_RX_BUFFER_HEAD = 0;
  G_RX_BUFFER_TAIL = 0;
  if (verbose) {
    printf("INFO:  rx_buffer_init:\n");
    printf("INFO:  depth:         %d\n", RX_BUFFER_DEPTH);
    printf("INFO:  output bytes:  %d\n", RX_BUFFER_BYTES);
  }
}

void rx_buffer_status(){
  unsigned count = rx_buffer_count();
  unsigned head  = G_RX_BUFFER_HEAD;
  unsigned tail  = G_RX_BUFFER_TAIL;
  printf("rx_buffer count: %d  head %d tail %d\n", count, head, tail);
}

void rx_buffer_print_output(void * src){
  uint64_t * rx_data = (uint64_t *) src;

  printf("rx:  0x");
  for (int i=0; i<RX_BUFFER_WORDS; i++){
    printf("%08lx", rx_data[RX_BUFFER_WORDS-i-1]);
  }
  printf("\n");
}

unsigned rx_buffer_count(){
  unsigned head = G_RX_BUFFER_HEAD;
  unsigned tail = G_RX_BUFFER_TAIL;
  return (RX_BUFFER_DEPTH + head - tail) % RX_BUFFER_DEPTH;  
}

unsigned rx_buffer_in(void * src){
  unsigned head = G_RX_BUFFER_HEAD;  
  uint64_t * rx_data = (uint64_t *) src;
  if (((head+1) % RX_BUFFER_DEPTH) == G_RX_BUFFER_TAIL)
    return 0;
  
  for (int i=0; i<RX_BUFFER_WORDS; i++){
    G_RX_BUFFER_DATA[head][i] = rx_data[i];
  }

  G_RX_BUFFER_HEAD = (head + 1) % RX_BUFFER_DEPTH; 
  return 1;
}

unsigned rx_buffer_out(void * dst){
  if (rx_buffer_count() == 0)
    return 0;

  unsigned tail = G_RX_BUFFER_TAIL;
  uint64_t * rx_data = (uint64_t *) dst;
  for (int i=0; i<RX_BUFFER_WORDS; i++){
    rx_data[i] = G_RX_BUFFER_DATA[tail][i];
  }
  G_RX_BUFFER_TAIL = (tail+1) % RX_BUFFER_DEPTH;
  return 1;
}
