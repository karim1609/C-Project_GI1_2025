#ifndef ATTENDANCE_H
#define ATTENDANCE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// Attendance record structure
typedef struct {
    int id;
    int student_id;
    int course_id;
    time_t date;
    int status;  // 0=absent, 1=present, 2=late, 3=excused
    char reason[200];
    int teacher_id;
    time_t recorded_time;
} AttendanceRecord;

// Attendance list structure
typedef struct {
    AttendanceRecord* records;
    int count;
    int capacity;
} AttendanceList;

// Attendance management functions
AttendanceList* attendance_list_create(void);
void attendance_list_destroy(AttendanceList* list);
int attendance_list_add(AttendanceList* list, AttendanceRecord record);
int attendance_list_remove(AttendanceList* list, int record_id);
AttendanceRecord* attendance_list_find_by_id(AttendanceList* list, int record_id);
AttendanceRecord* attendance_list_find_by_student_date(AttendanceList* list, int student_id, time_t date);
AttendanceRecord* attendance_list_find_by_course_date(AttendanceList* list, int course_id, time_t date);

// Attendance operations
int mark_attendance(AttendanceList* list, int student_id, int course_id, time_t date, int status, int teacher_id);
int update_attendance(AttendanceList* list, int record_id, int new_status, const char* reason);
int get_attendance_for_date(AttendanceList* list, int course_id, time_t date, AttendanceRecord** records, int* count);

// Attendance display
void attendance_list_display_all(AttendanceList* list);
void attendance_display_record(AttendanceRecord* record);

// File operations
int attendance_list_save_to_file(AttendanceList* list, const char* filename);
int attendance_list_load_from_file(AttendanceList* list, const char* filename);

// Attendance constants
#define ATTENDANCE_ABSENT 0
#define ATTENDANCE_PRESENT 1
#define ATTENDANCE_LATE 2
#define ATTENDANCE_EXCUSED 3

#endif // ATTENDANCE_H
