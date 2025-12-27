# Tetris CLI - AI Agent Skill Guide

This document describes how an AI agent (LLM) can play the Tetris CLI game.

## Overview

Tetris is a turn-based CLI game where you control falling tetrominoes (pieces) on a 10x20 grid. The goal is to complete horizontal lines by filling them with blocks. Completed lines are cleared and award points. The game ends when pieces stack to the top of the board.

## Commands

All commands are executed via the `tetris` CLI tool:

| Command | Description |
|---------|-------------|
| `./tetris show` | Display current game state (no state change) |
| `./tetris next` | Advance one frame (gravity moves piece down) |
| `./tetris press <key>` | Perform an action |

### Keys for `press` command

| Key | Alternate | Action |
|-----|-----------|--------|
| `a` | `left` | Move piece left by 1 cell |
| `d` | `right` | Move piece right by 1 cell |
| `w` | `up` | Rotate piece 90° clockwise |
| `s` | `down` | Hard drop (instantly drop piece to bottom) |
| `q` | `reset` | Start a new game |

## Understanding the Display

```
┌─────────────────────┐ ┌──Stats───┐
│ . . . . . . . . . . │ │ Score  0 │
│ . . . .██ . . . . . │ │ Level  1 │
│ . . .████ . . . . . │ └──────────┘
│ . . .██ . . . . . . │ 
│ . . . . . . . . . . │ ┌──Next────┐
│ . . . . . . . . . . │ │  . . . . │
│ . . . . . . . . . . │ │  .████ . │
│ . . . . . . . . . . │ │  .████ . │
│ . . . . . . . . . . │ │  . . . . │
│ . . . . . . . . . . │ └──────────┘
│ . . . . . . . . . . │ 
│ . . . . . . . . . . │ ┌──Help────┐
│ . . . . . . . . . . │ │ Start!   │
│ . . . . . . . . . . │ │          │
│ . . . . . . . . . . │ │ Left   a │
│ . . . . . . . . . . │ │ Right  d │
│ . . . . . . . . . . │ │ Rotate w │
│ . . . . . . . . . . │ │ Drop   s │
│ . . . . . . . . . . │ │ Reset  q │
│ . . . . . . . . . . │ └──────────┘
└─────────────────────┘ 
```

### Grid Elements

- ` .` (space + dot) = Empty cell
- `██` (two block characters) = Filled cell (piece block)
- The main board is 10 columns × 20 rows
- Column 0 is leftmost, column 9 is rightmost
- Row 0 is top, row 19 is bottom

### Side Panels

- **Stats**: Current score and level
- **Next**: Preview of the next piece that will spawn
- **Help**: Shows current message and controls

### Messages

The Help panel displays status messages:
- `Start!` - Game just started
- `GameOver` - Game has ended (pieces reached top)
- `Level Up` - Level increased
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

### Hard Drop
- The `down`/`s` command instantly drops the piece to the lowest valid position
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

## Strategy Tips for AI Agents

1. **Keep the board flat**: Avoid creating holes or tall towers
2. **Plan for the next piece**: Use the "Next" preview to plan placement
3. **Clear lines efficiently**: Aim for Tetrises (4 lines at once) for maximum points
4. **Avoid blocking columns**: Don't leave gaps that are hard to fill
5. **Use rotations wisely**: Some pieces fit better after rotation
6. **Hard drop for speed**: Use `s` when placement is certain

## Example Play Session

```bash
# Start a new game
./tetris press q

# See the initial state
./tetris show

# Move piece left
./tetris press a

# Rotate piece
./tetris press w

# Hard drop to place piece
./tetris press s

# Advance frame (see next piece fall)
./tetris next

# Continue playing...
```

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
- The `GameOver` message appears in the Help panel

To achieve high scores:
1. Survive as long as possible
2. Clear multiple lines at once (Tetrises = 4 lines)
3. Reach higher levels for score multipliers
