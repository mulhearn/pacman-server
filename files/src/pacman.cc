#include <stdlib.h>
#include <stdio.h>

#include "pacman.hh"
#include "tx_buffer.hh"

int pacman_init(int verbose){
  printf("WARNING:  *** Using simulated hardware driver.  ***\n");
  return EXIT_SUCCESS;
}

int pacman_poll_rx(){
  return EXIT_SUCCESS;
}

uint32_t G_PACMAN_TXA = 0xA;
uint32_t G_PACMAN_TXB = 0xB;

int pacman_poll_tx(){
  uint64_t output[TX_BUFFER_BYTES/4];
  if (tx_buffer_out(output) == 0)
    return EXIT_SUCCESS;

  printf("DEBUG:  extracted output %lx\n", output[2]);
  
  G_PACMAN_TXA = output[2] & 0xFFFFFFFF;
  G_PACMAN_TXB = (output[2]>>32) & 0xFFFFFFFF;
  
  return EXIT_SUCCESS;
}

uint32_t G_PACMAN_SCRA = 0x0;
uint32_t G_PACMAN_SCRB = 0x0;

int pacman_write(uint32_t addr, uint32_t value){
  if (addr == 0x0)
    G_PACMAN_SCRA = value;
  else if (addr == 0x4)
    G_PACMAN_SCRB = value;
  
  return EXIT_SUCCESS;
}

uint32_t pacman_read(uint32_t addr, int * status){
  if (status)
    *status = EXIT_SUCCESS;
  if (addr == 0x0)
    return G_PACMAN_SCRA;
  else if (addr == 0x4)
    return G_PACMAN_SCRB;
  else if (addr == 0x8)
    return G_PACMAN_TXA;
  else if (addr == 0xC)
    return G_PACMAN_TXB;
  return 0x0;  
}



