# Tetris CLI

A CLI-based Tetris clone designed for AI/LLM players, implemented in portable C99.

## Features

- Turn-based/frame-by-frame gameplay via CLI
- ASCII art rendering with ANSI true color (24-bit)
- Game state persistence between CLI invocations
- Standard 10x20 playing field
- All 7 standard Tetris pieces with 4 rotations each

## Building

```bash
# Build with gcc (Linux/macOS)
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
```

### Keys

| Key | Alt | Action |
|-----|-----|--------|
| `a` | `left` | Move piece left |
| `d` | `right` | Move piece right |
| `w` | `up` | Rotate piece clockwise |
| `s` | `down` | Hard drop piece |
| `q` | `reset` | Start new game |

## Example Output

```
┌──────────────────────┐ ┌──Stats───┐
│  . . . . . . . . . . │ │ Score  0 │
│  . . . . . . . . . . │ │ Level  1 │
│  . . . . . . . . . . │ └──────────┘
│  . . . . . . . . . . │ 
│  . . . . . . . . . . │ ┌──Next────┐
│  . . . . . . . . . . │ │  . . . . │
│  . . . . . . . . . . │ │  .████ . │
│  . . . . . . . . . . │ │  .████ . │
│  . . . . . . . . . . │ │  . . . . │
│  . . . . . . . . . . │ └──────────┘
│  . . . . . . . . . . │ 
│  . . . . . . . . . . │ ┌──Help────┐
│  . . . . . . . . . . │ │ Start!   │
│  . . . . . . . . . . │ │          │
│  . . . . . . . . . . │ │ Left   a │
│  . . . . . . . . . . │ │ Right  d │
│  . . . . . . . . . . │ │ Rotate w │
│  . . . . . . . . . . │ │ Drop   s │
│  . . . . . . . . . . │ │ Reset  q │
│  . . . . . . . . . . │ └──────────┘
└──────────────────────┘ 
```

## Game State File

Game state is saved to `~/.tetris_state` (or `TETRIS_STATE_FILE` environment variable).

## License

MIT
