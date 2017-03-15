#include "cpu.h"

/* Chip 8 basic font set */
uint8_t fontset[80] = {
  0xF0, 0x90, 0x90, 0x90, 0xF0, // 0
  0x20, 0x60, 0x20, 0x20, 0x70, // 1
  0xF0, 0x10, 0xF0, 0x80, 0xF0, // 2
  0xF0, 0x10, 0xF0, 0x10, 0xF0, // 3
  0x90, 0x90, 0xF0, 0x10, 0x10, // 4
  0xF0, 0x80, 0xF0, 0x10, 0xF0, // 5
  0xF0, 0x80, 0xF0, 0x90, 0xF0, // 6
  0xF0, 0x10, 0x20, 0x40, 0x40, // 7
  0xF0, 0x90, 0xF0, 0x90, 0xF0, // 8
  0xF0, 0x90, 0xF0, 0x10, 0xF0, // 9
  0xF0, 0x90, 0xF0, 0x90, 0x90, // A
  0xE0, 0x90, 0xE0, 0x90, 0xE0, // B
  0xF0, 0x80, 0x80, 0x80, 0xF0, // C
  0xE0, 0x90, 0x90, 0x90, 0xE0, // D
  0xF0, 0x80, 0xF0, 0x80, 0xF0, // E
  0xF0, 0x80, 0xF0, 0x80, 0x80  // F
};

int main(int argc, char **argv) {
  struct machine *machine = malloc(sizeof(struct machine));
  
  assert(machine != NULL);
  machine_init(machine);
}

void machine_init(struct machine *m) {
  assert(m != NULL);

  /* Initialise positions of pointer registers */
  m->i = 0;
  m->pc = 0x200;
  m->sp = 0;
  
  /* Reset the timers and draw flag */
  m->delay_timer = 0;
  m->sound_timer = 0;
  m->needs_redraw = 0;
  
  /* Clear display, registers, keys, stack, and memory */
  memset(m->display, 0, sizeof(m->display));
  memset(m->key, false, sizeof(m->key));
  memset(m->V, 0, sizeof(m->V));
  memset(m->stack, 0, sizeof(m->stack));
  memset(m->memory, 0, sizeof(m->memory));

  /* Load fontset into memory at the expected location */
  memcpy(&m->memory[0x050], fontset, sizeof(fontset));

}

uint16_t fetch_instruction(struct machine *m) {
  return m->memory[m->pc] << 8 | m->memory[m->pc + 1];
}

void decode_instruction(uint16_t instruction, struct machine *m) {
  switch(instruction & 0xF000) {
    case 0x0000:
      switch(instruction & 0x0F00) {
        case 0x0000:
          // TODO
          break;
        default:
          // TODO
          break;
      }
      break;
    case 0x1000:
      // Jump to address NNN
      m->pc = instruction & 0x0FFF;
      break;
    case 0x2000:
      // Jump to subroutine at NNN
      break;
    case 0x3000:
      // Skips next instruction if VX == NN
      if (m->V[(instruction & 0x0F00) >> 8] == (instruction & 0x00FF))
        m->pc += 4;
      else
        m->pc += 2;
      break;
    case 0x4000:
      // Skips next instruction if VX != NN
      if (m->V[(instruction & 0x0F00) >> 8] != (instruction & 0x00FF))
        m->pc += 4;
      else
        m->pc += 2;
      break;
    case 0x5000:
      // Skips next instruction if VX == VY
      if (m->V[(instruction & 0x0F00) >> 8] == m->V[(instruction & 0x00F0) >> 4])
        m->pc += 4;
      else
        m->pc += 2;
      break;
    case 0x6000:
      // Sets VX to NN
      m->V[(instruction & 0x0F00) >> 8] = instruction & 0x00FF;
      m->pc += 2;
      break;
    case 0x7000:
      // Adds NN to VX
      m->V[(instruction & 0x0F00) >> 8] += instruction & 0x00FF;
      m->pc += 2;
      break;
    case 0x8000:
      // TODO
      break;
    case 0x9000:
      // Skips next instruction if VX != VY
      if (m->V[(instruction & 0x0F00) >> 8] != m->V[(instruction & 0x00F0) >> 4])
        m->pc += 4;
      else
        m->pc += 2;
      break;
    case 0xA000:
      // Sets I to address NNN
      m->i = instruction & 0x0FFF;
      m->pc += 2;
      break;
    case 0xB000:
      // Jumps to address NNN plus V0
      m->pc = m->V[0] + (instruction & 0x0FFF);
      break;
    case 0xC000:
      // Sets VX to the result of a bitwise and operation on rand() and NN
      m->V[instruction & 0x0F00] = (rand() % 256) & (instruction & 0x00FF);
      m->pc += 2;
      break;
    case 0xD000:
      // Draw sprite at VX, VY, width 8px, height Npx (see Wikipedia)
      {
        uint8_t x = (instruction & 0x0F00) >> 8;
        uint8_t y = (instruction & 0x00F0) >> 4;
        uint8_t h = instruction & 0x000F;
        uint8_t row;

        m->V[0xF] = 0;
        for (int i = 0; i < h; i++) { // Work downwards - y axis
          row = m->memory[m->i + i];
          for (int j = 0; j < 8; j++) { // Work across - x axis
            // Check if current pixel is not turned off
            if ((row & (0x80 >> j)) != 0) {
              // See if the corresponding pixel on the screen is already on.
              // If it is, then we need to set the VF flag to indicate a 
              // collision.
              // TODO Remove magic numbers
              if (m->display[(x + j + ((y + i) * 64))] == 1)
                m->V[0xF] = 1;
              m->display[x + j + ((y + i) * 64)] ^= 1;
            } 
          }
        }

        m->needs_redraw = true;
        m->pc += 2;
      }
      break;
    case 0xE000:
      switch (instruction & 0x00FF) {
        case 0x009E:
          if (m->keys[(instruction & 0x0F00) >> 8])
            m->pc += 2; // Skip the next instruction
          m->pc += 2;
          break;
        case 0x00A1:
          if (!m->keys[(instruction & 0x0F00) >> 8])
            m->pc += 2; // Skip the next instruction
          m->pc += 2;
          break;
      }
      break;
    case 0xF000:
      // TODO
      break;
  }
}

void key_press(uint16_t i, struct machine *m) {
  m->key[i] = true;
}

void key_unpress(uint16_t i, struct machine *m) {
  m->key[i] = false;
}
