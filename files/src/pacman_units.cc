#include <stdio.h>

#include "tx_buffer.hh"
#include "rx_buffer.hh"

int test_tx_buffer(){
  int success = 1;
  uint64_t tx_data;
  uint64_t output[TX_BUFFER_BYTES/8];
  
  printf("INFO:  ****** running tx buffer unit test. *******\n");
  tx_buffer_init(1);

  printf("INFO:  checking basic functionality.\n");
  
  success &= (tx_buffer_out(NULL)==0);
  
  for (int i=0; i<3; i++){
    tx_data = (uint64_t) 0xAA00+i;
    success &= (tx_buffer_in(5, &tx_data)==1);
  }
  tx_data = (uint64_t) 0xBB;
  tx_buffer_in(1, &tx_data);

  for (int i=0; i<TX_BUFFER_DEPTH-1; i++){
    tx_data = (uint64_t) i;    
    success &= (tx_buffer_in(10, &tx_data)==1);
  }
  success &= (tx_buffer_in(10, &tx_data)==0);

  tx_buffer_status();

  success &= (tx_buffer_out(output)==1);
  tx_buffer_print_output(output);

  success &= (tx_buffer_out(output)==1);
  tx_buffer_print_output(output);

  success &= (tx_buffer_out(output)==1);
  tx_buffer_print_output(output);

  success &= (tx_buffer_out(output)==1);
  tx_buffer_print_output(output);
  
  for (int i=0; i<TX_BUFFER_DEPTH-5; i++){    
    success &= (tx_buffer_out(output)==1);
  }
  // check we are empty:
  success &= (tx_buffer_out(output)==0);

  if (success==0){
    printf("ERROR:  failed basic functionality test.\n");
    return 0;
  } else {
    printf("INFO:  ...success so far.\n");
  }

  printf("INFO:  Filling the entire buffer.\n");
  
  // completely fill the entire buffer:
  tx_data = 0;
  for (unsigned i=0; i<TX_BUFFER_CHAN; i++){
    for (unsigned j=0; j<TX_BUFFER_DEPTH-1; j++){
      tx_data = (j<<8) + i;
      success &= (tx_buffer_in(i, &tx_data)==1);
    }
  }
  // how about this wafer thin mint?
  success &= (tx_buffer_in(20, &tx_data)==0);

  if (success==0){
    printf("ERROR:  failed filing the entire buffer.\n");
    return 0;
  } else {
    printf("INFO:  ...success so far.\n");
  }

  printf("INFO:  Draining the entire buffer and checking contents.\n");
  
  // completely drain the entire buffer:
  for (int i=0; i<TX_BUFFER_DEPTH-1; i++){
    success &= (tx_buffer_out(&output)==1);
    success &= (output[0] == 0x000000FFFFFFFFFF);
    for (int j=0; j<TX_BUFFER_CHAN; j++){
      success &= ((output[2+j]&0xFF) == j);
      success &= ((output[2+j]>>8) == i);
      //printf("DEBUG: j:%x %lx\n", j, output[2+j]);
    }    
  }
  // check we are empty:
  success &= (tx_buffer_out(output)==0);

  if (success==0){
    printf("ERROR:  failed draining the entire buffer and checking contents.\n");
    return 0;
  } else {
    printf("INFO:  ...success so far.\n");
  }

  
  if (success==0){
    printf("ERROR:  tx buffer unit test FAILED.\n");
    return 0;
  }
  
  printf("SUMMARY:  tx buffer unit test SUCCESS.\n");
  return 1;
}

int test_rx_buffer(){
  int success = 1;
  uint64_t rx_data[2];
  
  printf("INFO:  ****** running rx buffer unit test. ****** \n");
  rx_buffer_init(1);

  printf("INFO:  checking basic functionality.\n");
  
  rx_buffer_status();

  rx_data[0]=0xBBBBBBBBAAAAAAAA;
  rx_data[1]=0xDDDDDDDDCCCCCCCC;

  success &= (rx_buffer_in(rx_data)==1);
  success &= (rx_buffer_in(rx_data)==1);
  success &= (rx_buffer_in(rx_data)==1);
  success &= (rx_buffer_count()==3);

  rx_buffer_status();
  
  success &= (rx_buffer_out(rx_data)==1);
  rx_buffer_print_output(rx_data);
  success &= (rx_buffer_count()==2);

  success &= (rx_buffer_out(rx_data)==1);
  success &= (rx_buffer_out(rx_data)==1);
  success &= (rx_buffer_out(rx_data)==0);

  rx_buffer_status();
  
  if (success==0){
    printf("ERROR:  failed basic functionality test.\n");
    return 0;
  } else {
    printf("INFO:  ...success so far.\n");
  }

  printf("INFO:  Filling the entire buffer.\n");
  
  // completely fill the entire buffer:
  for (unsigned i=0; i<RX_BUFFER_DEPTH-1; i++){
    rx_data[0] = i;
    rx_data[1] = 2*i;    
    success &= (rx_buffer_in(rx_data)==1);
  }
  // how about this wafer thin mint?
  success &= (rx_buffer_in(rx_data)==0);

  if (success==0){
    printf("ERROR:  filling the entire buffer failed.\n");
    return 0;
  } else {
    printf("INFO:  ...success so far.\n");
  }

  printf("INFO:  Draining the entire buffer and checking contents\n");
  
  // completely fill the entire buffer:
  for (unsigned i=0; i<RX_BUFFER_DEPTH-1; i++){
    success &= (rx_buffer_out(rx_data)==1);
    success &= (rx_data[0] == i);
    success &= (rx_data[1] == 2*i);    
    //printf("DEBUG: rx_data %3d 0x%lx %lx\n", i, rx_data[1], rx_data[0]);
    if (success==0)
      return 0;
  }

  //confirm empty:
  success &= (rx_buffer_out(rx_data)==0);

  rx_buffer_status();

  if (success==0){
    printf("ERROR:  rx buffer unit test FAILED.\n");
    return 0;
  }
  
  printf("SUMMARY:  rx buffer unit test SUCCESS.\n");
  return 1;
}


int main(){
  int success = 1;
  success &= test_tx_buffer();
  success &= test_rx_buffer();  
  if (success) {
    printf("SUMMARY:  *************************************************\n");
    printf("SUMMARY:  Congratulations!  All unit tests were successful.\n");
    printf("SUMMARY:  *************************************************\n");
    return 0;
  } else {
    printf("ERROR:  *** Failures detected! ***\n");
    return 1;
  }
}
