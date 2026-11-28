<div align="center">

# 🔴 Connect Four 🟡

**A local multiplayer Connect Four game built from scratch in C++ and SDL2.**

![C++](https://img.shields.io/badge/C++-d92727?style=for-the-badge&logo=cplusplus&logoColor=white)
![SDL2](https://img.shields.io/badge/SDL2-2.32-d92727?style=for-the-badge)
![Platform](https://img.shields.io/badge/Windows%20%7C%20Linux-d92727?style=for-the-badge)

</div>

---

## 📖 About

Drop your coins, outsmart your opponent, and connect four to win. This is a two-player Connect Four game for one computer, with animated coin drops, particle explosions on victory, player accounts, match history, and a glowing pixel-art look. Guest mode is there for quick casual games.

I built it as a personal project while studying BSCS at FAST, and I learned SDL2 graphics, game states, and file handling along the way.

---

## ✨ Features

| Feature | Details |
|---|---|
| 🎮 Local multiplayer | Two players take turns on the same computer |
| 🪙 Animated coin drops | Coins fall into the column with smooth motion |
| 🎆 Victory effects | Particle explosions and glow when someone connects four |
| 👤 Player accounts | Register and log in to save your results |
| 🧑‍🤝‍🧑 Guest mode | Jump into a game without an account |
| 📜 Match history | Past games with results and match duration |
| 🏆 High scores | A leaderboard of the best players |
| 🔤 Pixel-art style | Custom bitmap font and a starfield background, with no extra font library |

---

## 🕹️ How to Play

1. Start the game and choose **Log in**, **Register**, or **Guest**.
2. Players take turns dropping a coin into one of the seven columns.
3. Click a column to drop your coin.
4. The first player to connect **four coins** in a row wins. The row can be horizontal, vertical, or diagonal.
5. If the board fills up with no winner, the game is a draw.

---

## 🛠️ Build and Run

### Windows (Visual Studio)

1. Install **Visual Studio** with the *Desktop development with C++* workload.
2. Download the **SDL2 development libraries (VC)**, version 2.32.x, from the [SDL releases page](https://github.com/libsdl-org/SDL/releases).
3. Extract them to `D:\SDL2-2.32.10`. If you use another folder, change the paths in **Project → Properties → VC++ Directories** and **Linker**.
4. Open `ConnectGame.slnx` (or `ConnectGame.vcxproj`).
5. Choose **x64** and **Release**, then **Build**.
6. Copy `SDL2.dll` from the SDL2 `lib\x64` folder next to `ConnectGame.exe`.
7. Run `ConnectGame.exe`.

### Linux (Ubuntu / Kali)

```bash
sudo apt install g++ libsdl2-dev
g++ -std=c++17 ConnectGame.cpp -o connect_four $(sdl2-config --cflags --libs)
./connect_four
```

---

## 📂 Project Structure

| File | Purpose |
|---|---|
| `ConnectGame.cpp` | Program entry point |
| `game.h` | Game states, screens, rendering, and effects |
| `board.h` | Board logic and win checking |
| `user.h` | Accounts, login, and match history storage |
| `common.h` | Shared types and constants |
| `Documentation/` | Project documentation |

Most of the code is in headers. The `.cpp` files other than `ConnectGame.cpp` are empty placeholders for now.

---

## 💾 Data Files

Accounts and match history are saved in plain text files (`users.txt` and `history.txt`) next to the game. The game creates them when it needs them. They are not part of this repo.

---

## ⚠️ Known Limitations

- Passwords are stored as **plain text** in `users.txt`. This is fine for a local learning project, but it is not secure.
- The SDL2 path is fixed in the Visual Studio project, so you must change it if SDL2 is somewhere else.
- The game runs on one computer only. There is no online play.

## 🚀 Ideas for the Future

- Hash passwords before saving them
- Move the code from headers into the `.cpp` files
- Add an AI opponent
- Publish a Release build so people can play without compiling
