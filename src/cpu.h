#ifndef CPU_H
#define CPU_H

#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <stdbool.h>
#include <string.h>
#include <assert.h>

#define NUM_PIXELS_X 64
#define NUM_PIXELS_Y 32
#define PROG_ADDR 0x200
#define FONT_ADDR 0x050
#define MEM_SIZE 4096
#define BYTES_PER_INSTRUCTION 2
#define BYTES_PER_CHARACTER 5

struct machine {
  /* Registers and memory */
  uint8_t  memory[MEM_SIZE];     /* 4K worth of memory */
  uint8_t  V[16];            /* Array holding all 16 8-bit registers */
  uint16_t i;                /* 16-bit address register */
  uint16_t pc;               /* 16-bit program counter */
  uint16_t stack[16];        /* Call stack which can handle 16 nested calls */
  uint16_t sp;               /* 16-bit stack pointer register */
  
  /* Information for display and sound */
  uint8_t  delay_timer;      /* 8-bit delay timer register */
  uint8_t  sound_timer;      /* 8-bit sound timer register */
                             /* Array representing each pixel of the display 
                              * true = switched on/white, false = off/black */
  bool     needs_redraw;     /* Flag used to determine whether or not to
                              * redraw the pixels on screen. */
  bool     display[NUM_PIXELS_X * NUM_PIXELS_Y]; 
  bool     key[16];          /* Array representing the state of the hex keyboard
                              * true = pressed, false = not pressed */
};

/* Sets up the machine with correct initial values */
void machine_init(struct machine *m); 

/* Run one tick of the machine */
void machine_tick(struct machine *m); 

/* Fetch the next instruction and increase the program counter */
uint16_t fetch_instruction(struct machine *m);

/* Decodes the instruction based on the opcode */
void decode_instruction(uint16_t instruction, struct machine *m);

/* Functions to handle key presses */
void key_press(uint8_t i, struct machine *m);
void key_unpress(uint8_t i, struct machine *m);

/* Used to load the program stored at path into machine m. Returns true
 * if successful, false otherwise */
bool load_program(char *path, struct machine *m);

/* Advances the program to the next instruction */
void next_instruction(struct machine *m);
void goto_instruction(uint16_t i, struct machine *m);

/* Prints out the values of every item in memory, for debugging purposes */
void print_memory(struct machine *m);
void print_registers(struct machine *m);

/* Functions for different instructions */
/*void instr_call_rca(uint16_t addr, struct machine *m);
void instr_clear_display(struct machine *m);
void instr_return(struct machine *m);
void instr_goto(uint16_t addr, struct machine *m);
void instr_call(uint16_t addr, struct machine *m);
void instr_skip_eq_direct(uint8_t x, uint8_t nn, struct machine *m);
void instr_skip_neq_direct(uint8_t x, uint8_t nn, struct machine *m);
void instr_skip_eq_y(uint8_t x, uint8_t y, struct machine *m);
// TODO Finish these functions
*/

#endif /* CPU_H */
