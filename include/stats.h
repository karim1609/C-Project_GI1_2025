#ifndef STATS_H
#define STATS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <math.h>
#include "config.h"
#include "student.h"
#include "grade.h"

// Type aliases are defined in grade.h which is already included
#include "attendance.h"
#include "club.h"

// General statistics structure
typedef struct {
    int total_students;
    int active_students;
    int inactive_students;
    int total_courses;
    int total_grades;
    int total_attendance_records;
    int total_clubs;
    int total_memberships;
    time_t last_updated;
} SystemStats;

// Student statistics structure
typedef struct {
    int total_students;
    int students_by_year[5];  // Index 0 unused, 1-4 for years
    int students_by_course[20];  // Top 20 courses
    float average_age;
    int age_distribution[10];  // Age ranges
    float average_gpa;
    int gpa_distribution[5];  // 0-1, 1-2, 2-3, 3-4, 4+
    Student top_performers[10];  // Top 10 students by GPA
    Student struggling_students[10];  // Bottom 10 students by GPA
} StudentStats;

// Grade statistics structure
typedef struct {
    int total_grades;
    int grades_by_level[5];  // A, B, C, D, F counts
    int passing_grades;
    int failing_grades;
    float pass_rate;
    float average_gpa;
    float highest_gpa;
    float lowest_gpa;
    int courses_with_grades;
    float course_averages[20];  // Top 20 courses
} GradeStats;

// Attendance statistics structure
typedef struct {
    int total_records;
    int present_count;
    int absent_count;
    int late_count;
    int excused_count;
    float overall_attendance_rate;
    float average_daily_attendance;
    int students_with_perfect_attendance;
    int students_with_poor_attendance;
    float attendance_by_month[12];
} AttendanceStats;

// Club statistics structure
typedef struct {
    int total_clubs;
    int active_clubs;
    int total_memberships;
    int active_memberships;
    float average_members_per_club;
    int most_popular_club_id;
    int least_popular_club_id;
    int clubs_by_category[10];
    int students_in_multiple_clubs;
} ClubStats;

// Statistics management functions
SystemStats* calculate_system_stats(StudentList* students, CourseList* courses, 
                                   GradeList* grades, AttendanceList* attendance, 
                                   ClubList* clubs, MembershipList* memberships);
void display_system_stats(SystemStats* stats);
void free_system_stats(SystemStats* stats);

StudentStats* calculate_student_stats(StudentList* students, GradeList* grades);
void display_student_stats(StudentStats* stats);
void free_student_stats(StudentStats* stats);

GradeStats* calculate_grade_stats(GradeList* grades, CourseList* courses);
void display_grade_stats(GradeStats* stats);
void free_grade_stats(GradeStats* stats);

AttendanceStats* calculate_attendance_stats(AttendanceList* attendance);
void display_attendance_stats(AttendanceStats* stats);
void free_attendance_stats(AttendanceStats* stats);

ClubStats* calculate_club_stats(ClubList* clubs, MembershipList* memberships);
void display_club_stats(ClubStats* stats);
void free_club_stats(ClubStats* stats);

#endif // STATS_H
