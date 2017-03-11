#ifndef CPU_H
#define CPU_H

#include <stdint.h>
#include <stdbool.h>
#include <string.h>

struct machine {
  /* Registers and memory */
  uint8_t  memory[4096];     /* 4K worth of memory */
  uint8_t  V[16];            /* Array holding all 16 8-bit registers */
  uint16_t i;                /* 16-bit address register */
  uint16_t pc;               /* 16-bit program counter */
  uint16_t stack[16];        /* Call stack which can handle 16 nested calls */
  uint16_t sp;               /* 16-bit stack pointer register */
  
  /* Information for display and sound */
  uint8_t  delay_timer;      /* 8-bit delay timer register */
  uint8_t  sound_timer;      /* 8-bit sound timer register */
  bool     display[64 * 32]; /* Array representing each pixel of the display 
                                true = switched on (white), false = off/black */
  bool     keys[16];         /* Array representing the state of the hex keyboard
                                true = pressed, false = not pressed */
};

/* Sets up the machine with correct initial values */
void init_machine(struct machine *); 

#endif /* CPU_H */
