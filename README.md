# Tetris

A CLI-based Tetris clone designed for AI/LLM players, implemented in portable C99.

## Motivation

Demonstrates long-range planning, and long-running agentic workflows.  
_How many levels can your agent play coherently, without losing context?_

## Features

- Turn-based/frame-by-frame gameplay via CLI
- ASCII art rendering with ANSI true color (24-bit)
- Game state persistence between CLI invocations
- Standard 10x20 playing field
- All 7 standard Tetris pieces with 4 rotations each
- 7-bag randomizer (all 7 pieces appear before any repeat)

## Building

```bash
# Build with clang (Linux/macOS)
make

# Or directly with clang
clang -std=c99 -O2 -o tetris src/main.c -Wall -Wextra -pedantic
```

## Usage

```bash
# Show current game state
./tetris show

# Advance to next frame (gravity tick)
./tetris next

# Press a key (movement/action)
./tetris press <key>

# Press multiple keys in sequence
./tetris press <key1> <key2> <key3> ...
```

**Note:** The `press` and `next` commands do not output to stdout. Use `show` to display the current game state.

### Keys

| Key | Alt | Action |
|-----|-----|--------|
| `a` | `left` | Move piece left |
| `d` | `right` | Move piece right |
| `w` | `up` | Rotate piece clockwise |
| `s` | `down` | Soft drop (move down 1 row) |
| `space` | `sp`, `drop` | Hard drop (instant drop to bottom) |
| `q` | `reset` | Start new game |

### Multi-Key Input

You can press multiple keys in a single command:

```bash
# Move left twice, rotate, then hard drop
./tetris press a a w space

# Soft drop 3 times then move right
./tetris press s s s d
```

## Example Output

```
┌─────────────────────┐ ┌──Stats───────┐
│ . . . .██ . . . . . │ │ Score      0 │
│ . . .████ . . . . . │ │ Level      1 │
│ . . .██ . . . . . . │ │ Lines   0/10 │
│ . . . . . . . . . . │ └──────────────┘
│ . . . . . . . . . . │ ┌──Next────────┐
│ . . . . . . . . . . │ │   .██ . .    │
│ . . . . . . . . . . │ │   .██ . .    │
│ . . . . . . . . . . │ │   .██ . .    │
│ . . . . . . . . . . │ │   .██ . .    │
│ . . . . . . . . . . │ └──────────────┘
│ . . . . . . . . . . │ ┌──Help────────┐
│ . . . . . . . . . . │ │              │
│ . . . . . . . . . . │ │              │
│ . . . . . . . . . . │ │ Left       a │
│ . . . . . . . . . . │ │ Right      d │
│ . . . . . . . . . . │ │ Rotate     w │
│ . . . . . . . . . . │ │ Down       s │
│ . . . .██ . . . . . │ │ Drop      sp │
│ .██ . .██ . . . . . │ │ Reset      q │
│██████ .████ . . . . │ └──────────────┘
└─────────────────────┘ 
```

## Tick Speed (for real-time implementations)

The CLI is turn-based, but for real-time implementations, here are the recommended tick intervals per level:

| Level | Tick Interval (ms) |
|-------|-------------------|
| 1 | 1000 |
| 2 | 793 |
| 3 | 618 |
| 4 | 473 |
| 5 | 355 |
| 6 | 262 |
| 7 | 190 |
| 8 | 135 |
| 9 | 94 |
| 10+ | 64 |

## Game State File

Game state is saved to `~/.tetris_state` (or `TETRIS_STATE_FILE` environment variable).

## License

MIT
