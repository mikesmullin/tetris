// Tetris CLI - Piece Definitions
// All 7 Tetrominoes with 4 rotations each
#pragma once

#include "tetris.h"

// @class Piece
// Function | Purpose
// --- | ---
// Piece__get_def(kind) | Get piece definition by kind
// Piece__get_cell(def, rot, row, col) | Check if cell is filled in piece rotation
// Piece__get_color(kind) | Get ANSI color code for piece kind

// ---
// Piece Rotation Data
//
// Each piece is stored as a 4x4 grid in a u16 bitmask.
// Bit layout (MSB to LSB):
//   bits 15-12: row 0 (top)
//   bits 11-8:  row 1
//   bits 7-4:   row 2
//   bits 3-0:   row 3 (bottom)
// Within each row, bit 3 = col 0 (left), bit 0 = col 3 (right)
//
// Visual mapping:
//   row 0: [bit15 bit14 bit13 bit12]
//   row 1: [bit11 bit10 bit9  bit8 ]
//   row 2: [bit7  bit6  bit5  bit4 ]
//   row 3: [bit3  bit2  bit1  bit0 ]

// Macro to encode a piece row (4 cells, left to right)
#define ROW(a, b, c, d) (((a) << 3) | ((b) << 2) | ((c) << 1) | (d))
#define PIECE(r0, r1, r2, r3) ((u16)(((r0) << 12) | ((r1) << 8) | ((r2) << 4) | (r3)))

// ---
// Piece Definitions

static const PieceDef PIECES[NUM_PIECES] = {
  // PIECE_I - Cyan
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  .X..           ....           ..X.           ....
  //  .X..           XXXX           ..X.           ....
  //  .X..           ....           ..X.           XXXX
  //  .X..           ....           ..X.           ....
  [PIECE_I] = {
    .rotations = {
      PIECE(ROW(0,1,0,0), ROW(0,1,0,0), ROW(0,1,0,0), ROW(0,1,0,0)),  // rot 0
      PIECE(ROW(0,0,0,0), ROW(1,1,1,1), ROW(0,0,0,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,1,0), ROW(0,0,1,0), ROW(0,0,1,0), ROW(0,0,1,0)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,0,0,0), ROW(1,1,1,1), ROW(0,0,0,0)),  // rot 3
    }
  },
  
  // PIECE_O - Yellow (no rotation change)
  // All rotations:
  //  ....
  //  .XX.
  //  .XX.
  //  ....
  [PIECE_O] = {
    .rotations = {
      PIECE(ROW(0,0,0,0), ROW(0,1,1,0), ROW(0,1,1,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,0,0,0), ROW(0,1,1,0), ROW(0,1,1,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,1,1,0), ROW(0,1,1,0), ROW(0,0,0,0)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,1,1,0), ROW(0,1,1,0), ROW(0,0,0,0)),  // rot 3
    }
  },
  
  // PIECE_T - Purple/Magenta
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  .X..           ..X.           ....           ....
  //  XXX.           ..XX           ....           .X..
  //  ....           ..X.           .XXX           XX..
  //  ....           ....           ..X.           .X..
  [PIECE_T] = {
    .rotations = {
      PIECE(ROW(0,1,0,0), ROW(1,1,1,0), ROW(0,0,0,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,0,1,0), ROW(0,0,1,1), ROW(0,0,1,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,0,0,0), ROW(0,1,1,1), ROW(0,0,1,0)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,1,0,0), ROW(1,1,0,0), ROW(0,1,0,0)),  // rot 3
    }
  },
  
  // PIECE_S - Green
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  X...           ..XX           ....           ....
  //  XX..           .XX.           ..X.           ....
  //  .X..           ....           ..XX           .XX.
  //  ....           ....           ...X           XX..
  [PIECE_S] = {
    .rotations = {
      PIECE(ROW(1,0,0,0), ROW(1,1,0,0), ROW(0,1,0,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,0,1,1), ROW(0,1,1,0), ROW(0,0,0,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,0,1,0), ROW(0,0,1,1), ROW(0,0,0,1)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,0,0,0), ROW(0,1,1,0), ROW(1,1,0,0)),  // rot 3
    }
  },
  
  // PIECE_Z - Red
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  .X..           .XX.           ....           ....
  //  XX..           ..XX           ...X           ....
  //  X...           ....           ..XX           XX..
  //  ....           ....           ..X.           .XX.
  [PIECE_Z] = {
    .rotations = {
      PIECE(ROW(0,1,0,0), ROW(1,1,0,0), ROW(1,0,0,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,1,1,0), ROW(0,0,1,1), ROW(0,0,0,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,0,0,1), ROW(0,0,1,1), ROW(0,0,1,0)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,0,0,0), ROW(1,1,0,0), ROW(0,1,1,0)),  // rot 3
    }
  },
  
  // PIECE_J - Blue
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  .XX.           ....           ....           ....
  //  .X..           .XXX           ..X.           X...
  //  .X..           ...X           ..X.           XXX.
  //  ....           ....           .XX.           ....
  [PIECE_J] = {
    .rotations = {
      PIECE(ROW(0,1,1,0), ROW(0,1,0,0), ROW(0,1,0,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,0,0,0), ROW(0,1,1,1), ROW(0,0,0,1), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,0,1,0), ROW(0,0,1,0), ROW(0,1,1,0)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(1,0,0,0), ROW(1,1,1,0), ROW(0,0,0,0)),  // rot 3
    }
  },
  
  // PIECE_L - Orange
  // Rotation 0:    Rotation 1:    Rotation 2:    Rotation 3:
  //  XX..           ...X           ....           ....
  //  .X..           .XXX           ..X.           ....
  //  .X..           ....           ..X.           XXX.
  //  ....           ....           ..XX           X...
  [PIECE_L] = {
    .rotations = {
      PIECE(ROW(1,1,0,0), ROW(0,1,0,0), ROW(0,1,0,0), ROW(0,0,0,0)),  // rot 0
      PIECE(ROW(0,0,0,1), ROW(0,1,1,1), ROW(0,0,0,0), ROW(0,0,0,0)),  // rot 1
      PIECE(ROW(0,0,0,0), ROW(0,0,1,0), ROW(0,0,1,0), ROW(0,0,1,1)),  // rot 2
      PIECE(ROW(0,0,0,0), ROW(0,0,0,0), ROW(1,1,1,0), ROW(1,0,0,0)),  // rot 3
    }
  },
};

// Get piece definition by kind
static inline const PieceDef* Piece__get_def(PieceKind kind) {
  return &PIECES[kind];
}

// Check if cell is filled in piece rotation
// Returns true if the cell at (row, col) is filled for the given piece and rotation
static inline bool Piece__get_cell(const PieceDef* def, u8 rotation, u8 row, u8 col) {
  if (row >= PIECE_SIZE || col >= PIECE_SIZE) {
    return false;
  }
  u16 mask = def->rotations[rotation];
  // Calculate bit position: row * 4 + (3 - col) since col 0 is at bit 3
  u8 bit_pos = (3 - row) * 4 + (3 - col);
  return (mask >> bit_pos) & 1;
}

// Get ANSI color code for piece kind
static inline const char* Piece__get_color(PieceKind kind) {
  static const char* colors[NUM_PIECES] = {
    ANSI_CYAN,     // PIECE_I
    ANSI_YELLOW,   // PIECE_O
    ANSI_MAGENTA,  // PIECE_T
    ANSI_GREEN,    // PIECE_S
    ANSI_RED,      // PIECE_Z
    ANSI_BLUE,     // PIECE_J
    ANSI_ORANGE,   // PIECE_L
  };
  return colors[kind];
}
