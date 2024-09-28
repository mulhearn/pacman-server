#ifndef pacman_dataserver_cc
#define pacman_dataserver_cc

#include <thread>
#include <chrono>

#include <algorithm>
#include <fcntl.h>
#include <stdio.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <string.h>
#include <zmq.h>
#include <ctime>

//#include "addr_conf.hh"
//#include "dma.hh"
#include "larpix.hh"
#include "message_format.hh"

#define MAX_MSG_LEN 65535 // words
#define PUB_SOCKET_BINDING "tcp://*:5556"

volatile bool msg_ready = true;

void clear_msg(void*, void*) {
    msg_ready = true;
}

int main(int argc, char* argv[]){
  printf("Start pacman-dataserver\n");
  // create zmq connection
  void* ctx = zmq_ctx_new();
  void* pub_socket = zmq_socket(ctx, ZMQ_PUB);
  int hwm = 100;
  zmq_setsockopt(pub_socket, ZMQ_SNDHWM, &hwm, sizeof(hwm));
  int linger = 0;
  zmq_setsockopt(pub_socket, ZMQ_LINGER, &linger, sizeof(linger));
  int timeo = 1000;
  zmq_setsockopt(pub_socket, ZMQ_SNDTIMEO, &timeo, sizeof(timeo));
  if (zmq_bind(pub_socket, PUB_SOCKET_BINDING) !=0 ) {
    printf("Failed to bind socket!\n");
    return 1;
  }
  printf("ZMQ socket created\n");
  
  auto start_time = std::chrono::high_resolution_clock::now().time_since_epoch();
  auto last_time  = start_time;
  auto now = start_time;
  auto last_sent_msg = now;  
  uint64_t total_words = 0;
  uint32_t words = 0;
  uint32_t msg_bytes;
  uint32_t sent_msgs = 0;
  uint32_t word_idx;
  char word_type;
  char* word;
  char msg_buffer[HEADER_LEN + MAX_MSG_LEN*WORD_LEN]; // pre-allocate message buffer
  zmq_msg_t* pub_msg = new zmq_msg_t();  
  bool err = false;
  printf("Begin loop\n");
  while(1) {
    std::this_thread::sleep_for(std::chrono::milliseconds(2000));
    
    total_words += words;
    now = std::chrono::high_resolution_clock::now().time_since_epoch();

    words = 4;
    
    // create new message
    init_msg(msg_buffer, words, MSG_TYPE_DATA);
    msg_bytes = get_msg_bytes(words);      

    zmq_msg_init_data(pub_msg, msg_buffer, msg_bytes, clear_msg, NULL);

    word_idx = 0;
    // copy data into message
    while(word_idx < words) {
	word = get_word(msg_buffer, word_idx);
	unsigned chan = (word_idx & 0xFF);
	uint32_t dummy[] = {0xABCD0044+(chan<<8), 0xABCDABCD, 0xABCDABCD, 0xABCDABCD};
        memcpy(word, &dummy, 16);
	word_idx++;
    }
    
    msg_ready = false;    
    if (zmq_msg_send(pub_msg, pub_socket, 0) < 0)
        printf("Error sending message!\n");
    else
        sent_msgs++;
    last_sent_msg = std::chrono::high_resolution_clock::now().time_since_epoch();
    zmq_msg_close(pub_msg);

    printf("Data messages sent: %d\n", sent_msgs);
    sent_msgs = 0;
    words = 0;
  }
  return 0;
}

#endif
