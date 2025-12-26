#ifndef CONFIG_H
#define CONFIG_H

// Application configuration constants
#define APP_NAME "Student Management System"
#define APP_VERSION "1.0.0"
#define MAX_STUDENTS 1000
#define MAX_NAME_LENGTH 100
#define MAX_EMAIL_LENGTH 150
#define MAX_PHONE_LENGTH 20
#define MAX_ADDRESS_LENGTH 200
#define MAX_COURSE_LENGTH 50
#define MAX_CLUB_LENGTH 50
#define MAX_CLUBS 15
#define MAX_DESC_LENGTH 200  // Maximum description length 
// File paths
#define DATA_DIR "c:\\Users\\Karim erradi\\Documents\\c-project1\\data\\"
#define STUDENTS_FILE "students.txt"
#define USERS_FILE "users.txt"
#define GRADES_FILE "grades.txt"
#define CLUBS_FILE "clubs.txt"
#define MEMBERSHIPS_FILE "club_memberships.txt"
#define ATTENDANCE_FILE "attendance.txt"
#define LOGS_FILE "logs.txt"
#define PROF_NOTES_FILE "prof_notes.txt"

// UI settings
#define WINDOW_WIDTH 1200
#define WINDOW_HEIGHT 800
// Note: THEME_DARK and THEME_LIGHT are defined in theme.h as enum values
// These string constants are kept for backward compatibility but should not be used with ThemeType enum
#define THEME_DARK_STR "dark"
#define THEME_LIGHT_STR "light"

// User roles
typedef enum {
    ROLE_ADMIN = 1,
    ROLE_TEACHER = 2,
    ROLE_STUDENT = 3
} UserRole;

// Grade levels
typedef enum {
    GRADE_A = 4,
    GRADE_B = 3,
    GRADE_C = 2,
    GRADE_D = 1,
    GRADE_F = 0
} GradeLevel;

#endif // CONFIG_H
