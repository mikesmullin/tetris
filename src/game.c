// Tetris CLI - Game Logic
// Core gameplay functions: collision, movement, rotation, line clearing
#pragma once

#include "tetris.h"
#include "pieces.c"

// @class Game
// Function | Purpose
// --- | ---
// Game__init(state) | Initialize game state for new game
// Game__reset(state) | Reset game to initial state (new game)
// Game__random(state) | Generate next random number using LCG
// Game__shuffle_bag(state) | Shuffle the 7-bag for piece randomization
// Game__next_piece_from_bag(state) | Get next piece from 7-bag
// Game__spawn_piece(state) | Spawn a new piece at top of board
// Game__check_collision(state, x, y, rotation) | Check if piece would collide at position
// Game__lock_piece(state) | Lock current piece into board
// Game__clear_lines(state) | Clear completed lines and update score
// Game__move_left(state) | Move piece left if possible
// Game__move_right(state) | Move piece right if possible
// Game__rotate(state) | Rotate piece clockwise if possible
// Game__soft_drop(state) | Soft drop piece down by 1 row
// Game__drop(state) | Hard drop piece to bottom
// Game__tick(state) | Apply gravity (move piece down one row)

// Linear congruential generator for deterministic random numbers
static u32 Game__random(GameState* state) {
  // LCG parameters (same as glibc)
  state->seed = state->seed * 1103515245 + 12345;
  return (state->seed >> 16) & 0x7FFF;
}

// Shuffle the 7-bag using Fisher-Yates algorithm
static void Game__shuffle_bag(GameState* state) {
  // Fill bag with all 7 pieces
  for (u8 i = 0; i < NUM_PIECES; i++) {
    state->bag[i] = i;
  }
  
  // Fisher-Yates shuffle
  for (u8 i = NUM_PIECES - 1; i > 0; i--) {
    u8 j = (u8)(Game__random(state) % (i + 1));
    u8 tmp = state->bag[i];
    state->bag[i] = state->bag[j];
    state->bag[j] = tmp;
  }
  
  state->bag_index = 0;
}

// Get next piece from 7-bag
static PieceKind Game__next_piece_from_bag(GameState* state) {
  // Refill bag if exhausted
  if (state->bag_index >= NUM_PIECES) {
    Game__shuffle_bag(state);
  }
  
  return (PieceKind)state->bag[state->bag_index++];
}

// Initialize game state for new game
static void Game__init(GameState* state) {
  // Clear all state
  memset(state, 0, sizeof(GameState));
  
  // Set version
  state->version = SAVE_VERSION;
  
  // Initialize random seed from time
  state->seed = (u32)time(NULL);
  
  // Set initial level
  state->level = 1;
  
  // Initialize 7-bag and generate first piece
  Game__shuffle_bag(state);
  state->next_piece = Game__next_piece_from_bag(state);
  
  // Mark as started
  state->game_started = true;
  state->message = MSG_START;
  
  // Initialize frame counter
  state->frame = 0;
}

// Reset game to initial state (new game)
static void Game__reset(GameState* state) {
  Game__init(state);
}

// Check if piece would collide at position
// Returns true if collision detected
static bool Game__check_collision(const GameState* state, s8 x, s8 y, u8 rotation) {
  const PieceDef* def = Piece__get_def(state->current.kind);
  
  for (u8 row = 0; row < PIECE_SIZE; row++) {
    for (u8 col = 0; col < PIECE_SIZE; col++) {
      if (!Piece__get_cell(def, rotation, row, col)) {
        continue;  // Empty cell in piece
      }
      
      s8 board_x = x + (s8)col;
      s8 board_y = y + (s8)row;
      
      // Check bounds
      if (board_x < 0 || board_x >= BOARD_WIDTH) {
        return true;  // Left/right wall collision
      }
      if (board_y >= BOARD_HEIGHT) {
        return true;  // Floor collision
      }
      // Allow above board (y < 0) for spawning
      if (board_y < 0) {
        continue;
      }
      
      // Check board collision
      if (state->board.cells[board_y][board_x] != 0) {
        return true;  // Collision with locked piece
      }
    }
  }
  
  return false;
}

// Spawn a new piece at top of board
// Returns false if spawn would cause game over
static bool Game__spawn_piece(GameState* state) {
  // Set current piece to next
  state->current.kind = state->next_piece;
  state->current.rotation = 0;
  
  // Center horizontally (I and O pieces spawn centered differently)
  if (state->current.kind == PIECE_I) {
    state->current.x = (BOARD_WIDTH - PIECE_SIZE) / 2;
    state->current.y = -1;  // Start 1 row above to show 4th row of I-piece
  } else if (state->current.kind == PIECE_O) {
    state->current.x = (BOARD_WIDTH - 2) / 2;  // O is 2 wide, center it
    state->current.y = 0;
  } else {
    state->current.x = (BOARD_WIDTH - PIECE_SIZE) / 2;
    state->current.y = 0;
  }
  
  // Generate next piece from 7-bag
  state->next_piece = Game__next_piece_from_bag(state);
  
  // Check if spawn position is valid
  if (Game__check_collision(state, state->current.x, state->current.y, state->current.rotation)) {
    state->game_over = true;
    state->message = MSG_GAME_OVER;
    return false;
  }
  
  return true;
}

// Lock current piece into board
static void Game__lock_piece(GameState* state) {
  const PieceDef* def = Piece__get_def(state->current.kind);
  
  for (u8 row = 0; row < PIECE_SIZE; row++) {
    for (u8 col = 0; col < PIECE_SIZE; col++) {
      if (!Piece__get_cell(def, state->current.rotation, row, col)) {
        continue;
      }
      
      s8 board_x = state->current.x + (s8)col;
      s8 board_y = state->current.y + (s8)row;
      
      // Skip cells above board
      if (board_y < 0 || board_x < 0 || board_x >= BOARD_WIDTH || board_y >= BOARD_HEIGHT) {
        continue;
      }
      
      // Store piece kind + 1 (so 0 remains empty)
      state->board.cells[board_y][board_x] = (u8)(state->current.kind + 1);
    }
  }
}

// Clear completed lines and update score
// Returns number of lines cleared
static u32 Game__clear_lines(GameState* state) {
  u32 lines_cleared = 0;
  
  // Check each row from bottom to top
  for (s32 row = BOARD_HEIGHT - 1; row >= 0; row--) {
    bool line_complete = true;
    
    for (u8 col = 0; col < BOARD_WIDTH; col++) {
      if (state->board.cells[row][col] == 0) {
        line_complete = false;
        break;
      }
    }
    
    if (line_complete) {
      lines_cleared++;
      
      // Move all rows above down by one
      for (s32 move_row = row; move_row > 0; move_row--) {
        for (u8 col = 0; col < BOARD_WIDTH; col++) {
          state->board.cells[move_row][col] = state->board.cells[move_row - 1][col];
        }
      }
      
      // Clear top row
      for (u8 col = 0; col < BOARD_WIDTH; col++) {
        state->board.cells[0][col] = 0;
      }
      
      // Re-check same row (since rows shifted down)
      row++;
    }
  }
  
  // Update score based on lines cleared
  if (lines_cleared > 0) {
    u32 score_add = 0;
    switch (lines_cleared) {
      case 1: score_add = SCORE_SINGLE * state->level; break;
      case 2: score_add = SCORE_DOUBLE * state->level; break;
      case 3: score_add = SCORE_TRIPLE * state->level; break;
      case 4: score_add = SCORE_TETRIS * state->level; break;
      default: score_add = SCORE_TETRIS * state->level; break;
    }
    state->score += score_add;
    state->lines_cleared += lines_cleared;
    
    // Check for level up
    u32 new_level = (state->lines_cleared / LINES_PER_LEVEL) + 1;
    if (new_level > state->level) {
      state->level = new_level;
      state->message = MSG_LEVEL_UP;
    }
  }
  
  return lines_cleared;
}

// Move piece left if possible
// Returns true if move succeeded
static bool Game__move_left(GameState* state) {
  if (state->game_over) {
    return false;
  }
  
  if (!Game__check_collision(state, state->current.x - 1, state->current.y, state->current.rotation)) {
    state->current.x--;
    return true;
  }
  return false;
}

// Move piece right if possible
// Returns true if move succeeded
static bool Game__move_right(GameState* state) {
  if (state->game_over) {
    return false;
  }
  
  if (!Game__check_collision(state, state->current.x + 1, state->current.y, state->current.rotation)) {
    state->current.x++;
    return true;
  }
  return false;
}

// Rotate piece clockwise if possible
// Returns true if rotation succeeded
static bool Game__rotate(GameState* state) {
  if (state->game_over) {
    return false;
  }
  
  u8 new_rotation = (state->current.rotation + 1) % NUM_ROTATIONS;
  
  // Try normal rotation
  if (!Game__check_collision(state, state->current.x, state->current.y, new_rotation)) {
    state->current.rotation = new_rotation;
    return true;
  }
  
  // Try wall kick left
  if (!Game__check_collision(state, state->current.x - 1, state->current.y, new_rotation)) {
    state->current.x--;
    state->current.rotation = new_rotation;
    return true;
  }
  
  // Try wall kick right
  if (!Game__check_collision(state, state->current.x + 1, state->current.y, new_rotation)) {
    state->current.x++;
    state->current.rotation = new_rotation;
    return true;
  }
  
  // Try wall kick up (for floor kicks)
  if (!Game__check_collision(state, state->current.x, state->current.y - 1, new_rotation)) {
    state->current.y--;
    state->current.rotation = new_rotation;
    return true;
  }
  
  return false;
}

// Soft drop piece by 1 row
// Returns true if piece moved, false if it would lock (doesn't lock)
static bool Game__soft_drop(GameState* state) {
  if (state->game_over) {
    return false;
  }
  
  // Check if piece can move down
  if (!Game__check_collision(state, state->current.x, state->current.y + 1, state->current.rotation)) {
    state->current.y++;
    return true;
  }
  
  return false;
}

// Hard drop piece to bottom
static void Game__drop(GameState* state) {
  if (state->game_over) {
    return;
  }
  
  // Move down until collision
  while (!Game__check_collision(state, state->current.x, state->current.y + 1, state->current.rotation)) {
    state->current.y++;
  }
  
  // Lock piece
  Game__lock_piece(state);
  
  // Clear lines
  Game__clear_lines(state);
  
  // Spawn new piece
  Game__spawn_piece(state);
}

// Apply gravity (move piece down one row)
// Returns true if piece moved, false if piece locked
static bool Game__tick(GameState* state) {
  if (state->game_over) {
    return false;
  }
  
  // Clear start message after first tick
  if (state->message == MSG_START) {
    state->message = MSG_NONE;
  }
  
  // Increment frame counter
  state->frame++;
  
  // Check if piece can move down
  if (!Game__check_collision(state, state->current.x, state->current.y + 1, state->current.rotation)) {
    state->current.y++;
    return true;
  }
  
  // Piece cannot move down, lock it
  Game__lock_piece(state);
  
  // Clear lines
  Game__clear_lines(state);
  
  // Spawn new piece
  Game__spawn_piece(state);
  
  return false;
}
