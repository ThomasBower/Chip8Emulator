#include "cpu.h"

int main(int argc, char **argv) {
  struct machine *machine;
  machine_init(machine);
}

void machine_init(struct machine *machine) {
  // TODO: put an assert here checking if null
  
  /* Initialise positions of pointer registers */
  machine->i = 0;
  machine->pc = 0x200;
  machine->sp = 0;
  
  /* Reset the timers */
  machine->delay_timer = 0;
  machine->sound_timer = 0;
  
  /* Clear display, registers, keys, stack, and memory */
  memset(machine->display, 0, sizeof(machine->display));
  memset(machine->keys, 0, sizeof(machine->keys));
  memset(machine->V, 0, sizeof(machine->V));
  memset(machine->stack, 0, sizeof(machine->stack));
  memset(machine->memory, 0, sizeof(machine->memory));

  // TODO: load the fontset into memory
  //       between 0x050 and 0x0A0

}
