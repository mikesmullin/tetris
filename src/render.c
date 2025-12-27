// Tetris CLI - Rendering
// ASCII art rendering with ANSI True Color support
#pragma once

#include "tetris.h"
#include "pieces.c"

// @class Render
// Function | Purpose
// --- | ---
// Render__frame(state) | Render complete game frame to stdout
// Render__board(state) | Render game board with current piece
// Render__stats(state) | Render stats panel (score, level)
// Render__next(state) | Render next piece preview
// Render__help(state) | Render help panel with controls

// Character constants
#define CELL_FILLED "\u2588\u2588"  // ██ (full block x2)
#define CELL_EMPTY " ."             // space + dot
#define BOX_TL "\u250c"            // ┌
#define BOX_TR "\u2510"            // ┐
#define BOX_BL "\u2514"            // └
#define BOX_BR "\u2518"            // ┘
#define BOX_H "\u2500"             // ─
#define BOX_V "\u2502"             // │

// Panel widths
#define BOARD_DISPLAY_WIDTH (22)  // "│" + 10 cells * 2 chars + "│"
#define STATS_WIDTH (12)          // "┌──Stats───┐"
#define NEXT_WIDTH (12)           // "┌──Next────┐"
#define HELP_WIDTH (12)           // "┌──Help────┐"

// Check if cell is part of current piece
static bool Render__is_current_piece(const GameState* state, s8 board_x, s8 board_y) {
  const PieceDef* def = Piece__get_def(state->current.kind);
  
  s8 piece_col = board_x - state->current.x;
  s8 piece_row = board_y - state->current.y;
  
  if (piece_col < 0 || piece_col >= PIECE_SIZE || piece_row < 0 || piece_row >= PIECE_SIZE) {
    return false;
  }
  
  return Piece__get_cell(def, state->current.rotation, (u8)piece_row, (u8)piece_col);
}

// Get color for board cell (returns ANSI code or NULL for empty)
static const char* Render__get_cell_color(const GameState* state, s8 x, s8 y) {
  // Check if current piece occupies this cell
  if (Render__is_current_piece(state, x, y)) {
    return Piece__get_color(state->current.kind);
  }
  
  // Check board
  if (y >= 0 && y < BOARD_HEIGHT && x >= 0 && x < BOARD_WIDTH) {
    u8 cell = state->board.cells[y][x];
    if (cell > 0) {
      return Piece__get_color((PieceKind)(cell - 1));
    }
  }
  
  return NULL;  // Empty cell
}

// Render the next piece preview (4 lines)
static void Render__next_piece(const GameState* state, u8 preview_row) {
  const PieceDef* def = Piece__get_def(state->next_piece);
  const char* color = Piece__get_color(state->next_piece);
  
  printf("%s\u2502  %s", ANSI_WHITE, ANSI_RESET);  // │ + 2 leading spaces
  
  for (u8 col = 0; col < PIECE_SIZE; col++) {
    if (Piece__get_cell(def, 0, preview_row, col)) {
      printf("%s%s%s", color, CELL_FILLED, ANSI_RESET);
    } else {
      printf("%s%s%s", ANSI_GRAY, CELL_EMPTY, ANSI_RESET);
    }
  }
  
  printf("%s    \u2502%s", ANSI_WHITE, ANSI_RESET);  // 4 spaces + │
}

// Get message string for current game state (12 chars to fit panel)
static const char* Render__get_message(const GameState* state) {
  switch (state->message) {
    case MSG_START:     return "Start!      ";
    case MSG_GAME_OVER: return "Game Over!  ";
    case MSG_LEVEL_UP:  return "Level Up!   ";
    default:            return "            ";
  }
}

// Render complete game frame to stdout
static void Render__frame(const GameState* state) {
  // Panel inner width: 14 chars
  // Box width: 16 chars (│ + 14 inner + │)
  
  // --- Board top border + Stats header ---
  printf("%s" BOX_TL, ANSI_WHITE);
  for (u32 i = 0; i < BOARD_WIDTH * 2 + 1; i++) {
    printf(BOX_H);
  }
  printf(BOX_TR " " BOX_TL BOX_H BOX_H "Stats" BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_TR "%s\n", ANSI_RESET);
  
  // --- Board rows ---
  for (s8 row = 0; row < BOARD_HEIGHT; row++) {
    // Left border
    printf("%s" BOX_V "%s", ANSI_WHITE, ANSI_RESET);
    
    // Board cells
    for (s8 col = 0; col < BOARD_WIDTH; col++) {
      const char* color = Render__get_cell_color(state, col, row);
      if (color) {
        printf("%s%s%s", color, CELL_FILLED, ANSI_RESET);
      } else {
        printf("%s%s%s", ANSI_GRAY, CELL_EMPTY, ANSI_RESET);
      }
    }
    
    // Right border
    printf("%s " BOX_V " %s", ANSI_WHITE, ANSI_RESET);
    
    // Side panels based on row
    switch (row) {
      case 0:  // Score
        printf("%s" BOX_V " Score     %2u " BOX_V "%s", ANSI_WHITE, state->score, ANSI_RESET);
        break;
      case 1:  // Level
        printf("%s" BOX_V " Level     %2u " BOX_V "%s", ANSI_WHITE, state->level, ANSI_RESET);
        break;
      case 2:  // Lines
        printf("%s" BOX_V " Lines  %2u/%-2u " BOX_V "%s", ANSI_WHITE, 
               state->lines_cleared % LINES_PER_LEVEL, LINES_PER_LEVEL, ANSI_RESET);
        break;
      case 3:  // Stats bottom
        printf("%s" BOX_BL BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_BR "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 4:  // Next header
        printf("%s" BOX_TL BOX_H BOX_H "Next" BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_TR "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 5:  // Next piece row 0
      case 6:  // Next piece row 1
      case 7:  // Next piece row 2
      case 8:  // Next piece row 3
        Render__next_piece(state, (u8)(row - 5));
        break;
      case 9:  // Next bottom
        printf("%s" BOX_BL BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_BR "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 10: // Help header
        printf("%s" BOX_TL BOX_H BOX_H "Help" BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_TR "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 11: // Message
        printf("%s" BOX_V " %s " BOX_V "%s", ANSI_WHITE, Render__get_message(state), ANSI_RESET);
        break;
      case 12: // Empty row in help
        printf("%s" BOX_V "              " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 13: // Left
        printf("%s" BOX_V " Left       a " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 14: // Right
        printf("%s" BOX_V " Right      d " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 15: // Rotate
        printf("%s" BOX_V " Rotate     w " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 16: // Down (soft drop)
        printf("%s" BOX_V " Down       s " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 17: // Drop (hard drop)
        printf("%s" BOX_V " Drop      sp " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 18: // Reset
        printf("%s" BOX_V " Reset      q " BOX_V "%s", ANSI_WHITE, ANSI_RESET);
        break;
      case 19: // Help bottom
        printf("%s" BOX_BL BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_H BOX_BR "%s", ANSI_WHITE, ANSI_RESET);
        break;
      default:
        break;
    }
    
    printf("\n");
  }
  
  // --- Board bottom border ---
  printf("%s" BOX_BL, ANSI_WHITE);
  for (u32 i = 0; i < BOARD_WIDTH * 2 + 1; i++) {
    printf(BOX_H);
  }
  printf(BOX_BR " %s\n", ANSI_RESET);
}
