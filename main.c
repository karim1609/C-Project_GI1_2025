/*
 * Student Management System - Main Application
 * Combined UI and Main Entry Point
 */

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#include "include/config.h"
#include "include/auth.h"
#include "include/student.h"
#include "include/grade.h"
#include "include/attendance.h"
#include "include/club.h"
#include "include/professor.h"
#include "include/file_manager.h"
#include "include/theme.h"
#include "include/utils.h"
#include "include/ui.h"
#include "include/prof_note.h"

// Global application state
typedef struct {
    GtkApplication *app;
    GtkWidget *login_window;
    GtkWidget *main_window;
    
    // Data structures
    UserList *users;
    StudentList *students;
    ProfessorList *professors;
    GradeList *grades;
    AttendanceList *attendance;
    ClubList *clubs;
    MembershipList *memberships;
    ListeModules *modules;
    liste_examen *exams;
    ProfessorNoteList *prof_notes;
    
    // Current session
    Session *session;
    UserRole selected_role;
    
    // Theme
    ThemeType current_theme;
    
    char data_dir[256];
} AppState;

static AppState app_state = {0};

// Forward declarations
static void show_login_window(void);
static void show_main_window(void);
static void on_login_clicked(GtkWidget *widget, gpointer data);
static void on_role_selected(GtkWidget *widget, gpointer data);
static void on_manage_students_clicked(GtkWidget *widget, gpointer data);
static void on_manage_grades_clicked(GtkWidget *widget, gpointer data);
static void on_manage_attendance_clicked(GtkWidget *widget, gpointer data);
static void on_manage_clubs_clicked(GtkWidget *widget, gpointer data);
static void on_admin_view_clicked(GtkWidget *widget, gpointer data);
static void on_manage_modules_clicked(GtkWidget *widget, gpointer data);
static void on_assign_professor_clicked(GtkWidget *widget, gpointer data);
static void on_manage_notes_clicked(GtkWidget *widget, gpointer data);
static void on_manage_exams_clicked(GtkWidget *widget, gpointer data);
static void on_professor_create_notes_clicked(GtkWidget *widget, gpointer data);
static void on_professor_text_notes_clicked(GtkWidget *widget, gpointer data);
static void on_student_view_notes_clicked(GtkWidget *widget, gpointer data);
static int load_all_data(void);
static int save_all_data(void);
static void cleanup_app(void);

/*
 * Initialize application data structures
 */
static int initialize_app_data(void) {
    printf("[INFO] Initializing application data...\n");
    
    // Initialize file manager
    if (file_manager_init() != FILE_SUCCESS) {
        fprintf(stderr, "[ERROR] Failed to initialize file manager\n");
        return -1;
    }
    
    // Create data directory
    strcpy(app_state.data_dir, DATA_DIR);
    
    // Initialize user list
    app_state.users = user_list_create();
    if (!app_state.users) {
        fprintf(stderr, "[ERROR] Failed to create user list\n");
        return -1;
    }
    
    // Initialize student list
    app_state.students = student_list_create();
    if (!app_state.students) {
        fprintf(stderr, "[ERROR] Failed to create student list\n");
        return -1;
    }
    
    // Initialize professor list
    app_state.professors = professor_list_create();
    if (!app_state.professors) {
        fprintf(stderr, "[ERROR] Failed to create professor list\n");
        return -1;
    }
    
    // Initialize grade list
    app_state.grades = liste_note_create();
    if (!app_state.grades) {
        fprintf(stderr, "[ERROR] Failed to create grade list\n");
        return -1;
    }
    
    // Initialize attendance list
    app_state.attendance = attendance_list_create();
    if (!app_state.attendance) {
        fprintf(stderr, "[ERROR] Failed to create attendance list\n");
        return -1;
    }
    
    // Initialize club list
    app_state.clubs = club_list_create();
    if (!app_state.clubs) {
        fprintf(stderr, "[ERROR] Failed to create club list\n");
        return -1;
    }
    
    // Initialize membership list
    app_state.memberships = membership_list_create();
    if (!app_state.memberships) {
        fprintf(stderr, "[ERROR] Failed to create membership list\n");
        return -1;
    }
    
    // Initialize modules list
    app_state.modules = liste_cours_creer();
    if (!app_state.modules) {
        fprintf(stderr, "[ERROR] Failed to create modules list\n");
        return -1;
    }
    
    // Initialize exams list
    app_state.exams = cree_liste_examen();
    if (!app_state.exams) {
        fprintf(stderr, "[ERROR] Failed to create exams list\n");
        return -1;
    }

    // Initialize professor notes list
    app_state.prof_notes = prof_note_list_create();
    if (!app_state.prof_notes) {
        fprintf(stderr, "[ERROR] Failed to create professor notes list\n");
        return -1;
    }
    
    // Load data from files
    if (load_all_data() != 0) {
        fprintf(stderr, "[WARNING] Failed to load some data files\n");
        // Continue anyway - files might not exist yet
    }
    
    // Create default admin user if no users exist
    if (app_state.users->count == 0) {
        printf("[INFO] Creating default admin user...\n");
        if (!auth_register(app_state.users, "admin", "admin@system.com", "Admin123!", ROLE_ADMIN)) {
            fprintf(stderr, "[WARNING] Failed to create default admin user\n");
        } else {
            printf("[OK] Default admin user created (username: admin, password: Admin123!)\n");
            save_all_data();
        }
    }
    
    // Initialize theme
    app_state.current_theme = THEME_LIGHT;
    
    printf("[OK] Application data initialized\n");
    return 0;
}

/*
 * Load all data from files
 */
static int load_all_data(void) {
    char filepath[512];
    int errors = 0;
    
    // Load users
    snprintf(filepath, sizeof(filepath), "%s", USERS_FILE);
    if (auth_load_users(app_state.users, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load users from %s\n", filepath);
        errors++;
    }
    
    // Load students
    snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
    if (student_list_load_from_file(app_state.students, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load students from %s\n", filepath);
        errors++;
    }
    
    // Load professors
    snprintf(filepath, sizeof(filepath), "professors.txt");
    if (professor_list_load_from_file(app_state.professors, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load professors from %s\n", filepath);
        errors++;
    } else {
        printf("[INFO] Loaded %d professors\n", app_state.professors->count);
    }
    
    // Load grades
    snprintf(filepath, sizeof(filepath), "%s", GRADES_FILE);
    if (grade_list_load_from_file(app_state.grades, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load grades from %s\n", filepath);
        errors++;
    }
    
    // Load attendance
    snprintf(filepath, sizeof(filepath), "%s", ATTENDANCE_FILE);
    if (attendance_list_load_from_file(app_state.attendance, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load attendance from %s\n", filepath);
        errors++;
    }
    
    // Load clubs
    snprintf(filepath, sizeof(filepath), "%s", CLUBS_FILE);
    if (club_list_load_from_file(app_state.clubs, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load clubs from %s\n", filepath);
        errors++;
    }
    
    // Load memberships
    snprintf(filepath, sizeof(filepath), "%s", MEMBERSHIPS_FILE);
    if (membership_list_load_from_file(app_state.memberships, filepath) != 1) {
        fprintf(stderr, "[WARNING] Failed to load memberships from %s\n", filepath);
        errors++;
    }
    
    // Load modules
    if (app_state.modules) {
        strcpy(app_state.modules->filename, "modules.txt");
        if (remplire_liste_appartit_file(app_state.modules) != 1) {
            fprintf(stderr, "[WARNING] Failed to load modules, generating sample data\n");
            errors++;
            
            // Add sample modules
            const char *sample_modules[][9] = {
                {"1", "Mathematiques 1", "Calculus and linear algebra", "40", "30", "0", "1", "1", "F. Bouhafeer"},
                {"2", "Physique 1", "General physics principles", "35", "25", "15", "1", "1", "F. Moradi"},
                {"3", "Algorithmique", "Introduction to algorithms", "35", "25", "15", "1", "1", "M. Cherradi"},
                {"4", "Programmation C", "C programming language", "30", "20", "25", "1", "2", "Y. Messaoudi"},
                {"5", "Base de Donnees", "Database systems", "30", "25", "20", "2", "2", "E. Bouaza"}
            };
            
            for (int i = 0; i < 5; i++) {
                if (app_state.modules->count < app_state.modules->capacity) {
                    Module m;
                    m.id = atoi(sample_modules[i][0]);
                    strncpy(m.nom, sample_modules[i][1], MAX_NAME_LENGTH - 1);
                    strncpy(m.description, sample_modules[i][2], MAX_DESC_LENGTH - 1);
                    m.heures_cours = atoi(sample_modules[i][3]);
                    m.heures_td = atoi(sample_modules[i][4]);
                    m.heures_tp = atoi(sample_modules[i][5]);
                    m.niveau = atoi(sample_modules[i][6]);
                    m.semestre = atoi(sample_modules[i][7]);
                    strncpy(m.nom_prenom_enseignent, sample_modules[i][8], 49);
                    app_state.modules->cours[app_state.modules->count++] = m;
                }
            }
            printf("[INFO] Generated %d sample modules\n", app_state.modules->count);
        } else {
            printf("[INFO] Loaded %d modules\n", app_state.modules->count);
        }
    }
    
    // Load exams
    if (app_state.exams) {
        strcpy(app_state.exams->filename, "examens.txt");
        if (liste_examen_a_partir_file(app_state.exams) != 1) {
            fprintf(stderr, "[WARNING] Failed to load exams, generating sample data\n");
            errors++;
            
            // Add sample exams
            time_t current_time = time(NULL);
            struct tm *tm_info;
            
            const char *exam_modules[] = {"Mathematiques 1", "Physique 1", "Algorithmique", "Programmation C", "Base de Donnees"};
            int durations[] = {120, 150, 180, 120, 150};
            
            for (int i = 0; i < 5; i++) {
                if (app_state.exams->count < app_state.exams->capacity) {
                    Examen exam;
                    exam.id_examen = i + 1;
                    exam.id_module = i + 1;
                    strncpy(exam.nom_module, exam_modules[i], 19);
                    exam.nom_module[19] = '\0';
                    
                    // Set exam date to current time + (i * 7 days)
                    exam.date_examen = current_time + (i * 7 * 24 * 60 * 60);
                    exam.duree = durations[i];
                    
                    app_state.exams->exam[app_state.exams->count++] = exam;
                }
            }
            printf("[INFO] Generated %d sample exams\n", app_state.exams->count);
        } else {
            printf("[INFO] Loaded %d exams\n", app_state.exams->count);
        }
    }

    // Load professor notes
    snprintf(filepath, sizeof(filepath), "%s", PROF_NOTES_FILE);
    // If loading fails (e.g. file doesn't exist), just continue log it
    // Use full path if PROF_NOTES_FILE is just filename, but config.h defines it as just filename.
    // So we need to prepend data_dir.
    char full_notes_path[512];
    snprintf(full_notes_path, sizeof(full_notes_path), "%s%s", app_state.data_dir, PROF_NOTES_FILE);
    
    // We can also check if file exists first to avoid error log on first run
    FILE *f = fopen(full_notes_path, "r");
    if (f) {
        fclose(f);
        if (prof_note_load(app_state.prof_notes, full_notes_path) != 1) {
            fprintf(stderr, "[WARNING] Failed to load professor notes\n");
            // errors++; // Optional, maybe not critical
        } else {
            printf("[INFO] Loaded %d professor notes\n", app_state.prof_notes->count);
        }
    } else {
        printf("[INFO] No professor notes file found, starting fresh.\n");
    }
    
    return errors > 0 ? -1 : 0;
}

/*
 * Save all data to files
 */
static int save_all_data(void) {
    char filepath[512];
    int errors = 0;
    
    // Save users
    snprintf(filepath, sizeof(filepath), "%s", USERS_FILE);
    if (auth_save_users(app_state.users, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save users\n");
        errors++;
    }
    
    // Save students
    snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
    if (app_state.students && app_state.students->count > 0) {
        printf("[DEBUG] At shutdown: First student is %s %s\n", 
               app_state.students->students[0].first_name,
               app_state.students->students[0].last_name);
    }
    if (student_list_save_to_file(app_state.students, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save students\n");
        errors++;
    }
    
    // Save grades
    snprintf(filepath, sizeof(filepath), "%s", GRADES_FILE);
    if (grade_list_save_to_file(app_state.grades, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save grades\n");
        errors++;
    }
    
    // Save attendance
    snprintf(filepath, sizeof(filepath), "%s", ATTENDANCE_FILE);
    if (attendance_list_save_to_file(app_state.attendance, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save attendance\n");
        errors++;
    }
    
    // Save clubs
    snprintf(filepath, sizeof(filepath), "%s", CLUBS_FILE);
    if (club_list_save_to_file(app_state.clubs, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save clubs\n");
        errors++;
    }
    
    // Save memberships
    snprintf(filepath, sizeof(filepath), "%s", MEMBERSHIPS_FILE);
    if (membership_list_save_to_file(app_state.memberships, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save memberships\n");
        errors++;
    }

    // Save professor notes
    snprintf(filepath, sizeof(filepath), "%s%s", app_state.data_dir, PROF_NOTES_FILE);
    // Assuming prof_note_save handles nulls/errors gracefully or returns 0 on failure
    if (prof_note_save(app_state.prof_notes, filepath) != 1) {
        fprintf(stderr, "[ERROR] Failed to save professor notes\n");
        errors++;
    }
    
    return errors > 0 ? -1 : 0;
}

/*
 * Cleanup application resources
 */
static void cleanup_app(void) {
    printf("[INFO] Cleaning up application...\n");
    
    // Save data before cleanup
    save_all_data();
    
    // Destroy session
    if (app_state.session) {
        session_destroy(app_state.session);
        app_state.session = NULL;
    }
    
    // Destroy data structures
    if (app_state.users) {
        user_list_destroy(app_state.users);
        app_state.users = NULL;
    }
    
    if (app_state.students) {
        student_list_destroy(app_state.students);
        app_state.students = NULL;
    }
    
    if (app_state.professors) {
        professor_list_destroy(app_state.professors);
        app_state.professors = NULL;
    }
    
    if (app_state.grades) {
        liste_note_destroy(app_state.grades);
        app_state.grades = NULL;
    }
    
    if (app_state.attendance) {
        attendance_list_destroy(app_state.attendance);
        app_state.attendance = NULL;
    }
    
    if (app_state.clubs) {
        club_list_destroy(app_state.clubs);
        app_state.clubs = NULL;
    }
    
    if (app_state.memberships) {
        membership_list_destroy(app_state.memberships);
        app_state.memberships = NULL;
    }
    
    if (app_state.modules) {
        liste_cours_detruire(&app_state.modules);
    }
    
    if (app_state.exams) {
        free(app_state.exams->exam);
        free(app_state.exams);
        app_state.exams = NULL;
    }

    if (app_state.prof_notes) {
        prof_note_list_destroy(app_state.prof_notes);
        app_state.prof_notes = NULL;
    }
    
    file_manager_cleanup();
    
    printf("[OK] Cleanup complete\n");
}

/*
 * Role selection handler
 */
static void on_role_selected(GtkWidget *widget, gpointer data) {
    UserRole role = GPOINTER_TO_INT(data);
    app_state.selected_role = role;
    
    // Visual feedback - highlight selected button
    GtkWidget *parent = gtk_widget_get_parent(widget);
    GList *children = gtk_container_get_children(GTK_CONTAINER(parent));
    
    for (GList *l = children; l != NULL; l = l->next) {
        GtkWidget *btn = GTK_WIDGET(l->data);
        GtkStyleContext *context = gtk_widget_get_style_context(btn);
        gtk_style_context_remove_class(context, "suggested-action");
    }
    
    g_list_free(children);
    
    GtkStyleContext *context = gtk_widget_get_style_context(widget);
    gtk_style_context_add_class(context, "suggested-action");
}

/*
 * Login button handler
 */
static void on_login_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget **entries = (GtkWidget **)data;
    GtkWidget *username_entry = entries[0];
    GtkWidget *password_entry = entries[1];
    GtkWidget *message_label = entries[2];
    
    const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
    const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));
    
    // Validate input
    if (strlen(username) == 0 || strlen(password) == 0) {
        gtk_label_set_text(GTK_LABEL(message_label), "Please enter username and password");
        return;
    }
    
    // Create session
    if (!app_state.session) {
        app_state.session = session_create();
    }
    
    // Attempt login
    if (auth_login(app_state.users, username, password, app_state.session)) {
        // Check if role matches selection
        if (app_state.session->role != app_state.selected_role) {
            gtk_label_set_text(GTK_LABEL(message_label), 
                "Invalid credentials for selected role");
            session_destroy(app_state.session);
            app_state.session = NULL;
            return;
        }
        
        printf("[INFO] User '%s' logged in as %s\n", username, 
               auth_role_to_string(app_state.session->role));
        
        // Hide login window and show main window
        gtk_widget_hide(app_state.login_window);
        show_main_window();
        
    } else {
        gtk_label_set_text(GTK_LABEL(message_label), "Invalid username or password");
    }
}

/*
 * Logout handler
 */
static void on_logout_clicked(GtkWidget *widget, gpointer data) {
    if (app_state.session) {
        auth_logout(app_state.session);
        app_state.session = NULL;
    }
    
    if (app_state.main_window) {
        gtk_widget_destroy(app_state.main_window);
        app_state.main_window = NULL;
    }
    
    show_login_window();
}

/*
 * Create Account button handler
 */
static void on_create_account_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *message_label = GTK_WIDGET(data);
    
    // Create dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Create Student Account",
        GTK_WINDOW(app_state.login_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Create Account", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 300);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 20);
    gtk_container_add(GTK_CONTAINER(content), form_box);
    
    // Username field
    GtkWidget *username_label = gtk_label_new("Username:");
    gtk_widget_set_halign(username_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), username_label, FALSE, FALSE, 0);
    
    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Choose a username (3-50 chars)");
    gtk_box_pack_start(GTK_BOX(form_box), username_entry, FALSE, FALSE, 0);
    
    // Email field
    GtkWidget *email_label = gtk_label_new("Email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), email_label, FALSE, FALSE, 0);
    
    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "your.email@example.com");
    gtk_box_pack_start(GTK_BOX(form_box), email_entry, FALSE, FALSE, 0);
    
    // Password field
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), password_label, FALSE, FALSE, 0);
    
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Min 8 chars, uppercase, lowercase, number, special");
    gtk_box_pack_start(GTK_BOX(form_box), password_entry, FALSE, FALSE, 0);
    
    // Confirm password field
    GtkWidget *confirm_label = gtk_label_new("Confirm Password:");
    gtk_widget_set_halign(confirm_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), confirm_label, FALSE, FALSE, 0);
    
    GtkWidget *confirm_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(confirm_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(confirm_entry), "Re-enter password");
    gtk_box_pack_start(GTK_BOX(form_box), confirm_entry, FALSE, FALSE, 0);
    
    // Dialog message label
    GtkWidget *dialog_message = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(form_box), dialog_message, FALSE, FALSE, 5);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        const char *username = gtk_entry_get_text(GTK_ENTRY(username_entry));
        const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
        const char *password = gtk_entry_get_text(GTK_ENTRY(password_entry));
        const char *confirm = gtk_entry_get_text(GTK_ENTRY(confirm_entry));
        
        // Validate input
        if (strlen(username) == 0 || strlen(email) == 0 || strlen(password) == 0) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>All fields are required</span>");
            gtk_widget_destroy(dialog);
            return;
        }
        
        // Check password match
        if (strcmp(password, confirm) != 0) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Passwords do not match</span>");
            gtk_widget_destroy(dialog);
            return;
        }
        
        // Attempt registration
        int result = auth_register_student(app_state.users, username, email, password);
        
        if (result == 1) {
            // Success
            save_all_data();
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='green'>Account created successfully! Please login.</span>");
        } else if (result == -1) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Invalid email format</span>");
        } else if (result == -2) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Invalid username (3-50 chars, alphanumeric + underscore)</span>");
        } else if (result == -3) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Weak password (need 8+ chars, upper, lower, number, special)</span>");
        } else if (result == -4) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Username already exists</span>");
        } else if (result == -5) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Email already registered</span>");
        } else {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Registration failed</span>");
        }
    }
    
    gtk_widget_destroy(dialog);
}

/*
 * Forgot Password button handler
 */
static void on_forgot_password_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *message_label = GTK_WIDGET(data);
    
    // Create dialog for email input
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Reset Password",
        GTK_WINDOW(app_state.login_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Request Reset Code", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 400, 200);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 20);
    gtk_container_add(GTK_CONTAINER(content), form_box);
    
    // Email field
    GtkWidget *email_label = gtk_label_new("Enter your registered email:");
    gtk_widget_set_halign(email_label, GTK_ALIGN_START);
    gtk_box_pack_start(GTK_BOX(form_box), email_label, FALSE, FALSE, 0);
    
    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "your.email@example.com");
    gtk_box_pack_start(GTK_BOX(form_box), email_entry, FALSE, FALSE, 0);
    
    // Dialog message
    GtkWidget *dialog_message = gtk_label_new("");
    gtk_box_pack_start(GTK_BOX(form_box), dialog_message, FALSE, FALSE, 5);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
        
        if (strlen(email) == 0) {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Email is required</span>");
            gtk_widget_destroy(dialog);
            return;
        }
        
        // Generate reset code
        char reset_code[10];
        int result = auth_request_password_reset(app_state.users, email, reset_code);
        
        if (result == 1) {
            // Show reset code (in real app, this would be emailed)
            gtk_widget_destroy(dialog);
            
            // Create new dialog for code and new password
            GtkWidget *reset_dialog = gtk_dialog_new_with_buttons("Enter Reset Code",
                GTK_WINDOW(app_state.login_window),
                GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
                "Cancel", GTK_RESPONSE_CANCEL,
                "Reset Password", GTK_RESPONSE_ACCEPT,
                NULL);
            
            gtk_window_set_default_size(GTK_WINDOW(reset_dialog), 400, 300);
            
            GtkWidget *reset_content = gtk_dialog_get_content_area(GTK_DIALOG(reset_dialog));
            GtkWidget *reset_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
            gtk_container_set_border_width(GTK_CONTAINER(reset_box), 20);
            gtk_container_add(GTK_CONTAINER(reset_content), reset_box);
            
            // Show the code (in real app, would email it)
            char code_msg[200];
            snprintf(code_msg, sizeof(code_msg), 
                "Your reset code is: <b>%s</b>\n(In production, this would be emailed to you)", 
                reset_code);
            GtkWidget *code_label = gtk_label_new(NULL);
            gtk_label_set_markup(GTK_LABEL(code_label), code_msg);
            gtk_box_pack_start(GTK_BOX(reset_box), code_label, FALSE, FALSE, 10);
            
            // Code entry
            GtkWidget *code_label2 = gtk_label_new("Enter Reset Code:");
            gtk_widget_set_halign(code_label2, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(reset_box), code_label2, FALSE, FALSE, 0);
            
            GtkWidget *code_entry = gtk_entry_new();
            gtk_entry_set_placeholder_text(GTK_ENTRY(code_entry), "6-digit code");
            gtk_box_pack_start(GTK_BOX(reset_box), code_entry, FALSE, FALSE, 0);
            
            // New password
            GtkWidget *new_password_label = gtk_label_new("New Password:");
            gtk_widget_set_halign(new_password_label, GTK_ALIGN_START);
            gtk_box_pack_start(GTK_BOX(reset_box), new_password_label, FALSE, FALSE, 0);
            
            GtkWidget *new_password_entry = gtk_entry_new();
            gtk_entry_set_visibility(GTK_ENTRY(new_password_entry), FALSE);
            gtk_entry_set_placeholder_text(GTK_ENTRY(new_password_entry), "Enter new password");
            gtk_box_pack_start(GTK_BOX(reset_box), new_password_entry, FALSE, FALSE, 0);
            
            gtk_widget_show_all(reset_dialog);
            
            int reset_response = gtk_dialog_run(GTK_DIALOG(reset_dialog));
            
            if (reset_response == GTK_RESPONSE_ACCEPT) {
                const char *code = gtk_entry_get_text(GTK_ENTRY(code_entry));
                const char *new_password = gtk_entry_get_text(GTK_ENTRY(new_password_entry));
                
                // Verify code and reset password
                int reset_result = auth_reset_password_with_code(app_state.users, 
                    email, code, new_password);
                
                if (reset_result == 1) {
                    save_all_data();
                    gtk_label_set_markup(GTK_LABEL(message_label), 
                        "<span color='green'>Password reset successfully! Please login.</span>");
                } else if (reset_result == -2) {
                    gtk_label_set_markup(GTK_LABEL(message_label), 
                        "<span color='red'>Invalid reset code</span>");
                } else if (reset_result == -3) {
                    gtk_label_set_markup(GTK_LABEL(message_label), 
                        "<span color='red'>Weak password</span>");
                } else {
                    gtk_label_set_markup(GTK_LABEL(message_label), 
                        "<span color='red'>Password reset failed</span>");
                }
            }
            
            gtk_widget_destroy(reset_dialog);
        } else {
            gtk_label_set_markup(GTK_LABEL(message_label), 
                "<span color='red'>Email not found</span>");
        }
    }
    
    gtk_widget_destroy(dialog);
}

/*
 * Create and show login window
 */
static void show_login_window(void) {
    if (app_state.login_window) {
        gtk_widget_show_all(app_state.login_window);
        return;
    }
    
    // Create window - Professional desktop layout
    app_state.login_window = gtk_application_window_new(app_state.app);
    gtk_window_set_title(GTK_WINDOW(app_state.login_window), "Student Management System - Login");
    gtk_window_set_default_size(GTK_WINDOW(app_state.login_window), 1400, 800);
    gtk_window_set_position(GTK_WINDOW(app_state.login_window), GTK_WIN_POS_CENTER);
    
    // Apply blue gradient background to entire window
    const char *window_css = 
        "window { background: linear-gradient(135deg, #2196F3 0%, #1976D2 100%); }";
    GtkCssProvider *window_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(window_provider, window_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(app_state.login_window),
                                   GTK_STYLE_PROVIDER(window_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(window_provider);
    
    // Create horizontal split layout - Two equal panels filling entire window
    GtkWidget *split_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_hexpand(split_box, TRUE);
    gtk_widget_set_vexpand(split_box, TRUE);
    gtk_container_add(GTK_CONTAINER(app_state.login_window), split_box);
    
    // LEFT PANEL: Background Image (50% width) - Expand to fill with blue borders
    GtkWidget *image_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(image_panel, 700, -1);
    gtk_widget_set_hexpand(image_panel, TRUE);
    gtk_widget_set_vexpand(image_panel, TRUE);
    
    // Make image panel transparent to show blue background
    const char *image_panel_css = 
        "box { background: transparent; }";
    GtkCssProvider *image_panel_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(image_panel_provider, image_panel_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(image_panel),
                                   GTK_STYLE_PROVIDER(image_panel_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(image_panel_provider);
    
    GtkWidget *background_image = gtk_image_new_from_file("src/back.jpg");
    if (gtk_image_get_pixbuf(GTK_IMAGE(background_image))) {
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(background_image));
        // Scale smaller to show blue borders
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 640, 740, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(background_image), scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
    }
    
    // Add rounded corners to image
    const char *image_css = 
        "image { border-radius: 20px; }";
    GtkCssProvider *image_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(image_provider, image_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(background_image),
                                   GTK_STYLE_PROVIDER(image_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(image_provider);
    
    gtk_widget_set_halign(background_image, GTK_ALIGN_CENTER);
    gtk_widget_set_valign(background_image, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(background_image, 30);
    gtk_widget_set_margin_end(background_image, 30);
    gtk_widget_set_margin_top(background_image, 30);
    gtk_widget_set_margin_bottom(background_image, 30);
    gtk_box_pack_start(GTK_BOX(image_panel), background_image, TRUE, TRUE, 0);
    gtk_box_pack_start(GTK_BOX(split_box), image_panel, TRUE, TRUE, 0);
    
    // VERTICAL SEPARATOR between image and login panels
    GtkWidget *vertical_sep = gtk_separator_new(GTK_ORIENTATION_VERTICAL);
    const char *separator_css = 
        "separator { background: rgba(255, 255, 255, 0.3); min-width: 2px; }";
    GtkCssProvider *separator_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(separator_provider, separator_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(vertical_sep),
                                   GTK_STYLE_PROVIDER(separator_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(separator_provider);
    gtk_box_pack_start(GTK_BOX(split_box), vertical_sep, FALSE, FALSE, 0);
    
    // RIGHT PANEL: Login Form (50% width) - Expand to fill, transparent background
    GtkWidget *login_panel = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(login_panel, 700, -1);
    gtk_widget_set_hexpand(login_panel, TRUE);
    gtk_widget_set_vexpand(login_panel, TRUE);
    
    // Make panel transparent to show window gradient
    const char *panel_css = 
        "box { background: transparent; }";
    GtkCssProvider *panel_provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(panel_provider, panel_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(login_panel),
                                   GTK_STYLE_PROVIDER(panel_provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    g_object_unref(panel_provider);
    
    gtk_box_pack_start(GTK_BOX(split_box), login_panel, TRUE, TRUE, 0);
    
    // Center container with fixed width and vertical centering
    GtkWidget *center_align = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0);
    gtk_widget_set_valign(center_align, GTK_ALIGN_CENTER);
    gtk_widget_set_halign(center_align, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(login_panel), center_align, TRUE, FALSE, 0);
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_widget_set_size_request(main_box, 480, -1);
    gtk_box_pack_start(GTK_BOX(center_align), main_box, FALSE, FALSE, 0);
    
    // Logo - 15px margin bottom
    GtkWidget *logo = gtk_image_new_from_file("src/t1.png");
    if (gtk_image_get_pixbuf(GTK_IMAGE(logo))) {
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(logo));
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 90, 90, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(logo), scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
    }
    gtk_widget_set_halign(logo, GTK_ALIGN_CENTER);
    gtk_widget_set_margin_bottom(logo, 15);
    gtk_box_pack_start(GTK_BOX(main_box), logo, FALSE, FALSE, 0);
    
    // Title - 8px margin bottom
    GtkWidget *title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(title), 
        "<span size='26000' weight='bold' foreground='white' line_height='1.2'>Student\nManagement\nSystem</span>");
    gtk_widget_set_halign(title, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(title), GTK_JUSTIFY_CENTER);
    gtk_widget_set_margin_bottom(title, 8);
    gtk_box_pack_start(GTK_BOX(main_box), title, FALSE, FALSE, 0);
    
    // Subtitle - 20px margin bottom
    GtkWidget *subtitle = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle), 
        "<span size='10500' foreground='white'>Welcome! Please select your role and\nlogin to continue</span>");
    gtk_widget_set_halign(subtitle, GTK_ALIGN_CENTER);
    gtk_label_set_justify(GTK_LABEL(subtitle), GTK_JUSTIFY_CENTER);
    gtk_widget_set_margin_bottom(subtitle, 20);
    gtk_box_pack_start(GTK_BOX(main_box), subtitle, FALSE, FALSE, 0);
    
    // Role selection - 12px margin bottom
    GtkWidget *role_frame = gtk_frame_new("Select Role");
    gtk_widget_set_margin_bottom(role_frame, 12);
    gtk_box_pack_start(GTK_BOX(main_box), role_frame, FALSE, FALSE, 0);
    
    GtkWidget *role_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(role_box), 16);
    gtk_container_add(GTK_CONTAINER(role_frame), role_box);
    
    // Student button - Equal width
    GtkWidget *student_btn = gtk_button_new_with_label("Student");
    gtk_widget_set_size_request(student_btn, 140, 60);
    g_signal_connect(student_btn, "clicked", G_CALLBACK(on_role_selected), 
                     GINT_TO_POINTER(ROLE_STUDENT));
    gtk_box_pack_start(GTK_BOX(role_box), student_btn, TRUE, TRUE, 0);
    
    // Teacher button - Equal width
    GtkWidget *teacher_btn = gtk_button_new_with_label("Professor");
    gtk_widget_set_size_request(teacher_btn, 140, 60);
    g_signal_connect(teacher_btn, "clicked", G_CALLBACK(on_role_selected), 
                     GINT_TO_POINTER(ROLE_TEACHER));
    gtk_box_pack_start(GTK_BOX(role_box), teacher_btn, TRUE, TRUE, 0);
    
    // Admin button - Equal width
    GtkWidget *admin_btn = gtk_button_new_with_label("Admin");
    gtk_widget_set_size_request(admin_btn, 140, 60);
    g_signal_connect(admin_btn, "clicked", G_CALLBACK(on_role_selected), 
                     GINT_TO_POINTER(ROLE_ADMIN));
    gtk_box_pack_start(GTK_BOX(role_box), admin_btn, TRUE, TRUE, 0);
    
    // Set admin as default
    app_state.selected_role = ROLE_ADMIN;
    GtkStyleContext *context = gtk_widget_get_style_context(admin_btn);
    gtk_style_context_add_class(context, "suggested-action");
    
    // Login form - no bottom margin (forms already have spacing)
    GtkWidget *login_frame = gtk_frame_new("Login");
    gtk_box_pack_start(GTK_BOX(main_box), login_frame, FALSE, FALSE, 0);
    
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 12);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 18);
    gtk_container_add(GTK_CONTAINER(login_frame), form_box);
    
    // Username - 3px spacing after label
    GtkWidget *username_label = gtk_label_new("Username:");
    gtk_widget_set_halign(username_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(username_label, 3);
    gtk_box_pack_start(GTK_BOX(form_box), username_label, FALSE, FALSE, 0);
    
    GtkWidget *username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Enter username");
    gtk_widget_set_size_request(username_entry, -1, 40);
    gtk_box_pack_start(GTK_BOX(form_box), username_entry, FALSE, FALSE, 0);
    
    // Password - 3px spacing after label
    GtkWidget *password_label = gtk_label_new("Password:");
    gtk_widget_set_halign(password_label, GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(password_label, 3);
    gtk_box_pack_start(GTK_BOX(form_box), password_label, FALSE, FALSE, 0);
    
    GtkWidget *password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Enter password");
    gtk_widget_set_size_request(password_entry, -1, 40);
    gtk_box_pack_start(GTK_BOX(form_box), password_entry, FALSE, FALSE, 0);
    
    // Message label - 10px top margin
    GtkWidget *message_label = gtk_label_new("");
    gtk_label_set_markup(GTK_LABEL(message_label), "<span color='red'></span>");
    gtk_widget_set_margin_top(message_label, 10);
    gtk_box_pack_start(GTK_BOX(form_box), message_label, FALSE, FALSE, 0);
    
    // Login button - 45px height, 12px top margin
    GtkWidget *login_btn = gtk_button_new_with_label("Login");
    GtkStyleContext *btn_context = gtk_widget_get_style_context(login_btn);
    gtk_style_context_add_class(btn_context, "suggested-action");
    gtk_widget_set_size_request(login_btn, -1, 45);
    gtk_widget_set_margin_top(login_btn, 12);
    
    // Store widgets for callback
    static GtkWidget *entries[3];
    entries[0] = username_entry;
    entries[1] = password_entry;
    entries[2] = message_label;
    
    g_signal_connect(login_btn, "clicked", G_CALLBACK(on_login_clicked), entries);
    gtk_box_pack_start(GTK_BOX(form_box), login_btn, FALSE, FALSE, 0);
    
    // Separator - 18px margins
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_widget_set_margin_top(separator, 18);
    gtk_widget_set_margin_bottom(separator, 18);
    gtk_box_pack_start(GTK_BOX(form_box), separator, FALSE, FALSE, 0);
    
    // Button box for Create Account and Forgot Password - 12px spacing
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12);
    gtk_box_pack_start(GTK_BOX(form_box), button_box, FALSE, FALSE, 0);
    
    // Create Account button - Equal width 38px height
    GtkWidget *create_account_btn = gtk_button_new_with_label("Create Student Account");
    gtk_widget_set_size_request(create_account_btn, -1, 38);
    g_signal_connect(create_account_btn, "clicked", G_CALLBACK(on_create_account_clicked), message_label);
    gtk_box_pack_start(GTK_BOX(button_box), create_account_btn, TRUE, TRUE, 0);
    
    // Forgot Password button - Equal width 38px height
    GtkWidget *forgot_password_btn = gtk_button_new_with_label("Forgot Password?");
    gtk_widget_set_size_request(forgot_password_btn, -1, 38);
    g_signal_connect(forgot_password_btn, "clicked", G_CALLBACK(on_forgot_password_clicked), message_label);
    gtk_box_pack_start(GTK_BOX(button_box), forgot_password_btn, TRUE, TRUE, 0);
    
    // Info label - 15px top margin, subtle styling
    GtkWidget *info_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(info_label), 
        "<span size='9000' foreground='#B8B8B8'>Default Admin: username=admin, password=Admin123!</span>");
    gtk_widget_set_halign(info_label, GTK_ALIGN_CENTER);
    gtk_label_set_line_wrap(GTK_LABEL(info_label), TRUE);
    gtk_widget_set_margin_top(info_label, 15);
    gtk_box_pack_start(GTK_BOX(main_box), info_label, FALSE, FALSE, 0);
    
    // Connect window close event
    g_signal_connect(app_state.login_window, "destroy", G_CALLBACK(gtk_main_quit), NULL);
    
    gtk_widget_show_all(app_state.login_window);
}

/*
 * Create main application window after login
 */
static void show_main_window(void) {
    // Create window
    app_state.main_window = gtk_application_window_new(app_state.app);
    gtk_window_set_title(GTK_WINDOW(app_state.main_window), "Student Management System");
    gtk_window_set_default_size(GTK_WINDOW(app_state.main_window), 850, 650);
    gtk_window_set_position(GTK_WINDOW(app_state.main_window), GTK_WIN_POS_CENTER);
    gtk_window_set_resizable(GTK_WINDOW(app_state.main_window), TRUE);
    
    // Set minimum window size to ensure usability
    GdkGeometry hints;
    hints.min_width = 800;
    hints.min_height = 500;
    gtk_window_set_geometry_hints(GTK_WINDOW(app_state.main_window), NULL, &hints, GDK_HINT_MIN_SIZE);
    
    // Apply modern theme CSS - DISABLED to use custom CSS instead
    // ThemeConfig* theme_config = theme_config_create();
    // if (theme_config) {
    //     char* css = theme_generate_css(theme_config);
    //     if (css) {
    //         theme_apply_css(app_state.main_window, css);
    //         free(css);
    //     }
    //     theme_config_destroy(theme_config);
    // }
    
    // Main container
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 0);
    gtk_container_add(GTK_CONTAINER(app_state.main_window), main_box);
    
    // Header bar
    GtkWidget *header_bar = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(header_bar), 10);
    gtk_box_pack_start(GTK_BOX(main_box), header_bar, FALSE, FALSE, 0);
    
    // Logo in header
    GtkWidget *logo = gtk_image_new_from_file("src/t1.png");
    if (gtk_image_get_pixbuf(GTK_IMAGE(logo))) {
        GdkPixbuf *pixbuf = gtk_image_get_pixbuf(GTK_IMAGE(logo));
        GdkPixbuf *scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, 40, 40, GDK_INTERP_BILINEAR);
        gtk_image_set_from_pixbuf(GTK_IMAGE(logo), scaled_pixbuf);
        g_object_unref(scaled_pixbuf);
        gtk_box_pack_start(GTK_BOX(header_bar), logo, FALSE, FALSE, 5);
    }
    
    // Welcome label
    char welcome_text[256];
    snprintf(welcome_text, sizeof(welcome_text), "Welcome, %s (%s)", 
             app_state.session->username, 
             auth_role_to_string(app_state.session->role));
    GtkWidget *welcome_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(welcome_label), 
        g_markup_printf_escaped("<span size='large' weight='bold'>%s</span>", welcome_text));
    gtk_box_pack_start(GTK_BOX(header_bar), welcome_label, TRUE, TRUE, 0);
    
    // Logout button
    GtkWidget *logout_btn = gtk_button_new_with_label("Logout");
    g_signal_connect(logout_btn, "clicked", G_CALLBACK(on_logout_clicked), NULL);
    gtk_box_pack_end(GTK_BOX(header_bar), logout_btn, FALSE, FALSE, 0);
    
    // Separator
    GtkWidget *separator = gtk_separator_new(GTK_ORIENTATION_HORIZONTAL);
    gtk_box_pack_start(GTK_BOX(main_box), separator, FALSE, FALSE, 0);
    
    // Scrolled window for content
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_NEVER, GTK_POLICY_AUTOMATIC);
    gtk_scrolled_window_set_min_content_height(GTK_SCROLLED_WINDOW(scrolled), 400);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Viewport for proper scrolling
    GtkWidget *viewport = gtk_viewport_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(scrolled), viewport);
    
    // Content area with gradient background
    GtkWidget *content_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 8);
    gtk_container_set_border_width(GTK_CONTAINER(content_box), 10);
    gtk_container_add(GTK_CONTAINER(viewport), content_box);
    
    // Apply vibrant blue gradient background for dashboard
    // Modern animated dashboard CSS with blue gradients
    const char *professional_css = 
        "* { font-family: 'Segoe UI', 'Inter', 'Roboto', sans-serif; }"
        "window { "
        "  background: linear-gradient(145deg, #1976D2 0%, #1E88E5 35%, #2196F3 70%, #42A5F5 100%); "
        "  background-size: 400% 400%; "
        "}"
        "box { background-color: transparent; }"
        
        "frame { "
        "  background: linear-gradient(145deg, rgba(255,255,255,0.7) 0%, rgba(227,242,253,0.8) 100%); "
        "  border-radius: 20px; "
        "  border: 2px solid rgba(100, 181, 246, 0.3); "
        "  box-shadow: 0 8px 32px rgba(100, 181, 246, 0.25), "
        "              0 4px 16px rgba(144, 202, 249, 0.2),"
        "              inset 0 1px 0 rgba(255, 255, 255, 0.6); "
        "  padding: 20px; "
        "  min-width: 170px; "
        "  min-height: 115px; "
        "  transition: all 400ms cubic-bezier(0.34, 1.56, 0.64, 1); "
        "}"
        
        "frame:hover { "
        "  background: linear-gradient(145deg, rgba(255,255,255,0.9) 0%, rgba(227,242,253,0.95) 100%); "
        "  box-shadow: 0 16px 48px rgba(100, 181, 246, 0.35), "
        "              0 8px 24px rgba(144, 202, 249, 0.25),"
        "              inset 0 2px 0 rgba(255, 255, 255, 0.8); "
        "  border-color: rgba(100, 181, 246, 0.5); "
        "}"
        
        "button { "
        "  background: linear-gradient(135deg, #42A5F5 0%, #2196F3 100%); "
        "  color: white; "
        "  border: none; "
        "  border-radius: 14px; "
        "  padding: 14px 28px; "
        "  font-weight: 700; "
        "  font-size: 14px; "
        "  min-height: 48px; "
        "  box-shadow: 0 6px 20px rgba(66, 165, 245, 0.35), "
        "              0 3px 10px rgba(33, 150, 243, 0.25); "
        "  transition: all 350ms cubic-bezier(0.34, 1.56, 0.64, 1); "
        "}"
        
        "button:hover { "
        "  background: linear-gradient(135deg, #64B5F6 0%, #42A5F5 100%); "
        "  box-shadow: 0 12px 36px rgba(100, 181, 246, 0.5), "
        "              0 6px 18px rgba(66, 165, 245, 0.35); "
        "}"
        
        "label { color: #1F2937; }";
    
    GtkCssProvider *provider = gtk_css_provider_new();
    gtk_css_provider_load_from_data(provider, professional_css, -1, NULL);
    gtk_style_context_add_provider(gtk_widget_get_style_context(content_box),
                                   GTK_STYLE_PROVIDER(provider),
                                   GTK_STYLE_PROVIDER_PRIORITY_APPLICATION);
    
    // Enhanced Dashboard title with role and decorative elements
    GtkWidget *title_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 2);
    gtk_box_pack_start(GTK_BOX(content_box), title_box, FALSE, FALSE, 5);
    
    // Welcome badge - smaller
    GtkWidget *welcome_badge = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(welcome_badge), 
        "<span size='8000' weight='bold' foreground='#0D47A1'>✨ WELCOME</span>");
    gtk_box_pack_start(GTK_BOX(title_box), welcome_badge, FALSE, FALSE, 0);
    
    GtkWidget *dashboard_title = gtk_label_new(NULL);
    if (app_state.session->role == ROLE_ADMIN) {
        gtk_label_set_markup(GTK_LABEL(dashboard_title), 
            "<span size='18000' weight='bold' foreground='#0D47A1'>👑 Administrator Dashboard</span>");
    } else if (app_state.session->role == ROLE_TEACHER) {
        gtk_label_set_markup(GTK_LABEL(dashboard_title), 
            "<span size='18000' weight='bold' foreground='#0D47A1'>🎓 Professor Dashboard</span>");
    } else {
        gtk_label_set_markup(GTK_LABEL(dashboard_title), 
            "<span size='18000' weight='bold' foreground='#0D47A1'>📚 Student Dashboard</span>");
    }
    gtk_box_pack_start(GTK_BOX(title_box), dashboard_title, FALSE, FALSE, 3);
    
    GtkWidget *subtitle = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(subtitle), 
        "<span size='9000' foreground='#1565C0'>Manage your academic activities efficiently</span>");
    gtk_box_pack_start(GTK_BOX(title_box), subtitle, FALSE, FALSE, 0);
    
    // Stats Cards Section (horizontal grid with modern design)
    GtkWidget *stats_grid = gtk_grid_new();
    gtk_grid_set_column_spacing(GTK_GRID(stats_grid), 12);
    gtk_grid_set_row_spacing(GTK_GRID(stats_grid), 12);
    gtk_widget_set_halign(stats_grid, GTK_ALIGN_CENTER);
    gtk_box_pack_start(GTK_BOX(content_box), stats_grid, FALSE, FALSE, 10);
    
    // Create stat cards based on role
    if (app_state.session->role == ROLE_ADMIN) {
        // Users Card with professional design
        GtkWidget *users_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(users_card, 150, 95);
        GtkWidget *users_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_container_set_border_width(GTK_CONTAINER(users_box), 12);
        gtk_container_add(GTK_CONTAINER(users_card), users_box);
        gtk_widget_set_halign(users_box, GTK_ALIGN_CENTER);
        
        GtkWidget *users_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(users_icon), "<span size=\"26000\">👥</span>");
        gtk_box_pack_start(GTK_BOX(users_box), users_icon, FALSE, FALSE, 2);
        
        char users_text[128];
        snprintf(users_text, sizeof(users_text), "<span size=\"28000\" weight=\"900\" foreground=\"#2196F3\">%d</span>", app_state.users->count);
        GtkWidget *users_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(users_count), users_text);
        gtk_box_pack_start(GTK_BOX(users_box), users_count, FALSE, FALSE, 4);
        
        GtkWidget *users_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(users_label), "<span size=\"9500\" weight=\"bold\" foreground=\"#6B7280\">TOTAL USERS</span>");
        gtk_box_pack_start(GTK_BOX(users_box), users_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), users_card, 0, 0, 1, 1);
        
        // Students Card with professional design
        GtkWidget *students_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(students_card, 150, 95);
        GtkWidget *students_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_container_set_border_width(GTK_CONTAINER(students_box), 12);
        gtk_container_add(GTK_CONTAINER(students_card), students_box);
        gtk_widget_set_halign(students_box, GTK_ALIGN_CENTER);
        
        GtkWidget *students_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(students_icon), "<span size=\"26000\">🎓</span>");
        gtk_box_pack_start(GTK_BOX(students_box), students_icon, FALSE, FALSE, 2);
        
        char students_text[128];
        snprintf(students_text, sizeof(students_text), "<span size=\"28000\" weight=\"900\" foreground=\"#1976D2\">%d</span>", app_state.students->count);
        GtkWidget *students_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(students_count), students_text);
        gtk_box_pack_start(GTK_BOX(students_box), students_count, FALSE, FALSE, 4);
        
        GtkWidget *students_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(students_label), "<span size=\"9500\" weight=\"bold\" foreground=\"#6B7280\">STUDENTS</span>");
        gtk_box_pack_start(GTK_BOX(students_box), students_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), students_card, 1, 0, 1, 1);
        
        // Grades Card with professional design
        GtkWidget *grades_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(grades_card, 150, 95);
        GtkWidget *grades_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_container_set_border_width(GTK_CONTAINER(grades_box), 12);
        gtk_container_add(GTK_CONTAINER(grades_card), grades_box);
        gtk_widget_set_halign(grades_box, GTK_ALIGN_CENTER);
        
        GtkWidget *grades_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(grades_icon), "<span size=\"26000\">📝</span>");
        gtk_box_pack_start(GTK_BOX(grades_box), grades_icon, FALSE, FALSE, 2);
        
        char grades_text[128];
        snprintf(grades_text, sizeof(grades_text), "<span size=\"28000\" weight=\"900\" foreground=\"#1E88E5\">%d</span>", app_state.grades->count);
        GtkWidget *grades_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(grades_count), grades_text);
        gtk_box_pack_start(GTK_BOX(grades_box), grades_count, FALSE, FALSE, 4);
        
        GtkWidget *grades_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(grades_label), "<span size=\"9500\" weight=\"bold\" foreground=\"#6B7280\">TOTAL GRADES</span>");
        gtk_box_pack_start(GTK_BOX(grades_box), grades_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), grades_card, 2, 0, 1, 1);
        
        // Clubs Card with professional design
        GtkWidget *clubs_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(clubs_card, 150, 95);
        GtkWidget *clubs_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        gtk_container_set_border_width(GTK_CONTAINER(clubs_box), 12);
        gtk_container_add(GTK_CONTAINER(clubs_card), clubs_box);
        gtk_widget_set_halign(clubs_box, GTK_ALIGN_CENTER);
        
        GtkWidget *clubs_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(clubs_icon), "<span size=\"26000\">🎯</span>");
        gtk_box_pack_start(GTK_BOX(clubs_box), clubs_icon, FALSE, FALSE, 2);
        
        char clubs_text[128];
        snprintf(clubs_text, sizeof(clubs_text), "<span size=\"28000\" weight=\"900\" foreground=\"#1565C0\">%d</span>", app_state.clubs->count);
        GtkWidget *clubs_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(clubs_count), clubs_text);
        gtk_box_pack_start(GTK_BOX(clubs_box), clubs_count, FALSE, FALSE, 4);
        
        GtkWidget *clubs_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(clubs_label), "<span size=\"9500\" weight=\"bold\" foreground=\"#6B7280\">ACTIVE CLUBS</span>");
        gtk_box_pack_start(GTK_BOX(clubs_box), clubs_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), clubs_card, 3, 0, 1, 1);
    } else if (app_state.session->role == ROLE_TEACHER) {
        // Similar stat cards for professor
        GtkWidget *students_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(students_card, 220, 120);
        GtkWidget *students_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_set_border_width(GTK_CONTAINER(students_box), 15);
        gtk_container_add(GTK_CONTAINER(students_card), students_box);
        
        GtkWidget *students_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(students_icon), "<span size='30000'>👨‍🎓</span>");
        gtk_box_pack_start(GTK_BOX(students_box), students_icon, FALSE, FALSE, 0);
        
        char students_text[64];
        snprintf(students_text, sizeof(students_text), "<span size='24000' weight='bold'>%d</span>", app_state.students->count);
        GtkWidget *students_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(students_count), students_text);
        gtk_box_pack_start(GTK_BOX(students_box), students_count, FALSE, FALSE, 0);
        
        GtkWidget *students_label = gtk_label_new("My Students");
        gtk_box_pack_start(GTK_BOX(students_box), students_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), students_card, 0, 0, 1, 1);
        
        GtkWidget *grades_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(grades_card, 220, 120);
        GtkWidget *grades_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_set_border_width(GTK_CONTAINER(grades_box), 15);
        gtk_container_add(GTK_CONTAINER(grades_card), grades_box);
        
        GtkWidget *grades_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(grades_icon), "<span size='30000'>📊</span>");
        gtk_box_pack_start(GTK_BOX(grades_box), grades_icon, FALSE, FALSE, 0);
        
        char grades_text[64];
        snprintf(grades_text, sizeof(grades_text), "<span size='24000' weight='bold'>%d</span>", app_state.grades->count);
        GtkWidget *grades_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(grades_count), grades_text);
        gtk_box_pack_start(GTK_BOX(grades_box), grades_count, FALSE, FALSE, 0);
        
        GtkWidget *grades_label = gtk_label_new("Grades Given");
        gtk_box_pack_start(GTK_BOX(grades_box), grades_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), grades_card, 1, 0, 1, 1);
        
        GtkWidget *attendance_card = gtk_frame_new(NULL);
        gtk_widget_set_size_request(attendance_card, 220, 120);
        GtkWidget *attendance_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
        gtk_container_set_border_width(GTK_CONTAINER(attendance_box), 15);
        gtk_container_add(GTK_CONTAINER(attendance_card), attendance_box);
        
        GtkWidget *attendance_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(attendance_icon), "<span size='30000'>✅</span>");
        gtk_box_pack_start(GTK_BOX(attendance_box), attendance_icon, FALSE, FALSE, 0);
        
        char attendance_text[64];
        snprintf(attendance_text, sizeof(attendance_text), "<span size='24000' weight='bold'>%d</span>", app_state.attendance->count);
        GtkWidget *attendance_count = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(attendance_count), attendance_text);
        gtk_box_pack_start(GTK_BOX(attendance_box), attendance_count, FALSE, FALSE, 0);
        
        GtkWidget *attendance_label = gtk_label_new("Attendance");
        gtk_box_pack_start(GTK_BOX(attendance_box), attendance_label, FALSE, FALSE, 0);
        gtk_grid_attach(GTK_GRID(stats_grid), attendance_card, 2, 0, 1, 1);
    }
    
    // Quick Actions Section Title - Professional Design
    GtkWidget *actions_header_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 3);
    gtk_box_pack_start(GTK_BOX(content_box), actions_header_box, FALSE, FALSE, 8);
    
    GtkWidget *actions_title = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(actions_title), 
        "<span size='14000' weight='900' foreground='#0D47A1' letter_spacing='500'>⚡ QUICK ACTIONS</span>");
    gtk_box_pack_start(GTK_BOX(actions_header_box), actions_title, FALSE, FALSE, 0);
    
    GtkWidget *actions_subtitle = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(actions_subtitle), 
        "<span size='8500' foreground='#1565C0'>Perform common tasks quickly and efficiently</span>");
    gtk_box_pack_start(GTK_BOX(actions_header_box), actions_subtitle, FALSE, FALSE, 0);
    
    // Management buttons - Admin Dashboard with Asymmetric Layout
    if (app_state.session->role == ROLE_ADMIN) {
        GtkWidget *button_grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(button_grid), 10);
        gtk_grid_set_column_spacing(GTK_GRID(button_grid), 10);
        gtk_widget_set_halign(button_grid, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(content_box), button_grid, FALSE, FALSE, 5);
        
        // Create interactive gradient action buttons with modern asymmetric design
        const char *button_css =
            "button {"
            "  background: linear-gradient(135deg, rgba(144,202,249,0.3) 0%, rgba(100,181,246,0.4) 100%);"
            "  color: #0D47A1;"
            "  border-radius: 22px;"
            "  border: 2px solid rgba(100, 181, 246, 0.4);"
            "  font-size: 15px;"
            "  font-weight: 800;"
            "  min-height: 110px;"
            "  min-width: 260px;"
            "  padding: 28px;"
            "  box-shadow: 0 10px 40px rgba(13, 71, 161, 0.15),"
            "              0 6px 20px rgba(25, 118, 210, 0.12),"
            "              inset 0 2px 0 rgba(255,255,255,0.4),"
            "              inset 0 -2px 10px rgba(100, 181, 246, 0.2);"
            "  transition: all 450ms cubic-bezier(0.34, 1.56, 0.64, 1);"
            "}"
            "button:hover {"
            "  background: linear-gradient(135deg, rgba(227,242,253,0.95) 0%, rgba(187,222,251,0.98) 100%);"
            "  color: #0D47A1;"
            "  border-color: rgba(144, 202, 249, 0.6);"
            "  box-shadow: 0 20px 60px rgba(100, 181, 246, 0.4),"
            "              0 12px 30px rgba(144, 202, 249, 0.3),"
            "              inset 0 2px 0 rgba(255,255,255,0.6),"
            "              0 0 0 4px rgba(144, 202, 249, 0.3);"
            "  transform: translateY(-8px) scale(1.05);"
            "}"
            "button:active {"
            "  transform: translateY(-4px) scale(1.02);"
            "  box-shadow: 0 12px 40px rgba(100, 181, 246, 0.3),"
            "              0 6px 20px rgba(144, 202, 249, 0.2);"
            "}";
        
        GtkCssProvider *btn_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_provider, button_css, -1, NULL);
        
        // Student Management button - Large Featured Card
        GtkWidget *student_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *student_btn = gtk_button_new();
        gtk_widget_set_size_request(student_btn, 210, 75);
        gtk_container_add(GTK_CONTAINER(student_btn), student_btn_box);
        
        GtkWidget *student_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_icon), "<span size='35000'>📚</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_icon, FALSE, FALSE, 2);
        
        GtkWidget *student_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_label), "<span weight='800' size='12500' foreground='#0D47A1'>Manage Students</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_label, FALSE, FALSE, 3);
        
        GtkWidget *student_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_desc), "<span size='8500' foreground='#546E7A'>Add, Edit, View &amp; Export</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(student_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(student_btn, "clicked", G_CALLBACK(on_manage_students_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), student_btn, 0, 0, 3, 1);
        
        // Club Management button - Larger Card
        GtkWidget *club_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *club_btn = gtk_button_new();
        gtk_widget_set_size_request(club_btn, 320, 85);
        gtk_container_add(GTK_CONTAINER(club_btn), club_btn_box);
        
        GtkWidget *club_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(club_icon), "<span size='40000'>🎭</span>");
        gtk_box_pack_start(GTK_BOX(club_btn_box), club_icon, FALSE, FALSE, 3);
        
        GtkWidget *club_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(club_label), "<span weight='800' size='14000' foreground='#2196F3'>Manage Clubs</span>");
        gtk_box_pack_start(GTK_BOX(club_btn_box), club_label, FALSE, FALSE, 4);
        
        GtkWidget *club_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(club_desc), "<span size='9500' foreground='#546E7A'>Create &amp; Manage Clubs</span>");
        gtk_box_pack_start(GTK_BOX(club_btn_box), club_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(club_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(club_btn, "clicked", G_CALLBACK(on_manage_clubs_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), club_btn, 0, 1, 1, 1);
        
        // Admin View button - Larger Card
        GtkWidget *admin_view_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *admin_view_btn = gtk_button_new();
        gtk_widget_set_size_request(admin_view_btn, 320, 85);
        gtk_container_add(GTK_CONTAINER(admin_view_btn), admin_view_btn_box);
        
        GtkWidget *admin_view_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(admin_view_icon), "<span size='40000'>🔐</span>");
        gtk_box_pack_start(GTK_BOX(admin_view_btn_box), admin_view_icon, FALSE, FALSE, 3);
        
        GtkWidget *admin_view_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(admin_view_label), "<span weight='800' size='14000' foreground='#0D47A1'>Admin View</span>");
        gtk_box_pack_start(GTK_BOX(admin_view_btn_box), admin_view_label, FALSE, FALSE, 4);
        
        GtkWidget *admin_view_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(admin_view_desc), "<span size='9500' foreground='#546E7A'>View All Users &amp; Professors</span>");
        gtk_box_pack_start(GTK_BOX(admin_view_btn_box), admin_view_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(admin_view_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(admin_view_btn, "clicked", G_CALLBACK(on_admin_view_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), admin_view_btn, 1, 1, 2, 1);
        
        // Modules Management button - Row 2 (Admin only)
        GtkWidget *modules_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 4);
        GtkWidget *modules_btn = gtk_button_new();
        gtk_widget_set_size_request(modules_btn, 320, 85);
        gtk_container_add(GTK_CONTAINER(modules_btn), modules_btn_box);
        
        GtkWidget *modules_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_icon), "<span size='40000'>📖</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_icon, FALSE, FALSE, 3);
        
        GtkWidget *modules_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_label), "<span weight='800' size='14000' foreground='#1565C0'>Manage Modules</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_label, FALSE, FALSE, 4);
        
        GtkWidget *modules_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_desc), "<span size='9500' foreground='#546E7A'>View &amp; Manage Courses</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(modules_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(modules_btn, "clicked", G_CALLBACK(on_manage_modules_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), modules_btn, 0, 2, 3, 1);
        
        g_object_unref(btn_provider);
        
    } else if (app_state.session->role == ROLE_TEACHER) {
        // Professor Dashboard - Limited buttons (no Admin View)
        GtkWidget *button_grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(button_grid), 14);
        gtk_grid_set_column_spacing(GTK_GRID(button_grid), 14);
        gtk_widget_set_halign(button_grid, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(content_box), button_grid, TRUE, FALSE, 10);
        
        // Create professional action buttons with semi-transparent blue
        const char *button_css =
            "button {"
            "  background: linear-gradient(135deg, rgba(144,202,249,0.3) 0%, rgba(100,181,246,0.4) 100%);"
            "  color: #0D47A1;"
            "  border-radius: 14px;"
            "  border: 2px solid rgba(100, 181, 246, 0.4);"
            "  font-size: 13px;"
            "  font-weight: 600;"
            "  min-height: 70px;"
            "  min-width: 190px;"
            "  padding: 16px;"
            "  box-shadow: 0 6px 20px rgba(100, 181, 246, 0.25),"
            "              0 2px 8px rgba(144, 202, 249, 0.15);"
            "  transition: all 0.3s cubic-bezier(0.4, 0, 0.2, 1);"
            "}"
            "button:hover {"
            "  background: linear-gradient(135deg, rgba(227,242,253,0.95) 0%, rgba(187,222,251,0.98) 100%);"
            "  box-shadow: 0 10px 28px rgba(100, 181, 246, 0.35),"
            "              0 4px 12px rgba(144, 202, 249, 0.25);"
            "}"
            "button:active {"
            "  box-shadow: 0 4px 16px rgba(100, 181, 246, 0.25),"
            "              0 2px 6px rgba(144, 202, 249, 0.15);"
            "}";
        
        GtkCssProvider *btn_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_provider, button_css, -1, NULL);
        
        // View Students button - Professor can view but not add/delete
        GtkWidget *student_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *student_btn = gtk_button_new();
        gtk_widget_set_size_request(student_btn, 280, 95);
        gtk_container_add(GTK_CONTAINER(student_btn), student_btn_box);
        
        GtkWidget *student_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_icon), "<span size='35000'>👨‍🎓</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_icon, FALSE, FALSE, 2);
        
        GtkWidget *student_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_label), "<span weight='800' size='13000' foreground='#2196F3'>View Students</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_label, FALSE, FALSE, 3);
        
        GtkWidget *student_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(student_desc), "<span size='9000' foreground='#6B7280'>View Student Information</span>");
        gtk_box_pack_start(GTK_BOX(student_btn_box), student_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(student_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(student_btn, "clicked", G_CALLBACK(on_manage_students_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), student_btn, 0, 0, 1, 1);
        
        // Attendance Management button - Professor can take attendance
        GtkWidget *attendance_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *attendance_btn = gtk_button_new();
        gtk_widget_set_size_request(attendance_btn, 280, 95);
        gtk_container_add(GTK_CONTAINER(attendance_btn), attendance_btn_box);
        
        GtkWidget *attendance_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(attendance_icon), "<span size='35000'>✅</span>");
        gtk_box_pack_start(GTK_BOX(attendance_btn_box), attendance_icon, FALSE, FALSE, 2);
        
        GtkWidget *attendance_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(attendance_label), "<span weight='800' size='13000' foreground='#1976D2'>Take Attendance</span>");
        gtk_box_pack_start(GTK_BOX(attendance_btn_box), attendance_label, FALSE, FALSE, 3);
        
        GtkWidget *attendance_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(attendance_desc), "<span size='9000' foreground='#6B7280'>Track Student Presence</span>");
        gtk_box_pack_start(GTK_BOX(attendance_btn_box), attendance_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(attendance_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(attendance_btn, "clicked", G_CALLBACK(on_manage_attendance_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), attendance_btn, 1, 0, 1, 1);
        
        // Modules button - Row 1
        GtkWidget *modules_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *modules_btn = gtk_button_new();
        gtk_widget_set_size_request(modules_btn, 280, 95);
        gtk_container_add(GTK_CONTAINER(modules_btn), modules_btn_box);
        
        GtkWidget *modules_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_icon), "<span size='35000'>📖</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_icon, FALSE, FALSE, 2);
        
        GtkWidget *modules_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_label), "<span weight='800' size='13000' foreground='#1565C0'>My Modules</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_label, FALSE, FALSE, 3);
        
        GtkWidget *modules_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_desc), "<span size='9000' foreground='#6B7280'>View Course Modules</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(modules_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(modules_btn, "clicked", G_CALLBACK(on_manage_modules_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), modules_btn, 0, 1, 1, 1);
        
        // Exams button - Row 1
        GtkWidget *exams_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *exams_btn = gtk_button_new();
        gtk_widget_set_size_request(exams_btn, 280, 95);
        gtk_container_add(GTK_CONTAINER(exams_btn), exams_btn_box);
        
        GtkWidget *exams_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(exams_icon), "<span size='35000'>📄</span>");
        gtk_box_pack_start(GTK_BOX(exams_btn_box), exams_icon, FALSE, FALSE, 2);
        
        GtkWidget *exams_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(exams_label), "<span weight='800' size='13000' foreground='#2196F3'>Manage Exams</span>");
        gtk_box_pack_start(GTK_BOX(exams_btn_box), exams_label, FALSE, FALSE, 3);
        
        GtkWidget *exams_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(exams_desc), "<span size='9000' foreground='#6B7280'>Schedule Exams</span>");
        gtk_box_pack_start(GTK_BOX(exams_btn_box), exams_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(exams_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(exams_btn, "clicked", G_CALLBACK(on_manage_exams_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), exams_btn, 1, 1, 1, 1);
        
        // Student Notes button - Row 2
        GtkWidget *notes_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *notes_btn = gtk_button_new();
        gtk_widget_set_size_request(notes_btn, 280, 95);
        gtk_container_add(GTK_CONTAINER(notes_btn), notes_btn_box);
        
        GtkWidget *notes_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(notes_icon), "<span size='35000'>📝</span>");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_icon, FALSE, FALSE, 2);
        
        GtkWidget *notes_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(notes_label), "<span weight='800' size='13000' foreground='#2196F3'>Student Notes</span>");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_label, FALSE, FALSE, 3);
        
        GtkWidget *notes_desc = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(notes_desc), "<span size='9000' foreground='#6B7280'>Add &amp; View Notes</span>");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(notes_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(notes_btn, "clicked", G_CALLBACK(on_professor_text_notes_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), notes_btn, 0, 2, 2, 1);
;
        
        g_object_unref(btn_provider);
        
    } else if (app_state.session->role == ROLE_STUDENT) {
        // Student view buttons
        GtkWidget *button_grid = gtk_grid_new();
        gtk_grid_set_row_spacing(GTK_GRID(button_grid), 14);
        gtk_grid_set_column_spacing(GTK_GRID(button_grid), 14);
        gtk_widget_set_halign(button_grid, GTK_ALIGN_CENTER);
        gtk_box_pack_start(GTK_BOX(content_box), button_grid, TRUE, FALSE, 10);
        
        const char *button_css =
            "button {"
            "  background: linear-gradient(135deg, rgba(144,202,249,0.3) 0%, rgba(100,181,246,0.4) 100%);"
            "  color: #0D47A1;"
            "  border-radius: 14px;"
            "  border: 2px solid rgba(100, 181, 246, 0.4);"
            "  font-size: 13px;"
            "  font-weight: 600;"
            "  box-shadow: 0 4px 16px rgba(100, 181, 246, 0.25);"
            "}"
            "button:hover {"
            "  background: linear-gradient(135deg, rgba(227,242,253,0.95) 0%, rgba(187,222,251,0.98) 100%);"
            "  border: 2px solid rgba(144, 202, 249, 0.6);"
            "  box-shadow: 0 8px 24px rgba(100, 181, 246, 0.35);"
            "}";
        
        GtkCssProvider *btn_provider = gtk_css_provider_new();
        gtk_css_provider_load_from_data(btn_provider, button_css, -1, NULL);
        
        // View Modules button (read-only for students)
        GtkWidget *modules_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *modules_btn = gtk_button_new();
        gtk_widget_set_size_request(modules_btn, 200, 75);
        gtk_container_add(GTK_CONTAINER(modules_btn), modules_btn_box);
        
        GtkWidget *modules_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_icon), "<span size='24000'>📖</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_icon, FALSE, FALSE, 0);
        
        GtkWidget *modules_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(modules_label), "<span weight='700' size='11000'>My Modules</span>");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_label, FALSE, FALSE, 0);
        
        GtkWidget *modules_desc = gtk_label_new("View Course Modules");
        gtk_box_pack_start(GTK_BOX(modules_btn_box), modules_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(modules_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(modules_btn, "clicked", G_CALLBACK(on_manage_modules_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), modules_btn, 0, 0, 1, 1);
        
        // Join Clubs
        GtkWidget *clubs_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *clubs_btn = gtk_button_new();
        gtk_widget_set_size_request(clubs_btn, 200, 75);
        gtk_container_add(GTK_CONTAINER(clubs_btn), clubs_btn_box);
        
        GtkWidget *clubs_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(clubs_icon), "<span size='24000'>🎭</span>");
        gtk_box_pack_start(GTK_BOX(clubs_btn_box), clubs_icon, FALSE, FALSE, 0);
        
        GtkWidget *clubs_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(clubs_label), "<span weight='700' size='11000'>My Clubs</span>");
        gtk_box_pack_start(GTK_BOX(clubs_btn_box), clubs_label, FALSE, FALSE, 0);
        
        GtkWidget *clubs_desc = gtk_label_new("Join & View Club Activities");
        gtk_box_pack_start(GTK_BOX(clubs_btn_box), clubs_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(clubs_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(clubs_btn, "clicked", G_CALLBACK(on_manage_clubs_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), clubs_btn, 1, 0, 1, 1);
        
        // My Notes button
        GtkWidget *notes_btn_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 6);
        GtkWidget *notes_btn = gtk_button_new();
        gtk_widget_set_size_request(notes_btn, 200, 75);
        gtk_container_add(GTK_CONTAINER(notes_btn), notes_btn_box);
        
        GtkWidget *notes_icon = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(notes_icon), "<span size='24000'>📝</span>");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_icon, FALSE, FALSE, 0);
        
        GtkWidget *notes_label = gtk_label_new(NULL);
        gtk_label_set_markup(GTK_LABEL(notes_label), "<span weight='700' size='11000'>My Notes</span>");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_label, FALSE, FALSE, 0);
        
        GtkWidget *notes_desc = gtk_label_new("View Teacher Notes");
        gtk_box_pack_start(GTK_BOX(notes_btn_box), notes_desc, FALSE, FALSE, 0);
        
        gtk_style_context_add_provider(gtk_widget_get_style_context(notes_btn),
                                       GTK_STYLE_PROVIDER(btn_provider),
                                       GTK_STYLE_PROVIDER_PRIORITY_USER);
        g_signal_connect(notes_btn, "clicked", G_CALLBACK(on_student_view_notes_clicked), NULL);
        gtk_grid_attach(GTK_GRID(button_grid), notes_btn, 2, 0, 1, 1);
        
        g_object_unref(btn_provider);
    }
    
    // Connect window close event
    g_signal_connect(app_state.main_window, "destroy", G_CALLBACK(on_logout_clicked), NULL);
    
    gtk_widget_show_all(app_state.main_window);
}

/*
 * Management window callbacks
 */
static void on_manage_students_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Student Management window...\n");
    
    // Create UI state from app state
    UIState *ui_state = (UIState*)malloc(sizeof(UIState));
    if (!ui_state) return;
    
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_session = app_state.session;
    ui_state->users = app_state.users;
    ui_state->students = app_state.students;
    ui_state->grades = app_state.grades;
    ui_state->attendance = app_state.attendance;
    ui_state->clubs = app_state.clubs;
    ui_state->memberships = app_state.memberships;
    
    // Create and show student management window
    GtkWindow *student_window = ui_create_student_window(ui_state);
    if (student_window) {
        ui_state->current_window = student_window;
        
        // Populate with data
        GtkTreeView *treeview = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(student_window), "treeview"));
        if (treeview && app_state.students) {
            ui_student_treeview_populate(treeview, app_state.students);
        }
        
        gtk_window_set_transient_for(student_window, GTK_WINDOW(app_state.main_window));
        g_signal_connect(student_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &ui_state);
        gtk_widget_show_all(GTK_WIDGET(student_window));
    }
}

static void on_manage_grades_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Grade Management window...\n");
    
    UIState *ui_state = (UIState*)malloc(sizeof(UIState));
    if (!ui_state) return;
    
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_session = app_state.session;
    ui_state->users = app_state.users;
    ui_state->students = app_state.students;
    ui_state->grades = app_state.grades;
    ui_state->attendance = app_state.attendance;
    ui_state->clubs = app_state.clubs;
    ui_state->memberships = app_state.memberships;
    
    GtkWindow *grade_window = ui_create_grade_window(ui_state);
    if (grade_window) {
        ui_state->current_window = grade_window;
        gtk_window_set_transient_for(grade_window, GTK_WINDOW(app_state.main_window));
        g_signal_connect(grade_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &ui_state);
        gtk_widget_show_all(GTK_WIDGET(grade_window));
    }
}

static void on_manage_attendance_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Attendance Management window...\n");
    
    UIState *ui_state = (UIState*)malloc(sizeof(UIState));
    if (!ui_state) return;
    
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_session = app_state.session;
    ui_state->users = app_state.users;
    ui_state->students = app_state.students;
    ui_state->grades = app_state.grades;
    ui_state->attendance = app_state.attendance;
    ui_state->clubs = app_state.clubs;
    ui_state->memberships = app_state.memberships;
    
    GtkWindow *attendance_window = ui_create_attendance_window(ui_state);
    if (attendance_window) {
        ui_state->current_window = attendance_window;
        gtk_window_set_transient_for(attendance_window, GTK_WINDOW(app_state.main_window));
        g_signal_connect(attendance_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &ui_state);
        gtk_widget_show_all(GTK_WIDGET(attendance_window));
    }
}

static void on_manage_clubs_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Club Management window...\n");
    
    UIState *ui_state = (UIState*)malloc(sizeof(UIState));
    if (!ui_state) return;
    
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_session = app_state.session;
    ui_state->users = app_state.users;
    ui_state->students = app_state.students;
    ui_state->grades = app_state.grades;
    ui_state->attendance = app_state.attendance;
    ui_state->clubs = app_state.clubs;
    ui_state->memberships = app_state.memberships;
    
    // Set current_user from session - CRITICAL for role-based UI!
    if (app_state.session && app_state.session->user_id > 0 && app_state.users) {
        ui_state->current_user = user_list_find_by_id(app_state.users, app_state.session->user_id);
        printf("[DEBUG] Set current_user from session: %p (user_id=%d)\n", 
               (void*)ui_state->current_user, app_state.session->user_id);
    }
    
    GtkWindow *club_window = ui_create_club_window(ui_state);
    if (club_window) {
        ui_state->current_window = club_window;
        gtk_window_set_transient_for(club_window, GTK_WINDOW(app_state.main_window));
        g_signal_connect(club_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &ui_state);
        gtk_widget_show_all(GTK_WIDGET(club_window));
    }
}

static void on_admin_view_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Admin View window...\n");
    
    // Verify admin role
    if (!app_state.session || app_state.session->role != ROLE_ADMIN) {
        GtkWidget *dialog = gtk_message_dialog_new(
            GTK_WINDOW(app_state.main_window),
            GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
            GTK_MESSAGE_ERROR,
            GTK_BUTTONS_OK,
            "Access Denied: Admin privileges required");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    UIState *ui_state = (UIState*)malloc(sizeof(UIState));
    if (!ui_state) return;
    
    memset(ui_state, 0, sizeof(UIState));
    ui_state->current_session = app_state.session;
    ui_state->users = app_state.users;
    ui_state->students = app_state.students;
    ui_state->grades = app_state.grades;
    ui_state->attendance = app_state.attendance;
    ui_state->clubs = app_state.clubs;
    ui_state->memberships = app_state.memberships;
    
    // Set current_user from session
    if (app_state.session && app_state.session->user_id > 0 && app_state.users) {
        ui_state->current_user = user_list_find_by_id(app_state.users, app_state.session->user_id);
        printf("[DEBUG] Admin View - current_user: %p (user_id=%d)\n", 
               (void*)ui_state->current_user, app_state.session->user_id);
    }
    
    GtkWindow *admin_window = ui_create_admin_view_window(ui_state);
    if (admin_window) {
        ui_state->current_window = admin_window;
        gtk_window_set_transient_for(admin_window, GTK_WINDOW(app_state.main_window));
        g_signal_connect(admin_window, "destroy", G_CALLBACK(gtk_widget_destroyed), &ui_state);
        gtk_widget_show_all(GTK_WIDGET(admin_window));
    }
}

// Helper function to check if professor username matches assigned name
// username format: "m.addam" -> display format: "M. Addam"
static int professor_name_matches(const char *assigned_name, const char *username) {
    if (!assigned_name || !username) return 0;
    
    // Convert username "a.bahri3" to display format "A. Bahri"
    // Strip trailing numbers from username first
    char clean_username[100];
    strncpy(clean_username, username, sizeof(clean_username) - 1);
    clean_username[sizeof(clean_username) - 1] = '\0';
    
    // Remove trailing digits
    int len = strlen(clean_username);
    while (len > 0 && isdigit(clean_username[len - 1])) {
        clean_username[--len] = '\0';
    }
    
    // Convert to display format "M. Addam"
    char display_format[100];
    int i = 0, j = 0;
    
    // Copy and uppercase first char
    if (clean_username[i]) {
        display_format[j++] = toupper(clean_username[i++]);
    }
    
    // Copy dot
    if (clean_username[i] == '.') {
        display_format[j++] = clean_username[i++];
        display_format[j++] = ' '; // Add space after dot
    }
    
    // Copy rest, capitalizing first letter of last name
    if (clean_username[i]) {
        display_format[j++] = toupper(clean_username[i++]);
    }
    while (clean_username[i]) {
        display_format[j++] = clean_username[i++];
    }
    display_format[j] = '\0';
    
    // Debug output
    printf("[DEBUG] Comparing: '%s' with '%s' (from username '%s')\n", 
           assigned_name, display_format, username);
    
    return strcasecmp(assigned_name, display_format) == 0;
}

static void on_manage_modules_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Modules Management window...\n");
    
    // Get current logged-in user
    User *current_user = NULL;
    if (app_state.session && app_state.session->is_valid) {
        current_user = user_list_find_by_id(app_state.users, app_state.session->user_id);
    }
    
    // Create modules window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Modules Management");
    gtk_window_set_default_size(GTK_WINDOW(window), 1000, 550);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<span size='large' weight='bold'>Course Modules</span>");
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 5);
    
    // Count modules to display (filter by professor for teachers)
    int module_count = 0;
    if (app_state.modules) {
        if (current_user && current_user->role == ROLE_TEACHER) {
            // Count only modules for this professor
            for (int i = 0; i < app_state.modules->count; i++) {
                Module *m = &app_state.modules->cours[i];
                // Match professor name using helper function
                if (professor_name_matches(m->nom_prenom_enseignent, current_user->username)) {
                    module_count++;
                }
            }
        } else {
            // Admin sees all modules
            module_count = app_state.modules->count;
        }
    }
    
    // Info label
    char info[128];
    snprintf(info, sizeof(info), "Total modules: %d", module_count);
    GtkWidget *info_label = gtk_label_new(info);
    gtk_box_pack_start(GTK_BOX(main_box), info_label, FALSE, FALSE, 0);
    
    // Scrolled window with tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Create list store
    GtkListStore *store = gtk_list_store_new(8, 
        G_TYPE_INT,    // ID
        G_TYPE_STRING, // Name
        G_TYPE_STRING, // Description
        G_TYPE_INT,    // Hours Course
        G_TYPE_INT,    // Hours TD
        G_TYPE_INT,    // Hours TP
        G_TYPE_INT,    // Semester
        G_TYPE_STRING  // Professor
    );
    
    // Populate with actual data (filtered by professor for teachers)
    if (app_state.modules && app_state.modules->count > 0) {
        for (int i = 0; i < app_state.modules->count; i++) {
            Module *m = &app_state.modules->cours[i];
            
            // Filter by professor for teachers
            if (current_user && current_user->role == ROLE_TEACHER) {
                // Only show modules assigned to this professor
                if (!professor_name_matches(m->nom_prenom_enseignent, current_user->username)) {
                    continue; // Skip this module
                }
            }
            
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter,
                0, m->id,
                1, m->nom,
                2, m->description,
                3, m->heures_cours,
                4, m->heures_td,
                5, m->heures_tp,
                6, m->semestre,
                7, m->nom_prenom_enseignent,
                -1);
        }
    }
    
    // Create tree view
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Module Name", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Description", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "H.Cours", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "H.TD", renderer, "text", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "H.TP", renderer, "text", 5, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Sem", renderer, "text", 6, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Professor", renderer, "text", 7, NULL);
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    // Button box for action buttons
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    // Assign Professor button (Admin only)
    if (current_user && current_user->role == ROLE_ADMIN) {
        GtkWidget *assign_btn = gtk_button_new_with_label("Assign Professor to Module");
        
        // Pass tree_view as data
        g_signal_connect(assign_btn, "clicked", G_CALLBACK(on_assign_professor_clicked), tree_view);
        gtk_box_pack_start(GTK_BOX(button_box), assign_btn, FALSE, FALSE, 0);
    }
    
    // Close button
    GtkWidget *close_btn = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_box_pack_end(GTK_BOX(button_box), close_btn, FALSE, FALSE, 0);
    
    gtk_widget_show_all(window);
}

static void on_assign_professor_clicked(GtkWidget *widget, gpointer data) {
    GtkTreeView *tree_view = GTK_TREE_VIEW(data);
    GtkTreeSelection *selection = gtk_tree_view_get_selection(tree_view);
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    // Check if a module is selected
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_WARNING,
                                                    GTK_BUTTONS_OK,
                                                    "Please select a module first.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }
    
    // Get the selected module ID and store the iter path
    int module_id;
    gtk_tree_model_get(model, &iter, 0, &module_id, -1);
    GtkTreePath *path = gtk_tree_model_get_path(model, &iter);
    
    // Find the module
    Module *module = cours_rechercher_par_id(app_state.modules, module_id);
    if (!module) {
        GtkWidget *dialog = gtk_message_dialog_new(NULL,
                                                    GTK_DIALOG_MODAL,
                                                    GTK_MESSAGE_ERROR,
                                                    GTK_BUTTONS_OK,
                                                    "Module not found.");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        gtk_tree_path_free(path);
        return;
    }
    
    // Store module name for dialog
    char module_name[MAX_NAME_LENGTH];
    strncpy(module_name, module->nom, MAX_NAME_LENGTH - 1);
    module_name[MAX_NAME_LENGTH - 1] = '\0';
    
    // Create dialog for professor selection
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Assign Professor",
                                                      NULL,
                                                      GTK_DIALOG_MODAL,
                                                      "Cancel", GTK_RESPONSE_CANCEL,
                                                      "Assign", GTK_RESPONSE_ACCEPT,
                                                      NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 450, 300);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *vbox = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(vbox), 10);
    gtk_container_add(GTK_CONTAINER(content), vbox);
    
    // Module info
    char info[256];
    snprintf(info, sizeof(info), "Assigning professor to:\n%s", module_name);
    GtkWidget *module_label = gtk_label_new(info);
    gtk_box_pack_start(GTK_BOX(vbox), module_label, FALSE, FALSE, 5);
    
    // Professor selection combo box
    GtkWidget *combo_label = gtk_label_new("Select Professor:");
    gtk_box_pack_start(GTK_BOX(vbox), combo_label, FALSE, FALSE, 0);
    
    GtkWidget *combo = gtk_combo_box_text_new();
    
    // Populate combo box with professors - keep all professors to maintain index mapping
    printf("[DEBUG] Populating professor combo: count=%d\n", app_state.professors ? app_state.professors->count : -1);
    if (app_state.professors && app_state.professors->professors && app_state.professors->count > 0) {
        for (int i = 0; i < app_state.professors->count; i++) {
            Professor *prof = &app_state.professors->professors[i];
            if (prof) {
                char prof_text[256];
                int len = snprintf(prof_text, sizeof(prof_text), "%s %s - %s", 
                         prof->first_name, prof->last_name, prof->department);
                printf("[DEBUG] Adding professor %d: %s (len=%d)\n", i, prof_text, len);
                gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), prof_text);
            }
        }
    } else {
        printf("[DEBUG] No professors to populate\n");
        gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(combo), "No professors available");
    }
    
    gtk_box_pack_start(GTK_BOX(vbox), combo, FALSE, FALSE, 5);
    
    gtk_widget_show_all(dialog);
    
    printf("[DEBUG] Running professor assignment dialog...\n");
    gint response = gtk_dialog_run(GTK_DIALOG(dialog));
    printf("[DEBUG] Dialog response: %d\n", response);
    
    if (response == GTK_RESPONSE_ACCEPT) {
        gint selected_index = gtk_combo_box_get_active(GTK_COMBO_BOX(combo));
        printf("[DEBUG] Selected index: %d, Professor count: %d\n", selected_index, app_state.professors ? app_state.professors->count : 0);
        
        if (selected_index >= 0 && app_state.professors && selected_index < app_state.professors->count) {
            // Find the module again after dialog
            module = cours_rechercher_par_id(app_state.modules, module_id);
            if (module && app_state.modules) {
                Professor *selected_prof = &app_state.professors->professors[selected_index];
                printf("[DEBUG] Assigning professor: %s %s to module: %s\n", 
                       selected_prof->first_name, selected_prof->last_name, module->nom);
                
                // Update module with professor name (first letter of first name + dot + space + last name)
                snprintf(module->nom_prenom_enseignent, sizeof(module->nom_prenom_enseignent),
                         "%c. %s", selected_prof->first_name[0], selected_prof->last_name);
                
                printf("[DEBUG] About to save modules...\n");
                printf("[DEBUG] Modules filename: '%s'\n", app_state.modules ? app_state.modules->filename : "NULL");
                printf("[DEBUG] Modules count: %d\n", app_state.modules ? app_state.modules->count : 0);
                // Save modules to file - pass by value, let the function handle the copy
                if (app_state.modules && app_state.modules->filename[0] != '\0' && 
                    sauvegarder_modules_ds_file(*app_state.modules) == 1) {
                    printf("[DEBUG] Modules saved successfully\n");
                    // Update the tree view using the saved path
                    GtkTreeIter update_iter;
                    if (gtk_tree_model_get_iter(model, &update_iter, path)) {
                        gtk_list_store_set(GTK_LIST_STORE(model), &update_iter,
                                           7, module->nom_prenom_enseignent,
                                           -1);
                    }
                    
                    GtkWidget *success_dialog = gtk_message_dialog_new(NULL,
                                                                        GTK_DIALOG_MODAL,
                                                                        GTK_MESSAGE_INFO,
                                                                        GTK_BUTTONS_OK,
                                                                        "Professor assigned successfully!");
                    gtk_dialog_run(GTK_DIALOG(success_dialog));
                    gtk_widget_destroy(success_dialog);
                } else {
                    printf("[DEBUG] Failed to save modules\n");
                    GtkWidget *error_dialog = gtk_message_dialog_new(NULL,
                                                                      GTK_DIALOG_MODAL,
                                                                      GTK_MESSAGE_ERROR,
                                                                      GTK_BUTTONS_OK,
                                                                      "Failed to save assignment.");
                    gtk_dialog_run(GTK_DIALOG(error_dialog));
                    gtk_widget_destroy(error_dialog);
                }
            } else {
                printf("[DEBUG] Module not found after dialog\n");
            }
        } else {
            printf("[DEBUG] Invalid selection or no selection made\n");
            GtkWidget *warning_dialog = gtk_message_dialog_new(NULL,
                                                                GTK_DIALOG_MODAL,
                                                                GTK_MESSAGE_WARNING,
                                                                GTK_BUTTONS_OK,
                                                                "Please select a professor.");
            gtk_dialog_run(GTK_DIALOG(warning_dialog));
            gtk_widget_destroy(warning_dialog);
        }
    }
    
    printf("[DEBUG] Destroying dialog...\n");
    gtk_widget_destroy(dialog);
    gtk_tree_path_free(path);
    printf("[DEBUG] Assign professor callback complete\n");
}

static void on_manage_notes_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Notes Management window...\n");
    
    // Create notes window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Notes Management");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 550);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<span size='large' weight='bold'>Student Notes & Grades</span>");
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 5);
    
    // Info label
    GtkWidget *info = gtk_label_new("Displaying all student exam notes and grades");
    gtk_box_pack_start(GTK_BOX(main_box), info, FALSE, FALSE, 0);
    
    // Scrolled window with tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Create list store
    GtkListStore *store = gtk_list_store_new(5, 
        G_TYPE_INT,    // Student ID
        G_TYPE_INT,    // Exam ID
        G_TYPE_FLOAT,  // Note
        G_TYPE_STRING, // Present status
        G_TYPE_STRING  // Status color
    );
    
    // Populate with actual data
    if (app_state.grades && app_state.grades->count > 0) {
        for (int i = 0; i < app_state.grades->count; i++) {
            Note *note = &app_state.grades->note[i];
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            
            char present[20];
            snprintf(present, sizeof(present), "%s", note->present ? "Present" : "Absent");
            
            char *color = note->note_obtenue >= 10.0 ? "green" : "red";
            
            gtk_list_store_set(store, &iter,
                0, note->id_etudiant,
                1, note->id_examen,
                2, note->note_obtenue,
                3, present,
                4, color,
                -1);
        }
    }
    
    // Create tree view
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Student ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Exam ID", renderer, "text", 1, NULL);
    
    renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Note", renderer, "text", 2, "foreground", 4, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Attendance", renderer, "text", 3, NULL);
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    // Stats label
    char stats[256];
    snprintf(stats, sizeof(stats), "Total notes: %d", app_state.grades ? app_state.grades->count : 0);
    GtkWidget *stats_label = gtk_label_new(stats);
    gtk_box_pack_start(GTK_BOX(main_box), stats_label, FALSE, FALSE, 5);
    
    // Close button
    GtkWidget *close_btn = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_box_pack_start(GTK_BOX(main_box), close_btn, FALSE, FALSE, 0);
    
    gtk_widget_show_all(window);
}

static void on_manage_exams_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Exams Management window...\n");
    
    // Create exams window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Exams Management");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 520);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), TRUE);
    
    // Prevent GtkApplication from tracking this window
    gtk_window_set_application(GTK_WINDOW(window), NULL);
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 10);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<span size='large' weight='bold'>Exam Schedule & Management</span>");
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 5);
    
    // Info label
    char info[128];
    snprintf(info, sizeof(info), "Total exams: %d", app_state.exams ? app_state.exams->count : 0);
    GtkWidget *info_label = gtk_label_new(info);
    gtk_box_pack_start(GTK_BOX(main_box), info_label, FALSE, FALSE, 0);
    
    // Scrolled window with tree view
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Create list store
    GtkListStore *store = gtk_list_store_new(5, 
        G_TYPE_INT,    // Exam ID
        G_TYPE_INT,    // Module ID
        G_TYPE_STRING, // Module Name
        G_TYPE_STRING, // Date & Time
        G_TYPE_INT     // Duration (minutes)
    );
    
    // Populate with actual data
    if (app_state.exams && app_state.exams->count > 0) {
        for (int i = 0; i < app_state.exams->count; i++) {
            Examen *exam = &app_state.exams->exam[i];
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            
            // Format date
            char date_str[64];
            struct tm *tm_info = localtime(&exam->date_examen);
            strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M", tm_info);
            
            gtk_list_store_set(store, &iter,
                0, exam->id_examen,
                1, exam->id_module,
                2, exam->nom_module,
                3, date_str,
                4, exam->duree,
                -1);
        }
    }
    
    // Create tree view
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Exam ID", renderer, "text", 0, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Module ID", renderer, "text", 1, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Module Name", renderer, "text", 2, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Date & Time", renderer, "text", 3, NULL);
    gtk_tree_view_insert_column_with_attributes(GTK_TREE_VIEW(tree_view), -1, "Duration (min)", renderer, "text", 4, NULL);
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    // Button box
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    // Close button
    GtkWidget *close_btn = gtk_button_new_with_label("Close");
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    gtk_box_pack_start(GTK_BOX(button_box), close_btn, TRUE, TRUE, 0);
    
    gtk_widget_show_all(window);
}

/*
 * Callback structures and functions for Professor Notes
 */
typedef struct {
    GtkWidget *student_id_entry;
    GtkWidget *exam_combo;
    GtkWidget *grade_entry;
    GtkWidget *attendance_check;
    GtkWidget *result_label;
} CreateNoteWidgets;

typedef struct {
    GtkWidget *combo;
    GtkWidget *label;
} StatsWidgets;

typedef struct {
    int student_id;
    GtkWidget *grade_entry;
    GtkWidget *attendance_check;
} StudentGradeRow;

typedef struct {
    GtkWidget *exam_combo;
    GtkWidget *result_label;
    StudentGradeRow *rows;
    int row_count;
} BatchGradeWidgets;

static void cleanup_batch_grade_widgets(GtkWidget *window, gpointer data) {
    BatchGradeWidgets *bgw = (BatchGradeWidgets*)data;
    if (bgw) {
        if (bgw->rows) {
            g_free(bgw->rows);
        }
        g_free(bgw);
    }
}

static void on_create_batch_grades_clicked(GtkWidget *widget, gpointer data) {
    BatchGradeWidgets *w = (BatchGradeWidgets*)data;
    
    int exam_index = gtk_combo_box_get_active(GTK_COMBO_BOX(w->exam_combo));
    if (exam_index < 0 || !app_state.exams) {
        gtk_label_set_markup(GTK_LABEL(w->result_label), 
            "<span foreground='red'>Error: Please select an exam</span>");
        return;
    }
    
    int exam_id = app_state.exams->exam[exam_index].id_examen;
    int success_count = 0;
    int error_count = 0;
    
    // Create grades for all students with non-zero values
    for (int i = 0; i < w->row_count; i++) {
        float grade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(w->rows[i].grade_entry));
        int present = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->rows[i].attendance_check)) ? 1 : 0;
        
        // Only create grade if student was marked as present or if they have a grade entered
        if (present || grade > 0) {
            if (professor_create_note_for_student(app_state.grades, w->rows[i].student_id, exam_id, grade, present)) {
                success_count++;
            } else {
                error_count++;
            }
        }
    }
    
    // Save grades to file if any were created
    if (success_count > 0 && app_state.grades) {
        sauvegarder_notes_ds_file(app_state.grades);
        printf("[INFO] Saved %d new grades to file\n", success_count);
    }
    
    // Show result
    char result_msg[512];
    if (success_count > 0 && error_count == 0) {
        snprintf(result_msg, sizeof(result_msg), 
            "<span foreground='green' weight='bold'>✓ Successfully created %d grade(s)</span>", 
            success_count);
    } else if (success_count > 0 && error_count > 0) {
        snprintf(result_msg, sizeof(result_msg), 
            "<span foreground='orange' weight='bold'>⚠ Created %d grade(s), %d failed (may already exist)</span>", 
            success_count, error_count);
    } else if (error_count > 0) {
        snprintf(result_msg, sizeof(result_msg), 
            "<span foreground='red' weight='bold'>✗ Failed to create grades (%d errors)</span>", 
            error_count);
    } else {
        snprintf(result_msg, sizeof(result_msg), 
            "<span foreground='blue'>No grades to create (select students and enter grades)</span>");
    }
    gtk_label_set_markup(GTK_LABEL(w->result_label), result_msg);
}

static void on_create_grade_clicked(GtkWidget *widget, gpointer data) {
    CreateNoteWidgets *w = (CreateNoteWidgets*)data;
    
    const char *student_id_str = gtk_entry_get_text(GTK_ENTRY(w->student_id_entry));
    int student_id = atoi(student_id_str);
    
    int exam_index = gtk_combo_box_get_active(GTK_COMBO_BOX(w->exam_combo));
    if (exam_index < 0 || !app_state.exams) {
        gtk_label_set_markup(GTK_LABEL(w->result_label), 
            "<span foreground='red'>Error: Please select an exam</span>");
        return;
    }
    
    int exam_id = app_state.exams->exam[exam_index].id_examen;
    float grade = gtk_spin_button_get_value(GTK_SPIN_BUTTON(w->grade_entry));
    int present = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(w->attendance_check)) ? 1 : 0;
    
    // Create the note
    if (professor_create_note_for_student(app_state.grades, student_id, exam_id, grade, present)) {
        char success_msg[256];
        snprintf(success_msg, sizeof(success_msg), 
            "<span foreground='green'>✓ Grade successfully created for Student %d (%.2f/20)</span>", 
            student_id, grade);
        gtk_label_set_markup(GTK_LABEL(w->result_label), success_msg);
        
        // Clear form
        gtk_entry_set_text(GTK_ENTRY(w->student_id_entry), "");
        gtk_spin_button_set_value(GTK_SPIN_BUTTON(w->grade_entry), 10.0);
    } else {
        gtk_label_set_markup(GTK_LABEL(w->result_label), 
            "<span foreground='red'>✗ Failed to create grade. Check student/exam ID or if grade already exists.</span>");
    }
}

static void on_show_stats_clicked(GtkWidget *widget, gpointer data) {
    StatsWidgets *w = (StatsWidgets*)data;
    
    int exam_index = gtk_combo_box_get_active(GTK_COMBO_BOX(w->combo));
    if (exam_index < 0 || !app_state.exams || !app_state.grades) {
        gtk_label_set_text(GTK_LABEL(w->label), "No exam selected or no data available");
        return;
    }
    
    int exam_id = app_state.exams->exam[exam_index].id_examen;
    char *exam_name = app_state.exams->exam[exam_index].nom_module;
    
    // Calculate statistics
    float sum = 0, min = 20, max = 0;
    int count = 0, present = 0, absent = 0, passed = 0;
    
    for (int i = 0; i < app_state.grades->count; i++) {
        if (app_state.grades->note[i].id_examen == exam_id) {
            if (app_state.grades->note[i].present) {
                present++;
                sum += app_state.grades->note[i].note_obtenue;
                count++;
                if (app_state.grades->note[i].note_obtenue < min) min = app_state.grades->note[i].note_obtenue;
                if (app_state.grades->note[i].note_obtenue > max) max = app_state.grades->note[i].note_obtenue;
                if (app_state.grades->note[i].note_obtenue >= 10.0) passed++;
            } else {
                absent++;
            }
        }
    }
    
    char stats_text[1024];
    if (count > 0) {
        snprintf(stats_text, sizeof(stats_text),
            "<b>Statistics for Exam: %s (ID: %d)</b>\n\n"
            "Number of students: %d\n"
            "Present: %d\n"
            "Absent: %d\n"
            "Average grade: %.2f / 20\n"
            "Minimum grade: %.2f\n"
            "Maximum grade: %.2f\n"
            "Passed (≥10): %d\n"
            "Failed (<10): %d\n"
            "Pass rate: %.2f%%\n"
            "Absence rate: %.2f%%",
            exam_name, exam_id,
            present + absent,
            present,
            absent,
            count > 0 ? sum / count : 0,
            min,
            max,
            passed,
            count - passed,
            count > 0 ? (passed * 100.0 / count) : 0,
            (present + absent) > 0 ? (absent * 100.0 / (present + absent)) : 0
        );
    } else {
        snprintf(stats_text, sizeof(stats_text), 
            "<b>No grades found for exam: %s</b>", exam_name);
    }
    
    gtk_label_set_markup(GTK_LABEL(w->label), stats_text);
}

/*
 * Professor Create Notes Window
 */
// Structure to hold filter data
typedef struct {
    GtkWidget *tree_view;
    GtkWidget *exam_combo;
    GtkWidget *stats_label;
} GradeFilterData;

static void on_filter_grades_by_exam(GtkWidget *widget, gpointer data) {
    GradeFilterData *filter_data = (GradeFilterData*)data;
    int exam_index = gtk_combo_box_get_active(GTK_COMBO_BOX(filter_data->exam_combo));
    
    if (exam_index < 0 || !app_state.exams || !app_state.grades) {
        return;
    }
    
    // Get the selected exam ID
    int exam_id = app_state.exams->exam[exam_index].id_examen;
    
    // Get the tree view model
    GtkTreeModel *model = gtk_tree_view_get_model(GTK_TREE_VIEW(filter_data->tree_view));
    GtkListStore *store = GTK_LIST_STORE(model);
    
    // Clear existing data
    gtk_list_store_clear(store);
    
    // Populate with filtered data
    int total_count = 0;
    int present_count = 0;
    int absent_count = 0;
    int passed_count = 0;
    float sum_grades = 0.0;
    
    for (int i = 0; i < app_state.grades->count; i++) {
        Note *note = &app_state.grades->note[i];
        
        if (note->id_examen == exam_id) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            
            // Find student name
            char student_name[128] = "Unknown";
            if (app_state.students) {
                for (int j = 0; j < app_state.students->count; j++) {
                    if (app_state.students->students[j].id == note->id_etudiant) {
                        snprintf(student_name, sizeof(student_name), "%s %s",
                                app_state.students->students[j].first_name,
                                app_state.students->students[j].last_name);
                        break;
                    }
                }
            }
            
            char attendance[20];
            snprintf(attendance, sizeof(attendance), "%s", note->present ? "Present" : "Absent");
            
            char *color = note->note_obtenue >= 10.0 ? "green" : "red";
            
            gtk_list_store_set(store, &iter,
                0, note->id_etudiant,
                1, student_name,
                2, note->note_obtenue,
                3, attendance,
                4, color,
                -1);
            
            total_count++;
            if (note->present) {
                present_count++;
                sum_grades += note->note_obtenue;
                if (note->note_obtenue >= 10.0) passed_count++;
            } else {
                absent_count++;
            }
        }
    }
    
    // Update statistics
    char stats[512];
    if (present_count > 0) {
        snprintf(stats, sizeof(stats),
                "<b>Statistics:</b>  Total: %d  |  Present: %d  |  Absent: %d  |  "
                "Passed: %d (%.1f%%)  |  Average: %.2f/20",
                total_count, present_count, absent_count, passed_count,
                (present_count > 0) ? (passed_count * 100.0 / present_count) : 0.0,
                (present_count > 0) ? (sum_grades / present_count) : 0.0);
    } else {
        snprintf(stats, sizeof(stats), "<b>No grades found for this exam</b>");
    }
    gtk_label_set_markup(GTK_LABEL(filter_data->stats_label), stats);
}

static void on_professor_create_notes_clicked(GtkWidget *widget, gpointer data) {
    printf("[INFO] Opening Professor Notes View window...\n");
    
    // Create window
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "View Student Grades by Exam");
    gtk_window_set_default_size(GTK_WINDOW(window), 900, 650);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    gtk_window_set_destroy_with_parent(GTK_WINDOW(window), FALSE);
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 15);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 20);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    // Header
    GtkWidget *header = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(header), "<span size='xx-large' weight='bold'>Student Grades by Exam</span>");
    gtk_box_pack_start(GTK_BOX(main_box), header, FALSE, FALSE, 5);
    
    // Exam selection
    GtkWidget *filter_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), filter_box, FALSE, FALSE, 5);
    
    GtkWidget *exam_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(exam_label), "<b>Select Exam:</b>");
    gtk_box_pack_start(GTK_BOX(filter_box), exam_label, FALSE, FALSE, 0);
    
    GtkWidget *exam_combo = gtk_combo_box_text_new();
    gtk_widget_set_size_request(exam_combo, 400, -1);
    
    // Populate exam dropdown with all exams
    if (app_state.exams && app_state.exams->count > 0) {
        for (int i = 0; i < app_state.exams->count; i++) {
            char exam_str[256];
            char date_str[64] = "";
            
            if (app_state.exams->exam[i].date_examen > 0) {
                struct tm* tm_info = localtime(&app_state.exams->exam[i].date_examen);
                strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
            }
            
            snprintf(exam_str, sizeof(exam_str), "[ID:%d] %s%s%s", 
                    app_state.exams->exam[i].id_examen,
                    app_state.exams->exam[i].nom_module,
                    date_str[0] ? " - " : "",
                    date_str);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(exam_combo), exam_str);
        }
        gtk_combo_box_set_active(GTK_COMBO_BOX(exam_combo), 0);
    }
    gtk_box_pack_start(GTK_BOX(filter_box), exam_combo, FALSE, FALSE, 0);
    
    // View button
    GtkWidget *view_btn = gtk_button_new_with_label("View Grades");
    gtk_widget_set_size_request(view_btn, 120, -1);
    gtk_box_pack_start(GTK_BOX(filter_box), view_btn, FALSE, FALSE, 0);
    
    // Separator
    gtk_box_pack_start(GTK_BOX(main_box), gtk_separator_new(GTK_ORIENTATION_HORIZONTAL), FALSE, FALSE, 5);
    
    // Scrolled window for grades table
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_scrolled_window_set_policy(GTK_SCROLLED_WINDOW(scrolled), GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 0);
    
    // Create list store
    GtkListStore *store = gtk_list_store_new(5, 
        G_TYPE_INT,    // Student ID
        G_TYPE_STRING, // Student Name
        G_TYPE_FLOAT,  // Grade
        G_TYPE_STRING, // Attendance
        G_TYPE_STRING  // Color
    );
    
    // Create tree view
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Add columns
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    
    GtkTreeViewColumn *col_id = gtk_tree_view_column_new_with_attributes("Student ID", renderer, "text", 0, NULL);
    gtk_tree_view_column_set_min_width(col_id, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_id);
    
    GtkTreeViewColumn *col_name = gtk_tree_view_column_new_with_attributes("Student Name", renderer, "text", 1, NULL);
    gtk_tree_view_column_set_min_width(col_name, 250);
    gtk_tree_view_column_set_expand(col_name, TRUE);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_name);
    
    renderer = gtk_cell_renderer_text_new();
    g_object_set(renderer, "weight", PANGO_WEIGHT_BOLD, NULL);
    GtkTreeViewColumn *col_grade = gtk_tree_view_column_new_with_attributes("Grade", renderer, "text", 2, "foreground", 4, NULL);
    gtk_tree_view_column_set_min_width(col_grade, 100);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_grade);
    
    renderer = gtk_cell_renderer_text_new();
    GtkTreeViewColumn *col_attendance = gtk_tree_view_column_new_with_attributes("Attendance", renderer, "text", 3, NULL);
    gtk_tree_view_column_set_min_width(col_attendance, 120);
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), col_attendance);
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    // Statistics label
    GtkWidget *stats_label = gtk_label_new(NULL);
    gtk_label_set_markup(GTK_LABEL(stats_label), "<b>Select an exam and click 'View Grades'</b>");
    gtk_box_pack_start(GTK_BOX(main_box), stats_label, FALSE, FALSE, 5);
    
    // Close button
    GtkWidget *button_box = gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10);
    gtk_box_pack_start(GTK_BOX(main_box), button_box, FALSE, FALSE, 0);
    
    GtkWidget *close_btn = gtk_button_new_with_label("Close");
    gtk_widget_set_size_request(close_btn, 100, 40);
    gtk_box_pack_end(GTK_BOX(button_box), close_btn, FALSE, FALSE, 0);
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    
    // Setup filter callback
    GradeFilterData *filter_data = g_new(GradeFilterData, 1);
    filter_data->tree_view = tree_view;
    filter_data->exam_combo = exam_combo;
    filter_data->stats_label = stats_label;
    
    g_signal_connect(view_btn, "clicked", G_CALLBACK(on_filter_grades_by_exam), filter_data);
    g_signal_connect(exam_combo, "changed", G_CALLBACK(on_filter_grades_by_exam), filter_data);
    g_signal_connect(window, "destroy", G_CALLBACK(g_free), filter_data);
    
    // Load first exam automatically
    if (app_state.exams && app_state.exams->count > 0) {
        on_filter_grades_by_exam(exam_combo, filter_data);
    }
    
    gtk_widget_show_all(window);
}

/*
 * Professor Text Notes Feature
 */
typedef struct {
    GtkWidget *module_combo;
    GtkWidget *student_combo;
    GtkWidget *content_entry;
    GtkWidget *tree_view;
    GtkWidget *window;
} TextNoteWidgets;

static void on_add_note_clicked(GtkWidget *widget, gpointer data) {
    TextNoteWidgets *w = (TextNoteWidgets*)data;
    
    // Get selected module
    int module_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(w->module_combo));
    if (module_idx < 0) return;
    
    char *student_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w->student_combo));
    if (!student_text) return;
    
    int student_id = 0;
    sscanf(student_text, "[ID:%d]", &student_id);
    g_free(student_text);
    
    const char *content = gtk_entry_get_text(GTK_ENTRY(w->content_entry));
    if (strlen(content) == 0) return;
    
    char *module_text = gtk_combo_box_text_get_active_text(GTK_COMBO_BOX_TEXT(w->module_combo));
    int module_id = 0;
    if (module_text) {
        sscanf(module_text, "[ID:%d]", &module_id);
        g_free(module_text);
    }
    
    int prof_id = app_state.session ? app_state.session->user_id : 0;
    
    if (prof_note_create(app_state.prof_notes, student_id, module_id, prof_id, content)) {
        GtkListStore *store = GTK_LIST_STORE(gtk_tree_view_get_model(GTK_TREE_VIEW(w->tree_view)));
        gtk_list_store_clear(store);
        
        for (int i = 0; i < app_state.prof_notes->count; i++) {
            ProfessorNote *n = &app_state.prof_notes->notes[i];
            
            char student_name[100] = "Unknown";
            Student *s = student_list_find_by_id(app_state.students, n->student_id);
            if (s) snprintf(student_name, sizeof(student_name), "%s %s", s->first_name, s->last_name);
            
            char module_name[100] = "Unknown";
            if (app_state.modules) {
                 Module *m = chercher_module_par_id(*app_state.modules, n->module_id);
                 if(m) strncpy(module_name, m->nom, sizeof(module_name)-1);
            }
            
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            gtk_list_store_set(store, &iter, 0, n->id, 1, student_name, 2, module_name, 3, n->content, 4, n->date, -1);
        }
        
        gtk_entry_set_text(GTK_ENTRY(w->content_entry), "");
        
        GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window), GTK_DIALOG_MODAL, GTK_MESSAGE_INFO, GTK_BUTTONS_OK, "Note added successfully");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    } else {
         GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(w->window), GTK_DIALOG_MODAL, GTK_MESSAGE_ERROR, GTK_BUTTONS_OK, "Failed to add note");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
    }
}

static void on_professor_text_notes_clicked(GtkWidget *widget, gpointer data) {
    if (!app_state.students || !app_state.prof_notes || !app_state.modules) return;
    
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "Manage Student Notes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 600);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    
    GtkWidget *main_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(main_box), 15);
    gtk_container_add(GTK_CONTAINER(window), main_box);
    
    GtkWidget *add_frame = gtk_frame_new("Add New Note");
    gtk_box_pack_start(GTK_BOX(main_box), add_frame, FALSE, FALSE, 5);
    
    GtkWidget *add_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 5);
    gtk_container_set_border_width(GTK_CONTAINER(add_box), 10);
    gtk_container_add(GTK_CONTAINER(add_frame), add_box);
    
    gtk_box_pack_start(GTK_BOX(add_box), gtk_label_new("Select Module:"), FALSE, FALSE, 0);
    GtkWidget *module_combo = gtk_combo_box_text_new();
    if (app_state.modules) {
        for(int i=0; i<app_state.modules->count; i++) {
            char buf[256];
            snprintf(buf, sizeof(buf), "[ID:%d] %s", app_state.modules->cours[i].id, app_state.modules->cours[i].nom);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(module_combo), buf);
        }
    }
    gtk_box_pack_start(GTK_BOX(add_box), module_combo, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(add_box), gtk_label_new("Select Student:"), FALSE, FALSE, 0);
    GtkWidget *student_combo = gtk_combo_box_text_new();
    if (app_state.students) {
        for(int i=0; i<app_state.students->count; i++) {
            char buf[256];
            snprintf(buf, sizeof(buf), "[ID:%d] %s %s", app_state.students->students[i].id, app_state.students->students[i].first_name, app_state.students->students[i].last_name);
            gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(student_combo), buf);
        }
    }
    gtk_box_pack_start(GTK_BOX(add_box), student_combo, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(add_box), gtk_label_new("Note Content:"), FALSE, FALSE, 0);
    GtkWidget *content_entry = gtk_entry_new();
    gtk_box_pack_start(GTK_BOX(add_box), content_entry, FALSE, FALSE, 0);
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_widget_set_size_request(scrolled, -1, 300);
    gtk_box_pack_start(GTK_BOX(main_box), scrolled, TRUE, TRUE, 5);
    
    GtkListStore *store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("ID", renderer, "text", 0, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Student", renderer, "text", 1, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Module", renderer, "text", 2, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Note", renderer, "text", 3, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Date", renderer, "text", 4, NULL));
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    for (int i = 0; i < app_state.prof_notes->count; i++) {
        ProfessorNote *n = &app_state.prof_notes->notes[i];
        
        char student_name[100] = "Unknown";
        Student *s = student_list_find_by_id(app_state.students, n->student_id);
        if (s) snprintf(student_name, sizeof(student_name), "%s %s", s->first_name, s->last_name);
        
        char module_name[100] = "Unknown";
         if (app_state.modules) {
             Module *m = chercher_module_par_id(*app_state.modules, n->module_id);
             if(m) strncpy(module_name, m->nom, sizeof(module_name)-1);
        }
        
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        gtk_list_store_set(store, &iter, 0, n->id, 1, student_name, 2, module_name, 3, n->content, 4, n->date, -1);
    }
    
    TextNoteWidgets *w = g_new(TextNoteWidgets, 1);
    w->module_combo = module_combo;
    w->student_combo = student_combo;
    w->content_entry = content_entry;
    w->tree_view = tree_view;
    w->window = window;
    
    GtkWidget *add_btn = gtk_button_new_with_label("Add Note");
    g_signal_connect(add_btn, "clicked", G_CALLBACK(on_add_note_clicked), w);
    gtk_box_pack_start(GTK_BOX(add_box), add_btn, FALSE, FALSE, 5);
    
    g_signal_connect_swapped(window, "destroy", G_CALLBACK(g_free), w);
    
    gtk_widget_show_all(window);
}

static void on_student_view_notes_clicked(GtkWidget *widget, gpointer data) {
    GtkWidget *window = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(window), "My Notes");
    gtk_window_set_default_size(GTK_WINDOW(window), 800, 500);
    gtk_window_set_transient_for(GTK_WINDOW(window), GTK_WINDOW(app_state.main_window));
    
    GtkWidget *scrolled = gtk_scrolled_window_new(NULL, NULL);
    gtk_container_add(GTK_CONTAINER(window), scrolled);
    
    GtkListStore *store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    GtkWidget *tree_view = gtk_tree_view_new_with_model(GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    GtkCellRenderer *renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Date", renderer, "text", 0, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Module", renderer, "text", 1, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Professor", renderer, "text", 2, NULL));
    gtk_tree_view_append_column(GTK_TREE_VIEW(tree_view), gtk_tree_view_column_new_with_attributes("Note", renderer, "text", 3, NULL));
    
    gtk_container_add(GTK_CONTAINER(scrolled), tree_view);
    
    if (app_state.session && app_state.students && app_state.prof_notes) {
         User *u = user_list_find_by_id(app_state.users, app_state.session->user_id);
         if (u) {
             Student *me = student_list_find_by_email(app_state.students, u->email);
             if (me) {
                 for (int i = 0; i < app_state.prof_notes->count; i++) {
                    ProfessorNote *n = &app_state.prof_notes->notes[i];
                    if (n->student_id == me->id) {
                        char module_name[100] = "Unknown";
                         if (app_state.modules) {
                             Module *m = chercher_module_par_id(*app_state.modules, n->module_id);
                             if(m) strncpy(module_name, m->nom, sizeof(module_name)-1);
                        }
                        
                        char prof_name[100] = "Unknown";
                        User *prof_user = user_list_find_by_id(app_state.users, n->professor_id);
                        if(prof_user) {
                           if (app_state.professors) {
                               Professor *p = professor_list_find_by_email(app_state.professors, prof_user->email);
                               if (p) {
                                   snprintf(prof_name, sizeof(prof_name), "%s %s", p->first_name, p->last_name);
                               } else {
                                   snprintf(prof_name, sizeof(prof_name), "%s", prof_user->username);
                               }
                           } else {
                               snprintf(prof_name, sizeof(prof_name), "%s", prof_user->username);
                           }
                        }
                        
                        GtkTreeIter iter;
                        gtk_list_store_append(store, &iter);
                        gtk_list_store_set(store, &iter, 0, n->date, 1, module_name, 2, prof_name, 3, n->content, -1);
                    }
                 }
             }
         }
    }
    
    gtk_widget_show_all(window);
}

/*
 * GTK application activate callback
 */
static void on_activate(GtkApplication *app, gpointer user_data) {
    printf("[INFO] Application activated\n");
    show_login_window();
}

/*
 * GTK application shutdown callback
 */
static void on_shutdown(GApplication *app, gpointer user_data) {
    printf("[INFO] Application shutting down\n");
    cleanup_app();
}

/*
 * Main entry point
 */
int main(int argc, char *argv[]) {
    printf("\n╔══════════════════════════════════════════════════════════╗\n");
    printf("║     STUDENT MANAGEMENT SYSTEM v%s                 ║\n", APP_VERSION);
    printf("║     GTK Application                                      ║\n");
    printf("╚══════════════════════════════════════════════════════════╝\n\n");
    
    // Initialize application data
    if (initialize_app_data() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize application\n");
        return EXIT_FAILURE;
    }
    
    // Create GTK application
    app_state.app = gtk_application_new("org.studentmgmt.app", G_APPLICATION_DEFAULT_FLAGS);
    if (!app_state.app) {
        fprintf(stderr, "[ERROR] Failed to create GTK application\n");
        cleanup_app();
        return EXIT_FAILURE;
    }
    
    // Connect signals
    g_signal_connect(app_state.app, "activate", G_CALLBACK(on_activate), NULL);
    g_signal_connect(app_state.app, "shutdown", G_CALLBACK(on_shutdown), NULL);
    
    // Run application
    printf("[INFO] Starting GTK application...\n");
    int status = g_application_run(G_APPLICATION(app_state.app), argc, argv);
    
    // Cleanup
    g_object_unref(app_state.app);
    
    printf("[INFO] Application exited with status %d\n", status);
    return status;
}
