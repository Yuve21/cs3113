# CS-UY 3113: Intro to Game Programming (Fall 2026)

Yuvraj Chandyok (ysc8940@nyu.edu)

## Project 1: Simple 2D Scene

**Project folder: [`project-1-wukong/`](project-1-wukong/)**
**Source file: [`project-1-wukong/main.cpp`](project-1-wukong/main.cpp)**

A Sun Wukong scene. Wukong walks a figure-eight while two spirit orbs orbit him
relative to his position, each spinning on its own axis; the amber orb also
pulses. The background cycles between two colours (extra credit).

To build and run, from inside `project-1-wukong/`:

```
make
./raylib_app.exe
```

## Project 2: Pong Variations

**Project folder: [`project-2-pong/`](project-2-pong/)**
**Source file: [`project-2-pong/main.cpp`](project-2-pong/main.cpp)**

Wukong Pong. The Destined One's golden staff (left, W/S) against Erlang Shen's
iron staff (right, Up/Down), batting spirit orbs. First to 3 points wins, and a
banner announces the winner (extra credit).

- `T` switches Erlang between player-controlled and following the orbs on his own (1-player mode)
- `1` / `2` / `3` sets how many orbs are in play, at any time. The first press starts the duel
- `Q` quits

To build and run, from inside `project-2-pong/`:

```
make
./raylib_app.exe
```

## Project 3: Lunar Lander!

**Project folder: [`project-3-lunar-lander/`](project-3-lunar-lander/)**
**Source file: [`project-3-lunar-lander/main.cpp`](project-3-lunar-lander/main.cpp)**

Land a ship on a moving pad under low gravity. W thrusts up, A and D thrust sideways; every key only
changes the ship's acceleration, so it drifts after you let go. Each thruster burns fuel (the cells in
the top-left corner) and stops working at zero. Land softly on the green pad for Mission Accomplished;
land too hard, hit the pad from the side or below, hit a rock or leave the screen for Mission Failed. The thruster flame is animated from a
sprite sheet (extra credit). Q quits.

To build and run, from inside `project-3-lunar-lander/`:

```
make
./raylib_app.exe
```

## Project 4: Rise of the AI

**Project folder: [`project-4-rise-of-the-ai/`](project-4-rise-of-the-ai/)**
**Source file: [`project-4-rise-of-the-ai/main.cpp`](project-4-rise-of-the-ai/main.cpp)** (the scenes and classes are in
[`project-4-rise-of-the-ai/CS3113/`](project-4-rise-of-the-ai/CS3113/))

Alien Escape, a side-scrolling platformer. A menu scene, three levels that get longer and harder, and
"You Win" / "You Lose" scenes. Three lives for the whole game: touching an enemy or falling into a pit
costs one and restarts the level; reaching the flag clears it. Enemies use three kinds of AI: slimes
pace back and forth (wanderer), snails crawl after you once you are close (follower) and flies glide
after you through the air (lerper). Background music loops, with sound effects for jumping, getting
hurt, clearing a level, winning and losing.

- `ENTER` starts the game from the menu, and goes back to the menu from either ending (lives reset)
- `A` / `D` walk, `W` jumps
- `1` / `2` / `3` jump straight to level 1, 2 or 3 (debugging)
- `Q` quits

To build and run, from inside `project-4-rise-of-the-ai/`:

```
make
./raylib_app.exe
```

## Other folders

| folder | what it is |
| --- | --- |
| `01-introduction/`, `02-transformations/`, `03-textures-delta-time/`, `04-user-input-collisions/` | lecture demos |
| `exercise-01-transformations/`, `exercise-02-textures/`, `exercise-03-collisions/` | weekly exercises |
| `sandbox/`, `sine-demo/` | scratch work |
