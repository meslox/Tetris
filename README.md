# 🎮 Terminal Tetris

A colorful **Tetris game written in C++** using **ncursesw**, designed to run directly inside a Linux terminal.

The game includes:

- 🧱 7 classic Tetris pieces: I, O, T, L, J, S, Z
- 🔄 Piece rotation
- ⬅️➡️ Horizontal movement
- ⬇️ Soft drop
- 🧹 Line clearing
- 🏆 Score and persistent high score
- ⏸️ Pause menu    
- 🎵 Background music
- 🎨 Colored terminal graphics
- ⌨️ Keyboard controls
- ✨ Unicode/block-character based graphics

---

## 📸 Gameplay

> Add a gameplay GIF here.

Replace `docs/gameplay.gif` with the path to your GIF:

```markdown
![Tetris Gameplay](docs/gameplay.gif)
```

You can also use a full-width image/GIF hosted online:

```markdown
![Tetris Gameplay](https://your-domain.com/gameplay.gif)
```

### Recommended repository structure

```text
Tetris/
├── tetris.cpp
├── letters.h
├── tetris.mp3
├── highscore.txt
├── tetris
├── README.md
└── docs/
    └── gameplay.gif
```

---

# 🛠️ Requirements

This project is currently intended for **Linux**.

### Required software

| Requirement | Purpose |
|---|---|
| C++ compiler (`g++`) | Compile the game |
| ncursesw | Terminal UI and Unicode support |
| mpv | Play background music |
| socat | Communicate with mpv for pause/resume |
| Git | Optional — clone the repository |

### C++ standard

The source uses modern C++ features, so a compiler supporting **C++17 or newer** is recommended.

---

# 🐧 Linux Installation

## Fedora

The project was developed/tested on Fedora Linux.

Install the required packages:

```bash
sudo dnf install gcc-c++ ncurses-devel mpv socat git
```

Check that they were installed:

```bash
g++ --version
mpv --version
socat -V
```

You should also have the ncurses development package installed.

---

## Ubuntu / Debian

Install the dependencies with:

```bash
sudo apt update
sudo apt install g++ libncurses-dev mpv socat git
```

Then verify:

```bash
g++ --version
mpv --version
socat -V
```

---

## Arch Linux

```bash
sudo pacman -S base-devel ncurses mpv socat git
```

---

# 📥 Download the Project

## Option 1 — Clone using Git

If the project is hosted on GitHub:

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

Enter the project directory:

```bash
cd YOUR_REPOSITORY
```

> Replace `YOUR_USERNAME/YOUR_REPOSITORY` with the actual GitHub repository.

---

## Option 2 — Download ZIP

1. Open the GitHub repository.
2. Click **Code**.
3. Select **Download ZIP**.
4. Extract the ZIP file.
5. Open a terminal in the extracted project directory.

For example:

```bash
cd ~/Downloads/Tetris-main
```

---

# 🔨 Compile the Game

From the project directory, run:

```bash
g++ -std=c++17 tetris.cpp -o tetris -lncursesw
```

If compilation succeeds, an executable named `tetris` will be created.

Check:

```bash
ls
```

You should see something similar to:

```text
letters.h
tetris.cpp
tetris.mp3
highscore.txt
tetris
README.md
```

---

# ▶️ Run the Game

Run:

```bash
./tetris
```

### Important

Run the executable **from the project directory**.

This is important because the game loads:

```text
tetris.mp3
highscore.txt
```

using relative paths.

So prefer:

```bash
cd Tetris-main
./tetris
```

instead of running the executable from another directory.

---

# 🎵 Music

The game uses **mpv** for background music.

The source starts mpv with:

```text
mpv --no-video --loop=inf
```

The music file expected by the program is:

```text
tetris.mp3
```

Therefore, make sure `tetris.mp3` is in the same directory as the executable.

### Check:

```bash
ls tetris.mp3
```

If it exists, the music system can use it.

### If you don't want music

The source contains:

```cpp
//play_music();
```

You can leave music disabled by keeping this line commented.

The game itself does not require music to play correctly, but `mpv` and `socat` are required for the current music/pause implementation.

---

# ⌨️ Controls

| Key | Action |
|---|---|
| `A` / `a` | Move left |
| `D` / `d` | Move right |
| `S` / `s` | Move down |
| `W` / `w` | Rotate |
| `←` | Move left |
| `→` | Move right |
| `↓` | Move down |
| `↑` | Rotate |
| `P` / `p` | Pause |
| `Tab` | Pause |
| `Enter` | Start / select menu option |
| `Q` / `q` | Quit |
| `Esc` | Quit |

---

# 🏆 High Score

The game stores the high score in:

```text
highscore.txt
```

The file is read when the game starts and updated when the game exits.

If `highscore.txt` does not exist, create it with:

```bash
echo 0 > highscore.txt
```

---

# 🖥️ Terminal Requirements

The game uses Unicode characters such as:

```text
██████
██
▄▖
▙▌
```

For the best appearance:

- Use a modern terminal emulator.
- Use a font with good Unicode/block-character support.
- Use UTF-8 locale.
- Use a reasonably large terminal window.

Check your locale:

```bash
locale
```

You should normally see UTF-8 in the output, for example:

```text
LANG=en_US.UTF-8
```

The program also calls:

```cpp
setlocale(LC_ALL, "");
```

to enable wide-character/Unicode handling.

---

# 🧩 Project Files

```text
Tetris/
│
├── tetris.cpp       # Main game source code
├── letters.h        # Unicode ASCII/block-art letters and numbers
├── tetris.mp3       # Background music
├── highscore.txt    # Persistent high score
├── tetris           # Compiled executable
├── README.md        # Project documentation
│
└── docs/
    └── gameplay.gif # Optional gameplay GIF for README
```

### `tetris.cpp`

Contains the main game implementation, including:

- Game state
- Tetris pieces
- Rotation
- Collision detection
- Movement
- Falling system
- Line clearing
- Score calculation
- Rendering
- Input handling
- Pause system
- Music control

### `letters.h`

Contains Unicode block-character graphics used to render:

- TETRIS logo
- Score
- Lines
- High score
- Play/Quit menu
- Numbers

---

# 🧱 Tetris Pieces

The game implements the seven standard Tetris pieces:

```text
I    O    T    L    J    S    Z
```

Each piece is represented internally using a 4×4 grid and can be rotated during gameplay.

---

# 🎯 Scoring

The game awards points when lines are cleared.

The current scoring system is implemented in `tetris.cpp` and distinguishes between normal line clears and clearing four lines at once.

---

# 🐛 Troubleshooting

## `fatal error: ncursesw/curses.h: No such file or directory`

Install the ncurses development package.

### Fedora

```bash
sudo dnf install ncurses-devel
```

### Ubuntu/Debian

```bash
sudo apt install libncurses-dev
```

### Arch

```bash
sudo pacman -S ncurses
```

Then compile again:

```bash
g++ -std=c++17 tetris.cpp -o tetris -lncursesw
```

---

## `mpv: command not found`

Install mpv.

Fedora:

```bash
sudo dnf install mpv
```

Ubuntu/Debian:

```bash
sudo apt install mpv
```

Arch:

```bash
sudo pacman -S mpv
```

---

## `socat: command not found`

Install socat.

Fedora:

```bash
sudo dnf install socat
```

Ubuntu/Debian:

```bash
sudo apt install socat
```

Arch:

```bash
sudo pacman -S socat
```

---

## Music does not play

Check that `tetris.mp3` exists:

```bash
ls -l tetris.mp3
```

Check that mpv works:

```bash
mpv tetris.mp3
```

If mpv can play the file normally, run the game again from the project directory:

```bash
./tetris
```

---

## Game looks broken or characters are not displayed correctly

Make sure your terminal is using UTF-8:

```bash
locale
```

You can also try:

```bash
export LANG=C.UTF-8
export LC_ALL=C.UTF-8
```

Then run:

```bash
./tetris
```

---

## Game is too large/small

The game calculates positions using the current terminal dimensions.

Try resizing the terminal window and restarting the game.

A larger terminal is recommended because the game contains the board, logo, score, controls and menu.

---

# 🧪 Development Build

For development with useful compiler warnings:

```bash
g++ -std=c++17 -Wall -Wextra -pedantic tetris.cpp -o tetris -lncursesw
```

For debugging with AddressSanitizer:

```bash
g++ -std=c++17 -Wall -Wextra -g -fsanitize=address tetris.cpp -o tetris -lncursesw
```

Run:

```bash
./tetris
```

---

# 🚀 Quick Start

For Fedora:

```bash
sudo dnf install gcc-c++ ncurses-devel mpv socat git
```

Clone the repository:

```bash
git clone https://github.com/YOUR_USERNAME/YOUR_REPOSITORY.git
```

Enter the directory:

```bash
cd YOUR_REPOSITORY
```

Compile:

```bash
g++ -std=c++17 tetris.cpp -o tetris -lncursesw
```

Run:

```bash
./tetris
```

That's it! 🎮

---

# 🔮 Future Improvements

Possible improvements for future versions:

- [ ] CMake build system
- [ ] Better wall-kick rotation system
- [ ] Hold-piece system
- [ ] Ghost piece
- [ ] Increasing difficulty
- [ ] Multiple game modes
- [ ] Better scoring system
- [ ] Configuration file
- [ ] Custom key bindings
- [ ] Improved audio controls
- [ ] Cross-platform support
- [ ] Windows support
- [ ] macOS support

---

# 📜 License

Add your preferred license here.

For example:

```text
MIT License
```

---

# 👨‍💻 Author

**Sreenandh PS**

GitHub: `https://github.com/meslox`

---

⭐ If you like the project, consider giving the repository a star!
