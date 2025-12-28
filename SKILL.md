# Tetris CLI - AI Agent Skill Guide

This document describes how an AI agent (LLM) can play the Tetris CLI game.

## Overview

Tetris is a turn-based CLI game where you control falling tetrominoes (pieces) on a 10x20 grid. The goal is to complete horizontal lines by filling them with blocks. Completed lines are cleared and award points. The game ends when pieces stack to the top of the board.

## Commands

All commands are executed via the `tetris` CLI tool:

| Command | Description |
|---------|-------------|
| `./tetris show` | Display current game state |
| `./tetris next` | Advance one frame (gravity moves piece down) |
| `./tetris press <key>` | Perform an action |
| `./tetris press <key1> <key2> ...` | Perform multiple actions in sequence |

**Important:** The `press` and `next` commands produce no output. You must call `show` to see the current game state.

### Keys for `press` command

| Key | Alternate | Action |
|-----|-----------|--------|
| `a` | `left` | Move piece left by 1 cell |
| `d` | `right` | Move piece right by 1 cell |
| `w` | `up` | Rotate piece 90° clockwise |
| `s` | `down` | Soft drop (move piece down 1 row) |
| `space` | `sp`, `drop` | Hard drop (instantly drop piece to bottom) |
| `q` | `reset` | Start a new game |

### Multi-Key Input

You can execute multiple keys in a single command, which is more efficient:

```bash
# Move left 4 times, rotate, then hard drop
./tetris press a a a a w space

# Soft drop 3 times then move right
./tetris press s s s d
```

This is equivalent to running separate commands but faster and atomic.

## Understanding the Display

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

### Grid Elements

- ` .` (space + dot) = Empty cell
- `██` (two block characters) = Filled cell (piece block)
- The main board is 10 columns × 20 rows
- Column 0 is leftmost, column 9 is rightmost
- Row 0 is top, row 19 is bottom

### Side Panels

- **Stats**: Current score, level, and lines progress (X/10 toward next level)
- **Next**: Preview of the next piece that will spawn
- **Help**: Shows current message and controls

### Messages

The Help panel displays status messages:
- `Start!` - Game just started (first frame only)
- `Game Over!` - Game has ended (pieces reached top)
- `Level Up!` - Level increased
- Empty when no message

## Piece Types (Tetrominoes)

There are 7 piece types, each with a distinct color and shape:

| Piece | Color | Shape Description |
|-------|-------|-------------------|
| I | Cyan | 4 blocks in a line |
| O | Yellow | 2×2 square |
| T | Magenta | T-shape |
| S | Green | S-shape (zigzag) |
| Z | Red | Z-shape (reverse zigzag) |
| J | Blue | J-shape |
| L | Orange | L-shape |

Each piece can be rotated 4 times (0°, 90°, 180°, 270°).

### 7-Bag Randomizer

The game uses a 7-bag randomizer: all 7 pieces appear exactly once before any piece repeats. This prevents long droughts of needed pieces and makes the game more predictable for strategic planning.

## Game Mechanics

### Spawning
- New pieces spawn at the top-center of the board
- If a piece cannot spawn (blocked), the game ends

### Movement
- `left`/`right`: Moves piece horizontally if not blocked
- Pieces cannot move through walls or other blocks

### Rotation
- Rotates piece 90° clockwise
- If rotation would cause collision, wall kicks are attempted:
  1. Try rotating in place
  2. Try shifting left 1 cell
  3. Try shifting right 1 cell
  4. Try shifting up 1 cell
- If all fail, rotation is rejected

### Gravity
- Each `next` command moves the piece down by 1 row
- If the piece cannot move down, it locks in place
- After locking, completed lines are cleared and a new piece spawns

### Soft Drop
- The `s`/`down` command moves the piece down by 1 row
- Does NOT lock the piece if it reaches bottom (use for precise positioning)

### Hard Drop
- The `space`/`drop` command instantly drops the piece to the lowest valid position
- The piece locks immediately after a hard drop

### Line Clearing
- When a horizontal row is completely filled, it is cleared
- All rows above shift down
- Multiple lines can clear at once

### Scoring

| Lines Cleared | Points (× Level) |
|---------------|------------------|
| 1 (Single) | 100 |
| 2 (Double) | 300 |
| 3 (Triple) | 500 |
| 4 (Tetris) | 800 |

### Leveling
- Every 10 lines cleared increases the level by 1
- Higher levels award more points per line clear
- The `Lines X/10` display shows progress toward next level

## Example Play Session

```bash
# Start a new game
./tetris press q

# See the initial state (shows "Start!" message)
./tetris show

# Move piece left, rotate, soft drop to position
./tetris press a
./tetris press w
./tetris press s
./tetris press s

# Hard drop to lock the piece
./tetris press space

# See the result
./tetris show

# Continue playing...
```

## Efficient Play (Preserving Context)

Since `press` and `next` don't output anything, you can chain multiple moves and only call `show` when needed:

```bash
# Chain multiple moves silently
./tetris press a && ./tetris press a && ./tetris press w && ./tetris press space

# Then view the result
./tetris show
```

This preserves LLM context by reducing output verbosity.

## Game State Persistence

- Game state is saved to `~/.tetris_state` after each action
- State persists between CLI invocations
- Use `./tetris press q` to reset to a new game

## Parsing the Board State

To programmatically parse the board:

1. Each row starts with `│ ` and ends with ` │`
2. Each cell is 2 characters wide (` .` or `██`)
3. The board has exactly 10 columns and 20 rows
4. Coordinates: `board[row][col]` where row 0 = top, col 0 = left

## Determining Piece Position

The current falling piece is rendered on the board. To find it:
1. Look for colored `██` blocks that form one of the 7 piece shapes
2. The piece spawns at approximately column 3-4 (center)
3. Track movement based on commands issued

## Winning Strategy

There is no "win" condition - the goal is to maximize score. The game ends when:
- A new piece cannot spawn (board too full)
- The `Game Over!` message appears in the Help panel

To achieve high scores:
- Clear as many lines as possible
- Reach higher levels for score multipliers
