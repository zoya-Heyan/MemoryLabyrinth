# Memory Labyrinth

A text-based adventure game built with SFML, where you navigate through a mysterious labyrinth while gradually losing your memories. Each step forward costs you a piece of your past, but the streets become increasingly familiar as you progress.

## Features

- Interactive story-driven gameplay
- Memory loss mechanics that affect gameplay
- Procedurally generated scenes and choices
- Street familiarity system
- Multiple endings based on your choices

## Requirements

- C++17 compatible compiler
- CMake 3.16 or higher
- SFML 2.6.2 or compatible version

### macOS Installation

Install SFML using Homebrew:

```bash
brew install sfml@2
```

## Building

1. Clone the repository:
```bash
git clone <repository-url>
cd MemoryLabyrinth
```

2. Create a build directory:
```bash
mkdir build
cd build
```

3. Configure and build:
```bash
cmake ..
make
```

4. Run the game:
```bash
./MemoryLabyrinth
```

## Gameplay

### Controls

- **Text Input**: Type your name when prompted
- **Enter/C**: Confirm your name and start the game
- **Number Keys (1-9)**: Select a choice from the available options
- **Q**: Quit the game
- **ESC**: Close the window

### Game Mechanics

- **Memory Points**: You start with 10 memory points. Each step forward consumes memory.
- **Choices**: Some choices may cost additional memory points.
- **Familiarity**: As you explore, the streets become more familiar, unlocking new narrative elements.
- **Memories**: Collect and lose memories throughout your journey. Each memory represents a piece of your identity.

### Objective

Navigate through the labyrinth while managing your remaining memories. The game ends when all your memories are lost, but the story continues to unfold based on your choices and familiarity with the streets.

## Project Structure

```
MemoryLabyrinth/
├── CMakeLists.txt          # CMake build configuration
├── include/                # Header files
│   ├── StoryGame.hpp      # Main game class
│   └── ...
├── src/                    # Source files
│   └── main.cpp           # Entry point
├── src_modules/           # Additional source modules
│   └── StoryGame.cpp     # Game implementation
├── assets/                # Game assets (fonts, images, etc.)
└── build/                 # Build output directory (generated)
```

## Dependencies

- **SFML 2.6.2**: Graphics, window, and system libraries
  - sfml-graphics
  - sfml-window
  - sfml-system

## Troubleshooting

### SFML Not Found

If CMake cannot find SFML, ensure it is installed and the path is correct. On macOS with Homebrew, SFML should be at `/opt/homebrew/opt/sfml@2`.

### Font Loading Issues

The game uses Roboto font from the `assets/` directory. Ensure `Roboto-SemiBold.ttf` is present in the `assets/` folder.

### Build Errors

- Ensure you have the correct C++17 compiler
- Verify CMake version is 3.16 or higher
- Check that all SFML libraries are properly installed

## License

See LICENSE file for details.

## Contributing

Contributions are welcome. Please ensure your code follows the existing style and includes appropriate comments.

## Author

Cherno

