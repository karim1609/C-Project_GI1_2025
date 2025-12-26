#ifndef STUDENT_H
#define STUDENT_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// Student structure
typedef struct {
    int id;
    char first_name[MAX_NAME_LENGTH];
    char last_name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char address[MAX_ADDRESS_LENGTH];
    int age;
    char course[MAX_COURSE_LENGTH];
    int year;    float gpa;    time_t enrollment_date;
    int is_active;
} Student;

// Student list structure
typedef struct {
    Student* students;
    int count;
    int capacity;
    int is_loaded;          // Flag to track if data is loaded in memory
    char filename[256];      // Source filename for storage
    int auto_save_enabled;   // Flag for automatic saving
    time_t last_save_time;   // Timestamp of last save
} StudentList;

// Function declarations
StudentList* student_list_create(void);
void student_list_destroy(StudentList* list);
int student_list_add(StudentList* list, Student student);
int student_list_remove(StudentList* list, int student_id);
Student* student_list_find_by_id(StudentList* list, int student_id);
Student* student_list_find_by_name(StudentList* list, const char* first_name, const char* last_name);
Student* student_list_find_by_email(StudentList* list, const char* email);
void student_list_display_all(StudentList* list);
void student_list_display_student(Student* student);
int student_list_save_to_file(StudentList* list, const char* filename);
int student_list_load_from_file(StudentList* list, const char* filename);
void student_list_sort_by_name(StudentList* list);
void student_list_sort_by_id(StudentList* list);
void student_list_sort_by_gpa(StudentList* list);
int student_list_get_count(StudentList* list);
Student* student_list_get_student(StudentList* list, int index);

// File management functions for storage
int student_list_ensure_loaded(StudentList* list);
int student_list_save_and_unload(StudentList* list);
int student_list_auto_save(StudentList* list);
int student_list_is_loaded(StudentList* list);
void student_list_set_filename(StudentList* list, const char* filename);

// Student validation functions
int student_validate_email(const char* email);
int student_validate_phone(const char* phone);
int student_validate_age(int age);
int student_validate_gpa(float gpa);

// Student input functions
Student student_input_new(void);
void student_input_edit(Student* student);
void student_display_summary(StudentList* list);

#endif // STUDENT_H
