#include "../include/professor.h"
#include "../include/utils.h"
#include <ctype.h>

#define INITIAL_CAPACITY 100

// Create a new professor list
ProfessorList* professor_list_create(void) {
    ProfessorList* list = (ProfessorList*)malloc(sizeof(ProfessorList));
    if (!list) {
        fprintf(stderr, "Error: Memory allocation failed for professor list\n");
        return NULL;
    }
    
    list->professors = (Professor*)malloc(INITIAL_CAPACITY * sizeof(Professor));
    if (!list->professors) {
        fprintf(stderr, "Error: Memory allocation failed for professors array\n");
        free(list);
        return NULL;
    }
    
    list->count = 0;
    list->capacity = INITIAL_CAPACITY;
    list->is_loaded = 0;
    list->auto_save_enabled = 0;
    list->last_save_time = time(NULL);
    strcpy(list->filename, "data/professors.txt");
    
    return list;
}

// Destroy professor list and free memory
void professor_list_destroy(ProfessorList* list) {
    if (!list) return;
    
    if (list->professors) {
        free(list->professors);
    }
    free(list);
}

// Add a professor to the list
int professor_list_add(ProfessorList* list, Professor professor) {
    if (!list) {
        fprintf(stderr, "Error: Invalid professor list\n");
        return 0;
    }
    
    // Check if professor with same ID already exists
    if (professor_list_find_by_id(list, professor.id) != NULL) {
        fprintf(stderr, "Error: Professor with ID %d already exists\n", professor.id);
        return 0;
    }
    
    // Check if we need to expand capacity
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        Professor* new_professors = (Professor*)realloc(list->professors, new_capacity * sizeof(Professor));
        if (!new_professors) {
            fprintf(stderr, "Error: Memory reallocation failed\n");
            return 0;
        }
        list->professors = new_professors;
        list->capacity = new_capacity;
    }
    
    list->professors[list->count++] = professor;
    
    if (list->auto_save_enabled) {
        professor_list_save_to_file(list, list->filename);
    }
    
    return 1;
}

// Remove a professor from the list by ID
int professor_list_remove(ProfessorList* list, int professor_id) {
    if (!list) return 0;
    
    for (int i = 0; i < list->count; i++) {
        if (list->professors[i].id == professor_id) {
            // Shift all elements after this one
            for (int j = i; j < list->count - 1; j++) {
                list->professors[j] = list->professors[j + 1];
            }
            list->count--;
            
            if (list->auto_save_enabled) {
                professor_list_save_to_file(list, list->filename);
            }
            
            return 1;
        }
    }
    
    return 0;
}

// Find professor by ID
Professor* professor_list_find_by_id(ProfessorList* list, int professor_id) {
    if (!list) return NULL;
    
    for (int i = 0; i < list->count; i++) {
        if (list->professors[i].id == professor_id) {
            return &list->professors[i];
        }
    }
    
    return NULL;
}

// Find professor by name
Professor* professor_list_find_by_name(ProfessorList* list, const char* first_name, const char* last_name) {
    if (!list || !first_name || !last_name) return NULL;
    
    for (int i = 0; i < list->count; i++) {
        if (strcasecmp(list->professors[i].first_name, first_name) == 0 &&
            strcasecmp(list->professors[i].last_name, last_name) == 0) {
            return &list->professors[i];
        }
    }
    
    return NULL;
}

// Find professor by email
Professor* professor_list_find_by_email(ProfessorList* list, const char* email) {
    if (!list || !email) return NULL;
    
    for (int i = 0; i < list->count; i++) {
        if (strcasecmp(list->professors[i].email, email) == 0) {
            return &list->professors[i];
        }
    }
    
    return NULL;
}

// Display all professors
void professor_list_display_all(ProfessorList* list) {
    if (!list) {
        printf("Invalid professor list\n");
        return;
    }
    
    if (list->count == 0) {
        printf("No professors in the list\n");
        return;
    }
    
    printf("\n========== Professor List ==========\n");
    printf("Total professors: %d\n\n", list->count);
    
    for (int i = 0; i < list->count; i++) {
        professor_list_display_professor(&list->professors[i]);
        printf("-----------------------------------\n");
    }
}

// Display a single professor
void professor_list_display_professor(Professor* professor) {
    if (!professor) return;
    
    printf("ID: %d\n", professor->id);
    printf("Name: %s %s\n", professor->first_name, professor->last_name);
    printf("Email: %s\n", professor->email);
    printf("Phone: %s\n", professor->phone);
    printf("Department: %s\n", professor->department);
    printf("Specialization: %s\n", professor->specialization);
    printf("Years of Experience: %d\n", professor->years_of_experience);
    printf("Office: %s\n", professor->office_location);
    printf("Status: %s\n", professor->is_active ? "Active" : "Inactive");
}

// Save professor list to file
int professor_list_save_to_file(ProfessorList* list, const char* filename) {
    if (!list || !filename) return 0;
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        fprintf(stderr, "[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "w");
    if (!file) {
        fprintf(stderr, "Error: Cannot open file %s for writing\n", full_path);
        return 0;
    }
    
    for (int i = 0; i < list->count; i++) {
        Professor* p = &list->professors[i];
        fprintf(file, "%d|%s|%s|%s|%s|%s|%s|%s|%d|%s|%ld|%d\n",
                p->id,
                p->first_name,
                p->last_name,
                p->email,
                p->phone,
                p->address,
                p->department,
                p->specialization,
                p->years_of_experience,
                p->office_location,
                (long)p->hire_date,
                p->is_active);
    }
    
    fclose(file);
    list->last_save_time = time(NULL);
    strcpy(list->filename, filename);
    
    return 1;
}

// Load professor list from file
int professor_list_load_from_file(ProfessorList* list, const char* filename) {
    if (!list || !filename) return 0;
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        fprintf(stderr, "[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "r");
    if (!file) {
        fprintf(stderr, "Warning: Cannot open file %s. Creating new professor list.\n", full_path);
        return 0;
    }
    
    // Clear existing data
    list->count = 0;
    
    char line[1024];
    while (fgets(line, sizeof(line), file)) {
        Professor professor = {0};
        long hire_date_long;
        
        if (sscanf(line, "%d|%99[^|]|%99[^|]|%149[^|]|%19[^|]|%199[^|]|%49[^|]|%49[^|]|%d|%99[^|]|%ld|%d",
                   &professor.id,
                   professor.first_name,
                   professor.last_name,
                   professor.email,
                   professor.phone,
                   professor.address,
                   professor.department,
                   professor.specialization,
                   &professor.years_of_experience,
                   professor.office_location,
                   &hire_date_long,
                   &professor.is_active) == 12) {
            
            professor.hire_date = (time_t)hire_date_long;
            professor_list_add(list, professor);
        }
    }
    
    fclose(file);
    list->is_loaded = 1;
    strcpy(list->filename, filename);
    
    return 1;
}

// Comparison function for sorting by name
static int compare_by_name(const void* a, const void* b) {
    Professor* p1 = (Professor*)a;
    Professor* p2 = (Professor*)b;
    
    int cmp = strcasecmp(p1->last_name, p2->last_name);
    if (cmp == 0) {
        return strcasecmp(p1->first_name, p2->first_name);
    }
    return cmp;
}

// Comparison function for sorting by ID
static int compare_by_id(const void* a, const void* b) {
    Professor* p1 = (Professor*)a;
    Professor* p2 = (Professor*)b;
    return p1->id - p2->id;
}

// Comparison function for sorting by department
static int compare_by_department(const void* a, const void* b) {
    Professor* p1 = (Professor*)a;
    Professor* p2 = (Professor*)b;
    return strcasecmp(p1->department, p2->department);
}

// Sort professors by name
void professor_list_sort_by_name(ProfessorList* list) {
    if (!list || list->count <= 1) return;
    qsort(list->professors, list->count, sizeof(Professor), compare_by_name);
}

// Sort professors by ID
void professor_list_sort_by_id(ProfessorList* list) {
    if (!list || list->count <= 1) return;
    qsort(list->professors, list->count, sizeof(Professor), compare_by_id);
}

// Sort professors by department
void professor_list_sort_by_department(ProfessorList* list) {
    if (!list || list->count <= 1) return;
    qsort(list->professors, list->count, sizeof(Professor), compare_by_department);
}

// Update professor information
int professor_list_update(ProfessorList* list, int professor_id, Professor updated_professor) {
    if (!list) return 0;
    
    Professor* professor = professor_list_find_by_id(list, professor_id);
    if (!professor) {
        fprintf(stderr, "Error: Professor with ID %d not found\n", professor_id);
        return 0;
    }
    
    // Keep the original ID
    updated_professor.id = professor_id;
    *professor = updated_professor;
    
    if (list->auto_save_enabled) {
        professor_list_save_to_file(list, list->filename);
    }
    
    return 1;
}

// Count professors by department
int professor_list_count_by_department(ProfessorList* list, const char* department) {
    if (!list || !department) return 0;
    
    int count = 0;
    for (int i = 0; i < list->count; i++) {
        if (strcasecmp(list->professors[i].department, department) == 0) {
            count++;
        }
    }
    
    return count;
}

// Filter professors by department
Professor** professor_list_filter_by_department(ProfessorList* list, const char* department, int* result_count) {
    if (!list || !department || !result_count) return NULL;
    
    int count = professor_list_count_by_department(list, department);
    if (count == 0) {
        *result_count = 0;
        return NULL;
    }
    
    Professor** results = (Professor**)malloc(count * sizeof(Professor*));
    if (!results) {
        *result_count = 0;
        return NULL;
    }
    
    int index = 0;
    for (int i = 0; i < list->count; i++) {
        if (strcasecmp(list->professors[i].department, department) == 0) {
            results[index++] = &list->professors[i];
        }
    }
    
    *result_count = count;
    return results;
}

// Validate professor data
int professor_validate(Professor* professor) {
    if (!professor) return 0;
    
    if (professor->id <= 0) {
        fprintf(stderr, "Error: Invalid professor ID\n");
        return 0;
    }
    
    if (strlen(professor->first_name) == 0 || strlen(professor->last_name) == 0) {
        fprintf(stderr, "Error: Professor name cannot be empty\n");
        return 0;
    }
    
    if (strlen(professor->email) == 0) {
        fprintf(stderr, "Error: Professor email cannot be empty\n");
        return 0;
    }
    
    if (strlen(professor->department) == 0) {
        fprintf(stderr, "Error: Professor department cannot be empty\n");
        return 0;
    }
    
    return 1;
}

// Print professor information (formatted)
void professor_print_info(Professor* professor) {
    if (!professor) return;
    
    char full_name[MAX_NAME_LENGTH * 2];
    snprintf(full_name, sizeof(full_name), "%s %s", professor->first_name, professor->last_name);
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║              PROFESSOR INFORMATION                     ║\n");
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ ID:              %-38d║\n", professor->id);
    printf("║ Name:            %-38s║\n", full_name);
    printf("║ Email:           %-38s║\n", professor->email);
    printf("║ Phone:           %-38s║\n", professor->phone);
    printf("║ Department:      %-38s║\n", professor->department);
    printf("║ Specialization:  %-38s║\n", professor->specialization);
    printf("║ Experience:      %-38d║\n", professor->years_of_experience);
    printf("║ Office:          %-38s║\n", professor->office_location);
    printf("║ Status:          %-38s║\n", professor->is_active ? "Active" : "Inactive");
    printf("╚════════════════════════════════════════════════════════╝\n\n");
}

/*
 * Grade Management Functions for Professors
 */

// Create a single note for a student on an exam
int professor_create_note_for_student(liste_note* grades, int student_id, int exam_id, float note, int present) {
    if (!grades) {
        fprintf(stderr, "Error: Invalid grade list\n");
        return 0;
    }
    
    // Validate note range
    if (note < 0 || note > 20) {
        fprintf(stderr, "Error: Note must be between 0 and 20\n");
        return 0;
    }
    
    // Check if note already exists for this student/exam combination
    Note* existing = chercher_note(grades, student_id, exam_id);
    if (existing) {
        fprintf(stderr, "Error: Note already exists for student %d on exam %d\n", student_id, exam_id);
        return 0;
    }
    
    // Create new note
    Note new_note;
    new_note.id_etudiant = student_id;
    new_note.id_examen = exam_id;
    new_note.note_obtenue = note;
    new_note.present = present;
    
    // Add to list
    if (note_ajouter(grades, &new_note)) {
        printf("Note created successfully: Student %d - Exam %d - Grade %.2f\n", 
               student_id, exam_id, note);
        return 1;
    }
    
    fprintf(stderr, "Error: Failed to add note to list\n");
    return 0;
}

// Create notes for multiple students on a single exam
int professor_create_notes_for_exam(liste_note* grades, liste_examen* exams, int exam_id, 
                                   int* student_ids, float* notes, int* presences, int count) {
    if (!grades || !exams || !student_ids || !notes || !presences) {
        fprintf(stderr, "Error: Invalid parameters\n");
        return 0;
    }
    
    // Verify exam exists
    Examen* exam = chercher_examen_par_id(exams, exam_id);
    if (!exam) {
        fprintf(stderr, "Error: Exam with ID %d not found\n", exam_id);
        return 0;
    }
    
    printf("\n=== Creating notes for exam: %s (ID: %d) ===\n", exam->nom_module, exam_id);
    
    int success_count = 0;
    int failed_count = 0;
    
    for (int i = 0; i < count; i++) {
        if (professor_create_note_for_student(grades, student_ids[i], exam_id, notes[i], presences[i])) {
            success_count++;
        } else {
            failed_count++;
        }
    }
    
    printf("\n=== Summary ===\n");
    printf("Successfully created: %d notes\n", success_count);
    printf("Failed: %d notes\n", failed_count);
    printf("Total: %d notes\n", count);
    
    // Auto-save if any notes were created
    if (success_count > 0) {
        sauvegarder_notes_ds_file(grades);
    }
    
    return success_count;
}

// Modify an existing student note
int professor_modify_student_note(liste_note* grades, int student_id, int exam_id, float new_note) {
    if (!grades) {
        fprintf(stderr, "Error: Invalid grade list\n");
        return 0;
    }
    
    // Validate note range
    if (new_note < 0 || new_note > 20) {
        fprintf(stderr, "Error: Note must be between 0 and 20\n");
        return 0;
    }
    
    // Find existing note
    Note* note = chercher_note(grades, student_id, exam_id);
    if (!note) {
        fprintf(stderr, "Error: Note not found for student %d on exam %d\n", student_id, exam_id);
        return 0;
    }
    
    float old_note = note->note_obtenue;
    note->note_obtenue = new_note;
    
    printf("Note modified successfully: Student %d - Exam %d\n", student_id, exam_id);
    printf("Old grade: %.2f -> New grade: %.2f\n", old_note, new_note);
    
    // Auto-save
    sauvegarder_notes_ds_file(grades);
    
    return 1;
}

// Check if professor can modify notes for a specific exam (based on module assignment)
int professor_can_modify_note(Professor* professor, ListeModules* modules, int exam_id) {
    if (!professor || !modules) {
        return 0;
    }
    
    if (!professor->is_active) {
        return 0;
    }
    
    // Check if professor teaches any module
    // Module assignment is stored in nom_prenom_enseignent field
    char prof_name[150];
    snprintf(prof_name, sizeof(prof_name), "%c. %s", 
             professor->first_name[0], professor->last_name);
    
    int has_modules = 0;
    for (int i = 0; i < modules->count; i++) {
        if (strstr(modules->cours[i].nom_prenom_enseignent, professor->last_name) != NULL) {
            has_modules = 1;
            break;
        }
    }
    
    return has_modules;
}

// Display all notes for a specific exam
void professor_display_notes_by_exam(liste_note* grades, int exam_id) {
    if (!grades) {
        printf("Error: Invalid grade list\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║           NOTES FOR EXAM ID: %-26d║\n", exam_id);
    printf("╠════════════════════════════════════════════════════════╣\n");
    printf("║ Student ID │ Grade  │ Attendance │ Status             ║\n");
    printf("╠════════════╪════════╪════════════╪════════════════════╣\n");
    
    int count = 0;
    for (int i = 0; i < grades->count; i++) {
        if (grades->note[i].id_examen == exam_id) {
            const char* attendance = grades->note[i].present ? "Present" : "Absent";
            const char* status = grades->note[i].note_obtenue >= 10.0 ? "PASSED" : "FAILED";
            
            printf("║ %-10d │ %6.2f │ %-10s │ %-18s ║\n",
                   grades->note[i].id_etudiant,
                   grades->note[i].note_obtenue,
                   attendance,
                   status);
            count++;
        }
    }
    
    if (count == 0) {
        printf("║                    No notes found                      ║\n");
    }
    
    printf("╚════════════════════════════════════════════════════════╝\n");
    printf("Total notes: %d\n\n", count);
}

// Display all notes for a specific module (all exams of that module)
void professor_display_notes_by_module(liste_note* grades, liste_examen* exams, int module_id) {
    if (!grades || !exams) {
        printf("Error: Invalid parameters\n");
        return;
    }
    
    printf("\n╔════════════════════════════════════════════════════════╗\n");
    printf("║         NOTES FOR MODULE ID: %-27d║\n", module_id);
    printf("╚════════════════════════════════════════════════════════╝\n\n");
    
    int found = 0;
    for (int i = 0; i < exams->count; i++) {
        if (exams->exam[i].id_module == module_id) {
            found = 1;
            printf("Exam: %s (ID: %d)\n", exams->exam[i].nom_module, exams->exam[i].id_examen);
            professor_display_notes_by_exam(grades, exams->exam[i].id_examen);
        }
    }
    
    if (!found) {
        printf("No exams found for this module.\n");
    }
}

// Get count of modules taught by a professor
int professor_get_module_count(Professor* professor, ListeModules* modules) {
    if (!professor || !modules) {
        return 0;
    }
    
    int count = 0;
    for (int i = 0; i < modules->count; i++) {
        if (strstr(modules->cours[i].nom_prenom_enseignent, professor->last_name) != NULL) {
            count++;
        }
    }
    
    return count;
}

// Get list of modules taught by a professor
Module** professor_get_modules(Professor* professor, ListeModules* modules, int* count) {
    if (!professor || !modules || !count) {
        if (count) *count = 0;
        return NULL;
    }
    
    // First count modules
    int module_count = professor_get_module_count(professor, modules);
    *count = module_count;
    
    if (module_count == 0) {
        return NULL;
    }
    
    // Allocate array for module pointers
    Module** result = (Module**)malloc(module_count * sizeof(Module*));
    if (!result) {
        *count = 0;
        return NULL;
    }
    
    // Fill array with matching modules
    int idx = 0;
    for (int i = 0; i < modules->count && idx < module_count; i++) {
        if (strstr(modules->cours[i].nom_prenom_enseignent, professor->last_name) != NULL) {
            result[idx++] = &modules->cours[i];
        }
    }
    
    return result;
}
