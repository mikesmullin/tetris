// Tetris CLI - State Persistence
// Save/load game state to disk
#pragma once

#include "tetris.h"
#include "game.c"

// @class State
// Function | Purpose
// --- | ---
// State__get_path(buf, buf_sz) | Get path to state file
// State__save(state) | Save game state to disk
// State__load(state) | Load game state from disk
// State__exists() | Check if state file exists

// Get path to state file
// Returns path in buf, returns 0 on success, -1 on failure
static s8 State__get_path(char* buf, u32 buf_sz) {
  // Check for environment variable override
  const char* env_path = getenv("TETRIS_STATE_FILE");
  if (env_path) {
    u32 len = (u32)strlen(env_path);
    if (len >= buf_sz) {
      return -1;
    }
    memcpy(buf, env_path, len + 1);
    return 0;
  }
  
  // Default: ~/.tetris_state
  const char* home = getenv("HOME");
  if (!home) {
    // Fallback to current directory
    home = ".";
  }
  
  s32 written = snprintf(buf, buf_sz, "%s/.tetris_state", home);
  if (written < 0 || (u32)written >= buf_sz) {
    return -1;
  }
  
  return 0;
}

// Check if state file exists
static bool State__exists(void) {
  char path[MAX_PATH];
  if (State__get_path(path, MAX_PATH) != 0) {
    return false;
  }
  
  FILE* f = fopen(path, "rb");
  if (f) {
    fclose(f);
    return true;
  }
  return false;
}

// Save game state to disk
// Returns 0 on success, -1 on failure
static s8 State__save(const GameState* state) {
  char path[MAX_PATH];
  if (State__get_path(path, MAX_PATH) != 0) {
    fprintf(stderr, "Error: Could not determine state file path\n");
    return -1;
  }
  
  FILE* f = fopen(path, "wb");
  if (!f) {
    fprintf(stderr, "Error: Could not open state file for writing: %s\n", path);
    return -1;
  }
  
  // Write magic bytes for verification
  const char magic[] = "TRS1";
  if (fwrite(magic, 1, 4, f) != 4) {
    fclose(f);
    return -1;
  }
  
  // Write state
  if (fwrite(state, sizeof(GameState), 1, f) != 1) {
    fclose(f);
    return -1;
  }
  
  fclose(f);
  return 0;
}

// Load game state from disk
// Returns 0 on success, -1 on failure (state will be initialized if file doesn't exist)
static s8 State__load(GameState* state) {
  char path[MAX_PATH];
  if (State__get_path(path, MAX_PATH) != 0) {
    // Initialize new game if path fails
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  FILE* f = fopen(path, "rb");
  if (!f) {
    // No state file, initialize new game
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  // Verify magic bytes
  char magic[4];
  if (fread(magic, 1, 4, f) != 4) {
    fclose(f);
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  if (magic[0] != 'T' || magic[1] != 'R' || magic[2] != 'S' || magic[3] != '1') {
    fprintf(stderr, "Warning: Invalid state file format, starting new game\n");
    fclose(f);
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  // Read state
  if (fread(state, sizeof(GameState), 1, f) != 1) {
    fprintf(stderr, "Warning: Could not read state file, starting new game\n");
    fclose(f);
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  fclose(f);
  
  // Verify version
  if (state->version != SAVE_VERSION) {
    fprintf(stderr, "Warning: State file version mismatch, starting new game\n");
    Game__init(state);
    Game__spawn_piece(state);
    return 0;
  }
  
  return 0;
}

// Delete state file (for reset)
static s8 State__delete(void) {
  char path[MAX_PATH];
  if (State__get_path(path, MAX_PATH) != 0) {
    return -1;
  }
  
  // Use remove() to delete the file
  if (remove(path) != 0) {
    // File might not exist, which is fine
    return 0;
  }
  
  return 0;
}
