# Shape Wars 2D Game

**Shape Wars 2D** is a fast-paced action game where players control randomly generated shapes and battle against various enemy shapes. The goal is to survive as long as possible, earn points, and aim for the highest score. The game uses **SFML** for graphics rendering and input processing.

**Video Demo**: [Watch Demo Video](https://www.youtube.com/watch?v=sr0TglMLr-w)

## Features

- Random Shape Generation: Player shapes are randomly generated, and enemies appear at random positions.
- Shooting Mechanism: Players can shoot bullets to defeat enemies. Normal shots have a cooldown, and a powerful super move can also be used to shoot bullets in all directions.
- Survival-Style Gameplay: Score increases by defeating enemies and surviving. The longer you survive, the higher your score.
- Enemy AI: Enemies appear in waves and move toward the player while rotating.
- HUD Display: Real-time information such as score, remaining lives, and super move availability.
- Super Move: A special ability that becomes available periodically, shooting bullets in all directions to clear enemies.

## Technologies Used

- SFML: Used for graphics rendering, window event handling, and input management.
- C++: Main language for implementing game logic and features.

## Gameplay

- Player Controls: Move the shape with arrow keys and shoot bullets with the space key.
- Enemy Appearance: Enemies of different shapes appear randomly. Players need to avoid collisions while defeating enemies.
- Lives: Players start with 3 lives. One life is lost when colliding with an enemy.
- Super Move: Becomes available after a cooldown, firing bullets in all directions to clear surrounding enemies.

## Installation and Build Instructions

### Prerequisites

SFML must be installed and properly linked to the project.

For macOS, you can install it with Homebrew:

### Clone the Repository

```
git clone https://github.com/ludovicocuoghi/CPP_2D_Shooting_Shapes.git
cd CPP_2D_Shooting_Shapes
```

### Build the Project

```
make
./Shape_Wars_2d_project
```

## Running the Game

```
.bin/sfml_app
```

## Game Controls

| Control Key | Action |
|-------------|--------|
| Arrow Keys | Move the player shape |
| Space Key | Fire bullets |
| Super Move Key (when available) | Fire bullets in all directions |

## Game States

- Playing: Game is in progress, player controls the shape and battles enemies.
- Game Over: Occurs when all lives are lost, displaying final score and best scores for each shape.

## Contributing

Feel free to fork the repository and contribute with improvements or new features.  
When contributing, please open an Issue or submit a Pull Request.