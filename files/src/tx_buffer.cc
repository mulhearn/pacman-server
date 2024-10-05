#include <stdio.h>
#include "tx_buffer.hh"

uint64_t G_TX_BUFFER_DATA[TX_BUFFER_CHAN][TX_BUFFER_DEPTH];
unsigned G_TX_BUFFER_HEAD[TX_BUFFER_CHAN];
unsigned G_TX_BUFFER_TAIL[TX_BUFFER_CHAN];

void tx_buffer_init(int verbose){
  for (int i=0; i<TX_BUFFER_CHAN; i++){
    G_TX_BUFFER_HEAD[i] = 0;
    G_TX_BUFFER_TAIL[i] = 0;
    for (int j=0; j<TX_BUFFER_CHAN; j++){
      G_TX_BUFFER_DATA[i][j];
    }
  }
  if (verbose) {
    printf("INFO:  tx_buffer_init:\n");
    printf("INFO:  channels:      %d\n", TX_BUFFER_CHAN);
    printf("INFO:  depth:         %d\n", TX_BUFFER_DEPTH);
    printf("INFO:  output bytes:  %d\n", TX_BUFFER_BYTES);
  }
}

void tx_buffer_status(){
  for (int i=0; i<TX_BUFFER_CHAN; i++){
    unsigned count = tx_buffer_count(i);
    unsigned head  = G_TX_BUFFER_HEAD[i];
    unsigned tail  = G_TX_BUFFER_TAIL[i];
    if ((count==0)&&(head==0)&&(tail==0))
      continue;
    printf("chan %3d %d  --- %d %d\n", i, count, head, tail);
  }
}

void tx_buffer_print_output(void * src){
  uint64_t * buf = (uint64_t *) src;
  uint64_t mask = buf[0];
  printf("channel mask:  0x%08lx\n", mask);
  for (int i=0; i<TX_BUFFER_CHAN; i++){
    if ((mask>>i)&1==1) {
      printf("chan: %3d tx_data: 0x%08lx\n", i, buf[2+i]);
    }
  }
}

unsigned tx_buffer_count(unsigned char chan){
  if (chan > TX_BUFFER_CHAN)
    return 0;
  unsigned head = G_TX_BUFFER_HEAD[chan];
  unsigned tail = G_TX_BUFFER_TAIL[chan];

  return (TX_BUFFER_DEPTH + head - tail) % TX_BUFFER_DEPTH;  
}

unsigned tx_buffer_in(unsigned char chan, uint64_t * tx_data){
  // ignoring broadcast for now
  if (chan > TX_BUFFER_CHAN)
    return 0;
  unsigned head = G_TX_BUFFER_HEAD[chan];
  
  if (((head+1) % TX_BUFFER_DEPTH) == G_TX_BUFFER_TAIL[chan])
    return 0;
  G_TX_BUFFER_DATA[chan][head] = *tx_data;
  G_TX_BUFFER_HEAD[chan] = (head + 1) % TX_BUFFER_DEPTH; 
  return 1;
}

unsigned tx_buffer_out(void * dst){
  uint64_t mask = 0;
  uint64_t one  = 1;

  //printf("DEBUG:  mask before loop:  %lx\n", mask);
  for (unsigned i=0; i<TX_BUFFER_CHAN; i++){
    if (tx_buffer_count(i) > 0)
      mask |= (one << i);
  }
  //printf("DEBUG:  mask after:  %lx\n", mask);
  
  if (mask==0)
    return 0;
  
  uint64_t * out = (uint64_t *) dst;
  out[0] = mask;

  for (int i=0; i<TX_BUFFER_CHAN; i++){
    if ((mask>>i)&1==1) {
      unsigned tail = G_TX_BUFFER_TAIL[i];
      out[2+i] = G_TX_BUFFER_DATA[i][tail];
      G_TX_BUFFER_TAIL[i] = (tail+1) % TX_BUFFER_DEPTH;
    } else {
      out[2+i] = 0;
    }
  }
  

  return 1;
}
