// Tetris CLI - Main Entry Point
// CLI tool for AI/LLM players to play Tetris
//
// Portable C99 implementation
// Unity build: all source files included here

#include "tetris.h"
#include "pieces.c"
#include "game.c"
#include "render.c"
#include "state.c"

// @class Main
// Function | Purpose
// --- | ---
// main(argc, argv) | Entry point, parse CLI args and dispatch commands
// Main__usage() | Print usage information
// Main__cmd_show(state) | Handle 'show' command
// Main__cmd_next(state) | Handle 'next' command  
// Main__cmd_press(state, key) | Handle 'press <key>' command

// Print usage information
static void Main__usage(void) {
  printf("Usage: tetris <command> [args]\n");
  printf("\n");
  printf("Commands:\n");
  printf("  show           Show current game state\n");
  printf("  next           Advance to next frame (gravity tick)\n");
  printf("  press <key>    Press a key to perform an action\n");
  printf("\n");
  printf("Keys:\n");
  printf("  a, left        Move piece left\n");
  printf("  d, right       Move piece right\n");
  printf("  w, up          Rotate piece clockwise\n");
  printf("  s, down        Hard drop piece\n");
  printf("  q, reset       Reset game (new game)\n");
  printf("\n");
  printf("Environment:\n");
  printf("  TETRIS_STATE_FILE  Override state file path (default: ~/.tetris_state)\n");
}

// Handle 'show' command
static s8 Main__cmd_show(GameState* state) {
  Render__frame(state);
  return 0;
}

// Handle 'next' command
static s8 Main__cmd_next(GameState* state) {
  if (!state->game_over) {
    Game__tick(state);
  }
  Render__frame(state);
  return 0;
}

// Handle 'press <key>' command
static s8 Main__cmd_press(GameState* state, const char* key) {
  if (!key || key[0] == '\0') {
    fprintf(stderr, "Error: No key specified\n");
    fprintf(stderr, "Usage: tetris press <key>\n");
    return -1;
  }
  
  // Match key
  bool handled = false;
  
  // Single character keys
  if (key[1] == '\0') {
    switch (key[0]) {
      case 'a':
        Game__move_left(state);
        handled = true;
        break;
      case 'd':
        Game__move_right(state);
        handled = true;
        break;
      case 'w':
        Game__rotate(state);
        handled = true;
        break;
      case 's':
        Game__drop(state);
        handled = true;
        break;
      case 'q':
        Game__reset(state);
        Game__spawn_piece(state);
        handled = true;
        break;
    }
  }
  
  // Word keys (strcmp)
  if (!handled) {
    if (strcmp(key, "left") == 0) {
      Game__move_left(state);
      handled = true;
    } else if (strcmp(key, "right") == 0) {
      Game__move_right(state);
      handled = true;
    } else if (strcmp(key, "up") == 0) {
      Game__rotate(state);
      handled = true;
    } else if (strcmp(key, "down") == 0) {
      Game__drop(state);
      handled = true;
    } else if (strcmp(key, "reset") == 0) {
      Game__reset(state);
      Game__spawn_piece(state);
      handled = true;
    }
  }
  
  if (!handled) {
    fprintf(stderr, "Error: Unknown key '%s'\n", key);
    fprintf(stderr, "Valid keys: a/left, d/right, w/up, s/down, q/reset\n");
    return -1;
  }
  
  Render__frame(state);
  return 0;
}

// Main entry point
int main(int argc, char* argv[]) {
  // No command provided
  if (argc < 2) {
    Main__usage();
    return 1;
  }
  
  const char* command = argv[1];
  
  // Help command
  if (strcmp(command, "help") == 0 || strcmp(command, "--help") == 0 || strcmp(command, "-h") == 0) {
    Main__usage();
    return 0;
  }
  
  // Load game state
  GameState state;
  State__load(&state);
  
  s8 result = 0;
  
  // Dispatch command
  if (strcmp(command, "show") == 0) {
    result = Main__cmd_show(&state);
  } else if (strcmp(command, "next") == 0) {
    result = Main__cmd_next(&state);
    // Save state after advancing frame
    if (result == 0) {
      State__save(&state);
    }
  } else if (strcmp(command, "press") == 0) {
    const char* key = (argc >= 3) ? argv[2] : NULL;
    result = Main__cmd_press(&state, key);
    // Save state after key press
    if (result == 0) {
      State__save(&state);
    }
  } else {
    fprintf(stderr, "Error: Unknown command '%s'\n", command);
    Main__usage();
    return 1;
  }
  
  return (result == 0) ? 0 : 1;
}
