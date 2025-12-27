- you are a Software Development agent.
- you will help me design a cli-based game
  - the game will be: a tetris clone, called `tetris`
  - it will be accessible to ai/agentic (LLMs) players via CLI
    - via a kind of turn-based/frame-by-frame gameplay
    - with each frame rendered rendered to stdout (as ascii/text ui)

- we will generate it as a cli tool (`tetris`)
  - it should be implemented in portable c99 code
    - read and observe my code style guide `tmp/c99-server/docs/my-code-style.md`
  - game state will be preserved to disk (in a config file) between cli invocations
  - `tetris` subcommands:
    - `press <key>`: where `key` is one of: 
      - `q` or `reset`: reset game state (ie. new game)
      - `a` or `left`: nudge block left by 1 grid unit
      - `d` or `right`: nudge block right by 1 grid unit
      - `s` or `down`: drops block (like gravity) to ground
      - `w` or `up`: rotates block 90deg right
    - `show`: prints (to stdout) current game frame (as ascii art)
    - `next`: advances simulation to next frame, and shows current frame
      - can also be used to wait/(advance frame/time without any move input)

- the ui design (stdout frames) will look like this:
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
  - where:
    - `██` is one cell (with a block piece)
    - ` .` is one grid (dot; empt space, no block piece occupying)
    - `Start!` is space where game can communicate to player, ie.
      - game started (as shown; only appears on first frame)
      - win/lose condition (remains until game is reset)
    - the game playing field is the standard 10x20 2D matrix
    - valid shapes are (each shape, and its 4 possible rotations):
      - "I" Piece
        ```
         .██ . .   . . . .   . .██ .   . . . .
         .██ . .  ████████   . .██ .   . . . .
         .██ . .   . . . .   . .██ .  ████████
         .██ . .   . . . .   . .██ .   . . . .
        ```
      - "O" Piece
        ```
         . . . .   . . . .   . . . .   . . . .
         .████ .   .████ .   .████ .   .████ .
         .████ .   .████ .   .████ .   .████ .
         . . . .   . . . .   . . . .   . . . .
        ```
      - "L" Piece
        ```
        ████ . .   . . .██   . . . .   . . . .
         .██ . .   .██████   . .██ .   . . . .
         .██ . .   . . . .   . .██ .  ██████ .
         . . . .   . . . .   . .████  ██ . . .
        ```
      - "J" Piece
        ```
         .████ .   . . . .   . . . .   . . . .
         .██ . .   .██████   . .██ .  ██ . . .
         .██ . .   . . .██   . .██ .  ██████ .
         . . . .   . . . .   .████ .   . . . .
        ```
      - "T" Piece
        ```
         .██ . .   . .██ .   . . . .   . . . .
        ██████ .   . .████   . . . .   .██ . .
         . . . .   . .██ .   .██████  ████ . .
         . . . .   . . . .   . .██ .   .██ . .
        ```
      - "Z" Piece
        ```
         .██ . .   .████ .   . . . .   . . . .
        ████ . .   . .████   . . .██   . . . .
        ██ . . .   . . . .   . .████  ████ . .
         . . . .   . . . .   . .██ .   .████ .
        ```
      - "S" Piece
        ```
        ██ . . .   . .████   . . . .   . . . .
        ████ . .   .████ .   . .██ .   . . . .
         .██ . .   . . . .   . .████   .████ .
         . . . .   . . . .   . . .██  ████ . .
        ```
    - blocks are rendered in ANSI colors (True Color (24-bit))
      | Piece | Foreground Code | Approx. Color |
      |-------|-----------------|---------------|
      | I (Cyan)   | \033[38;2;0;255;255m   | Cyan   |
      | O (Yellow) | \033[38;2;255;255;0m  | Yellow|
      | T (Purple) | \033[38;2;255;0;255m  | Magenta      |
      | S (Green)  | \033[38;2;0;255;0m   | Green |
      | Z (Red)    | \033[38;2;255;0;0m  | Red   |
      | J (Blue)   | \033[38;2;0;0;255m   | Blue    |
      | L (Orange) | \033[38;2;255;165;0m  | Orange       |
      | Grid dots  | \033[38;2;50;50;50m  | Dark gray    |
      | Text/Border| \033[38;2;255;255;255m   | White        |

---

- you a Game Player agent.
- play this game of Tetris.
  - read `SKILL.md` to understand how the game is played
- goal: beat Level 1 (or get a high score for it)
