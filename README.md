# Student Management System (C)

Repository: https://github.com/karim1609/C-Project_GI1_2025.git

A small C-based application for managing students, courses, clubs and professors.

## ⚙️ Build & Run (Quick)

### Linux / macOS (recommended)
- Requirements: `gcc`, `pkg-config`, **GTK+ 3 dev** (for GUI parts) and standard build tools.
- Debian/Ubuntu example:

```bash
sudo apt update
sudo apt install build-essential pkg-config libgtk-3-dev
```

- Build & run using the included script:

```bash
./build.sh
```

This script compiles to `/tmp/student_mgmt.exe` and runs it from the project directory so that the `data/` paths resolve correctly.

### Windows (MSYS2 / Git Bash)
- Install MSYS2 and the Mingw toolchain, then install GTK packages (example):

```
pacman -Syu
pacman -S mingw-w64-x86_64-gcc mingw-w64-x86_64-gtk3 mingw-w64-x86_64-pkg-config
```

- Use the provided wrapper to invoke the build script from Windows:

```bat
compile.bat
```

This runs `build.sh` inside MSYS2 so the same build steps are used.

## ✅ Tests

- `test_paths.sh` builds and runs a small test program that verifies data file paths and file manager initialization:

```bash
./test_paths.sh
```

> Use a bash environment (Linux, macOS, WSL or MSYS2) to run the `.sh` scripts.

## 📁 Project structure

- `main.c` — application entry point
- `src/` — implementation files (C source)
- `include/` — public headers for modules
- `data/` — runtime data files consumed by the program (students, grades, etc.)
- `build.sh` — POSIX build script (compiles and runs executable)
- `compile.bat` — Windows wrapper to call `build.sh` via MSYS2
- `test_*.c`, `test_*.sh` — small test programs and runners
- helper generators: `generate_clubs.c`, `generate_hashes.c`, etc.

## 🔧 Data files (high level)
- `data/students.txt` — student records
- `data/professors.txt` — professors
- `data/clubs.txt`, `data/club_memberships.txt` — club data
- `data/grades.txt`, `data/examens.txt` — grading/exam records
- `data/prof_notes.txt` — professor notes
- `data/logs.txt` — runtime logs
- See `data/README.md` for more details.

## 💡 Notes & Tips

- The build scripts assume a POSIX-like shell for compilation and use `pkg-config` to obtain GTK compile/link flags.
- If you prefer to compile manually, an example (Linux) compile command is:

```bash
gcc -o /tmp/student_mgmt.exe main.c src/*.c -Iinclude $(pkg-config --cflags --libs gtk+-3.0) -lm -Wall
```

- Keep your working directory at the project root when running the executable or tests so `data/` relative paths work properly.

---

If you want, I can also add usage examples, a CONTRIBUTING section, or CI instructions. ✅
