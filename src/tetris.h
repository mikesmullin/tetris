// Tetris CLI - Core Types and Definitions
// Portable C99 implementation for AI/LLM players
#pragma once

// ---
// Standard Library (minimal usage)

#include <stdint.h>
#include <stdbool.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// ---
// Fixed-Width Type Aliases

typedef int8_t s8;
typedef int16_t s16;
typedef int32_t s32;
typedef int64_t s64;
typedef uint8_t u8;
typedef uint16_t u16;
typedef uint32_t u32;
typedef uint64_t u64;

// ---
// Constants

#define BOARD_WIDTH (10)
#define BOARD_HEIGHT (20)
#define PIECE_SIZE (4)
#define NUM_PIECES (7)
#define NUM_ROTATIONS (4)
#define MAX_PATH (512)

// Scoring constants
#define SCORE_SINGLE (100)
#define SCORE_DOUBLE (300)
#define SCORE_TRIPLE (500)
#define SCORE_TETRIS (800)
#define LINES_PER_LEVEL (10)

// ---
// Piece Types

typedef enum {
  PIECE_I = 0,
  PIECE_O = 1,
  PIECE_T = 2,
  PIECE_S = 3,
  PIECE_Z = 4,
  PIECE_J = 5,
  PIECE_L = 6
} PieceKind;

// ---
// ANSI Color Codes (24-bit True Color)

// Foreground colors for pieces
#define ANSI_RESET "\033[0m"
#define ANSI_CYAN "\033[38;2;0;255;255m"     // I piece
#define ANSI_YELLOW "\033[38;2;255;255;0m"   // O piece
#define ANSI_MAGENTA "\033[38;2;255;0;255m"  // T piece
#define ANSI_GREEN "\033[38;2;0;255;0m"      // S piece
#define ANSI_RED "\033[38;2;255;0;0m"        // Z piece
#define ANSI_BLUE "\033[38;2;0;0;255m"       // J piece
#define ANSI_ORANGE "\033[38;2;255;165;0m"   // L piece
#define ANSI_GRAY "\033[38;2;50;50;50m"      // Grid dots
#define ANSI_WHITE "\033[38;2;255;255;255m"  // Text/borders

// ---
// Game Message Types

typedef enum {
  MSG_NONE = 0,
  MSG_START,
  MSG_GAME_OVER,
  MSG_LEVEL_UP
} MsgKind;

// ---
// Piece Definition (4x4 grid with 4 rotations)

typedef struct {
  // 4 rotations, each is 4x4 grid (stored as 16 bits)
  // bit order: row0[0-3], row1[0-3], row2[0-3], row3[0-3]
  u16 rotations[NUM_ROTATIONS];
} PieceDef;

// ---
// Active Piece State

typedef struct {
  PieceKind kind;
  u8 rotation;  // 0-3
  s8 x;         // position on board (can be negative during spawn)
  s8 y;         // position on board
} Piece;

// ---
// Game Board

typedef struct {
  // Each cell stores 0 (empty) or piece kind + 1 (for color)
  u8 cells[BOARD_HEIGHT][BOARD_WIDTH];
} Board;

// ---
// Game State (persisted to disk)

typedef struct {
  // Version for save file compatibility
  u32 version;
  
  // Game board
  Board board;
  
  // Current falling piece
  Piece current;
  
  // Next piece to spawn
  PieceKind next_piece;
  
  // Score and level
  u32 score;
  u32 level;
  u32 lines_cleared;
  
  // Game status
  bool game_over;
  bool game_started;
  
  // Message to display
  MsgKind message;
  
  // Random seed for deterministic piece generation
  u32 seed;
  
  // Frame counter
  u32 frame;
  
  // 7-bag randomizer state
  u8 bag[NUM_PIECES];  // Shuffled bag of pieces
  u8 bag_index;        // Next piece index in bag (0-6, 7 = need refill)
} GameState;

// ---
// Save File Version

#define SAVE_VERSION (1)

// ---
// Function Declarations (documented per module)

// See: pieces.c, game.c, render.c, state.c
