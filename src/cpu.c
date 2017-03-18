#include "cpu.h"

//#define PROG_ADDR 0x200
//#define BIT_MASK(n, s, b) \
//  ((n >> s) & (0x10)

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


// TODO Move this out into a different file - this should contain only
// CPU logic
int main(int argc, char **argv) {
  struct machine *m = malloc(sizeof(struct machine));
  
  assert(m != NULL);
  machine_init(m);

  load_program("../roms/TETRIS", m);

  /*for (int i = 0; i < 2000; i++) {  
    machine_tick(m);
    //printf("%02x %02x\n", m->pc, m->memory[m->pc]);
  }

  for (int i = 0; i < 64*32; i++) {
    if(m->display[i])
      printf("1");
    else
      printf("0");
    if ((i + 1) % 64 == 0)
      printf("\n");
  }*/

}

void machine_init(struct machine *m) {
  assert(m != NULL);

  // Initialise positions of pointer registers
  m->i = 0;
  m->pc = PROG_ADDR;
  m->sp = 0;
  
  // Reset the timers and draw flag
  m->delay_timer = 0;
  m->sound_timer = 0;
  m->needs_redraw = 0;
  
  // Clear display, registers, keys, stack, and memory
  memset(m->display, 0, sizeof(m->display));
  memset(m->key, false, sizeof(m->key));
  memset(m->V, 0, sizeof(m->V));
  memset(m->stack, 0, sizeof(m->stack));
  memset(m->memory, 0, sizeof(m->memory));

  // Load fontset into memory at the expected location
  memcpy(&m->memory[FONT_ADDR], fontset, sizeof(fontset));

}

void machine_tick(struct machine *m) {
  uint16_t instr = fetch_instruction(m);
  decode_instruction(instr, m);
}

uint16_t fetch_instruction(struct machine *m) {
  return m->memory[m->pc] << 8 | m->memory[m->pc + 1];
}

void decode_instruction(uint16_t instruction, struct machine *m) {

  uint8_t c = (instruction & 0xF000) >> 12;
  uint8_t x = (instruction & 0x0F00) >> 8; 
  uint8_t y = (instruction & 0x00F0) >> 4; 
  uint16_t nnn = (instruction & 0x0FFF); 
  uint8_t nn = (instruction & 0x00FF); 
  uint8_t n = (instruction & 0x000F); 

  switch(c) {
    case 0x0:
      switch(nnn) {
        case 0xE0:
          // TODO Move this out of here - horrible repetition
          // Clears screen
          memset(m->display, 0, sizeof(m->display));
          m->pc += 2;
          break;
        case 0xEE:
          // Return from subroutine
          m->pc = m->stack[--m->sp] + 2;
          break;
        default:
          // Calls RCA 1802 program at address NNN
          // TODO
          m->pc += 2;
          break;
      }
      break;
    case 0x1:
      // Jump to address NNN
      m->pc = nnn;
      break;
    case 0x2:
      // Jump to subroutine at NNN
      // TODO Check that stack hasn't overflowed
      m->stack[m->sp++] = m->pc;
      m->pc = nnn;
      break;
    case 0x3:
      // Skips next instruction if VX == NN
      if (m->V[x] == nn)
        m->pc += 2;
      m->pc += 2;
      break;
    case 0x4:
      // Skips next instruction if VX != NN
      if (m->V[x] != nn)
        m->pc += 2;
      m->pc += 2;
      break;
    case 0x5:
      // Skips next instruction if VX == VY
      if (m->V[x] == m->V[y])
        m->pc += 2;
      m->pc += 2;
      break;
    case 0x6:
      // Sets VX to NN
      m->V[x] = nn;
      m->pc += 2;
      break;
    case 0x7:
      // Adds NN to VX
      m->V[x] += nn;
      m->pc += 2;
      break;
    case 0x8:
      switch (n) {
        case 0:
          m->V[x] = m->V[y];
          break;
        case 1:
          m->V[x] |= m->V[y];
          m->V[0xF] = 0;
          break;
        case 2:
          m->V[x] &= m->V[y];
          m->V[0xF] = 0;
          break;
        case 3:
          m->V[x] ^= m->V[y];
          m->V[0xF] = 0;
          break;
        case 4:
          m->V[x] += m->V[y];
          m->V[0xF] = m->V[x] < m->V[y]; // Overflow flag
          break;
        case 5:
          m->V[x] -= m->V[y];
          m->V[0xF] = m->V[x] > m->V[y]; // Overflow flag
          break;
        case 6:
          m->V[0xF] = m->V[x] & 1; // Set VF to LSB
          m->V[x] >>= 1;
          break;
        case 7:
          m->V[x] = m->V[y] - m->V[x];
          m->V[0xF] = m->V[x] > m->V[y]; // Overflow flag
          break;
        case 0xE:
          m->V[0xF] = m->V[x] & 0x80; // Set VF to MSB
          m->V[x] <<= 1;
          break;
      }
      m->pc += 2;
      break;
    case 0x9:
      // Skips next instruction if VX != VY
      if (m->V[x] != m->V[y])
        m->pc += 2;
      m->pc += 2;
      break;
    case 0xA:
      // Sets I to address NNN
      m->i = nnn;
      m->pc += 2;
      break;
    case 0xB:
      // Jumps to address NNN plus V0
      m->pc = m->V[0] + nnn;
      break;
    case 0xC:
      // Sets VX to the result of a bitwise and operation on rand() and NN
      m->V[x] = (rand() % 256) & nn;
      m->pc += 2;
      break;
    case 0xD:
      // Draw sprite at VX, VY, width 8px, height Npx (see Wikipedia)
      {
        uint8_t row;

        m->V[0xF] = 0;
        for (int i = 0; i < n; i++) { // Work downwards - y axis
          row = m->memory[m->i + i];
          for (int j = 0; j < 8; j++) { // Work across - x axis
            // Check if current pixel is not turned off
            if ((row & (0x80 >> j)) != 0) {
              // See if the corresponding pixel on the screen is already on.
              // If it is, then we need to set the VF flag to indicate a 
              // collision.
              // TODO Remove magic numbers
              if (m->display[(m->V[x] + j + ((m->V[y] + i) * 64))] == 1)
                m->V[0xF] = 1;
              m->display[m->V[x] + j + ((m->V[y] + i) * 64)] ^= 1;
            } 
          }
        }

        m->needs_redraw = true;
        m->pc += 2;
      }
      break;
    case 0xE:
      switch (instruction & 0x00FF) {
        case 0x009E:
          if (m->key[m->V[x]])
            m->pc += 2; // Skip the next instruction
          m->pc += 2;
          break;
        case 0x00A1:
          if (!m->key[m->V[x]])
            m->pc += 2; // Skip the next instruction
          m->pc += 2;
          break;
      }
      break;
    case 0xF:
      {
        switch (nn) {
          case 0x7:
            m->V[x] = m->delay_timer;
            break;
          case 0xA:
            // TODO
            break;
          case 0x15:
            m->delay_timer = m->V[x];
            break;
          case 0x18:
            m->sound_timer = m->V[x];
            break;
          case 0x1E:
            m->i += m->V[x];
            break;
          case 0x29:
            m->i = FONT_ADDR + (BYTES_PER_CHARACTER * m->V[x]);
            break;
          case 0x33:
            m->memory[m->i]     = m->V[x] / 100;        // Hundreds
            m->memory[m->i + 1] = (m->V[x] / 10) % 10;  // Tens
            m->memory[m->i + 2] = (m->V[x] % 100) % 10; // Digits
            break;
          case 0x55:
            // Stores V0 to VX in memory, starting from address I
            memcpy(&m->memory[m->i], m->V, sizeof(m->V[0]) * (x + 1));
            break;
          case 0x65:
            // Fills registers with values in memory, starting from address I
            memcpy(m->V, &m->memory[m->i], sizeof(m->memory[0]) * (x + 1));
            break;
        }
        m->pc += 2;
      }
      break;
  }
}

void key_press(uint8_t i, struct machine *m) {

  m->key[i] = true;
}

void key_unpress(uint8_t i, struct machine *m) {
  m->key[i] = false;
}

bool load_program(char *path, struct machine *m) {
  // Open file in binary read mode and calculate file size
  FILE *f = fopen(path, "rb");
  if (!f)
    return false;
  fseek(f, 0, SEEK_END);
  long fsize = ftell(f);
  // Go back to start of file
  rewind(f);
  // Read the entire file into the 'buffer' at 0x200 in memory (the location
  // where programs are expected to begin at), then clean up after ourselves
  fread(&m->memory[PROG_ADDR], fsize, 1, f);
  fclose(f);
  return true;
}

void next_instruction(struct machine *m) {
  m->pc += BYTES_PER_INSTRUCTION;
}

void goto_instruction(uint16_t i, struct machine *m) {
  m->pc = i;
}

void print_memory(struct machine *m) {
  for (int i = 0; i < sizeof(m->memory) / sizeof(m->memory[0]); i++) {
    if (i % 16 == 0)
      printf("\n0x%03x: ", i);
    printf("%02x ", m->memory[i]);
  }
}

void print_registers(struct machine *m) {
  for (int i = 0; i < sizeof(m->V) / sizeof(m->V[0]); i++) {
    printf("%x: %02x, ", i, m->V[i]);
    if ((i + 1) % 8 == 0)
      printf("\n");
  }
  printf("------\n");
}
