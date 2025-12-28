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
  printf("  s, down        Soft drop (move down 1 row)\n");
  printf("  space, drop    Hard drop (instant drop to bottom)\n");
  printf("  q, reset       Reset game (new game)\n");
  printf("\n");
  printf("Environment:\n");
  printf("  TETRIS_STATE_FILE  Override state file path (default: ~/.tetris_state)\n");
}

// Handle 'dump' command
static s8 Main__cmd_dump(GameState* state) {
  printf("{\n");
  printf("  \"score\": %u,\n", state->score);
  printf("  \"level\": %u,\n", state->level);
  printf("  \"lines\": %u,\n", state->lines_cleared);
  printf("  \"game_over\": %s,\n", state->game_over ? "true" : "false");
  printf("  \"current\": {\n");
  printf("    \"kind\": %d,\n", state->current.kind);
  printf("    \"x\": %d,\n", state->current.x);
  printf("    \"y\": %d,\n", state->current.y);
  printf("    \"rotation\": %d\n", state->current.rotation);
  printf("  },\n");
  printf("  \"next_piece\": %d,\n", state->next_piece);
  printf("  \"board\": [\n");
  for (int y = 0; y < BOARD_HEIGHT; y++) {
    printf("    [");
    for (int x = 0; x < BOARD_WIDTH; x++) {
      printf("%d%s", state->board.cells[y][x], x < BOARD_WIDTH - 1 ? ", " : "");
    }
    printf("]%s\n", y < BOARD_HEIGHT - 1 ? "," : "");
  }
  printf("  ]\n");
  printf("}\n");
  return 0;
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
  // Clear start message (we're past first frame)
  if (state->message == MSG_START) {
    state->message = MSG_NONE;
  }
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
  bool is_reset = false;
  
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
        Game__soft_drop(state);
        handled = true;
        break;
      case 'q':
        Game__reset(state);
        Game__spawn_piece(state);
        handled = true;
        is_reset = true;
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
      Game__soft_drop(state);
      handled = true;
    } else if (strcmp(key, "space") == 0 || strcmp(key, "sp") == 0 || strcmp(key, "drop") == 0) {
      Game__drop(state);
      handled = true;
    } else if (strcmp(key, "reset") == 0) {
      Game__reset(state);
      Game__spawn_piece(state);
      handled = true;
      is_reset = true;
    }
  }
  
  if (!handled) {
    fprintf(stderr, "Error: Unknown key '%s'\n", key);
    fprintf(stderr, "Valid keys: a/left, d/right, w/up, s/down, space/drop, q/reset\n");
    return -1;
  }
  
  // Clear start message after first input (but not on reset)
  if (state->message == MSG_START && !is_reset) {
    state->message = MSG_NONE;
  }
  
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
  } else if (strcmp(command, "dump") == 0) {
    result = Main__cmd_dump(&state);
  } else if (strcmp(command, "next") == 0) {
    result = Main__cmd_next(&state);
    // Save state after advancing frame
    if (result == 0) {
      State__save(&state);
    }
  } else if (strcmp(command, "press") == 0) {
    if (argc < 3) {
      result = Main__cmd_press(&state, NULL);
    } else {
      // Process all keys provided as arguments
      for (int i = 2; i < argc && result == 0; i++) {
        result = Main__cmd_press(&state, argv[i]);
      }
    }
    // Save state after key presses
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
