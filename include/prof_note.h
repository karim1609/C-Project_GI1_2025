#ifndef PROF_NOTE_H
#define PROF_NOTE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// Note structure
typedef struct {
    int id;
    int student_id;
    int module_id;
    int professor_id;
    char content[512];
    char date[20];
} ProfessorNote;

// List structure for notes
typedef struct {
    ProfessorNote* notes;
    int count;
    int capacity;
    char filename[256];
} ProfessorNoteList;

// Function declarations
ProfessorNoteList* prof_note_list_create(void);
void prof_note_list_destroy(ProfessorNoteList* list);
int prof_note_create(ProfessorNoteList* list, int student_id, int module_id, int prof_id, const char* content);
int prof_note_save(ProfessorNoteList* list, const char* filename);
int prof_note_load(ProfessorNoteList* list, const char* filename);

// Helper functions
// Returns a dynamically allocated array of pointers to notes for a specific student
// count is updated with the number of notes found
// The caller is responsible for freeing the returned array (but not the notes themselves)
ProfessorNote** prof_note_find_by_student(ProfessorNoteList* list, int student_id, int* count);
ProfessorNote** prof_note_find_by_module(ProfessorNoteList* list, int module_id, int* count);

#endif // PROF_NOTE_H
