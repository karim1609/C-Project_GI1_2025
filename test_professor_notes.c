/*
 * Test Program for Professor Grade Creation Features
 * Demonstrates how professors can create notes for students
 */

#include <stdio.h>
#include <stdlib.h>
#include "include/professor.h"
#include "include/grade.h"
#include "include/student.h"
#include "include/utils.h"

// Function prototypes
void test_create_single_note(void);
void test_create_multiple_notes(void);
void test_modify_note(void);
void test_display_notes_by_exam(void);
void test_display_notes_by_module(void);

// Global data structures
liste_note *grades = NULL;
liste_examen *exams = NULL;
ListeModules *modules = NULL;
Professor test_professor;

int main(void) {
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║     PROFESSOR GRADE CREATION - TEST PROGRAM              ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
    
    // Initialize data structures
    grades = creer_liste_note(1000);
    exams = cree_liste_examen();
    modules = liste_cours_creer();
    
    if (!grades || !exams || !modules) {
        fprintf(stderr, "Error: Failed to initialize data structures\n");
        return EXIT_FAILURE;
    }
    
    // Load existing data
    printf("[INFO] Loading existing data...\n");
    charger_notes_depuis_file(grades);
    liste_examen_a_partir_file(exams);
    remplire_liste_appartit_file(modules);
    
    printf("[INFO] Loaded %d grades, %d exams, %d modules\n", 
           grades->count, exams->count, modules->count);
    
    // Setup test professor
    test_professor.id = 1;
    strcpy(test_professor.first_name, "Ahmed");
    strcpy(test_professor.last_name, "Bahri");
    strcpy(test_professor.email, "a.bahri@university.edu");
    strcpy(test_professor.department, "Computer Science");
    strcpy(test_professor.specialization, "Algorithms & Data Structures");
    test_professor.is_active = 1;
    
    int choice;
    do {
        printf("\n╔══════════════════════════════════════════════════════════╗\n");
        printf("║              PROFESSOR GRADE MANAGEMENT MENU             ║\n");
        printf("╠══════════════════════════════════════════════════════════╣\n");
        printf("║  1. Create a single grade for a student                 ║\n");
        printf("║  2. Create grades for multiple students (same exam)     ║\n");
        printf("║  3. Modify an existing grade                            ║\n");
        printf("║  4. Display all grades for an exam                      ║\n");
        printf("║  5. Display all grades for a module                     ║\n");
        printf("║  6. View exam statistics                                ║\n");
        printf("║  7. List all available exams                            ║\n");
        printf("║  8. List all available modules                          ║\n");
        printf("║  0. Exit                                                ║\n");
        printf("╚══════════════════════════════════════════════════════════╝\n");
        printf("Enter your choice: ");
        scanf("%d", &choice);
        getchar(); // Clear newline
        
        switch (choice) {
            case 1:
                test_create_single_note();
                break;
            case 2:
                test_create_multiple_notes();
                break;
            case 3:
                test_modify_note();
                break;
            case 4:
                test_display_notes_by_exam();
                break;
            case 5:
                test_display_notes_by_module();
                break;
            case 6: {
                int exam_id;
                printf("Enter exam ID: ");
                scanf("%d", &exam_id);
                getchar();
                statistiques_examen(grades, exam_id);
                break;
            }
            case 7:
                printf("\n=== AVAILABLE EXAMS ===\n");
                afficher_liste_examens(exams);
                break;
            case 8:
                printf("\n=== AVAILABLE MODULES ===\n");
                liste_cours_afficher(modules);
                break;
            case 0:
                printf("\n[INFO] Exiting...\n");
                break;
            default:
                printf("\n[ERROR] Invalid choice. Please try again.\n");
        }
    } while (choice != 0);
    
    // Cleanup
    if (grades) detruire_liste_notes(&grades);
    if (exams) free(exams->exam);
    if (exams) free(exams);
    if (modules) liste_cours_detruire(&modules);
    
    printf("\n[INFO] Program terminated successfully.\n");
    return EXIT_SUCCESS;
}

void test_create_single_note(void) {
    int student_id, exam_id, present;
    float grade;
    
    printf("\n=== CREATE SINGLE GRADE ===\n");
    
    printf("Enter student ID: ");
    scanf("%d", &student_id);
    
    printf("Enter exam ID: ");
    scanf("%d", &exam_id);
    
    printf("Enter grade (0-20): ");
    scanf("%f", &grade);
    
    printf("Was student present? (1=Yes, 0=No): ");
    scanf("%d", &present);
    getchar();
    
    if (professor_create_note_for_student(grades, student_id, exam_id, grade, present)) {
        printf("\n✓ Grade created successfully!\n");
    } else {
        printf("\n✗ Failed to create grade.\n");
    }
}

void test_create_multiple_notes(void) {
    int exam_id, num_students;
    
    printf("\n=== CREATE MULTIPLE GRADES FOR ONE EXAM ===\n");
    
    printf("Enter exam ID: ");
    scanf("%d", &exam_id);
    
    printf("How many students? ");
    scanf("%d", &num_students);
    getchar();
    
    if (num_students <= 0 || num_students > 100) {
        printf("Invalid number of students.\n");
        return;
    }
    
    int *student_ids = (int*)malloc(num_students * sizeof(int));
    float *notes = (float*)malloc(num_students * sizeof(float));
    int *presences = (int*)malloc(num_students * sizeof(int));
    
    if (!student_ids || !notes || !presences) {
        printf("Memory allocation failed.\n");
        free(student_ids);
        free(notes);
        free(presences);
        return;
    }
    
    for (int i = 0; i < num_students; i++) {
        printf("\n--- Student %d ---\n", i + 1);
        printf("Student ID: ");
        scanf("%d", &student_ids[i]);
        
        printf("Grade (0-20): ");
        scanf("%f", &notes[i]);
        
        printf("Present? (1=Yes, 0=No): ");
        scanf("%d", &presences[i]);
        getchar();
    }
    
    int created = professor_create_notes_for_exam(grades, exams, exam_id, 
                                                  student_ids, notes, presences, num_students);
    
    printf("\n✓ Successfully created %d/%d grades.\n", created, num_students);
    
    free(student_ids);
    free(notes);
    free(presences);
}

void test_modify_note(void) {
    int student_id, exam_id;
    float new_grade;
    
    printf("\n=== MODIFY EXISTING GRADE ===\n");
    
    printf("Enter student ID: ");
    scanf("%d", &student_id);
    
    printf("Enter exam ID: ");
    scanf("%d", &exam_id);
    
    // Show current grade
    Note *current = chercher_note(grades, student_id, exam_id);
    if (current) {
        printf("Current grade: %.2f\n", current->note_obtenue);
    } else {
        printf("No grade found for this student/exam combination.\n");
        return;
    }
    
    printf("Enter new grade (0-20): ");
    scanf("%f", &new_grade);
    getchar();
    
    if (professor_modify_student_note(grades, student_id, exam_id, new_grade)) {
        printf("\n✓ Grade modified successfully!\n");
    } else {
        printf("\n✗ Failed to modify grade.\n");
    }
}

void test_display_notes_by_exam(void) {
    int exam_id;
    
    printf("\n=== DISPLAY GRADES BY EXAM ===\n");
    printf("Enter exam ID: ");
    scanf("%d", &exam_id);
    getchar();
    
    professor_display_notes_by_exam(grades, exam_id);
}

void test_display_notes_by_module(void) {
    int module_id;
    
    printf("\n=== DISPLAY GRADES BY MODULE ===\n");
    printf("Enter module ID: ");
    scanf("%d", &module_id);
    getchar();
    
    professor_display_notes_by_module(grades, exams, module_id);
}
