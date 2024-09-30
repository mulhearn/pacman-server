#ifndef PACMAN_HH
#define PACMAN_HH

#include <stdint.h>

int pacman_init(int verbose=0);

int pacman_poll_rx();

int pacman_poll_tx();

int pacman_write(uint32_t addr, uint32_t value);

uint32_t pacman_read(uint32_t addr, int * status = NULL);

#endif
