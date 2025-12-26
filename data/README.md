Data files used by the Student Management System

- `attendance.txt` — attendance records
- `club_memberships.txt` — which students are in which club
- `clubs.txt` — club definitions
- `examens.txt` — exam listings
- `grades.txt` — grades for students
- `logs.txt` — application logs (debug / actions)
- `modules.txt` — academic modules
- `prof_notes.txt` — notes from professors
- `professors.txt` — professor records
- `students.txt` — student records
- `users.txt` — user accounts for the CLI/GUI

These files are read by the application. Keep them in `data/` and do not move the directory unless you update the path resolution logic in `src/utils.c`.
