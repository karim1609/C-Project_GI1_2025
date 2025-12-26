#ifndef PROFESSOR_H
#define PROFESSOR_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"
#include "grade.h"

// Professor structure
typedef struct {
    int id;
    char first_name[MAX_NAME_LENGTH];
    char last_name[MAX_NAME_LENGTH];
    char email[MAX_EMAIL_LENGTH];
    char phone[MAX_PHONE_LENGTH];
    char address[MAX_ADDRESS_LENGTH];
    char department[MAX_COURSE_LENGTH];
    char specialization[MAX_COURSE_LENGTH];
    int years_of_experience;
    char office_location[100];
    time_t hire_date;
    int is_active;
} Professor;

// Professor list structure
typedef struct {
    Professor* professors;
    int count;
    int capacity;
    int is_loaded;
    char filename[256];
    int auto_save_enabled;
    time_t last_save_time;
} ProfessorList;

// Function declarations
ProfessorList* professor_list_create(void);
void professor_list_destroy(ProfessorList* list);
int professor_list_add(ProfessorList* list, Professor professor);
int professor_list_remove(ProfessorList* list, int professor_id);
Professor* professor_list_find_by_id(ProfessorList* list, int professor_id);
Professor* professor_list_find_by_name(ProfessorList* list, const char* first_name, const char* last_name);
Professor* professor_list_find_by_email(ProfessorList* list, const char* email);
void professor_list_display_all(ProfessorList* list);
void professor_list_display_professor(Professor* professor);
int professor_list_save_to_file(ProfessorList* list, const char* filename);
int professor_list_load_from_file(ProfessorList* list, const char* filename);
void professor_list_sort_by_name(ProfessorList* list);
void professor_list_sort_by_id(ProfessorList* list);
void professor_list_sort_by_department(ProfessorList* list);
int professor_list_update(ProfessorList* list, int professor_id, Professor updated_professor);
int professor_list_count_by_department(ProfessorList* list, const char* department);
Professor** professor_list_filter_by_department(ProfessorList* list, const char* department, int* result_count);
int professor_validate(Professor* professor);
void professor_print_info(Professor* professor);

// Grade management functions for professors
int professor_create_note_for_student(liste_note* grades, int student_id, int exam_id, float note, int present);
int professor_create_notes_for_exam(liste_note* grades, liste_examen* exams, int exam_id, int* student_ids, float* notes, int* presences, int count);
int professor_modify_student_note(liste_note* grades, int student_id, int exam_id, float new_note);
int professor_can_modify_note(Professor* professor, ListeModules* modules, int exam_id);
void professor_display_notes_by_exam(liste_note* grades, int exam_id);
void professor_display_notes_by_module(liste_note* grades, liste_examen* exams, int module_id);
int professor_get_module_count(Professor* professor, ListeModules* modules);
Module** professor_get_modules(Professor* professor, ListeModules* modules, int* count);

#endif // PROFESSOR_H
