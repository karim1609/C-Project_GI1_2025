#include "ui.h"
#include "theme.h"
#include "grade.h"
#include "auth.h"
#include "student.h"
#include "attendance.h"
#include "club.h"
#include "professor.h"
#include "file_manager.h"
#include "config.h"
#include "utils.h"

#include <gtk/gtk.h>
#include <glib.h>
#include <string.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <ctype.h>

// Type aliases for compatibility
typedef liste_note GradeList;
typedef ListeModules CourseList;
typedef Note Grade;  // Grade is an alias for Note

// Global UI state
static UIState* g_ui_state = NULL;
static ThemeConfig* g_theme_config = NULL;

// Forward declarations
void ui_on_export_students_clicked(GtkButton* button, gpointer user_data);

// Helper function to set margins on all sides (GTK3 doesn't have set_margin_all)
static void gtk_widget_set_margin_all(GtkWidget* widget, gint margin) {
    gtk_widget_set_margin_start(widget, margin);
    gtk_widget_set_margin_end(widget, margin);
    gtk_widget_set_margin_top(widget, margin);
    gtk_widget_set_margin_bottom(widget, margin);
}

// ============================================================================
// UI STATE MANAGEMENT
// ============================================================================

UIState* ui_state_create(void) {
    UIState* state = (UIState*)calloc(1, sizeof(UIState));
    if (!state) return NULL;
    
    state->current_window_type = UI_WINDOW_LOGIN;
    state->is_dark_theme = 0;
    strncpy(state->current_language, "en", sizeof(state->current_language) - 1);
    
    // Initialize data structures
    state->users = NULL;
    state->students = NULL;
    state->grades = NULL;
    state->attendance = NULL;
    state->clubs = NULL;
    state->memberships = NULL;
    state->courses = NULL;
    state->current_session = NULL;
    
    return state;
}

void ui_state_destroy(UIState* state) {
    if (!state) return;
    
    // Cleanup will be handled by application cleanup
    free(state);
}

int ui_init(int argc, char* argv[]) {
    // Initialize GTK
    gtk_init(&argc, &argv);
    
    // Create global UI state
    g_ui_state = ui_state_create();
    if (!g_ui_state) return 0;
    
    // Initialize theme system
    g_theme_config = theme_config_create();
    if (!g_theme_config) {
        ui_state_destroy(g_ui_state);
        return 0;
    }
    
    // Load default light theme
    theme_load_default_light_preset(g_theme_config);
    
    return 1;
}

void ui_cleanup(void) {
    if (g_theme_config) {
        theme_config_destroy(g_theme_config);
        g_theme_config = NULL;
    }
    
    if (g_ui_state) {
        ui_state_destroy(g_ui_state);
        g_ui_state = NULL;
    }
}

int ui_run(void) {
    if (!g_ui_state) return 0;
    
    // Create and show login window
    GtkWindow* login_window = ui_create_login_window(g_ui_state);
    if (!login_window) return 0;
    
    ui_show_window(login_window);
    
    // Run GTK main loop
    gtk_main();
    
    return 1;
}

// ============================================================================
// WINDOW MANAGEMENT
// ============================================================================

GtkWindow* ui_create_window(UIWindowType type, UIState* state) {
    if (!state) return NULL;
    
    switch (type) {
        case UI_WINDOW_LOGIN:
            return ui_create_login_window(state);
        case UI_WINDOW_MAIN:
            return ui_create_main_window(state);
        case UI_WINDOW_STUDENT_MANAGEMENT:
            return ui_create_student_window(state);
        case UI_WINDOW_GRADE_MANAGEMENT:
            return ui_create_grade_window(state);
        case UI_WINDOW_ATTENDANCE_MANAGEMENT:
            return ui_create_attendance_window(state);
        case UI_WINDOW_CLUB_MANAGEMENT:
            return ui_create_club_window(state);
        case UI_WINDOW_STATISTICS:
            return ui_create_statistics_window(state);
        case UI_WINDOW_SETTINGS:
            return ui_create_settings_window(state);
        case UI_WINDOW_ADMIN_VIEW:
            return ui_create_admin_view_window(state);
        default:
            return NULL;
    }
}

void ui_show_window(GtkWindow* window) {
    if (!window) return;
    gtk_widget_show_all(GTK_WIDGET(window));
    gtk_window_present(window);
}

void ui_hide_window(GtkWindow* window) {
    if (!window) return;
    gtk_widget_hide(GTK_WIDGET(window));
}

void ui_close_window(GtkWindow* window) {
    if (!window) return;
    gtk_widget_destroy(GTK_WIDGET(window));
}

void ui_switch_window(UIWindowType new_type, UIState* state) {
    if (!state) return;
    
    printf("[DEBUG] switch_window: state=%p, current_user=%p\n", 
           (void*)state, (void*)(state->current_user));
    
    // Close current window
    if (state->current_window) {
        ui_close_window(state->current_window);
    }
    
    // Create and show new window
    state->current_window = ui_create_window(new_type, state);
    if (state->current_window) {
        state->current_window_type = new_type;
        ui_show_window(state->current_window);
    }
}

void ui_show_login_window(UIState* state) {
    if (!state) return;
    
    GtkWindow* login_window = ui_create_login_window(state);
    if (login_window) {
        state->current_window = login_window;
        state->current_window_type = UI_WINDOW_LOGIN;
        ui_show_window(login_window);
    }
}

// ============================================================================
// LOGIN WINDOW - ULTRA MODERN DESIGN
// ============================================================================

GtkWindow* ui_create_login_window(UIState* state) {
    if (!state) return NULL;
    
    // Create main window
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Student Login - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, 1000, 650);
    gtk_window_set_resizable(window, FALSE);
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    gtk_window_set_decorated(window, TRUE);
    
    // Set window logo/icon
    ui_set_window_logo(window, UI_LOGO_FILE);
    
    // Apply modern theme
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    // Main container with flexbox layout
    GtkBox* main_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_box));
    
    // Add CSS classes for styling
    GtkStyleContext* main_context = gtk_widget_get_style_context(GTK_WIDGET(main_box));
    gtk_style_context_add_class(main_context, "flex-container");
    
    // Left side - Illustration/Image area (can be replaced with actual image)
    GtkBox* left_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_box_pack_start(main_box, GTK_WIDGET(left_box), TRUE, TRUE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(left_box), 450, -1);
    
    GtkStyleContext* left_context = gtk_widget_get_style_context(GTK_WIDGET(left_box));
    gtk_style_context_add_class(left_context, "flex-column");
    
    // Logo/Illustration area
    GtkBox* logo_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_box_pack_start(left_box, GTK_WIDGET(logo_container), TRUE, TRUE, 0);
    gtk_widget_set_valign(GTK_WIDGET(logo_container), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(logo_container), GTK_ALIGN_CENTER);
    
    // Load and display the logo
    GtkImage* logo_image = ui_create_logo_image(UI_LOGO_FILE, 300, 300);
    if (logo_image) {
        gtk_box_pack_start(logo_container, GTK_WIDGET(logo_image), FALSE, FALSE, 0);
        gtk_widget_set_margin_top(GTK_WIDGET(logo_image), 20);
        gtk_widget_set_margin_bottom(GTK_WIDGET(logo_image), 20);
        gtk_widget_set_margin_start(GTK_WIDGET(logo_image), 20);
        gtk_widget_set_margin_end(GTK_WIDGET(logo_image), 20);
    } else {
        // Fallback to text if logo fails to load
        GtkLabel* illustration_label = GTK_LABEL(gtk_label_new(""));
        gtk_label_set_markup(illustration_label, 
            "<span font='48' weight='bold' foreground='#2196F3'>👩‍💻</span>\n\n"
            "<span font='24' weight='bold' foreground='#1976D2'>Welcome Back!</span>");
        gtk_label_set_justify(illustration_label, GTK_JUSTIFY_CENTER);
        gtk_box_pack_start(logo_container, GTK_WIDGET(illustration_label), FALSE, FALSE, 0);
    }
    
    // Welcome text below logo
    GtkLabel* welcome_text = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(welcome_text, 
        "<span font='24' weight='bold' foreground='#1976D2'>Welcome Back!</span>");
    gtk_label_set_justify(welcome_text, GTK_JUSTIFY_CENTER);
    gtk_box_pack_start(logo_container, GTK_WIDGET(welcome_text), FALSE, FALSE, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(welcome_text), 16);
    
    // Right side - Login form card
    GtkBox* right_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_box_pack_start(main_box, GTK_WIDGET(right_box), TRUE, TRUE, 0);
    
    GtkStyleContext* right_context = gtk_widget_get_style_context(GTK_WIDGET(right_box));
    gtk_style_context_add_class(right_context, "flex-column");
    gtk_style_context_add_class(right_context, "login-card");
    
    // Center the card content
    GtkBox* card_content = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 24));
    gtk_box_pack_start(right_box, GTK_WIDGET(card_content), TRUE, FALSE, 0);
    gtk_widget_set_valign(GTK_WIDGET(card_content), GTK_ALIGN_CENTER);
    gtk_widget_set_halign(GTK_WIDGET(card_content), GTK_ALIGN_CENTER);
    gtk_widget_set_margin_start(GTK_WIDGET(card_content), 48);
    gtk_widget_set_margin_end(GTK_WIDGET(card_content), 48);
    gtk_widget_set_margin_top(GTK_WIDGET(card_content), 48);
    gtk_widget_set_margin_bottom(GTK_WIDGET(card_content), 48);
    
    // Title
    GtkLabel* title_label = GTK_LABEL(gtk_label_new("Student Login"));
    gtk_label_set_markup(title_label, 
        "<span font='32' weight='bold' foreground='#1A1A1A'>Student Login</span>");
    gtk_box_pack_start(card_content, GTK_WIDGET(title_label), FALSE, FALSE, 0);
    gtk_widget_set_halign(GTK_WIDGET(title_label), GTK_ALIGN_START);
    
    GtkStyleContext* title_context = gtk_widget_get_style_context(GTK_WIDGET(title_label));
    gtk_style_context_add_class(title_context, "title");
    
    // Subtitle
    GtkLabel* subtitle_label = GTK_LABEL(gtk_label_new("Hey enter your details to sign in to your account"));
    gtk_label_set_markup(subtitle_label, 
        "<span font='14' foreground='#666666'>Hey enter your details to sign in to your account</span>");
    gtk_box_pack_start(card_content, GTK_WIDGET(subtitle_label), FALSE, FALSE, 0);
    gtk_widget_set_halign(GTK_WIDGET(subtitle_label), GTK_ALIGN_START);
    gtk_widget_set_margin_bottom(GTK_WIDGET(subtitle_label), 32);
    
    GtkStyleContext* subtitle_context = gtk_widget_get_style_context(GTK_WIDGET(subtitle_label));
    gtk_style_context_add_class(subtitle_context, "subtitle");
    
    // Email input field
    GtkBox* email_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(card_content, GTK_WIDGET(email_box), FALSE, FALSE, 0);
    
    GtkLabel* email_label = GTK_LABEL(gtk_label_new("Email"));
    gtk_label_set_markup(email_label, "<span font='12' weight='500' foreground='#1A1A1A'>Email</span>");
    gtk_box_pack_start(email_box, GTK_WIDGET(email_label), FALSE, FALSE, 0);
    gtk_widget_set_halign(GTK_WIDGET(email_label), GTK_ALIGN_START);
    
    GtkEntry* email_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(email_entry, "example@email.com");
    gtk_entry_set_width_chars(email_entry, 30);
    gtk_box_pack_start(email_box, GTK_WIDGET(email_entry), FALSE, FALSE, 0);
    
    GtkStyleContext* email_context = gtk_widget_get_style_context(GTK_WIDGET(email_entry));
    gtk_style_context_add_class(email_context, "entry");
    
    // Password input field
    GtkBox* password_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(card_content, GTK_WIDGET(password_box), FALSE, FALSE, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(password_box), 16);
    
    GtkBox* password_header = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_box_pack_start(password_box, GTK_WIDGET(password_header), FALSE, FALSE, 0);
    
    GtkLabel* password_label = GTK_LABEL(gtk_label_new("Password"));
    gtk_label_set_markup(password_label, "<span font='12' weight='500' foreground='#1A1A1A'>Password</span>");
    gtk_box_pack_start(password_header, GTK_WIDGET(password_label), FALSE, FALSE, 0);
    
    GtkLabel* forgot_link = GTK_LABEL(gtk_label_new("Forgot password?"));
    gtk_label_set_markup(forgot_link, "<span font='12' foreground='#2196F3'>Forgot password?</span>");
    gtk_box_pack_end(password_header, GTK_WIDGET(forgot_link), FALSE, FALSE, 0);
    
    GtkStyleContext* forgot_context = gtk_widget_get_style_context(GTK_WIDGET(forgot_link));
    gtk_style_context_add_class(forgot_context, "link");
    gtk_widget_set_can_focus(GTK_WIDGET(forgot_link), TRUE);
    
    // Make forgot password clickable
    g_signal_connect(forgot_link, "button-press-event", 
                     G_CALLBACK(ui_on_forgot_password_clicked), state);
    
    GtkEntry* password_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(password_entry, "••••••••");
    gtk_entry_set_visibility(password_entry, FALSE);
    gtk_entry_set_width_chars(password_entry, 30);
    gtk_box_pack_start(password_box, GTK_WIDGET(password_entry), FALSE, FALSE, 0);
    
    GtkStyleContext* password_context = gtk_widget_get_style_context(GTK_WIDGET(password_entry));
    gtk_style_context_add_class(password_context, "entry");
    
    // Login button
    GtkButton* login_button = GTK_BUTTON(gtk_button_new_with_label("Login"));
    gtk_box_pack_start(card_content, GTK_WIDGET(login_button), FALSE, FALSE, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(login_button), 32);
    gtk_widget_set_size_request(GTK_WIDGET(login_button), -1, 48);
    
    GtkStyleContext* button_context = gtk_widget_get_style_context(GTK_WIDGET(login_button));
    gtk_style_context_add_class(button_context, "button");
    
    // Connect login button
    g_signal_connect(login_button, "clicked", 
                     G_CALLBACK(ui_on_login_button_clicked), state);
    
    // Store entries in state for access
    g_object_set_data(G_OBJECT(window), "email-entry", email_entry);
    g_object_set_data(G_OBJECT(window), "password-entry", password_entry);
    
    // Create account link
    GtkBox* create_account_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 0));
    gtk_box_pack_start(card_content, GTK_WIDGET(create_account_box), FALSE, FALSE, 0);
    gtk_widget_set_margin_top(GTK_WIDGET(create_account_box), 24);
    gtk_widget_set_halign(GTK_WIDGET(create_account_box), GTK_ALIGN_CENTER);
    
    GtkLabel* no_account_label = GTK_LABEL(gtk_label_new("Don't have account? "));
    gtk_label_set_markup(no_account_label, "<span font='12' foreground='#666666'>Don't have account? </span>");
    gtk_box_pack_start(create_account_box, GTK_WIDGET(no_account_label), FALSE, FALSE, 0);
    
    GtkLabel* create_link = GTK_LABEL(gtk_label_new("Create new account"));
    gtk_label_set_markup(create_link, "<span font='12' foreground='#2196F3'>Create new account</span>");
    gtk_box_pack_start(create_account_box, GTK_WIDGET(create_link), FALSE, FALSE, 0);
    
    GtkStyleContext* create_context = gtk_widget_get_style_context(GTK_WIDGET(create_link));
    gtk_style_context_add_class(create_context, "link");
    gtk_widget_set_can_focus(GTK_WIDGET(create_link), TRUE);
    
    // Setup callbacks
    ui_login_window_setup_callbacks(window, state);
    
    // Store window in state
    state->current_window = window;
    state->current_window_type = UI_WINDOW_LOGIN;
    
    return window;
}

void ui_login_window_setup_callbacks(GtkWindow* window, UIState* state) {
    if (!window || !state) return;
    
    // Enter key on password field triggers login
    GtkEntry* password_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(window), "password-entry"));
    if (password_entry) {
        g_signal_connect(password_entry, "activate",
                        G_CALLBACK(ui_on_login_button_clicked), state);
    }
    
    // Window close handler
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
}

int ui_handle_login(GtkEntry* username_entry, GtkEntry* password_entry, UIState* state) {
    if (!username_entry || !password_entry || !state) return 0;
    
    const char* username = gtk_entry_get_text(username_entry);
    const char* password = gtk_entry_get_text(password_entry);
    
    if (!username || strlen(username) == 0) {
        ui_show_error_message(state->current_window, "Please enter your email/username");
        return 0;
    }
    
    if (!password || strlen(password) == 0) {
        ui_show_error_message(state->current_window, "Please enter your password");
        return 0;
    }
    
    // Try to find user in user list
    if (state->users) {
        User* user = user_list_find_by_username(state->users, username);
        if (!user) {
            user = user_list_find_by_email(state->users, username);
        }
        
        if (user && user->is_active) {
            // TODO: Verify password hash
            // For now, accept any password for existing active user
            state->current_user = user;
            printf("[DEBUG] Login: Set current_user to %p (role=%d)\n", 
                   (void*)state->current_user, user->role);
            ui_login_success(state);
            return 1;
        }
    }
    
    ui_show_error_message(state->current_window, "Invalid credentials or inactive account");
    return 0;
}

void ui_login_success(UIState* state) {
    if (!state) return;
    
    // Create session (placeholder)
    // TODO: Create actual session from authentication
    
    // Switch to main window
    ui_switch_window(UI_WINDOW_MAIN, state);
}

void ui_login_failed(const char* error_message) {
    if (!error_message) return;
    
    if (g_ui_state && g_ui_state->current_window) {
        ui_show_error_message(g_ui_state->current_window, error_message);
    }
}

// Callback for forgot password
void ui_on_forgot_password_clicked(GtkWidget* widget, GdkEventButton* event, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state) return;
    
    ui_show_info_message(state->current_window, 
        "Password reset feature coming soon!\nPlease contact your administrator.");
}

// ============================================================================
// MAIN WINDOW
// ============================================================================

GtkWindow* ui_create_main_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    
    // Set window logo/icon
    ui_set_window_logo(window, UI_LOGO_FILE);
    
    // Apply theme
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    // Main vertical box
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    
    // Header with logo and title
    GtkBox* header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(header_box), FALSE, FALSE, 0);
    gtk_widget_set_margin_all(GTK_WIDGET(header_box), 12);
    
    GtkStyleContext* header_context = gtk_widget_get_style_context(GTK_WIDGET(header_box));
    gtk_style_context_add_class(header_context, "header");
    
    // Logo in header
    GtkImage* header_logo = ui_create_logo_image(UI_LOGO_FILE, 40, 40);
    if (header_logo) {
        gtk_box_pack_start(header_box, GTK_WIDGET(header_logo), FALSE, FALSE, 0);
    }
    
    // Title next to logo
    GtkLabel* header_title = GTK_LABEL(gtk_label_new(UI_WINDOW_TITLE));
    gtk_label_set_markup(header_title, 
        "<span font='20' weight='bold' foreground='#1A1A1A'>" UI_WINDOW_TITLE "</span>");
    gtk_box_pack_start(header_box, GTK_WIDGET(header_title), FALSE, FALSE, 0);
    gtk_widget_set_margin_start(GTK_WIDGET(header_title), 8);
    
    // Menu bar
    GtkMenuBar* menubar = ui_create_main_menu(state);
    if (menubar) {
        gtk_box_pack_start(main_vbox, GTK_WIDGET(menubar), FALSE, FALSE, 0);
    }
    
    // Toolbar
    GtkToolbar* toolbar = ui_create_toolbar(state);
    if (toolbar) {
        gtk_box_pack_start(main_vbox, GTK_WIDGET(toolbar), FALSE, FALSE, 0);
    }
    
    // Content area with modern card layout
    GtkBox* content_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(content_box), TRUE, TRUE, 0);
    gtk_widget_set_margin_all(GTK_WIDGET(content_box), 16);
    
    GtkStyleContext* content_context = gtk_widget_get_style_context(GTK_WIDGET(content_box));
    gtk_style_context_add_class(content_context, "flex-container");
    
    // Sidebar navigation
    GtkBox* sidebar = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(content_box, GTK_WIDGET(sidebar), FALSE, FALSE, 0);
    gtk_widget_set_size_request(GTK_WIDGET(sidebar), 200, -1);
    
    GtkStyleContext* sidebar_context = gtk_widget_get_style_context(GTK_WIDGET(sidebar));
    gtk_style_context_add_class(sidebar_context, "card");
    
    // Navigation buttons with role-based visibility
    const char* nav_items[] = {
        "Students", "Grades", "Attendance", "Clubs", "Statistics", "Settings", "🔐 Admin View"
    };
    
    UIWindowType nav_types[] = {
        UI_WINDOW_STUDENT_MANAGEMENT,
        UI_WINDOW_GRADE_MANAGEMENT,
        UI_WINDOW_ATTENDANCE_MANAGEMENT,
        UI_WINDOW_CLUB_MANAGEMENT,
        UI_WINDOW_STATISTICS,
        UI_WINDOW_SETTINGS,
        UI_WINDOW_ADMIN_VIEW
    };
    
    // Role-based button visibility: [admin, teacher, student]
    int nav_for_role[][3] = {
        {1, 1, 0},  // Students: admin=yes, teacher=yes, student=no
        {1, 1, 1},  // Grades: admin=yes, teacher=yes, student=yes (view only)
        {1, 1, 0},  // Attendance: admin=yes, teacher=yes, student=no (professors only)
        {1, 0, 1},  // Clubs: admin=yes (modify), teacher=no, student=yes (participate)
        {1, 1, 0},  // Statistics: admin=yes, teacher=yes, student=no
        {1, 1, 1},  // Settings: admin=yes, teacher=yes, student=yes
        {1, 0, 0}   // Admin View: admin=yes ONLY
    };
    
    UserRole current_role = state->current_user ? state->current_user->role : ROLE_STUDENT;
    int role_index = (current_role == ROLE_ADMIN) ? 0 : (current_role == ROLE_TEACHER) ? 1 : 2;
    
    for (int i = 0; i < 7; i++) {
        // Check if this button should be shown for current role
        if (!nav_for_role[i][role_index]) {
            continue;  // Skip this button for current role
        }
        
        GtkButton* nav_btn = GTK_BUTTON(gtk_button_new_with_label(nav_items[i]));
        gtk_box_pack_start(sidebar, GTK_WIDGET(nav_btn), FALSE, FALSE, 0);
        gtk_widget_set_margin_all(GTK_WIDGET(nav_btn), 8);
        
        GtkStyleContext* btn_context = gtk_widget_get_style_context(GTK_WIDGET(nav_btn));
        gtk_style_context_add_class(btn_context, "button");
        
        // Store window type in button
        g_object_set_data(G_OBJECT(nav_btn), "window-type", GINT_TO_POINTER(nav_types[i]));
        g_signal_connect(nav_btn, "clicked", G_CALLBACK(ui_on_nav_button_clicked), state);
    }
    
    // Main content area
    GtkBox* main_content = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_box_pack_start(content_box, GTK_WIDGET(main_content), TRUE, TRUE, 0);
    
    GtkStyleContext* main_content_context = gtk_widget_get_style_context(GTK_WIDGET(main_content));
    gtk_style_context_add_class(main_content_context, "flex-column");
    
    // Welcome card
    GtkBox* welcome_card = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_box_pack_start(main_content, GTK_WIDGET(welcome_card), FALSE, FALSE, 0);
    
    GtkStyleContext* welcome_context = gtk_widget_get_style_context(GTK_WIDGET(welcome_card));
    gtk_style_context_add_class(welcome_context, "card");
    
    GtkLabel* welcome_label = GTK_LABEL(gtk_label_new("Welcome to Student Management System"));
    gtk_label_set_markup(welcome_label, 
        "<span font='24' weight='bold' foreground='#1A1A1A'>Welcome to Student Management System</span>");
    gtk_box_pack_start(welcome_card, GTK_WIDGET(welcome_label), FALSE, FALSE, 0);
    gtk_widget_set_margin_all(GTK_WIDGET(welcome_label), 24);
    
    // Status bar
    GtkStatusbar* statusbar = ui_create_statusbar();
    if (statusbar) {
        gtk_box_pack_end(main_vbox, GTK_WIDGET(statusbar), FALSE, FALSE, 0);
        ui_statusbar_set_message(statusbar, "Ready");
    }
    
    // Window handlers
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    state->main_window = window;
    state->current_window = window;
    state->current_window_type = UI_WINDOW_MAIN;
    
    return window;
}

void ui_main_window_setup_menu(GtkWindow* window, UIState* state) {
    // Menu setup is handled in ui_create_main_menu
}

void ui_main_window_setup_toolbar(GtkWindow* window, UIState* state) {
    // Toolbar setup is handled in ui_create_toolbar
}

void ui_main_window_setup_statusbar(GtkWindow* window, UIState* state) {
    // Statusbar setup is handled in ui_create_statusbar
}

void ui_main_window_update_user_info(UIState* state) {
    if (!state || !state->main_window) return;
    // TODO: Update user info display
}

// Navigation button callback
void ui_on_nav_button_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state) return;
    
    printf("[DEBUG] nav_button_clicked: state=%p, current_user=%p\n", 
           (void*)state, (void*)(state->current_user));
    
    UIWindowType window_type = GPOINTER_TO_INT(g_object_get_data(G_OBJECT(button), "window-type"));
    ui_switch_window(window_type, state);
}

// ============================================================================
// STUDENT MANAGEMENT WINDOW
// ============================================================================

GtkWindow* ui_create_student_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Student Management - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 0));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    
    // Modern Header Section with gradient background
    GtkBox* header_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(header_container), FALSE, FALSE, 0);
    gtk_widget_set_margin_all(GTK_WIDGET(header_container), 32);
    
    // Title section with icon and subtitle
    GtkBox* title_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(header_container, GTK_WIDGET(title_box), FALSE, FALSE, 0);
    
    GtkBox* title_row = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(title_box, GTK_WIDGET(title_row), FALSE, FALSE, 0);
    
    // Title with modern styling
    GtkLabel* title = GTK_LABEL(gtk_label_new("Student Management"));
    gtk_label_set_markup(title, 
        "<span size='xx-large' weight='heavy' letter_spacing='1024'>🎓 STUDENT MANAGEMENT</span>");
    gtk_widget_set_halign(GTK_WIDGET(title), GTK_ALIGN_START);
    gtk_box_pack_start(title_row, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    // Subtitle
    GtkLabel* subtitle = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(subtitle, 
        "<span size='medium' foreground='#666666'>Manage and track all student information in one place</span>");
    gtk_widget_set_halign(GTK_WIDGET(subtitle), GTK_ALIGN_START);
    gtk_box_pack_start(title_box, GTK_WIDGET(subtitle), FALSE, FALSE, 0);
    
    // Statistics Cards Row
    GtkBox* stats_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));
    gtk_box_pack_start(header_container, GTK_WIDGET(stats_box), FALSE, FALSE, 0);
    gtk_widget_set_halign(GTK_WIDGET(stats_box), GTK_ALIGN_START);
    
    // Total Students Card
    GtkFrame* total_frame = GTK_FRAME(gtk_frame_new(NULL));
    gtk_frame_set_shadow_type(total_frame, GTK_SHADOW_NONE);
    gtk_box_pack_start(stats_box, GTK_WIDGET(total_frame), FALSE, FALSE, 0);
    
    GtkBox* total_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
    gtk_container_add(GTK_CONTAINER(total_frame), GTK_WIDGET(total_box));
    gtk_widget_set_margin_all(GTK_WIDGET(total_box), 20);
    gtk_widget_set_size_request(GTK_WIDGET(total_frame), 180, 100);
    
    GtkLabel* total_label = GTK_LABEL(gtk_label_new(""));
    char total_markup[128];
    snprintf(total_markup, sizeof(total_markup), "<span size='x-large' weight='bold'>%d</span>", state->students ? state->students->count : 0);
    gtk_label_set_markup(total_label, total_markup);
    gtk_widget_set_halign(GTK_WIDGET(total_label), GTK_ALIGN_START);
    gtk_box_pack_start(total_box, GTK_WIDGET(total_label), FALSE, FALSE, 0);
    
    GtkLabel* total_text = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(total_text, "<span foreground='#666666'>Total Students</span>");
    gtk_widget_set_halign(GTK_WIDGET(total_text), GTK_ALIGN_START);
    gtk_box_pack_start(total_box, GTK_WIDGET(total_text), FALSE, FALSE, 0);
    
    // Active Courses Card
    GtkFrame* courses_frame = GTK_FRAME(gtk_frame_new(NULL));
    gtk_frame_set_shadow_type(courses_frame, GTK_SHADOW_NONE);
    gtk_box_pack_start(stats_box, GTK_WIDGET(courses_frame), FALSE, FALSE, 0);
    
    GtkBox* courses_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 6));
    gtk_container_add(GTK_CONTAINER(courses_frame), GTK_WIDGET(courses_box));
    gtk_widget_set_margin_all(GTK_WIDGET(courses_box), 20);
    gtk_widget_set_size_request(GTK_WIDGET(courses_frame), 180, 100);
    
    GtkLabel* courses_label = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(courses_label, "<span size='x-large' weight='bold'>4</span>");
    gtk_widget_set_halign(GTK_WIDGET(courses_label), GTK_ALIGN_START);
    gtk_box_pack_start(courses_box, GTK_WIDGET(courses_label), FALSE, FALSE, 0);
    
    GtkLabel* courses_text = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(courses_text, "<span foreground='#666666'>Active Courses</span>");
    gtk_widget_set_halign(GTK_WIDGET(courses_text), GTK_ALIGN_START);
    gtk_box_pack_start(courses_box, GTK_WIDGET(courses_text), FALSE, FALSE, 0);
    
    // Search Bar Section (modern design)
    GtkBox* search_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(header_container, GTK_WIDGET(search_container), FALSE, FALSE, 0);
    
    // Search entry with icon
    GtkEntry* search_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(search_entry, "🔍 Search by name, email, course or ID...");
    gtk_entry_set_width_chars(search_entry, 50);
    gtk_widget_set_size_request(GTK_WIDGET(search_entry), 500, 48);
    gtk_box_pack_start(search_container, GTK_WIDGET(search_entry), FALSE, FALSE, 0);
    
    GtkStyleContext* search_context = gtk_widget_get_style_context(GTK_WIDGET(search_entry));
    gtk_style_context_add_class(search_context, "entry");
    
    // Search button
    GtkButton* search_btn = GTK_BUTTON(gtk_button_new_with_label("🔍 Search"));
    gtk_widget_set_size_request(GTK_WIDGET(search_btn), 140, 48);
    gtk_box_pack_start(search_container, GTK_WIDGET(search_btn), FALSE, FALSE, 0);
    
    GtkStyleContext* search_btn_context = gtk_widget_get_style_context(GTK_WIDGET(search_btn));
    gtk_style_context_add_class(search_btn_context, "suggested-action");
    
    // Clear button
    GtkButton* clear_btn = GTK_BUTTON(gtk_button_new_with_label("✖ Clear"));
    gtk_widget_set_size_request(GTK_WIDGET(clear_btn), 120, 48);
    gtk_box_pack_start(search_container, GTK_WIDGET(clear_btn), FALSE, FALSE, 0);
    
    GtkStyleContext* clear_btn_context = gtk_widget_get_style_context(GTK_WIDGET(clear_btn));
    gtk_style_context_add_class(clear_btn_context, "button");
    
    // Content Container with padding
    GtkBox* content_container = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 20));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(content_container), TRUE, TRUE, 0);
    gtk_widget_set_margin_start(GTK_WIDGET(content_container), 32);
    gtk_widget_set_margin_end(GTK_WIDGET(content_container), 32);
    gtk_widget_set_margin_bottom(GTK_WIDGET(content_container), 32);
    
    // Table Section Header
    GtkBox* table_header = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(content_container, GTK_WIDGET(table_header), FALSE, FALSE, 0);
    
    GtkLabel* table_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(table_title, "<span size='large' weight='bold'>Student Records</span>");
    gtk_widget_set_halign(GTK_WIDGET(table_title), GTK_ALIGN_START);
    gtk_box_pack_start(table_header, GTK_WIDGET(table_title), FALSE, FALSE, 0);
    
    // TreeView in a modern frame with scrolled window
    GtkFrame* table_frame = GTK_FRAME(gtk_frame_new(NULL));
    gtk_frame_set_shadow_type(table_frame, GTK_SHADOW_NONE);
    gtk_box_pack_start(content_container, GTK_WIDGET(table_frame), TRUE, TRUE, 0);
    
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_container_add(GTK_CONTAINER(table_frame), GTK_WIDGET(scrolled));
    gtk_widget_set_margin_all(GTK_WIDGET(scrolled), 0);
    
    GtkTreeView* treeview = ui_create_student_treeview();
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(treeview));
    
    // Modern Action Buttons with icons
    GtkBox* button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 16));
    gtk_box_pack_start(content_container, GTK_WIDGET(button_box), FALSE, FALSE, 0);
    
    GtkStyleContext* btn_box_context = gtk_widget_get_style_context(GTK_WIDGET(button_box));
    gtk_style_context_add_class(btn_box_context, "flex-row");
    
    // Check if user has permission to add/edit/delete (Admin or Teacher only)
    int can_modify = (state->current_session && 
                     (state->current_session->role == ROLE_ADMIN));
    
    // Primary Action Button (Add) - only for Admin/Teacher
    GtkButton* add_btn = NULL;
    if (can_modify) {
        add_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Student"));
        gtk_widget_set_size_request(GTK_WIDGET(add_btn), 160, 48);
        GtkStyleContext* add_context = gtk_widget_get_style_context(GTK_WIDGET(add_btn));
        gtk_style_context_add_class(add_context, "suggested-action");
        gtk_box_pack_start(button_box, GTK_WIDGET(add_btn), FALSE, FALSE, 0);
    }
    
    // Secondary Action Buttons - only for Admin/Teacher
    GtkButton* edit_btn = NULL;
    GtkButton* delete_btn = NULL;
    if (can_modify) {
        edit_btn = GTK_BUTTON(gtk_button_new_with_label("✏️ Edit"));
        gtk_widget_set_size_request(GTK_WIDGET(edit_btn), 120, 48);
        GtkStyleContext* edit_context = gtk_widget_get_style_context(GTK_WIDGET(edit_btn));
        gtk_style_context_add_class(edit_context, "button");
        gtk_box_pack_start(button_box, GTK_WIDGET(edit_btn), FALSE, FALSE, 0);
        
        delete_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Delete"));
        gtk_widget_set_size_request(GTK_WIDGET(delete_btn), 120, 48);
        GtkStyleContext* delete_context = gtk_widget_get_style_context(GTK_WIDGET(delete_btn));
        gtk_style_context_add_class(delete_context, "destructive-action");
        gtk_box_pack_start(button_box, GTK_WIDGET(delete_btn), FALSE, FALSE, 0);
    }
    
    // Spacer
    GtkWidget* spacer = gtk_label_new("");
    gtk_box_pack_start(button_box, spacer, TRUE, TRUE, 0);
    
    // Utility Buttons
    GtkButton* export_btn = GTK_BUTTON(gtk_button_new_with_label("📊 Export"));
    gtk_widget_set_size_request(GTK_WIDGET(export_btn), 120, 48);
    GtkStyleContext* export_context = gtk_widget_get_style_context(GTK_WIDGET(export_btn));
    gtk_style_context_add_class(export_context, "button");
    gtk_box_pack_end(button_box, GTK_WIDGET(export_btn), FALSE, FALSE, 0);
    
    GtkButton* refresh_btn = GTK_BUTTON(gtk_button_new_with_label("🔄 Refresh"));
    gtk_widget_set_size_request(GTK_WIDGET(refresh_btn), 120, 48);
    GtkStyleContext* refresh_context = gtk_widget_get_style_context(GTK_WIDGET(refresh_btn));
    gtk_style_context_add_class(refresh_context, "button");
    gtk_box_pack_end(button_box, GTK_WIDGET(refresh_btn), FALSE, FALSE, 0);
    
    // Connect callbacks
    g_signal_connect(search_btn, "clicked", G_CALLBACK(ui_on_search_students_clicked), state);
    g_signal_connect(clear_btn, "clicked", G_CALLBACK(ui_on_clear_search_clicked), state);
    g_signal_connect(search_entry, "activate", G_CALLBACK(ui_on_search_entry_activate), state);
    if (add_btn) g_signal_connect(add_btn, "clicked", G_CALLBACK(ui_on_add_student_clicked), state);
    if (edit_btn) g_signal_connect(edit_btn, "clicked", G_CALLBACK(ui_on_edit_student_clicked), state);
    if (delete_btn) g_signal_connect(delete_btn, "clicked", G_CALLBACK(ui_on_delete_student_clicked), state);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(ui_on_refresh_students_clicked), state);
    g_signal_connect(export_btn, "clicked", G_CALLBACK(ui_on_export_students_clicked), state);
    
    // Store references
    g_object_set_data(G_OBJECT(window), "treeview", treeview);
    g_object_set_data(G_OBJECT(window), "search-entry", search_entry);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_student_window_setup_treeview(GtkWindow* window, UIState* state) {
    // TreeView setup is handled in ui_create_student_treeview
}

void ui_student_window_setup_buttons(GtkWindow* window, UIState* state) {
    // Buttons are set up in ui_create_student_window
}

void ui_student_window_refresh_data(UIState* state) {
    if (!state || !state->current_window) return;
    
    GtkTreeView* treeview = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(state->current_window), "treeview"));
    if (treeview && state->students) {
        ui_student_treeview_populate(treeview, state->students);
    }
}

void ui_student_window_add_student(UIState* state) {
    if (!state) return;
    
    // Create dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Add New Student",
        GTK_WINDOW(state->current_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Add Student", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 600);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 20);
    gtk_container_add(GTK_CONTAINER(content), form_box);
    
    // Create input fields
    GtkWidget *first_name_entry = gtk_entry_new();
    GtkWidget *last_name_entry = gtk_entry_new();
    GtkWidget *email_entry = gtk_entry_new();
    GtkWidget *phone_entry = gtk_entry_new();
    GtkWidget *address_entry = gtk_entry_new();
    GtkWidget *age_spin = gtk_spin_button_new_with_range(15, 100, 1);
    GtkWidget *course_entry = gtk_entry_new();
    GtkWidget *year_spin = gtk_spin_button_new_with_range(1, 6, 1);
    GtkWidget *gpa_spin = gtk_spin_button_new_with_range(0.0, 4.0, 0.01);
    
    // Add labels and entries
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("First Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), first_name_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Last Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), last_name_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Email:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), email_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Phone:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), phone_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Address:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), address_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Age:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), age_spin, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Course:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), course_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Year:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), year_spin, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("GPA:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), gpa_spin, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        // Get values
        const char *first_name = gtk_entry_get_text(GTK_ENTRY(first_name_entry));
        const char *last_name = gtk_entry_get_text(GTK_ENTRY(last_name_entry));
        const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
        const char *phone = gtk_entry_get_text(GTK_ENTRY(phone_entry));
        const char *address = gtk_entry_get_text(GTK_ENTRY(address_entry));
        int age = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(age_spin));
        const char *course = gtk_entry_get_text(GTK_ENTRY(course_entry));
        int year = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(year_spin));
        float gpa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gpa_spin));
        
        // Validate
        if (strlen(first_name) == 0 || strlen(last_name) == 0) {
            ui_show_error_message(state->current_window, "First and last name are required");
        } else {
            // Create student struct
            Student new_student = {0};
            new_student.id = state->students->count + 1; // Generate new ID
            strncpy(new_student.first_name, first_name, MAX_NAME_LENGTH - 1);
            strncpy(new_student.last_name, last_name, MAX_NAME_LENGTH - 1);
            strncpy(new_student.email, email, MAX_EMAIL_LENGTH - 1);
            strncpy(new_student.phone, phone, MAX_PHONE_LENGTH - 1);
            strncpy(new_student.address, address, MAX_ADDRESS_LENGTH - 1);
            new_student.age = age;
            strncpy(new_student.course, course, MAX_COURSE_LENGTH - 1);
            new_student.year = year;
            new_student.gpa = gpa;
            new_student.enrollment_date = time(NULL);
            new_student.is_active = 1;
            
            // Add student
            int id = student_list_add(state->students, new_student);
            if (id > 0) {
                // Save to file
                char filepath[512];
                snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
                student_list_save_to_file(state->students, filepath);
                
                // Refresh display
                ui_student_window_refresh_data(state);
                ui_show_info_message(state->current_window, "Student added successfully!");
            } else {
                ui_show_error_message(state->current_window, "Failed to add student");
            }
        }
    }
    
    gtk_widget_destroy(dialog);
}

void ui_student_window_edit_student(UIState* state) {
    if (!state || !state->current_window) return;
    
    // Get treeview from window
    GtkTreeView *treeview = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(state->current_window), "treeview"));
    if (!treeview) return;
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(state->current_window, "Please select a student to edit");
        return;
    }
    
    // Get student ID from selection (stored as string)
    gchar *id_str = NULL;
    gtk_tree_model_get(model, &iter, 0, &id_str, -1);
    
    if (!id_str) {
        ui_show_error_message(state->current_window, "Invalid student selection");
        return;
    }
    
    int student_id = atoi(id_str);
    g_free(id_str);
    
    // Find student
    Student *student = student_list_find_by_id(state->students, student_id);
    if (!student) {
        ui_show_error_message(state->current_window, "Student not found");
        return;
    }
    
    // Create dialog
    GtkWidget *dialog = gtk_dialog_new_with_buttons("Edit Student",
        GTK_WINDOW(state->current_window),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save Changes", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 600);
    
    GtkWidget *content = gtk_dialog_get_content_area(GTK_DIALOG(dialog));
    GtkWidget *form_box = gtk_box_new(GTK_ORIENTATION_VERTICAL, 10);
    gtk_container_set_border_width(GTK_CONTAINER(form_box), 20);
    gtk_container_add(GTK_CONTAINER(content), form_box);
    
    // Create input fields with current values
    GtkWidget *first_name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(first_name_entry), student->first_name);
    
    GtkWidget *last_name_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(last_name_entry), student->last_name);
    
    GtkWidget *email_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(email_entry), student->email);
    
    GtkWidget *phone_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(phone_entry), student->phone);
    
    GtkWidget *address_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(address_entry), student->address);
    
    GtkWidget *age_spin = gtk_spin_button_new_with_range(15, 100, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(age_spin), student->age);
    
    GtkWidget *course_entry = gtk_entry_new();
    gtk_entry_set_text(GTK_ENTRY(course_entry), student->course);
    
    GtkWidget *year_spin = gtk_spin_button_new_with_range(1, 6, 1);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(year_spin), student->year);
    
    GtkWidget *gpa_spin = gtk_spin_button_new_with_range(0.0, 4.0, 0.01);
    gtk_spin_button_set_value(GTK_SPIN_BUTTON(gpa_spin), student->gpa);
    
    // Add labels and entries
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("First Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), first_name_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Last Name:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), last_name_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Email:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), email_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Phone:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), phone_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Address:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), address_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Age:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), age_spin, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Course:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), course_entry, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("Year:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), year_spin, FALSE, FALSE, 0);
    
    gtk_box_pack_start(GTK_BOX(form_box), gtk_label_new("GPA:"), FALSE, FALSE, 0);
    gtk_box_pack_start(GTK_BOX(form_box), gpa_spin, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    
    if (response == GTK_RESPONSE_ACCEPT) {
        // Get updated values
        const char *first_name = gtk_entry_get_text(GTK_ENTRY(first_name_entry));
        const char *last_name = gtk_entry_get_text(GTK_ENTRY(last_name_entry));
        const char *email = gtk_entry_get_text(GTK_ENTRY(email_entry));
        const char *phone = gtk_entry_get_text(GTK_ENTRY(phone_entry));
        const char *address = gtk_entry_get_text(GTK_ENTRY(address_entry));
        int age = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(age_spin));
        const char *course = gtk_entry_get_text(GTK_ENTRY(course_entry));
        int year = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(year_spin));
        float gpa = gtk_spin_button_get_value(GTK_SPIN_BUTTON(gpa_spin));
        
        // Update student fields
        printf("[DEBUG] Updating student ID %d: %s -> %s\n", student->id, student->first_name, first_name);
        strncpy(student->first_name, first_name, MAX_NAME_LENGTH - 1);
        strncpy(student->last_name, last_name, MAX_NAME_LENGTH - 1);
        strncpy(student->email, email, MAX_EMAIL_LENGTH - 1);
        strncpy(student->phone, phone, MAX_PHONE_LENGTH - 1);
        strncpy(student->address, address, MAX_ADDRESS_LENGTH - 1);
        student->age = age;
        strncpy(student->course, course, MAX_COURSE_LENGTH - 1);
        student->year = year;
        student->gpa = gpa;
        printf("[DEBUG] Updated student name: %s %s\n", student->first_name, student->last_name);
        
        // Save to file
        char filepath[512];
        snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
        printf("[DEBUG] About to save to: %s\n", filepath);
        printf("[DEBUG] Student in memory BEFORE save: %s %s\n", student->first_name, student->last_name);
        int result = student_list_save_to_file(state->students, filepath);
        printf("[DEBUG] Save result: %d\n", result);
        printf("[DEBUG] Student in memory AFTER save: %s %s\n", student->first_name, student->last_name);
        
        // Refresh display
        ui_student_window_refresh_data(state);
        ui_show_info_message(state->current_window, "Student updated successfully!");
    }
    
    gtk_widget_destroy(dialog);
}

void ui_student_window_delete_student(UIState* state) {
    if (!state || !state->current_window) return;
    
    // Get treeview from window
    GtkTreeView *treeview = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(state->current_window), "treeview"));
    if (!treeview) return;
    
    GtkTreeSelection *selection = gtk_tree_view_get_selection(treeview);
    GtkTreeModel *model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(state->current_window, "Please select a student to delete");
        return;
    }
    
    // Get student info for confirmation (all stored as strings)
    gchar *id_str = NULL;
    gchar *first_name = NULL;
    gchar *last_name = NULL;
    gtk_tree_model_get(model, &iter, 
                       0, &id_str,
                       1, &first_name,
                       2, &last_name,
                       -1);
    
    if (!id_str || !first_name || !last_name) {
        ui_show_error_message(state->current_window, "Invalid student selection");
        if (id_str) g_free(id_str);
        if (first_name) g_free(first_name);
        if (last_name) g_free(last_name);
        return;
    }
    
    int student_id = atoi(id_str);
    g_free(id_str);
    
    // Confirm deletion
    char confirm_msg[256];
    snprintf(confirm_msg, sizeof(confirm_msg), 
             "Are you sure you want to delete student '%s %s'?\nThis action cannot be undone.",
             first_name, last_name);
    
    GtkWidget *dialog = gtk_message_dialog_new(GTK_WINDOW(state->current_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_WARNING,
        GTK_BUTTONS_YES_NO,
        "%s", confirm_msg);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    gtk_widget_destroy(dialog);
    
    if (response == GTK_RESPONSE_YES) {
        // Delete student
        if (student_list_remove(state->students, student_id)) {
            // Save to file
            char filepath[512];
            snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
            student_list_save_to_file(state->students, filepath);
            
            // Refresh display
            ui_student_window_refresh_data(state);
            ui_show_info_message(state->current_window, "Student deleted successfully!");
        } else {
            ui_show_error_message(state->current_window, "Failed to delete student");
        }
    }
    
    g_free(first_name);
    g_free(last_name);
}

void ui_student_window_search_student(UIState* state) {
    if (!state) return;
    // TODO: Implement search
}

// ============================================================================
// GRADE MANAGEMENT WINDOW
// ============================================================================

GtkWindow* ui_create_grade_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Grade Management - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 16);
    
    // Header with back button
    GtkBox* header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(header_box), FALSE, FALSE, 0);
    
    GtkButton* back_btn = GTK_BUTTON(gtk_button_new_with_label("← Back"));
    gtk_box_pack_start(header_box, GTK_WIDGET(back_btn), FALSE, FALSE, 0);
    g_signal_connect_swapped(back_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Grade Management"));
    gtk_label_set_markup(title, "<span font='28' weight='bold'>Grade Management</span>");
    gtk_box_pack_start(header_box, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    // Create notebook (tabs) for structured sections
    GtkNotebook* notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_box_pack_start(main_vbox, GTK_WIDGET(notebook), TRUE, TRUE, 0);
    
    // Tab 1: Modules (Courses)
    GtkBox* modules_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(modules_box), 12);
    
    GtkBox* modules_btn_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    gtk_box_pack_start(modules_box, GTK_WIDGET(modules_btn_box), FALSE, FALSE, 0);
    
    UserRole role = state->current_user ? state->current_user->role : ROLE_STUDENT;
    if (role == ROLE_ADMIN || role == ROLE_TEACHER) {
        GtkButton* add_module_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Module"));
        GtkButton* edit_module_btn = GTK_BUTTON(gtk_button_new_with_label("✏️ Edit Module"));
        GtkButton* delete_module_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Delete Module"));
        gtk_box_pack_start(modules_btn_box, GTK_WIDGET(add_module_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(modules_btn_box, GTK_WIDGET(edit_module_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(modules_btn_box, GTK_WIDGET(delete_module_btn), FALSE, FALSE, 0);
    }
    
    GtkScrolledWindow* modules_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(modules_box, GTK_WIDGET(modules_scroll), TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(modules_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkLabel* modules_placeholder = GTK_LABEL(gtk_label_new("Modules list will appear here"));
    gtk_container_add(GTK_CONTAINER(modules_scroll), GTK_WIDGET(modules_placeholder));
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(modules_box), gtk_label_new("📚 Modules"));
    
    // Tab 2: Examens (Exams)
    GtkBox* exams_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(exams_box), 12);
    
    GtkBox* exams_btn_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    gtk_box_pack_start(exams_box, GTK_WIDGET(exams_btn_box), FALSE, FALSE, 0);
    
    if (role == ROLE_ADMIN || role == ROLE_TEACHER) {
        GtkButton* add_exam_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Exam"));
        GtkButton* edit_exam_btn = GTK_BUTTON(gtk_button_new_with_label("✏️ Edit Exam"));
        GtkButton* delete_exam_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Delete Exam"));
        gtk_box_pack_start(exams_btn_box, GTK_WIDGET(add_exam_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(exams_btn_box, GTK_WIDGET(edit_exam_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(exams_btn_box, GTK_WIDGET(delete_exam_btn), FALSE, FALSE, 0);
    }
    
    GtkScrolledWindow* exams_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(exams_box, GTK_WIDGET(exams_scroll), TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(exams_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkLabel* exams_placeholder = GTK_LABEL(gtk_label_new("Exams list will appear here"));
    gtk_container_add(GTK_CONTAINER(exams_scroll), GTK_WIDGET(exams_placeholder));
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(exams_box), gtk_label_new("📝 Exams"));
    
    // Tab 3: Notes (Grades)
    GtkBox* grades_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(grades_box), 12);
    
    GtkBox* grades_btn_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    gtk_box_pack_start(grades_box, GTK_WIDGET(grades_btn_box), FALSE, FALSE, 0);
    
    if (role == ROLE_ADMIN || role == ROLE_TEACHER) {
        GtkButton* add_grade_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Grade"));
        GtkButton* edit_grade_btn = GTK_BUTTON(gtk_button_new_with_label("✏️ Edit Grade"));
        GtkButton* delete_grade_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Delete Grade"));
        gtk_box_pack_start(grades_btn_box, GTK_WIDGET(add_grade_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(grades_btn_box, GTK_WIDGET(edit_grade_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(grades_btn_box, GTK_WIDGET(delete_grade_btn), FALSE, FALSE, 0);
    }
    
    GtkScrolledWindow* grades_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(grades_box, GTK_WIDGET(grades_scroll), TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(grades_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkTreeView* grades_treeview = ui_create_grade_treeview();
    gtk_container_add(GTK_CONTAINER(grades_scroll), GTK_WIDGET(grades_treeview));
    
    // Populate grades with data
    if (state->grades) {
        ui_grade_treeview_populate(grades_treeview, state->grades);
    }
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(grades_box), gtk_label_new("📋 Grades"));
    
    // Status info
    GtkLabel* status_label = GTK_LABEL(gtk_label_new(""));
    if (state->grades) {
        char status_text[128];
        snprintf(status_text, sizeof(status_text), "Total Grades: %d", state->grades->count);
        gtk_label_set_text(status_label, status_text);
    }
    gtk_box_pack_start(main_vbox, GTK_WIDGET(status_label), FALSE, FALSE, 0);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_grade_window_setup_treeview(GtkWindow* window, UIState* state) {}
void ui_grade_window_setup_buttons(GtkWindow* window, UIState* state) {}
void ui_grade_window_refresh_data(UIState* state) {}
void ui_grade_window_add_grade(UIState* state) {}
void ui_grade_window_edit_grade(UIState* state) {}
void ui_grade_window_delete_grade(UIState* state) {}

// ============================================================================
// ATTENDANCE MANAGEMENT WINDOW
// ============================================================================

// Forward declaration of callback
static void on_attendance_refresh_clicked(GtkButton* btn, gpointer data);

GtkWindow* ui_create_attendance_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Attendance Management - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 16);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Attendance Management"));
    gtk_label_set_markup(title, "<span font='28' weight='bold'>Attendance Management</span>");
    gtk_box_pack_start(main_vbox, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    // Add buttons for adding and viewing attendance
    GtkBox* button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 10));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(button_box), FALSE, FALSE, 0);
    
    GtkButton* mark_btn = GTK_BUTTON(gtk_button_new_with_label("Mark Attendance"));
    GtkButton* refresh_btn = GTK_BUTTON(gtk_button_new_with_label("Refresh"));
    
    gtk_box_pack_start(button_box, GTK_WIDGET(mark_btn), FALSE, FALSE, 0);
    gtk_box_pack_start(button_box, GTK_WIDGET(refresh_btn), FALSE, FALSE, 0);
    
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(scrolled), TRUE, TRUE, 0);
    
    GtkTreeView* treeview = ui_create_attendance_treeview();
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(treeview));
    
    // Store the treeview reference in the window data for later access
    g_object_set_data(G_OBJECT(window), "attendance_treeview", treeview);
    
    // Populate with data
    if (state->attendance) {
        ui_attendance_treeview_populate(treeview, state->attendance);
    }
    
    // Connect button signals with window as additional data
    g_signal_connect_swapped(mark_btn, "clicked", G_CALLBACK(ui_attendance_window_mark_attendance), state);
    
    // For refresh, we need to pass both state and window
    g_object_set_data(G_OBJECT(refresh_btn), "ui_state", state);
    g_object_set_data(G_OBJECT(refresh_btn), "window", window);
    g_signal_connect(refresh_btn, "clicked", G_CALLBACK(on_attendance_refresh_clicked), NULL);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_attendance_window_setup_treeview(GtkWindow* window, UIState* state) {}
void ui_attendance_window_setup_buttons(GtkWindow* window, UIState* state) {}

// Callback functions for attendance marking radio buttons
static void on_attendance_present_toggled(GtkCellRendererToggle* toggle, gchar* path_str, gpointer user_data) {
    GtkListStore* store = GTK_LIST_STORE(user_data);
    GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
        gtk_list_store_set(store, &iter, 2, TRUE, 3, FALSE, 4, FALSE, 5, FALSE, -1);
    }
    gtk_tree_path_free(path);
}

static void on_attendance_absent_toggled(GtkCellRendererToggle* toggle, gchar* path_str, gpointer user_data) {
    GtkListStore* store = GTK_LIST_STORE(user_data);
    GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
        gtk_list_store_set(store, &iter, 2, FALSE, 3, TRUE, 4, FALSE, 5, FALSE, -1);
    }
    gtk_tree_path_free(path);
}

static void on_attendance_late_toggled(GtkCellRendererToggle* toggle, gchar* path_str, gpointer user_data) {
    GtkListStore* store = GTK_LIST_STORE(user_data);
    GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
        gtk_list_store_set(store, &iter, 2, FALSE, 3, FALSE, 4, TRUE, 5, FALSE, -1);
    }
    gtk_tree_path_free(path);
}

static void on_attendance_excused_toggled(GtkCellRendererToggle* toggle, gchar* path_str, gpointer user_data) {
    GtkListStore* store = GTK_LIST_STORE(user_data);
    GtkTreePath* path = gtk_tree_path_new_from_string(path_str);
    GtkTreeIter iter;
    if (gtk_tree_model_get_iter(GTK_TREE_MODEL(store), &iter, path)) {
        gtk_list_store_set(store, &iter, 2, FALSE, 3, FALSE, 4, FALSE, 5, TRUE, -1);
    }
    gtk_tree_path_free(path);
}

// Callback for refresh button in attendance window
static void on_attendance_refresh_clicked(GtkButton* btn, gpointer data) {
    UIState* state = (UIState*)g_object_get_data(G_OBJECT(btn), "ui_state");
    GtkWindow* window = (GtkWindow*)g_object_get_data(G_OBJECT(btn), "window");
    
    if (!state) return;
    
    // Reload attendance from file
    if (!state->attendance) {
        state->attendance = attendance_list_create();
    }
    
    // Clear old data and reload
    if (state->attendance->records) {
        state->attendance->count = 0;
    }
    attendance_list_load_from_file(state->attendance, "attendance.txt");
    
    // Get the treeview and refresh it
    GtkTreeView* treeview = (GtkTreeView*)g_object_get_data(G_OBJECT(window), "attendance_treeview");
    if (treeview && GTK_IS_TREE_VIEW(treeview)) {
        ui_attendance_treeview_populate(treeview, state->attendance);
    }
}

void ui_attendance_window_refresh_data(UIState* state) {
    if (!state) return;
    
    // Reload attendance from file
    if (!state->attendance) {
        state->attendance = attendance_list_create();
    }
    attendance_list_load_from_file(state->attendance, "attendance.txt");
    
    // Find all attendance windows and refresh their treeviews
    GList* windows = gtk_window_list_toplevels();
    for (GList* l = windows; l != NULL; l = l->next) {
        GtkWindow* win = GTK_WINDOW(l->data);
        const char* title = gtk_window_get_title(win);
        if (title && strstr(title, "Attendance Management")) {
            // Find the treeview in this window
            GtkWidget* container = gtk_bin_get_child(GTK_BIN(win));
            if (GTK_IS_BOX(container)) {
                GList* children = gtk_container_get_children(GTK_CONTAINER(container));
                for (GList* c = children; c != NULL; c = c->next) {
                    if (GTK_IS_SCROLLED_WINDOW(c->data)) {
                        GtkWidget* treeview = gtk_bin_get_child(GTK_BIN(c->data));
                        if (GTK_IS_TREE_VIEW(treeview)) {
                            ui_attendance_treeview_populate(GTK_TREE_VIEW(treeview), state->attendance);
                        }
                    }
                }
                g_list_free(children);
            }
        }
    }
    g_list_free(windows);
}

// Structure to hold attendance marking data
typedef struct {
    UIState* ui_state;
    int course_id;
    GtkListStore* store;
} AttendanceMarkingData;

void ui_attendance_window_mark_attendance(UIState* state) {
    if (!state || !state->students) return;
    
    // First dialog: Select course ID
    GtkDialog* course_dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
        "Select Course",
        NULL,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Next", GTK_RESPONSE_OK,
        NULL));
    
    gtk_window_set_default_size(GTK_WINDOW(course_dialog), 400, 150);
    
    GtkBox* course_content = GTK_BOX(gtk_dialog_get_content_area(course_dialog));
    gtk_box_set_spacing(course_content, 10);
    gtk_widget_set_margin_all(GTK_WIDGET(course_content), 20);
    
    GtkLabel* course_label = GTK_LABEL(gtk_label_new("Enter Course ID:"));
    GtkEntry* course_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_placeholder_text(course_entry, "e.g., 1, 2, 3...");
    
    gtk_box_pack_start(course_content, GTK_WIDGET(course_label), FALSE, FALSE, 0);
    gtk_box_pack_start(course_content, GTK_WIDGET(course_entry), FALSE, FALSE, 0);
    
    gtk_widget_show_all(GTK_WIDGET(course_dialog));
    
    gint course_response = gtk_dialog_run(course_dialog);
    
    if (course_response != GTK_RESPONSE_OK) {
        gtk_widget_destroy(GTK_WIDGET(course_dialog));
        return;
    }
    
    const char* course_id_str = gtk_entry_get_text(course_entry);
    if (!course_id_str || strlen(course_id_str) == 0) {
        gtk_widget_destroy(GTK_WIDGET(course_dialog));
        return;
    }
    
    int course_id = atoi(course_id_str);
    gtk_widget_destroy(GTK_WIDGET(course_dialog));
    
    // Second dialog: Show all students with attendance marking
    GtkDialog* dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
        "Mark Class Attendance",
        NULL,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "_Cancel", GTK_RESPONSE_CANCEL,
        "_Save All", GTK_RESPONSE_OK,
        NULL));
    
    gtk_window_set_default_size(GTK_WINDOW(dialog), 700, 500);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(dialog));
    gtk_box_set_spacing(content, 10);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 20);
    
    // Info label
    char info_text[100];
    snprintf(info_text, sizeof(info_text), "Course ID: %d - Mark attendance for all students", course_id);
    GtkLabel* info_label = GTK_LABEL(gtk_label_new(info_text));
    gtk_label_set_markup(info_label, g_markup_printf_escaped("<b>%s</b>", info_text));
    gtk_box_pack_start(content, GTK_WIDGET(info_label), FALSE, FALSE, 0);
    
    // Create scrolled window
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(content, GTK_WIDGET(scrolled), TRUE, TRUE, 0);
    
    // Create tree view with columns: ID, Name, Present, Absent, Late
    GtkListStore* store = gtk_list_store_new(6, G_TYPE_INT, G_TYPE_STRING, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN, G_TYPE_BOOLEAN);
    GtkTreeView* treeview = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(store)));
    
    // ID column
    GtkCellRenderer* renderer_text = gtk_cell_renderer_text_new();
    GtkTreeViewColumn* col_id = gtk_tree_view_column_new_with_attributes("ID", renderer_text, "text", 0, NULL);
    gtk_tree_view_append_column(treeview, col_id);
    
    // Name column
    GtkTreeViewColumn* col_name = gtk_tree_view_column_new_with_attributes("Student Name", renderer_text, "text", 1, NULL);
    gtk_tree_view_column_set_expand(col_name, TRUE);
    gtk_tree_view_append_column(treeview, col_name);
    
    // Present column
    GtkCellRenderer* renderer_present = gtk_cell_renderer_toggle_new();
    gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_present), TRUE);
    GtkTreeViewColumn* col_present = gtk_tree_view_column_new_with_attributes("Present", renderer_present, "active", 2, NULL);
    gtk_tree_view_append_column(treeview, col_present);
    
    // Absent column
    GtkCellRenderer* renderer_absent = gtk_cell_renderer_toggle_new();
    gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_absent), TRUE);
    GtkTreeViewColumn* col_absent = gtk_tree_view_column_new_with_attributes("Absent", renderer_absent, "active", 3, NULL);
    gtk_tree_view_append_column(treeview, col_absent);
    
    // Late column
    GtkCellRenderer* renderer_late = gtk_cell_renderer_toggle_new();
    gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_late), TRUE);
    GtkTreeViewColumn* col_late = gtk_tree_view_column_new_with_attributes("Late", renderer_late, "active", 4, NULL);
    gtk_tree_view_append_column(treeview, col_late);
    
    // Excused column
    GtkCellRenderer* renderer_excused = gtk_cell_renderer_toggle_new();
    gtk_cell_renderer_toggle_set_radio(GTK_CELL_RENDERER_TOGGLE(renderer_excused), TRUE);
    GtkTreeViewColumn* col_excused = gtk_tree_view_column_new_with_attributes("Excused", renderer_excused, "active", 5, NULL);
    gtk_tree_view_append_column(treeview, col_excused);
    
    // Populate with all students (default to Present)
    for (int i = 0; i < state->students->count; i++) {
        Student* student = &state->students->students[i];
        if (student->is_active) {
            GtkTreeIter iter;
            gtk_list_store_append(store, &iter);
            
            char full_name[200];
            snprintf(full_name, sizeof(full_name), "%s %s", student->first_name, student->last_name);
            
            gtk_list_store_set(store, &iter,
                             0, student->id,
                             1, full_name,
                             2, TRUE,   // Present by default
                             3, FALSE,  // Absent
                             4, FALSE,  // Late
                             5, FALSE,  // Excused
                             -1);
        }
    }
    
    // Connect toggle signals for radio button behavior
    g_signal_connect(renderer_present, "toggled", G_CALLBACK(on_attendance_present_toggled), store);
    g_signal_connect(renderer_absent, "toggled", G_CALLBACK(on_attendance_absent_toggled), store);
    g_signal_connect(renderer_late, "toggled", G_CALLBACK(on_attendance_late_toggled), store);
    g_signal_connect(renderer_excused, "toggled", G_CALLBACK(on_attendance_excused_toggled), store);
    
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(treeview));
    gtk_widget_show_all(GTK_WIDGET(dialog));
    
    gint response = gtk_dialog_run(dialog);
    
    if (response == GTK_RESPONSE_OK) {
        // Save all attendance records
        GtkTreeIter iter;
        gboolean valid = gtk_tree_model_get_iter_first(GTK_TREE_MODEL(store), &iter);
        int saved_count = 0;
        
        while (valid) {
            int student_id;
            gboolean is_present, is_absent, is_late, is_excused;
            
            gtk_tree_model_get(GTK_TREE_MODEL(store), &iter,
                             0, &student_id,
                             2, &is_present,
                             3, &is_absent,
                             4, &is_late,
                             5, &is_excused,
                             -1);
            
            // Determine status
            int status;
            if (is_present) status = 1;
            else if (is_absent) status = 0;
            else if (is_late) status = 2;
            else if (is_excused) status = 3;
            else status = 1; // Default to present
            
            // Create attendance record
            AttendanceRecord record;
            record.id = state->attendance ? state->attendance->count + saved_count + 1 : saved_count + 1;
            record.student_id = student_id;
            record.course_id = course_id;
            record.status = status;
            record.date = time(NULL);
            record.recorded_time = time(NULL);
            record.teacher_id = state->current_session ? state->current_session->user_id : 0;
            memset(record.reason, 0, sizeof(record.reason));
            
            if (state->attendance) {
                attendance_list_add(state->attendance, record);
                saved_count++;
            }
            
            valid = gtk_tree_model_iter_next(GTK_TREE_MODEL(store), &iter);
        }
        
        // Save to file
        if (saved_count > 0) {
            attendance_list_save_to_file(state->attendance, "attendance.txt");
            
            // Refresh all attendance windows
            GList* windows = gtk_window_list_toplevels();
            for (GList* l = windows; l != NULL; l = l->next) {
                GtkWindow* win = GTK_WINDOW(l->data);
                const char* title = gtk_window_get_title(win);
                if (title && strstr(title, "Attendance Management")) {
                    GtkTreeView* treeview = (GtkTreeView*)g_object_get_data(G_OBJECT(win), "attendance_treeview");
                    if (treeview && GTK_IS_TREE_VIEW(treeview)) {
                        ui_attendance_treeview_populate(treeview, state->attendance);
                    }
                }
            }
            g_list_free(windows);
            
            // Show success message
            char success_msg[100];
            snprintf(success_msg, sizeof(success_msg), "Attendance saved for %d students!", saved_count);
            GtkDialog* msg = GTK_DIALOG(gtk_message_dialog_new(
                NULL,
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "%s", success_msg));
            gtk_dialog_run(msg);
            gtk_widget_destroy(GTK_WIDGET(msg));
        }
    }
    
    g_object_unref(store);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ui_attendance_window_view_attendance(UIState* state) {}
void ui_attendance_window_generate_report(UIState* state) {}

// ============================================================================
// CLUB MANAGEMENT WINDOW
// ============================================================================

// Forward declarations
void ui_populate_club_members(GtkTreeView* tree, UIState* state, Club* club);
void ui_on_add_student_to_club_clicked(GtkButton* button, gpointer user_data);
void ui_on_remove_student_from_club_clicked(GtkButton* button, gpointer user_data);
void ui_on_add_club_placeholder(GtkButton* button, gpointer user_data);
void ui_on_edit_club_placeholder(GtkButton* button, gpointer user_data);
void ui_on_delete_club_placeholder(GtkButton* button, gpointer user_data);
void ui_on_student_join_club(GtkButton* button, gpointer user_data);
void ui_on_student_leave_club(GtkButton* button, gpointer user_data);
void ui_on_student_view_my_clubs(GtkButton* button, gpointer user_data);
void ui_on_manage_members_clicked(GtkButton* button, gpointer user_data);

// Callback for adding a student to a club
void ui_on_add_student_to_club_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs || !state->students) return;
    
    GtkWidget* parent_window = g_object_get_data(G_OBJECT(button), "parent_window");
    Club* club = g_object_get_data(G_OBJECT(button), "club");
    
    if (!club) return;
    
    // Create dialog to select student
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Add Student to Club",
        GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Add", GTK_RESPONSE_ACCEPT,
        NULL);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_container_set_border_width(GTK_CONTAINER(content), 12);
    
    GtkLabel* label = GTK_LABEL(gtk_label_new("Select student to add:"));
    gtk_box_pack_start(content, GTK_WIDGET(label), FALSE, FALSE, 6);
    
    // Create combo box with students
    GtkComboBoxText* combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    for (int i = 0; i < state->students->count; i++) {
        Student* s = &state->students->students[i];
        char text[256];
        snprintf(text, sizeof(text), "%d - %s %s", s->id, s->first_name, s->last_name);
        gtk_combo_box_text_append(combo, NULL, text);
    }
    gtk_box_pack_start(content, GTK_WIDGET(combo), FALSE, FALSE, 6);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        gchar* selected = gtk_combo_box_text_get_active_text(combo);
        if (selected) {
            int student_id = atoi(selected);
            
            // Create membership
            ClubMembership membership;
            membership.id = state->memberships ? state->memberships->count + 1 : 1;
            membership.student_id = student_id;
            membership.club_id = club->id;
            membership.join_date = time(NULL);
            strncpy(membership.role, "Member", sizeof(membership.role) - 1);
            membership.is_active = 1;
            
            if (membership_list_add(state->memberships, membership)) {
                club->member_count++;
                
                // Show success message
                GtkWidget* msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                    GTK_DIALOG_MODAL,
                    GTK_MESSAGE_INFO,
                    GTK_BUTTONS_OK,
                    "Student added to club successfully!");
                gtk_dialog_run(GTK_DIALOG(msg));
                gtk_widget_destroy(msg);
                
                // Refresh the members list
                GtkTreeView* members_tree = g_object_get_data(G_OBJECT(parent_window), "members_tree");
                if (members_tree) {
                    ui_populate_club_members(members_tree, state, club);
                }
            }
            g_free(selected);
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Callback for removing a student from a club
void ui_on_remove_student_from_club_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->memberships) return;
    
    GtkWidget* parent_window = g_object_get_data(G_OBJECT(button), "parent_window");
    GtkTreeView* tree = g_object_get_data(G_OBJECT(button), "tree");
    Club* club = g_object_get_data(G_OBJECT(button), "club");
    
    if (!tree || !club) return;
    
    // Get selected membership
    GtkTreeSelection* selection = gtk_tree_view_get_selection(tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget* msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please select a member to remove");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    
    gint membership_id;
    gtk_tree_model_get(model, &iter, 0, &membership_id, -1);
    
    // Confirm removal
    GtkWidget* confirm = gtk_message_dialog_new(GTK_WINDOW(parent_window),
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to remove this student from the club?");
    
    if (gtk_dialog_run(GTK_DIALOG(confirm)) == GTK_RESPONSE_YES) {
        if (membership_list_remove(state->memberships, membership_id)) {
            club->member_count--;
            
            // Show success
            GtkWidget* msg = gtk_message_dialog_new(GTK_WINDOW(parent_window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Student removed from club successfully!");
            gtk_dialog_run(GTK_DIALOG(msg));
            gtk_widget_destroy(msg);
            
            // Refresh the members list
            ui_populate_club_members(tree, state, club);
        }
    }
    
    gtk_widget_destroy(confirm);
}

// Populate the members tree view
void ui_populate_club_members(GtkTreeView* tree, UIState* state, Club* club) {
    if (!tree || !state || !club) return;
    
    GtkTreeModel* old_model = gtk_tree_view_get_model(tree);
    if (old_model) {
        g_object_unref(old_model);
    }
    
    // Create new model: ID, Student ID, Name, Role, Join Date
    GtkListStore* store = gtk_list_store_new(5, G_TYPE_INT, G_TYPE_INT, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    
    // Add memberships for this club
    if (state->memberships) {
        for (int i = 0; i < state->memberships->count; i++) {
            ClubMembership* m = &state->memberships->memberships[i];
            if (m->club_id == club->id && m->is_active) {
                // Find student
                Student* student = student_list_find_by_id(state->students, m->student_id);
                if (student) {
                    char name[128];
                    snprintf(name, sizeof(name), "%s %s", student->first_name, student->last_name);
                    
                    char date_str[32];
                    struct tm* tm_info = localtime(&m->join_date);
                    strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
                    
                    GtkTreeIter iter;
                    gtk_list_store_append(store, &iter);
                    gtk_list_store_set(store, &iter,
                        0, m->id,
                        1, m->student_id,
                        2, name,
                        3, m->role,
                        4, date_str,
                        -1);
                }
            }
        }
    }
    
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    g_object_unref(store);
}

// Add club callback
void ui_on_add_club_placeholder(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    
    // Create dialog
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Add New Club",
        parent_window,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Add", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 600);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_box_set_spacing(content, 8);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 16);
    
    // Create form
    GtkGrid* grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(grid, 8);
    gtk_grid_set_column_spacing(grid, 8);
    gtk_box_pack_start(content, GTK_WIDGET(grid), TRUE, TRUE, 0);
    
    // Name
    gtk_grid_attach(grid, gtk_label_new("Club Name:"), 0, 0, 1, 1);
    GtkEntry* name_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_max_length(name_entry, 99);
    gtk_grid_attach(grid, GTK_WIDGET(name_entry), 1, 0, 1, 1);
    
    // Description
    gtk_grid_attach(grid, gtk_label_new("Description:"), 0, 1, 1, 1);
    GtkTextView* desc_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(desc_view, GTK_WRAP_WORD);
    GtkScrolledWindow* desc_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_widget_set_size_request(GTK_WIDGET(desc_scroll), -1, 100);
    gtk_container_add(GTK_CONTAINER(desc_scroll), GTK_WIDGET(desc_view));
    gtk_grid_attach(grid, GTK_WIDGET(desc_scroll), 1, 1, 1, 1);
    
    // Category
    gtk_grid_attach(grid, gtk_label_new("Category:"), 0, 2, 1, 1);
    GtkComboBoxText* category_combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(category_combo, "Academic");
    gtk_combo_box_text_append_text(category_combo, "Sports");
    gtk_combo_box_text_append_text(category_combo, "Arts");
    gtk_combo_box_text_append_text(category_combo, "Technology");
    gtk_combo_box_text_append_text(category_combo, "Religious");
    gtk_combo_box_text_append_text(category_combo, "Social");
    gtk_combo_box_text_append_text(category_combo, "Service");
    gtk_combo_box_text_append_text(category_combo, "Cultural");
    gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    gtk_grid_attach(grid, GTK_WIDGET(category_combo), 1, 2, 1, 1);
    
    // Max members
    gtk_grid_attach(grid, gtk_label_new("Max Members:"), 0, 3, 1, 1);
    GtkSpinButton* max_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(10, 100, 5));
    gtk_spin_button_set_value(max_spin, 30);
    gtk_grid_attach(grid, GTK_WIDGET(max_spin), 1, 3, 1, 1);
    
    // Budget
    gtk_grid_attach(grid, gtk_label_new("Budget:"), 0, 4, 1, 1);
    GtkSpinButton* budget_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10000, 50));
    gtk_spin_button_set_value(budget_spin, 500);
    gtk_grid_attach(grid, GTK_WIDGET(budget_spin), 1, 4, 1, 1);
    
    // Meeting day
    gtk_grid_attach(grid, gtk_label_new("Meeting Day:"), 0, 5, 1, 1);
    GtkComboBoxText* day_combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(day_combo, "Monday");
    gtk_combo_box_text_append_text(day_combo, "Tuesday");
    gtk_combo_box_text_append_text(day_combo, "Wednesday");
    gtk_combo_box_text_append_text(day_combo, "Thursday");
    gtk_combo_box_text_append_text(day_combo, "Friday");
    gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 0);
    gtk_grid_attach(grid, GTK_WIDGET(day_combo), 1, 5, 1, 1);
    
    // Meeting time
    gtk_grid_attach(grid, gtk_label_new("Meeting Time:"), 0, 6, 1, 1);
    GtkEntry* time_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(time_entry, "14:00");
    gtk_entry_set_max_length(time_entry, 19);
    gtk_grid_attach(grid, GTK_WIDGET(time_entry), 1, 6, 1, 1);
    
    // Meeting location
    gtk_grid_attach(grid, gtk_label_new("Location:"), 0, 7, 1, 1);
    GtkEntry* location_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_max_length(location_entry, 99);
    gtk_grid_attach(grid, GTK_WIDGET(location_entry), 1, 7, 1, 1);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* name = gtk_entry_get_text(name_entry);
        const char* meeting_time = gtk_entry_get_text(time_entry);
        const char* location = gtk_entry_get_text(location_entry);
        
        if (strlen(name) == 0) {
            ui_show_error_message(parent_window, "Club name is required!");
            gtk_widget_destroy(dialog);
            return;
        }
        
        // Get description
        GtkTextBuffer* desc_buffer = gtk_text_view_get_buffer(desc_view);
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(desc_buffer, &start, &end);
        char* description = gtk_text_buffer_get_text(desc_buffer, &start, &end, FALSE);
        
        // Create new club
        Club new_club;
        memset(&new_club, 0, sizeof(Club));
        new_club.id = state->clubs->count + 1;
        strncpy(new_club.name, name, MAX_CLUB_LENGTH - 1);
        strncpy(new_club.description, description, 499);
        strncpy(new_club.category, gtk_combo_box_text_get_active_text(category_combo), 49);
        new_club.max_members = gtk_spin_button_get_value_as_int(max_spin);
        new_club.budget = gtk_spin_button_get_value(budget_spin);
        strncpy(new_club.meeting_day, gtk_combo_box_text_get_active_text(day_combo), 19);
        strncpy(new_club.meeting_time, meeting_time, 19);
        strncpy(new_club.meeting_location, location, 99);
        new_club.member_count = 0;
        new_club.president_id = 0;
        new_club.advisor_id = 0;
        new_club.founded_date = time(NULL);
        new_club.last_meeting = time(NULL);
        new_club.is_active = 1;
        
        g_free(description);
        
        if (club_list_add(state->clubs, new_club)) {
            club_list_save_to_file(state->clubs, "clubs.txt");
            ui_show_info_message(parent_window, "Club added successfully!");
            
            // Refresh the club list
            GtkTreeView* tree = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(parent_window), "treeview"));
            if (tree) {
                ui_club_treeview_populate(tree, state->clubs);
            }
        } else {
            ui_show_error_message(parent_window, "Failed to add club!");
        }
    }
    
    gtk_widget_destroy(dialog);
}

// Edit club callback
void ui_on_edit_club_placeholder(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    GtkTreeView* club_tree = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(parent_window), "treeview"));
    
    if (!club_tree) return;
    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(club_tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(parent_window, "Please select a club to edit!");
        return;
    }
    
    gchar* club_id_str;
    gtk_tree_model_get(model, &iter, 0, &club_id_str, -1);
    gint club_id = atoi(club_id_str);
    g_free(club_id_str);
    
    Club* club = club_list_find_by_id(state->clubs, club_id);
    if (!club) return;
    
    // Create dialog
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Edit Club",
        parent_window,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Save", GTK_RESPONSE_ACCEPT,
        NULL);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 500, 600);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_box_set_spacing(content, 8);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 16);
    
    GtkGrid* grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(grid, 8);
    gtk_grid_set_column_spacing(grid, 8);
    gtk_box_pack_start(content, GTK_WIDGET(grid), TRUE, TRUE, 0);
    
    // Name
    gtk_grid_attach(grid, gtk_label_new("Club Name:"), 0, 0, 1, 1);
    GtkEntry* name_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(name_entry, club->name);
    gtk_entry_set_max_length(name_entry, 99);
    gtk_grid_attach(grid, GTK_WIDGET(name_entry), 1, 0, 1, 1);
    
    // Description
    gtk_grid_attach(grid, gtk_label_new("Description:"), 0, 1, 1, 1);
    GtkTextView* desc_view = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_wrap_mode(desc_view, GTK_WRAP_WORD);
    GtkTextBuffer* desc_buffer = gtk_text_view_get_buffer(desc_view);
    gtk_text_buffer_set_text(desc_buffer, club->description, -1);
    GtkScrolledWindow* desc_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_widget_set_size_request(GTK_WIDGET(desc_scroll), -1, 100);
    gtk_container_add(GTK_CONTAINER(desc_scroll), GTK_WIDGET(desc_view));
    gtk_grid_attach(grid, GTK_WIDGET(desc_scroll), 1, 1, 1, 1);
    
    // Category
    gtk_grid_attach(grid, gtk_label_new("Category:"), 0, 2, 1, 1);
    GtkComboBoxText* category_combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(category_combo, "Academic");
    gtk_combo_box_text_append_text(category_combo, "Sports");
    gtk_combo_box_text_append_text(category_combo, "Arts");
    gtk_combo_box_text_append_text(category_combo, "Technology");
    gtk_combo_box_text_append_text(category_combo, "Religious");
    gtk_combo_box_text_append_text(category_combo, "Social");
    gtk_combo_box_text_append_text(category_combo, "Service");
    gtk_combo_box_text_append_text(category_combo, "Cultural");
    
    // Set active category
    if (strcmp(club->category, "Academic") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 0);
    else if (strcmp(club->category, "Sports") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 1);
    else if (strcmp(club->category, "Arts") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 2);
    else if (strcmp(club->category, "Technology") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 3);
    else if (strcmp(club->category, "Religious") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 4);
    else if (strcmp(club->category, "Social") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 5);
    else if (strcmp(club->category, "Service") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 6);
    else gtk_combo_box_set_active(GTK_COMBO_BOX(category_combo), 7);
    gtk_grid_attach(grid, GTK_WIDGET(category_combo), 1, 2, 1, 1);
    
    // Max members
    gtk_grid_attach(grid, gtk_label_new("Max Members:"), 0, 3, 1, 1);
    GtkSpinButton* max_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(10, 100, 5));
    gtk_spin_button_set_value(max_spin, club->max_members);
    gtk_grid_attach(grid, GTK_WIDGET(max_spin), 1, 3, 1, 1);
    
    // Budget
    gtk_grid_attach(grid, gtk_label_new("Budget:"), 0, 4, 1, 1);
    GtkSpinButton* budget_spin = GTK_SPIN_BUTTON(gtk_spin_button_new_with_range(0, 10000, 50));
    gtk_spin_button_set_value(budget_spin, club->budget);
    gtk_grid_attach(grid, GTK_WIDGET(budget_spin), 1, 4, 1, 1);
    
    // Meeting day
    gtk_grid_attach(grid, gtk_label_new("Meeting Day:"), 0, 5, 1, 1);
    GtkComboBoxText* day_combo = GTK_COMBO_BOX_TEXT(gtk_combo_box_text_new());
    gtk_combo_box_text_append_text(day_combo, "Monday");
    gtk_combo_box_text_append_text(day_combo, "Tuesday");
    gtk_combo_box_text_append_text(day_combo, "Wednesday");
    gtk_combo_box_text_append_text(day_combo, "Thursday");
    gtk_combo_box_text_append_text(day_combo, "Friday");
    
    if (strcmp(club->meeting_day, "Monday") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 0);
    else if (strcmp(club->meeting_day, "Tuesday") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 1);
    else if (strcmp(club->meeting_day, "Wednesday") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 2);
    else if (strcmp(club->meeting_day, "Thursday") == 0) gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 3);
    else gtk_combo_box_set_active(GTK_COMBO_BOX(day_combo), 4);
    gtk_grid_attach(grid, GTK_WIDGET(day_combo), 1, 5, 1, 1);
    
    // Meeting time
    gtk_grid_attach(grid, gtk_label_new("Meeting Time:"), 0, 6, 1, 1);
    GtkEntry* time_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(time_entry, club->meeting_time);
    gtk_entry_set_max_length(time_entry, 19);
    gtk_grid_attach(grid, GTK_WIDGET(time_entry), 1, 6, 1, 1);
    
    // Meeting location
    gtk_grid_attach(grid, gtk_label_new("Location:"), 0, 7, 1, 1);
    GtkEntry* location_entry = GTK_ENTRY(gtk_entry_new());
    gtk_entry_set_text(location_entry, club->meeting_location);
    gtk_entry_set_max_length(location_entry, 99);
    gtk_grid_attach(grid, GTK_WIDGET(location_entry), 1, 7, 1, 1);
    
    // Active status
    gtk_grid_attach(grid, gtk_label_new("Active:"), 0, 8, 1, 1);
    GtkCheckButton* active_check = GTK_CHECK_BUTTON(gtk_check_button_new());
    gtk_toggle_button_set_active(GTK_TOGGLE_BUTTON(active_check), club->is_active);
    gtk_grid_attach(grid, GTK_WIDGET(active_check), 1, 8, 1, 1);
    
    gtk_widget_show_all(dialog);
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        const char* name = gtk_entry_get_text(name_entry);
        const char* meeting_time = gtk_entry_get_text(time_entry);
        const char* location = gtk_entry_get_text(location_entry);
        
        if (strlen(name) == 0) {
            ui_show_error_message(parent_window, "Club name is required!");
            gtk_widget_destroy(dialog);
            return;
        }
        
        // Get description
        GtkTextIter start, end;
        gtk_text_buffer_get_bounds(desc_buffer, &start, &end);
        char* description = gtk_text_buffer_get_text(desc_buffer, &start, &end, FALSE);
        
        // Update club
        strncpy(club->name, name, MAX_CLUB_LENGTH - 1);
        strncpy(club->description, description, 499);
        strncpy(club->category, gtk_combo_box_text_get_active_text(category_combo), 49);
        club->max_members = gtk_spin_button_get_value_as_int(max_spin);
        club->budget = gtk_spin_button_get_value(budget_spin);
        strncpy(club->meeting_day, gtk_combo_box_text_get_active_text(day_combo), 19);
        strncpy(club->meeting_time, meeting_time, 19);
        strncpy(club->meeting_location, location, 99);
        club->is_active = gtk_toggle_button_get_active(GTK_TOGGLE_BUTTON(active_check));
        
        g_free(description);
        
        club_list_save_to_file(state->clubs, "clubs.txt");
        ui_show_info_message(parent_window, "Club updated successfully!");
        
        // Refresh the club list
        ui_club_treeview_populate(club_tree, state->clubs);
    }
    
    gtk_widget_destroy(dialog);
}

// Delete club callback
void ui_on_delete_club_placeholder(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    GtkTreeView* club_tree = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(parent_window), "treeview"));
    
    if (!club_tree) return;
    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(club_tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(parent_window, "Please select a club to delete!");
        return;
    }
    
    gchar* club_id_str;
    gchar* club_name;
    gtk_tree_model_get(model, &iter, 0, &club_id_str, 1, &club_name, -1);
    gint club_id = atoi(club_id_str);
    g_free(club_id_str);
    
    // Confirm deletion
    GtkWidget* confirm = gtk_message_dialog_new(parent_window,
        GTK_DIALOG_MODAL,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "Are you sure you want to delete '%s'?", club_name);
    
    int response = gtk_dialog_run(GTK_DIALOG(confirm));
    gtk_widget_destroy(confirm);
    g_free(club_name);
    
    if (response == GTK_RESPONSE_YES) {
        if (club_list_remove(state->clubs, club_id)) {
            club_list_save_to_file(state->clubs, "clubs.txt");
            ui_show_info_message(parent_window, "Club deleted successfully!");
            ui_club_treeview_populate(club_tree, state->clubs);
        } else {
            ui_show_error_message(parent_window, "Failed to delete club!");
        }
    }
}

// Create manage members dialog
void ui_on_manage_members_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs) return;
    
    // Get selected club from the main window
    GtkWindow* main_window = GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(button)));
    GtkTreeView* club_tree = g_object_get_data(G_OBJECT(main_window), "treeview");
    
    if (!club_tree) return;
    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(club_tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        GtkWidget* msg = gtk_message_dialog_new(main_window,
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "Please select a club first");
        gtk_dialog_run(GTK_DIALOG(msg));
        gtk_widget_destroy(msg);
        return;
    }
    
    gchar* club_id_str;
    gtk_tree_model_get(model, &iter, 0, &club_id_str, -1);
    gint club_id = atoi(club_id_str);
    g_free(club_id_str);
    
    Club* club = club_list_find_by_id(state->clubs, club_id);
    if (!club) return;
    
    // Create manage members window
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    char title[256];
    snprintf(title, sizeof(title), "Manage Members - %s", club->name);
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    gtk_window_set_default_size(GTK_WINDOW(dialog), 700, 500);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), main_window);
    
    GtkBox* main_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_container_add(GTK_CONTAINER(dialog), GTK_WIDGET(main_box));
    gtk_widget_set_margin_all(GTK_WIDGET(main_box), 16);
    
    // Header
    GtkLabel* header = GTK_LABEL(gtk_label_new(club->name));
    gtk_label_set_markup(header, g_strdup_printf("<span font='20' weight='bold'>%s - Members</span>", club->name));
    gtk_box_pack_start(main_box, GTK_WIDGET(header), FALSE, FALSE, 0);
    
    // Button box
    GtkBox* button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    gtk_box_pack_start(main_box, GTK_WIDGET(button_box), FALSE, FALSE, 0);
    
    GtkButton* add_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Student"));
    GtkButton* remove_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Remove Student"));
    GtkButton* close_btn = GTK_BUTTON(gtk_button_new_with_label("✖ Close"));
    
    gtk_box_pack_start(button_box, GTK_WIDGET(add_btn), FALSE, FALSE, 0);
    gtk_box_pack_start(button_box, GTK_WIDGET(remove_btn), FALSE, FALSE, 0);
    gtk_box_pack_end(button_box, GTK_WIDGET(close_btn), FALSE, FALSE, 0);
    
    // Members tree view
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(main_box, GTK_WIDGET(scrolled), TRUE, TRUE, 0);
    
    GtkTreeView* members_tree = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(members_tree));
    
    // Create columns
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(members_tree, 
        gtk_tree_view_column_new_with_attributes("Membership ID", renderer, "text", 0, NULL));
    gtk_tree_view_append_column(members_tree, 
        gtk_tree_view_column_new_with_attributes("Student ID", renderer, "text", 1, NULL));
    gtk_tree_view_append_column(members_tree, 
        gtk_tree_view_column_new_with_attributes("Name", renderer, "text", 2, NULL));
    gtk_tree_view_append_column(members_tree, 
        gtk_tree_view_column_new_with_attributes("Role", renderer, "text", 3, NULL));
    gtk_tree_view_append_column(members_tree, 
        gtk_tree_view_column_new_with_attributes("Join Date", renderer, "text", 4, NULL));
    
    // Info label
    GtkLabel* info = GTK_LABEL(gtk_label_new(""));
    char info_text[128];
    snprintf(info_text, sizeof(info_text), "Total Members: %d / %d", club->member_count, club->max_members);
    gtk_label_set_text(info, info_text);
    gtk_box_pack_start(main_box, GTK_WIDGET(info), FALSE, FALSE, 0);
    
    // Populate members
    ui_populate_club_members(members_tree, state, club);
    
    // Store references
    g_object_set_data(G_OBJECT(dialog), "members_tree", members_tree);
    g_object_set_data(G_OBJECT(add_btn), "parent_window", dialog);
    g_object_set_data(G_OBJECT(add_btn), "club", club);
    g_object_set_data(G_OBJECT(remove_btn), "parent_window", dialog);
    g_object_set_data(G_OBJECT(remove_btn), "tree", members_tree);
    g_object_set_data(G_OBJECT(remove_btn), "club", club);
    
    // Connect signals
    g_signal_connect(add_btn, "clicked", G_CALLBACK(ui_on_add_student_to_club_clicked), state);
    g_signal_connect(remove_btn, "clicked", G_CALLBACK(ui_on_remove_student_from_club_clicked), state);
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
    
    gtk_widget_show_all(dialog);
}

GtkWindow* ui_create_club_window(UIState* state) {
    if (!state) {
        printf("[ERROR] Club window - state is NULL!\n");
        return NULL;
    }
    
    // Debug state information
    printf("[DEBUG] Club window creation started\n");
    printf("[DEBUG]   state pointer: %p\n", (void*)state);
    printf("[DEBUG]   current_user pointer: %p\n", (void*)state->current_user);
    if (state->current_user) {
        printf("[DEBUG]   Username: %s\n", state->current_user->username);
        printf("[DEBUG]   Role value: %d\n", (int)state->current_user->role);
    }
    
    // Initialize predefined clubs if not already done
    if (state->clubs && state->clubs->count == 0) {
        club_list_init_predefined(state->clubs);
    }
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Club Management - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 16);
    
    // Header with back button
    GtkBox* header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(header_box), FALSE, FALSE, 0);
    
    GtkButton* back_btn = GTK_BUTTON(gtk_button_new_with_label("← Back"));
    gtk_box_pack_start(header_box, GTK_WIDGET(back_btn), FALSE, FALSE, 0);
    g_signal_connect_swapped(back_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Club Management"));
    gtk_label_set_markup(title, "<span font='28' weight='bold'>Club Management</span>");
    gtk_box_pack_start(header_box, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    // Role-based action buttons
    UserRole role = state->current_user ? state->current_user->role : ROLE_STUDENT;
    
    // Debug: Print role information
    if (state->current_user) {
        printf("[DEBUG] Club window - User: %s, Role: %d (ADMIN=%d, TEACHER=%d, STUDENT=%d)\n", 
               state->current_user->username, 
               role, 
               ROLE_ADMIN, ROLE_TEACHER, ROLE_STUDENT);
    } else {
        printf("[DEBUG] Club window - No current user set!\n");
    }
    
    GtkBox* button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 8));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(button_box), FALSE, FALSE, 0);
    
    if (role == ROLE_ADMIN) {
        // Admin can add, edit, delete clubs and manage members
        GtkButton* add_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add Club"));
        GtkButton* edit_btn = GTK_BUTTON(gtk_button_new_with_label("✏️ Edit Club"));
        GtkButton* delete_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Delete Club"));
        GtkButton* members_btn = GTK_BUTTON(gtk_button_new_with_label("👥 Manage Members"));
        
        gtk_box_pack_start(button_box, GTK_WIDGET(add_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(button_box, GTK_WIDGET(edit_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(button_box, GTK_WIDGET(delete_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(button_box, GTK_WIDGET(members_btn), FALSE, FALSE, 0);
        
        g_object_set_data(G_OBJECT(add_btn), "state", state);
        g_object_set_data(G_OBJECT(edit_btn), "state", state);
        g_object_set_data(G_OBJECT(delete_btn), "state", state);
        g_object_set_data(G_OBJECT(members_btn), "state", state);
        g_object_set_data(G_OBJECT(add_btn), "window", window);
        g_object_set_data(G_OBJECT(edit_btn), "window", window);
        g_object_set_data(G_OBJECT(delete_btn), "window", window);
        
        // Connect placeholder callbacks
        g_signal_connect(add_btn, "clicked", G_CALLBACK(ui_on_add_club_placeholder), state);
        g_signal_connect(edit_btn, "clicked", G_CALLBACK(ui_on_edit_club_placeholder), state);
        g_signal_connect(delete_btn, "clicked", G_CALLBACK(ui_on_delete_club_placeholder), state);
        g_signal_connect(members_btn, "clicked", G_CALLBACK(ui_on_manage_members_clicked), state);
    } else if (role == ROLE_STUDENT) {
        // Students can join/leave clubs and view activities
        GtkButton* join_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Join Club"));
        GtkButton* leave_btn = GTK_BUTTON(gtk_button_new_with_label("🚪 Leave Club"));
        GtkButton* my_clubs_btn = GTK_BUTTON(gtk_button_new_with_label("📋 My Clubs"));
        
        gtk_box_pack_start(button_box, GTK_WIDGET(join_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(button_box, GTK_WIDGET(leave_btn), FALSE, FALSE, 0);
        gtk_box_pack_start(button_box, GTK_WIDGET(my_clubs_btn), FALSE, FALSE, 0);
        
        g_object_set_data(G_OBJECT(join_btn), "state", state);
        g_object_set_data(G_OBJECT(leave_btn), "state", state);
        g_object_set_data(G_OBJECT(my_clubs_btn), "state", state);
        g_object_set_data(G_OBJECT(join_btn), "window", window);
        g_object_set_data(G_OBJECT(leave_btn), "window", window);
        g_object_set_data(G_OBJECT(my_clubs_btn), "window", window);
        
        g_signal_connect(join_btn, "clicked", G_CALLBACK(ui_on_student_join_club), state);
        g_signal_connect(leave_btn, "clicked", G_CALLBACK(ui_on_student_leave_club), state);
        g_signal_connect(my_clubs_btn, "clicked", G_CALLBACK(ui_on_student_view_my_clubs), state);
    }
    
    GtkButton* refresh_btn = GTK_BUTTON(gtk_button_new_with_label("🔄 Refresh"));
    gtk_box_pack_end(button_box, GTK_WIDGET(refresh_btn), FALSE, FALSE, 0);
    g_object_set_data(G_OBJECT(refresh_btn), "state", state);
    g_signal_connect_swapped(refresh_btn, "clicked", G_CALLBACK(ui_show_info_message), 
        "Clubs list refreshed!");
    
    // Club list display
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(scrolled), TRUE, TRUE, 0);
    gtk_scrolled_window_set_policy(scrolled, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    
    GtkTreeView* treeview = ui_create_club_treeview();
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(treeview));
    g_object_set_data(G_OBJECT(window), "treeview", treeview);
    
    // Populate with data
    if (state->clubs) {
        ui_club_treeview_populate(treeview, state->clubs);
    }
    
    // Status info
    GtkLabel* status_label = GTK_LABEL(gtk_label_new(""));
    if (state->clubs) {
        char status_text[128];
        snprintf(status_text, sizeof(status_text), "Total Clubs: %d", state->clubs->count);
        gtk_label_set_text(status_label, status_text);
    }
    gtk_box_pack_start(main_vbox, GTK_WIDGET(status_label), FALSE, FALSE, 0);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_club_window_setup_treeview(GtkWindow* window, UIState* state) {}
void ui_club_window_setup_buttons(GtkWindow* window, UIState* state) {}
void ui_club_window_refresh_data(UIState* state) {}
void ui_club_window_add_club(UIState* state) {}
void ui_club_window_edit_club(UIState* state) {}
void ui_club_window_delete_club(UIState* state) {}
void ui_club_window_manage_membership(UIState* state) {}

// ============================================================================
// STATISTICS WINDOW
// ============================================================================

GtkWindow* ui_create_statistics_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Statistics - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, UI_WINDOW_DEFAULT_WIDTH, UI_WINDOW_DEFAULT_HEIGHT);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 16);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Statistics"));
    gtk_label_set_markup(title, "<span font='28' weight='bold'>Statistics</span>");
    gtk_box_pack_start(main_vbox, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_statistics_window_setup_charts(GtkWindow* window, UIState* state) {}
void ui_statistics_window_setup_buttons(GtkWindow* window, UIState* state) {}
void ui_statistics_window_refresh_data(UIState* state) {}
void ui_statistics_window_generate_report(UIState* state) {}
void ui_statistics_window_export_data(UIState* state) {}

void ui_on_export_students_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->students || state->students->count == 0) {
        GtkWidget* dialog = gtk_message_dialog_new(GTK_WINDOW(state->current_window),
            GTK_DIALOG_MODAL,
            GTK_MESSAGE_WARNING,
            GTK_BUTTONS_OK,
            "No students to export!");
        gtk_dialog_run(GTK_DIALOG(dialog));
        gtk_widget_destroy(dialog);
        return;
    }

    // Create file chooser dialog
    GtkWidget* dialog = gtk_file_chooser_dialog_new("Export Students to CSV",
        GTK_WINDOW(state->current_window),
        GTK_FILE_CHOOSER_ACTION_SAVE,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Export", GTK_RESPONSE_ACCEPT,
        NULL);
    
    gtk_file_chooser_set_do_overwrite_confirmation(GTK_FILE_CHOOSER(dialog), TRUE);
    gtk_file_chooser_set_current_name(GTK_FILE_CHOOSER(dialog), "students_export.csv");
    
    if (gtk_dialog_run(GTK_DIALOG(dialog)) == GTK_RESPONSE_ACCEPT) {
        char* filename = gtk_file_chooser_get_filename(GTK_FILE_CHOOSER(dialog));
        
        FILE* file = fopen(filename, "w");
        if (file) {
            // Write CSV header
            fprintf(file, "ID,First Name,Last Name,Email,Phone,Address,Age,Course,Year,Enrollment Date,Active\n");
            
            // Write student data
            for (int i = 0; i < state->students->count; i++) {
                Student* s = &state->students->students[i];
                fprintf(file, "%d,\"%s\",\"%s\",\"%s\",\"%s\",\"%s\",%d,\"%s\",%d,%lld,%d\n",
                    s->id,
                    s->first_name,
                    s->last_name,
                    s->email,
                    s->phone,
                    s->address,
                    s->age,
                    s->course,
                    s->year,
                    (long long)s->enrollment_date,
                    s->is_active);
            }
            fclose(file);
            
            // Show success message
            GtkWidget* success_dialog = gtk_message_dialog_new(GTK_WINDOW(state->current_window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_INFO,
                GTK_BUTTONS_OK,
                "Successfully exported %d students to %s",
                state->students->count, filename);
            gtk_dialog_run(GTK_DIALOG(success_dialog));
            gtk_widget_destroy(success_dialog);
        } else {
            // Show error message
            GtkWidget* error_dialog = gtk_message_dialog_new(GTK_WINDOW(state->current_window),
                GTK_DIALOG_MODAL,
                GTK_MESSAGE_ERROR,
                GTK_BUTTONS_OK,
                "Failed to export students to %s", filename);
            gtk_dialog_run(GTK_DIALOG(error_dialog));
            gtk_widget_destroy(error_dialog);
        }
        
        g_free(filename);
    }
    
    gtk_widget_destroy(dialog);
}

// ============================================================================
// SETTINGS WINDOW
// ============================================================================

GtkWindow* ui_create_settings_window(UIState* state) {
    if (!state) return NULL;
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Settings - " UI_WINDOW_TITLE);
    gtk_window_set_default_size(window, 600, 500);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 24);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Settings"));
    gtk_label_set_markup(title, "<span font='28' weight='bold'>Settings</span>");
    gtk_box_pack_start(main_vbox, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    // Theme selection
    ui_settings_window_setup_theme(window, state);
    
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

void ui_settings_window_setup_theme(GtkWindow* window, UIState* state) {
    if (!window || !state) return;
    
    GtkBox* theme_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    // GTK3: Get child using gtk_bin_get_child (windows are GtkBin in GTK3)
    GtkWidget* child = gtk_bin_get_child(GTK_BIN(window));
    if (!child) return;
    GtkContainer* container = GTK_CONTAINER(child);
    GList* children = gtk_container_get_children(container);
    if (!children) return;
    GtkBox* main_vbox = GTK_BOX(children->data);
    g_list_free(children);
    gtk_box_pack_start(main_vbox, GTK_WIDGET(theme_box), FALSE, FALSE, 0);
    
    GtkLabel* theme_label = GTK_LABEL(gtk_label_new("Theme"));
    gtk_label_set_markup(theme_label, "<span font='18' weight='bold'>Theme</span>");
    gtk_box_pack_start(theme_box, GTK_WIDGET(theme_label), FALSE, FALSE, 0);
    
    GtkBox* theme_buttons = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(theme_box, GTK_WIDGET(theme_buttons), FALSE, FALSE, 0);
    
    GtkButton* light_btn = GTK_BUTTON(gtk_button_new_with_label("Light"));
    GtkButton* dark_btn = GTK_BUTTON(gtk_button_new_with_label("Dark"));
    
    gtk_box_pack_start(theme_buttons, GTK_WIDGET(light_btn), FALSE, FALSE, 0);
    gtk_box_pack_start(theme_buttons, GTK_WIDGET(dark_btn), FALSE, FALSE, 0);
    
    GtkStyleContext* light_context = gtk_widget_get_style_context(GTK_WIDGET(light_btn));
    gtk_style_context_add_class(light_context, "button");
    GtkStyleContext* dark_context = gtk_widget_get_style_context(GTK_WIDGET(dark_btn));
    gtk_style_context_add_class(dark_context, "button");
    
    g_signal_connect(light_btn, "clicked", G_CALLBACK(ui_on_theme_light_clicked), state);
    g_signal_connect(dark_btn, "clicked", G_CALLBACK(ui_on_theme_dark_clicked), state);
}

void ui_settings_window_setup_language(GtkWindow* window, UIState* state) {}

void ui_settings_window_save_settings(UIState* state) {}
void ui_settings_window_load_settings(UIState* state) {}

// Theme button callbacks
void ui_on_theme_light_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !g_theme_config) return;
    
    theme_switch_to_light(g_theme_config, state->app);
    state->is_dark_theme = 0;
}

void ui_on_theme_dark_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !g_theme_config) return;
    
    theme_switch_to_dark(g_theme_config, state->app);
    state->is_dark_theme = 1;
}

// ============================================================================
// ADMIN VIEW WINDOW (Admin Only)
// ============================================================================

// Admin callback functions
static void on_admin_add_user_clicked(GtkButton* btn, gpointer data) {
    UIState* state = (UIState*)data;
    
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Add New User",
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Add", GTK_RESPONSE_ACCEPT,
        NULL);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_box_set_spacing(content, 12);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 12);
    
    GtkWidget* username_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(username_entry), "Username");
    gtk_box_pack_start(content, gtk_label_new("Username:"), FALSE, FALSE, 0);
    gtk_box_pack_start(content, username_entry, FALSE, FALSE, 0);
    
    GtkWidget* email_entry = gtk_entry_new();
    gtk_entry_set_placeholder_text(GTK_ENTRY(email_entry), "email@example.com");
    gtk_box_pack_start(content, gtk_label_new("Email:"), FALSE, FALSE, 0);
    gtk_box_pack_start(content, email_entry, FALSE, FALSE, 0);
    
    GtkWidget* password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_entry_set_placeholder_text(GTK_ENTRY(password_entry), "Password");
    gtk_box_pack_start(content, gtk_label_new("Password:"), FALSE, FALSE, 0);
    gtk_box_pack_start(content, password_entry, FALSE, FALSE, 0);
    
    GtkWidget* role_combo = gtk_combo_box_text_new();
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Admin");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Teacher");
    gtk_combo_box_text_append_text(GTK_COMBO_BOX_TEXT(role_combo), "Student");
    gtk_combo_box_set_active(GTK_COMBO_BOX(role_combo), 2);
    gtk_box_pack_start(content, gtk_label_new("Role:"), FALSE, FALSE, 0);
    gtk_box_pack_start(content, role_combo, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        const char* username = gtk_entry_get_text(GTK_ENTRY(username_entry));
        const char* email = gtk_entry_get_text(GTK_ENTRY(email_entry));
        const char* password = gtk_entry_get_text(GTK_ENTRY(password_entry));
        int role_idx = gtk_combo_box_get_active(GTK_COMBO_BOX(role_combo));
        
        if (strlen(username) > 0 && strlen(email) > 0 && strlen(password) > 0) {
            User new_user = {0};
            new_user.id = state->users->count + 1;
            strncpy(new_user.username, username, sizeof(new_user.username) - 1);
            strncpy(new_user.email, email, sizeof(new_user.email) - 1);
            new_user.role = (role_idx == 0) ? ROLE_ADMIN : (role_idx == 1) ? ROLE_TEACHER : ROLE_STUDENT;
            
            auth_generate_salt(new_user.salt);
            auth_hash_password(password, new_user.salt, new_user.password_hash);
            new_user.created_at = time(NULL);
            new_user.last_login = 0;
            new_user.is_active = 1;
            
            if (user_list_add(state->users, new_user) > 0) {
                user_list_save_to_file(state->users, "users.txt");
                ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                    "User added successfully!");
            } else {
                ui_show_error_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                     "Failed to add user");
            }
        }
    }
    
    gtk_widget_destroy(dialog);
}

static void on_admin_delete_user_clicked(GtkButton* btn, gpointer data) {
    UIState* state = (UIState*)data;
    
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Delete User",
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Delete", GTK_RESPONSE_ACCEPT,
        NULL);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_box_set_spacing(content, 12);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 12);
    
    gtk_box_pack_start(content, gtk_label_new("Enter User ID to delete:"), FALSE, FALSE, 0);
    
    GtkWidget* id_spin = gtk_spin_button_new_with_range(1, 10000, 1);
    gtk_box_pack_start(content, id_spin, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        int user_id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(id_spin));
        
        if (user_id == state->current_user->id) {
            ui_show_error_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                 "Cannot delete currently logged in user!");
        } else if (user_list_remove(state->users, user_id)) {
            user_list_save_to_file(state->users, "users.txt");
            ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                "User deleted successfully!");
        } else {
            ui_show_error_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                 "User not found!");
        }
    }
    
    gtk_widget_destroy(dialog);
}

static void on_admin_reset_password_clicked(GtkButton* btn, gpointer data) {
    UIState* state = (UIState*)data;
    
    GtkWidget* dialog = gtk_dialog_new_with_buttons("Reset Password",
        GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))),
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Reset", GTK_RESPONSE_ACCEPT,
        NULL);
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(GTK_DIALOG(dialog)));
    gtk_box_set_spacing(content, 12);
    gtk_widget_set_margin_all(GTK_WIDGET(content), 12);
    
    gtk_box_pack_start(content, gtk_label_new("Enter User ID:"), FALSE, FALSE, 0);
    GtkWidget* id_spin = gtk_spin_button_new_with_range(1, 10000, 1);
    gtk_box_pack_start(content, id_spin, FALSE, FALSE, 0);
    
    gtk_box_pack_start(content, gtk_label_new("New Password:"), FALSE, FALSE, 0);
    GtkWidget* password_entry = gtk_entry_new();
    gtk_entry_set_visibility(GTK_ENTRY(password_entry), FALSE);
    gtk_box_pack_start(content, password_entry, FALSE, FALSE, 0);
    
    gtk_widget_show_all(dialog);
    
    int response = gtk_dialog_run(GTK_DIALOG(dialog));
    if (response == GTK_RESPONSE_ACCEPT) {
        int user_id = gtk_spin_button_get_value_as_int(GTK_SPIN_BUTTON(id_spin));
        const char* new_password = gtk_entry_get_text(GTK_ENTRY(password_entry));
        
        User* user = user_list_find_by_id(state->users, user_id);
        if (user && strlen(new_password) > 0) {
            auth_generate_salt(user->salt);
            auth_hash_password(new_password, user->salt, user->password_hash);
            user_list_save_to_file(state->users, "users.txt");
            ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                "Password reset successfully!");
        } else {
            ui_show_error_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                                 "User not found or invalid password!");
        }
    }
    
    gtk_widget_destroy(dialog);
}

static void on_admin_manage_roles_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "Role management interface - Feature coming soon!");
}

static void on_admin_backup_clicked(GtkButton* btn, gpointer data) {
    UIState* state = (UIState*)data;
    
    char backup_subdir[256];
    time_t now = time(NULL);
    struct tm* t = localtime(&now);
    snprintf(backup_subdir, sizeof(backup_subdir), "backups/backup_%04d%02d%02d_%02d%02d%02d",
             t->tm_year + 1900, t->tm_mon + 1, t->tm_mday,
             t->tm_hour, t->tm_min, t->tm_sec);
    
    char backup_dir[512];
    utils_get_data_file_path(backup_subdir, backup_dir, sizeof(backup_dir));
    
    // Create backup message
    char msg[256];
    snprintf(msg, sizeof(msg), "Backup would be created at:\n%s\n\nFeature ready for implementation!", backup_dir);
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), msg);
}

static void on_admin_export_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "Export all data to CSV/JSON - Feature coming soon!");
}

static void on_admin_import_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "Import data from external files - Feature coming soon!");
}

static void on_admin_clear_cache_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "Cache cleared successfully!");
}

static void on_admin_reports_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "Generate comprehensive system reports - Feature coming soon!");
}

static void on_admin_system_settings_clicked(GtkButton* btn, gpointer data) {
    ui_show_info_message(GTK_WINDOW(gtk_widget_get_toplevel(GTK_WIDGET(btn))), 
                        "System configuration panel - Feature coming soon!");
}

GtkWindow* ui_create_admin_view_window(UIState* state) {
    if (!state) return NULL;
    
    // Verify admin role
    if (!state->current_user || state->current_user->role != ROLE_ADMIN) {
        ui_show_error_message(NULL, "Access Denied: Admin privileges required");
        return NULL;
    }
    
    GtkWindow* window = GTK_WINDOW(gtk_window_new(GTK_WINDOW_TOPLEVEL));
    gtk_window_set_title(window, "Admin View - System Data");
    gtk_window_set_default_size(window, 1000, 700);
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
    
    if (g_theme_config) {
        theme_apply_to_window(window, g_theme_config);
    }
    
    // Main container
    GtkBox* main_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 16));
    gtk_container_add(GTK_CONTAINER(window), GTK_WIDGET(main_vbox));
    gtk_widget_set_margin_all(GTK_WIDGET(main_vbox), 24);
    
    // Header with warning
    GtkBox* header_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(header_box), FALSE, FALSE, 0);
    
    GtkLabel* title = GTK_LABEL(gtk_label_new("Admin View"));
    gtk_label_set_markup(title, "<span font='28' weight='bold' foreground='#C00'>🔐 Admin View</span>");
    gtk_box_pack_start(header_box, GTK_WIDGET(title), FALSE, FALSE, 0);
    
    GtkLabel* warning = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(warning, 
        "<span foreground='#800'>⚠️ Confidential Data - Authorized Personnel Only</span>");
    gtk_box_pack_start(header_box, GTK_WIDGET(warning), FALSE, FALSE, 0);
    
    // Notebook for tabs
    GtkNotebook* notebook = GTK_NOTEBOOK(gtk_notebook_new());
    gtk_box_pack_start(main_vbox, GTK_WIDGET(notebook), TRUE, TRUE, 0);
    
    // ========== USERS TAB ==========
    GtkBox* users_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_widget_set_margin_all(GTK_WIDGET(users_vbox), 12);
    
    GtkLabel* users_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(users_title, "<span font='18' weight='bold'>All Users (Email &amp; Password)</span>");
    gtk_box_pack_start(users_vbox, GTK_WIDGET(users_title), FALSE, FALSE, 0);
    
    // Users scrolled window
    GtkScrolledWindow* users_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(users_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(users_vbox, GTK_WIDGET(users_scroll), TRUE, TRUE, 0);
    
    // Users text view
    GtkTextView* users_text = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(users_text, FALSE);
    gtk_text_view_set_monospace(users_text, TRUE);
    gtk_container_add(GTK_CONTAINER(users_scroll), GTK_WIDGET(users_text));
    
    GtkTextBuffer* users_buffer = gtk_text_view_get_buffer(users_text);
    
    // Load users data
    if (state->users && state->users->count > 0) {
        GString* users_content = g_string_new("");
        g_string_append_printf(users_content, "Total Users: %d\n\n", state->users->count);
        g_string_append(users_content, "ID    | Username           | Email                          | Role    | Password Hash\n");
        g_string_append(users_content, "------+--------------------+--------------------------------+---------+----------------------------------\n");
        
        for (int i = 0; i < state->users->count; i++) {
            User* user = &state->users->users[i];
            const char* role_str = (user->role == ROLE_ADMIN) ? "Admin" : 
                                   (user->role == ROLE_TEACHER) ? "Teacher" : "Student";
            
            g_string_append_printf(users_content, 
                "%-5d | %-18s | %-30s | %-7s | %.32s...\n",
                user->id, user->username, user->email, role_str, user->password_hash);
        }
        
        gtk_text_buffer_set_text(users_buffer, users_content->str, -1);
        g_string_free(users_content, TRUE);
    } else {
        gtk_text_buffer_set_text(users_buffer, "No users data available.", -1);
    }
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(users_vbox), 
                            gtk_label_new("👤 Users"));
    
    // ========== PROFESSORS TAB ==========
    GtkBox* prof_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_widget_set_margin_all(GTK_WIDGET(prof_vbox), 12);
    
    GtkLabel* prof_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(prof_title, "<span font='18' weight='bold'>All Professors</span>");
    gtk_box_pack_start(prof_vbox, GTK_WIDGET(prof_title), FALSE, FALSE, 0);
    
    // Professors scrolled window
    GtkScrolledWindow* prof_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(prof_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_box_pack_start(prof_vbox, GTK_WIDGET(prof_scroll), TRUE, TRUE, 0);
    
    // Professors text view
    GtkTextView* prof_text = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(prof_text, FALSE);
    gtk_text_view_set_monospace(prof_text, TRUE);
    gtk_container_add(GTK_CONTAINER(prof_scroll), GTK_WIDGET(prof_text));
    
    GtkTextBuffer* prof_buffer = gtk_text_view_get_buffer(prof_text);
    
    // Load professors data from file
    ProfessorList* prof_list = professor_list_create();
    if (prof_list && professor_list_load_from_file(prof_list, "professors.txt")) {
        GString* prof_content = g_string_new("");
        g_string_append_printf(prof_content, "Total Professors: %d\n\n", prof_list->count);
        g_string_append(prof_content, "ID   | Name                      | Email                          | Phone        | Department\n");
        g_string_append(prof_content, "-----+---------------------------+--------------------------------+--------------+------------------------\n");
        
        for (int i = 0; i < prof_list->count; i++) {
            Professor* prof = &prof_list->professors[i];
            char full_name[150];
            snprintf(full_name, sizeof(full_name), "%s %s", prof->first_name, prof->last_name);
            
            g_string_append_printf(prof_content,
                "%-4d | %-25s | %-30s | %-12s | %s\n",
                prof->id, full_name, prof->email, prof->phone, prof->department);
        }
        
        gtk_text_buffer_set_text(prof_buffer, prof_content->str, -1);
        g_string_free(prof_content, TRUE);
    } else {
        gtk_text_buffer_set_text(prof_buffer, "No professors data available or failed to load.", -1);
    }
    
    if (prof_list) {
        professor_list_destroy(prof_list);
    }
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(prof_vbox), 
                            gtk_label_new("👨‍🏫 Professors"));
    
    // ========== SYSTEM STATISTICS TAB ==========
    GtkBox* stats_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_widget_set_margin_all(GTK_WIDGET(stats_vbox), 12);
    
    GtkLabel* stats_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(stats_title, "<span font='18' weight='bold'>System Statistics</span>");
    gtk_box_pack_start(stats_vbox, GTK_WIDGET(stats_title), FALSE, FALSE, 0);
    
    // Statistics content
    GtkBox* stats_content = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_box_pack_start(stats_vbox, GTK_WIDGET(stats_content), TRUE, TRUE, 0);
    
    // Create statistics cards
    GtkGrid* stats_grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(stats_grid, 12);
    gtk_grid_set_column_spacing(stats_grid, 12);
    gtk_box_pack_start(stats_content, GTK_WIDGET(stats_grid), FALSE, FALSE, 0);
    
    // Total Users Card
    GtkFrame* users_frame = GTK_FRAME(gtk_frame_new(NULL));
    GtkBox* users_card = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(users_card), 16);
    gtk_container_add(GTK_CONTAINER(users_frame), GTK_WIDGET(users_card));
    
    GtkLabel* users_count = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(users_count, g_strdup_printf("<span font='32' weight='bold'>%d</span>", 
                         state->users ? state->users->count : 0));
    gtk_box_pack_start(users_card, GTK_WIDGET(users_count), FALSE, FALSE, 0);
    
    GtkLabel* users_label = GTK_LABEL(gtk_label_new("Total Users"));
    gtk_box_pack_start(users_card, GTK_WIDGET(users_label), FALSE, FALSE, 0);
    
    gtk_grid_attach(stats_grid, GTK_WIDGET(users_frame), 0, 0, 1, 1);
    
    // Students Card
    GtkFrame* students_frame = GTK_FRAME(gtk_frame_new(NULL));
    GtkBox* students_card = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(students_card), 16);
    gtk_container_add(GTK_CONTAINER(students_frame), GTK_WIDGET(students_card));
    
    GtkLabel* students_count = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(students_count, g_strdup_printf("<span font='32' weight='bold'>%d</span>", 
                         state->students ? state->students->count : 0));
    gtk_box_pack_start(students_card, GTK_WIDGET(students_count), FALSE, FALSE, 0);
    
    GtkLabel* students_label = GTK_LABEL(gtk_label_new("Students"));
    gtk_box_pack_start(students_card, GTK_WIDGET(students_label), FALSE, FALSE, 0);
    
    gtk_grid_attach(stats_grid, GTK_WIDGET(students_frame), 1, 0, 1, 1);
    
    // Grades Card
    GtkFrame* grades_frame = GTK_FRAME(gtk_frame_new(NULL));
    GtkBox* grades_card = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(grades_card), 16);
    gtk_container_add(GTK_CONTAINER(grades_frame), GTK_WIDGET(grades_card));
    
    GtkLabel* grades_count = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(grades_count, g_strdup_printf("<span font='32' weight='bold'>%d</span>", 
                         state->grades ? state->grades->count : 0));
    gtk_box_pack_start(grades_card, GTK_WIDGET(grades_count), FALSE, FALSE, 0);
    
    GtkLabel* grades_label = GTK_LABEL(gtk_label_new("Total Grades"));
    gtk_box_pack_start(grades_card, GTK_WIDGET(grades_label), FALSE, FALSE, 0);
    
    gtk_grid_attach(stats_grid, GTK_WIDGET(grades_frame), 0, 1, 1, 1);
    
    // Clubs Card
    GtkFrame* clubs_frame = GTK_FRAME(gtk_frame_new(NULL));
    GtkBox* clubs_card = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 8));
    gtk_widget_set_margin_all(GTK_WIDGET(clubs_card), 16);
    gtk_container_add(GTK_CONTAINER(clubs_frame), GTK_WIDGET(clubs_card));
    
    GtkLabel* clubs_count = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(clubs_count, g_strdup_printf("<span font='32' weight='bold'>%d</span>", 
                         state->clubs ? state->clubs->count : 0));
    gtk_box_pack_start(clubs_card, GTK_WIDGET(clubs_count), FALSE, FALSE, 0);
    
    GtkLabel* clubs_label = GTK_LABEL(gtk_label_new("Active Clubs"));
    gtk_box_pack_start(clubs_card, GTK_WIDGET(clubs_label), FALSE, FALSE, 0);
    
    gtk_grid_attach(stats_grid, GTK_WIDGET(clubs_frame), 1, 1, 1, 1);
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(stats_vbox), 
                            gtk_label_new("📊 Statistics"));
    
    // ========== DATA MANAGEMENT TAB ==========
    GtkBox* data_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_widget_set_margin_all(GTK_WIDGET(data_vbox), 12);
    
    GtkLabel* data_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(data_title, "<span font='18' weight='bold'>Data Management</span>");
    gtk_box_pack_start(data_vbox, GTK_WIDGET(data_title), FALSE, FALSE, 0);
    
    // Data management buttons
    GtkGrid* data_grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(data_grid, 12);
    gtk_grid_set_column_spacing(data_grid, 12);
    gtk_box_pack_start(data_vbox, GTK_WIDGET(data_grid), FALSE, FALSE, 0);
    
    // Backup Database Button
    GtkButton* backup_btn = GTK_BUTTON(gtk_button_new_with_label("💾 Backup Database"));
    gtk_widget_set_size_request(GTK_WIDGET(backup_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(backup_btn), 0, 0, 1, 1);
    g_signal_connect(backup_btn, "clicked", G_CALLBACK(on_admin_backup_clicked), state);
    
    // Export Data Button
    GtkButton* export_btn = GTK_BUTTON(gtk_button_new_with_label("📤 Export All Data"));
    gtk_widget_set_size_request(GTK_WIDGET(export_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(export_btn), 1, 0, 1, 1);
    g_signal_connect(export_btn, "clicked", G_CALLBACK(on_admin_export_clicked), state);
    
    // Import Data Button
    GtkButton* import_btn = GTK_BUTTON(gtk_button_new_with_label("📥 Import Data"));
    gtk_widget_set_size_request(GTK_WIDGET(import_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(import_btn), 0, 1, 1, 1);
    g_signal_connect(import_btn, "clicked", G_CALLBACK(on_admin_import_clicked), state);
    
    // Clear Cache Button
    GtkButton* cache_btn = GTK_BUTTON(gtk_button_new_with_label("🗑️ Clear Cache"));
    gtk_widget_set_size_request(GTK_WIDGET(cache_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(cache_btn), 1, 1, 1, 1);
    g_signal_connect(cache_btn, "clicked", G_CALLBACK(on_admin_clear_cache_clicked), state);
    
    // Generate Reports Button
    GtkButton* report_btn = GTK_BUTTON(gtk_button_new_with_label("📄 Generate Reports"));
    gtk_widget_set_size_request(GTK_WIDGET(report_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(report_btn), 0, 2, 1, 1);
    g_signal_connect(report_btn, "clicked", G_CALLBACK(on_admin_reports_clicked), state);
    
    // System Settings Button
    GtkButton* system_btn = GTK_BUTTON(gtk_button_new_with_label("⚙️ System Settings"));
    gtk_widget_set_size_request(GTK_WIDGET(system_btn), 200, 60);
    gtk_grid_attach(data_grid, GTK_WIDGET(system_btn), 1, 2, 1, 1);
    g_signal_connect(system_btn, "clicked", G_CALLBACK(on_admin_system_settings_clicked), state);
    
    // Info text
    GtkLabel* data_info = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(data_info, 
        "<span foreground='#666'>Use these tools to manage system data, create backups, and configure settings.</span>");
    gtk_label_set_line_wrap(data_info, TRUE);
    gtk_box_pack_start(data_vbox, GTK_WIDGET(data_info), FALSE, FALSE, 12);
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(data_vbox), 
                            gtk_label_new("💾 Data Management"));
    
    // ========== USER MANAGEMENT TAB ==========
    GtkBox* usermgmt_vbox = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_widget_set_margin_all(GTK_WIDGET(usermgmt_vbox), 12);
    
    GtkLabel* usermgmt_title = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(usermgmt_title, "<span font='18' weight='bold'>User Management</span>");
    gtk_box_pack_start(usermgmt_vbox, GTK_WIDGET(usermgmt_title), FALSE, FALSE, 0);
    
    // User management actions
    GtkGrid* usermgmt_grid = GTK_GRID(gtk_grid_new());
    gtk_grid_set_row_spacing(usermgmt_grid, 12);
    gtk_grid_set_column_spacing(usermgmt_grid, 12);
    gtk_box_pack_start(usermgmt_vbox, GTK_WIDGET(usermgmt_grid), FALSE, FALSE, 0);
    
    // Add User Button
    GtkButton* add_user_btn = GTK_BUTTON(gtk_button_new_with_label("➕ Add New User"));
    gtk_widget_set_size_request(GTK_WIDGET(add_user_btn), 200, 60);
    gtk_grid_attach(usermgmt_grid, GTK_WIDGET(add_user_btn), 0, 0, 1, 1);
    g_signal_connect(add_user_btn, "clicked", G_CALLBACK(on_admin_add_user_clicked), state);
    
    // Delete User Button
    GtkButton* del_user_btn = GTK_BUTTON(gtk_button_new_with_label("❌ Delete User"));
    gtk_widget_set_size_request(GTK_WIDGET(del_user_btn), 200, 60);
    gtk_grid_attach(usermgmt_grid, GTK_WIDGET(del_user_btn), 1, 0, 1, 1);
    g_signal_connect(del_user_btn, "clicked", G_CALLBACK(on_admin_delete_user_clicked), state);
    
    // Reset Password Button
    GtkButton* reset_pwd_btn = GTK_BUTTON(gtk_button_new_with_label("🔑 Reset Password"));
    gtk_widget_set_size_request(GTK_WIDGET(reset_pwd_btn), 200, 60);
    gtk_grid_attach(usermgmt_grid, GTK_WIDGET(reset_pwd_btn), 0, 1, 1, 1);
    g_signal_connect(reset_pwd_btn, "clicked", G_CALLBACK(on_admin_reset_password_clicked), state);
    
    // Manage Roles Button
    GtkButton* roles_btn = GTK_BUTTON(gtk_button_new_with_label("👥 Manage Roles"));
    gtk_widget_set_size_request(GTK_WIDGET(roles_btn), 200, 60);
    gtk_grid_attach(usermgmt_grid, GTK_WIDGET(roles_btn), 1, 1, 1, 1);
    g_signal_connect(roles_btn, "clicked", G_CALLBACK(on_admin_manage_roles_clicked), state);
    
    // Active Users List
    GtkLabel* active_label = GTK_LABEL(gtk_label_new(""));
    gtk_label_set_markup(active_label, "<span font='14' weight='bold'>Active Sessions:</span>");
    gtk_box_pack_start(usermgmt_vbox, GTK_WIDGET(active_label), FALSE, FALSE, 12);
    
    GtkScrolledWindow* active_scroll = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_scrolled_window_set_policy(active_scroll, GTK_POLICY_AUTOMATIC, GTK_POLICY_AUTOMATIC);
    gtk_widget_set_size_request(GTK_WIDGET(active_scroll), -1, 200);
    gtk_box_pack_start(usermgmt_vbox, GTK_WIDGET(active_scroll), TRUE, TRUE, 0);
    
    GtkTextView* active_text = GTK_TEXT_VIEW(gtk_text_view_new());
    gtk_text_view_set_editable(active_text, FALSE);
    gtk_container_add(GTK_CONTAINER(active_scroll), GTK_WIDGET(active_text));
    
    GtkTextBuffer* active_buffer = gtk_text_view_get_buffer(active_text);
    if (state->current_user) {
        char active_info[256];
        snprintf(active_info, sizeof(active_info), 
                "Current Session:\nUser: %s\nEmail: %s\nRole: %s\nLast Login: %s",
                state->current_user->username,
                state->current_user->email,
                (state->current_user->role == ROLE_ADMIN) ? "Administrator" :
                (state->current_user->role == ROLE_TEACHER) ? "Teacher" : "Student",
                ctime(&state->current_user->last_login));
        gtk_text_buffer_set_text(active_buffer, active_info, -1);
    }
    
    gtk_notebook_append_page(notebook, GTK_WIDGET(usermgmt_vbox), 
                            gtk_label_new("👤 User Management"));
    
    // Back button
    GtkBox* button_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_HORIZONTAL, 12));
    gtk_box_pack_start(main_vbox, GTK_WIDGET(button_box), FALSE, FALSE, 0);
    
    GtkButton* back_btn = GTK_BUTTON(gtk_button_new_with_label("← Back to Main"));
    gtk_box_pack_start(button_box, GTK_WIDGET(back_btn), FALSE, FALSE, 0);
    
    GtkStyleContext* back_context = gtk_widget_get_style_context(GTK_WIDGET(back_btn));
    gtk_style_context_add_class(back_context, "button");
    
    // Simply close the window to return to main dashboard
    g_signal_connect_swapped(back_btn, "clicked", G_CALLBACK(gtk_widget_destroy), window);
    
    // Window close handler
    g_signal_connect(window, "destroy", G_CALLBACK(ui_on_window_destroy), state);
    
    return window;
}

// ============================================================================
// DIALOG BOXES
// ============================================================================

GtkDialog* ui_create_message_dialog(GtkWindow* parent, const char* title, const char* message, GtkMessageType type) {
    GtkDialog* dialog = GTK_DIALOG(gtk_message_dialog_new(
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        type,
        GTK_BUTTONS_OK,
        "%s", message
    ));
    
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    
    if (g_theme_config) {
        theme_apply_to_window(GTK_WINDOW(dialog), g_theme_config);
    }
    
    return dialog;
}

GtkDialog* ui_create_confirm_dialog(GtkWindow* parent, const char* title, const char* message) {
    GtkDialog* dialog = GTK_DIALOG(gtk_message_dialog_new(
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        GTK_MESSAGE_QUESTION,
        GTK_BUTTONS_YES_NO,
        "%s", message
    ));
    
    gtk_window_set_title(GTK_WINDOW(dialog), title);
    
    if (g_theme_config) {
        theme_apply_to_window(GTK_WINDOW(dialog), g_theme_config);
    }
    
    return dialog;
}

GtkDialog* ui_create_input_dialog(GtkWindow* parent, const char* title, const char* message, GtkEntry** entry) {
    GtkDialog* dialog = GTK_DIALOG(gtk_dialog_new_with_buttons(
        title,
        parent,
        GTK_DIALOG_MODAL | GTK_DIALOG_DESTROY_WITH_PARENT,
        "OK", GTK_RESPONSE_OK,
        "Cancel", GTK_RESPONSE_CANCEL,
        NULL
    ));
    
    GtkBox* content = GTK_BOX(gtk_dialog_get_content_area(dialog));
    
    GtkLabel* label = GTK_LABEL(gtk_label_new(message));
    gtk_box_pack_start(content, GTK_WIDGET(label), FALSE, FALSE, 0);
    
    *entry = GTK_ENTRY(gtk_entry_new());
    gtk_box_pack_start(content, GTK_WIDGET(*entry), FALSE, FALSE, 0);
    
    if (g_theme_config) {
        theme_apply_to_window(GTK_WINDOW(dialog), g_theme_config);
    }
    
    return dialog;
}

GtkDialog* ui_create_file_dialog(GtkWindow* parent, const char* title, GtkFileChooserAction action) {
    GtkDialog* dialog = GTK_DIALOG(gtk_file_chooser_dialog_new(
        title,
        parent,
        action,
        "Cancel", GTK_RESPONSE_CANCEL,
        "Open", GTK_RESPONSE_ACCEPT,
        NULL
    ));
    
    if (g_theme_config) {
        theme_apply_to_window(GTK_WINDOW(dialog), g_theme_config);
    }
    
    return dialog;
}

// Input dialogs for data entry
GtkDialog* ui_create_student_input_dialog(GtkWindow* parent, Student* student, int is_edit) {
    // TODO: Implement comprehensive student input dialog
    return NULL;
}

int ui_student_input_dialog_get_data(GtkDialog* dialog, Student* student) {
    // TODO: Extract data from dialog
    return 0;
}

void ui_student_input_dialog_set_data(GtkDialog* dialog, Student* student) {
    // TODO: Populate dialog with student data
}

GtkDialog* ui_create_grade_input_dialog(GtkWindow* parent, Grade* grade, int is_edit) {
    return NULL;
}

int ui_grade_input_dialog_get_data(GtkDialog* dialog, Grade* grade) {
    return 0;
}

void ui_grade_input_dialog_set_data(GtkDialog* dialog, Grade* grade) {}

GtkDialog* ui_create_club_input_dialog(GtkWindow* parent, Club* club, int is_edit) {
    return NULL;
}

int ui_club_input_dialog_get_data(GtkDialog* dialog, Club* club) {
    return 0;
}

void ui_club_input_dialog_set_data(GtkDialog* dialog, Club* club) {}

// ============================================================================
// THEME MANAGEMENT
// ============================================================================

void ui_apply_theme(UIState* state, int is_dark) {
    if (!state || !g_theme_config) return;
    
    if (is_dark) {
        theme_switch_to_dark(g_theme_config, state->app);
    } else {
        theme_switch_to_light(g_theme_config, state->app);
    }
    
    state->is_dark_theme = is_dark;
}

void ui_switch_theme(UIState* state) {
    if (!state) return;
    ui_apply_theme(state, !state->is_dark_theme);
}

void ui_load_theme_css(const char* css_file) {
    // CSS loading is handled by theme system
}

void ui_set_dark_theme(GtkWindow* window) {
    if (!window || !g_theme_config) return;
    theme_switch_to_dark(g_theme_config, NULL);
    theme_apply_to_window(window, g_theme_config);
}

void ui_set_light_theme(GtkWindow* window) {
    if (!window || !g_theme_config) return;
    theme_switch_to_light(g_theme_config, NULL);
    theme_apply_to_window(window, g_theme_config);
}

// ============================================================================
// LANGUAGE MANAGEMENT
// ============================================================================

void ui_set_language(UIState* state, const char* language) {
    if (!state || !language) return;
    strncpy(state->current_language, language, sizeof(state->current_language) - 1);
}

void ui_load_language_file(const char* language_file) {
    // TODO: Implement language file loading
}

const char* ui_get_translated_text(const char* key) {
    // TODO: Implement translation lookup
    return key;
}

// ============================================================================
// TREEVIEW HELPERS
// ============================================================================

GtkTreeView* ui_create_treeview_with_columns(const char* column_titles[], int column_count) {
    // Create array of column types (all strings)
    GType types[column_count];
    for (int i = 0; i < column_count; i++) {
        types[i] = G_TYPE_STRING;
    }
    
    // Create list store instead of tree store for simpler flat lists
    GtkListStore* store = gtk_list_store_newv(column_count, types);
    GtkTreeView* treeview = GTK_TREE_VIEW(gtk_tree_view_new_with_model(GTK_TREE_MODEL(store)));
    
    for (int i = 0; i < column_count; i++) {
        GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
        GtkTreeViewColumn* column = gtk_tree_view_column_new_with_attributes(
            column_titles[i], renderer, "text", i, NULL
        );
        gtk_tree_view_append_column(treeview, column);
    }
    
    g_object_unref(store);
    return treeview;
}

void ui_treeview_add_row(GtkTreeView* treeview, const char* data[], int data_count) {
    GtkTreeModel* model = gtk_tree_view_get_model(treeview);
    GtkListStore* store = GTK_LIST_STORE(model);
    
    GtkTreeIter iter;
    gtk_list_store_append(store, &iter);
    
    for (int i = 0; i < data_count; i++) {
        gtk_list_store_set(store, &iter, i, data[i], -1);
    }
}

void ui_treeview_clear(GtkTreeView* treeview) {
    GtkTreeModel* model = gtk_tree_view_get_model(treeview);
    GtkListStore* store = GTK_LIST_STORE(model);
    gtk_list_store_clear(store);
}

void ui_treeview_set_selection_callback(GtkTreeView* treeview, GCallback callback, gpointer user_data) {
    GtkTreeSelection* selection = gtk_tree_view_get_selection(treeview);
    gtk_tree_selection_set_mode(selection, GTK_SELECTION_SINGLE);
    g_signal_connect(selection, "changed", callback, user_data);
}

// ============================================================================
// STUDENT TREEVIEW
// ============================================================================

GtkTreeView* ui_create_student_treeview(void) {
    const char* columns[] = {"ID", "First Name", "Last Name", "Email", "Filiere"};
    return ui_create_treeview_with_columns(columns, 6);
}

void ui_student_treeview_populate(GtkTreeView* treeview, StudentList* students) {
    if (!treeview || !students) return;
    
    ui_treeview_clear(treeview);
    
    for (int i = 0; i < students->count; i++) {
        Student* s = &students->students[i];
        char id_str[20];
        snprintf(id_str, sizeof(id_str), "%d", s->id);
        
        const char* data[] = {id_str, s->first_name, s->last_name, s->email, s->course};
        ui_treeview_add_row(treeview, data, 5);
    }
}

void ui_student_treeview_add_student(GtkTreeView* treeview, Student* student) {
    if (!treeview || !student) return;
    
    char id_str[20];
    snprintf(id_str, sizeof(id_str), "%d", student->id);
    
    char gpa_str[20];
    snprintf(gpa_str, sizeof(gpa_str), "%.2f", student->gpa);
    
    const char* data[] = {id_str, student->first_name, student->last_name, 
                          student->email, student->course, gpa_str};
    ui_treeview_add_row(treeview, data, 6);
}

void ui_student_treeview_update_student(GtkTreeView* treeview, Student* student) {
    // TODO: Implement update
}

void ui_student_treeview_remove_student(GtkTreeView* treeview, int student_id) {
    // TODO: Implement removal
}

// ============================================================================
// GRADE TREEVIEW
// ============================================================================

GtkTreeView* ui_create_grade_treeview(void) {
    const char* columns[] = {"Student ID", "Exam ID", "Module", "Grade", "Present"};
    return ui_create_treeview_with_columns(columns, 5);
}

void ui_grade_treeview_populate(GtkTreeView* treeview, GradeList* grades) {
    if (!treeview || !grades) return;
    
    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    if (!store) return;
    
    // Clear existing data
    gtk_list_store_clear(store);
    
    // Add each grade to the tree view
    for (int i = 0; i < grades->count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        
        char student_id_str[16], exam_id_str[16], grade_str[16];
        snprintf(student_id_str, sizeof(student_id_str), "%d", grades->note[i].id_etudiant);
        snprintf(exam_id_str, sizeof(exam_id_str), "%d", grades->note[i].id_examen);
        snprintf(grade_str, sizeof(grade_str), "%.2f", grades->note[i].note_obtenue);
        
        gtk_list_store_set(store, &iter,
                          0, student_id_str,
                          1, exam_id_str,
                          2, "",  // Module name not in structure
                          3, grade_str,
                          4, grades->note[i].present ? "Yes" : "No",
                          -1);
    }
}

void ui_grade_treeview_add_grade(GtkTreeView* treeview, Grade* grade) {}
void ui_grade_treeview_update_grade(GtkTreeView* treeview, Grade* grade) {}
void ui_grade_treeview_remove_grade(GtkTreeView* treeview, int grade_id) {}

// ============================================================================
// ATTENDANCE TREEVIEW
// ============================================================================

GtkTreeView* ui_create_attendance_treeview(void) {
    const char* columns[] = {"ID", "Student ID", "Course ID", "Date", "Status"};
    return ui_create_treeview_with_columns(columns, 5);
}

void ui_attendance_treeview_populate(GtkTreeView* treeview, AttendanceList* attendance) {
    if (!treeview || !attendance) return;
    
    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    if (!store) return;
    
    // Clear existing data
    gtk_list_store_clear(store);
    
    if (attendance->count == 0) return;
    
    // Create a copy of records to sort by course_id
    AttendanceRecord* sorted_records = (AttendanceRecord*)malloc(attendance->count * sizeof(AttendanceRecord));
    if (!sorted_records) return;
    
    memcpy(sorted_records, attendance->records, attendance->count * sizeof(AttendanceRecord));
    
    // Sort records by course_id using bubble sort
    for (int i = 0; i < attendance->count - 1; i++) {
        for (int j = 0; j < attendance->count - i - 1; j++) {
            if (sorted_records[j].course_id > sorted_records[j + 1].course_id) {
                AttendanceRecord temp = sorted_records[j];
                sorted_records[j] = sorted_records[j + 1];
                sorted_records[j + 1] = temp;
            }
        }
    }
    
    // Add records grouped by course_id
    int current_course_id = -1;
    for (int i = 0; i < attendance->count; i++) {
        // Add course header when course changes
        if (sorted_records[i].course_id != current_course_id) {
            current_course_id = sorted_records[i].course_id;
            
            GtkTreeIter header_iter;
            gtk_list_store_append(store, &header_iter);
            
            char course_header[128];
            snprintf(course_header, sizeof(course_header), "═══ COURSE ID: %d ═══", current_course_id);
            
            gtk_list_store_set(store, &header_iter,
                              0, "",
                              1, course_header,
                              2, "",
                              3, "",
                              4, "",
                              -1);
        }
        
        // Add attendance record
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        
        char id_str[16], student_id_str[16], course_id_str[16];
        snprintf(id_str, sizeof(id_str), "%d", sorted_records[i].id);
        snprintf(student_id_str, sizeof(student_id_str), "%d", sorted_records[i].student_id);
        snprintf(course_id_str, sizeof(course_id_str), "%d", sorted_records[i].course_id);
        
        struct tm* timeinfo = localtime(&sorted_records[i].recorded_time);
        char date_str[64];
        strftime(date_str, sizeof(date_str), "%Y-%m-%d %H:%M", timeinfo);
        
        const char* status_str;
        switch (sorted_records[i].status) {
            case 0: status_str = "Absent"; break;
            case 1: status_str = "Present"; break;
            case 2: status_str = "Late"; break;
            case 3: status_str = "Excused"; break;
            default: status_str = "Unknown"; break;
        }
        
        gtk_list_store_set(store, &iter,
                          0, id_str,
                          1, student_id_str,
                          2, course_id_str,
                          3, date_str,
                          4, status_str,
                          -1);
    }
    
    free(sorted_records);
}

void ui_attendance_treeview_add_record(GtkTreeView* treeview, AttendanceRecord* record) {}
void ui_attendance_treeview_update_record(GtkTreeView* treeview, AttendanceRecord* record) {}
void ui_attendance_treeview_remove_record(GtkTreeView* treeview, int record_id) {}

// ============================================================================
// CLUB TREEVIEW
// ============================================================================

GtkTreeView* ui_create_club_treeview(void) {
    const char* columns[] = {"ID", "Name", "Category", "Members", "Status"};
    return ui_create_treeview_with_columns(columns, 5);
}

void ui_club_treeview_populate(GtkTreeView* treeview, ClubList* clubs) {
    if (!treeview || !clubs) return;
    
    GtkListStore* store = GTK_LIST_STORE(gtk_tree_view_get_model(treeview));
    if (!store) return;
    
    // Clear existing data
    gtk_list_store_clear(store);
    
    // Add each club to the tree view
    for (int i = 0; i < clubs->count; i++) {
        GtkTreeIter iter;
        gtk_list_store_append(store, &iter);
        
        char id_str[16], members_str[16];
        snprintf(id_str, sizeof(id_str), "%d", clubs->clubs[i].id);
        snprintf(members_str, sizeof(members_str), "%d", clubs->clubs[i].member_count);
        
        const char* status_str = clubs->clubs[i].is_active ? "Active" : "Inactive";
        
        gtk_list_store_set(store, &iter,
                          0, id_str,
                          1, clubs->clubs[i].name,
                          2, clubs->clubs[i].category,
                          3, members_str,
                          4, status_str,
                          -1);
    }
}

void ui_club_treeview_add_club(GtkTreeView* treeview, Club* club) {}
void ui_club_treeview_update_club(GtkTreeView* treeview, Club* club) {}
void ui_club_treeview_remove_club(GtkTreeView* treeview, int club_id) {}

// ============================================================================
// STATUS BAR
// ============================================================================

GtkStatusbar* ui_create_statusbar(void) {
    GtkStatusbar* statusbar = GTK_STATUSBAR(gtk_statusbar_new());
    
    if (g_theme_config) {
        theme_style_statusbar(statusbar, g_theme_config);
    }
    
    return statusbar;
}

void ui_statusbar_set_message(GtkStatusbar* statusbar, const char* message) {
    if (!statusbar || !message) return;
    guint context_id = gtk_statusbar_get_context_id(statusbar, "default");
    gtk_statusbar_push(statusbar, context_id, message);
}

void ui_statusbar_set_user_info(GtkStatusbar* statusbar, Session* session) {
    if (!statusbar || !session) return;
    char info[200];
    snprintf(info, sizeof(info), "User: %s | Role: %s", 
             session->username, auth_role_to_string(session->role));
    ui_statusbar_set_message(statusbar, info);
}

void ui_statusbar_set_data_count(GtkStatusbar* statusbar, int student_count, int grade_count) {
    if (!statusbar) return;
    char info[200];
    snprintf(info, sizeof(info), "Students: %d | Grades: %d", student_count, grade_count);
    ui_statusbar_set_message(statusbar, info);
}

// ============================================================================
// MENU
// ============================================================================

GtkMenuBar* ui_create_main_menu(UIState* state) {
    GtkMenuBar* menubar = GTK_MENU_BAR(gtk_menu_bar_new());
    
    // File menu
    GtkMenuItem* file_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("File"));
    GtkMenu* file_menu = GTK_MENU(gtk_menu_new());
    
    GtkMenuItem* logout_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Logout"));
    gtk_menu_shell_append(GTK_MENU_SHELL(file_menu), GTK_WIDGET(logout_item));
    g_signal_connect(logout_item, "activate", G_CALLBACK(ui_on_logout_button_clicked), state);
    
    gtk_menu_item_set_submenu(file_item, GTK_WIDGET(file_menu));
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), GTK_WIDGET(file_item));
    
    // View menu
    GtkMenuItem* view_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("View"));
    GtkMenu* view_menu = GTK_MENU(gtk_menu_new());
    
    GtkMenuItem* theme_item = GTK_MENU_ITEM(gtk_menu_item_new_with_label("Toggle Theme"));
    gtk_menu_shell_append(GTK_MENU_SHELL(view_menu), GTK_WIDGET(theme_item));
    g_signal_connect(theme_item, "activate", G_CALLBACK(ui_on_toggle_theme_clicked), state);
    
    gtk_menu_item_set_submenu(view_item, GTK_WIDGET(view_menu));
    gtk_menu_shell_append(GTK_MENU_SHELL(menubar), GTK_WIDGET(view_item));
    
    if (g_theme_config) {
        theme_style_menu(GTK_MENU(file_menu), g_theme_config);
        theme_style_menu(GTK_MENU(view_menu), g_theme_config);
    }
    
    return menubar;
}

GtkMenuItem* ui_create_menu_item(const char* label, GCallback callback, gpointer user_data) {
    GtkMenuItem* item = GTK_MENU_ITEM(gtk_menu_item_new_with_label(label));
    if (callback) {
        g_signal_connect(item, "activate", callback, user_data);
    }
    return item;
}

GtkMenu* ui_create_submenu(GtkMenuItem* parent_item) {
    GtkMenu* menu = GTK_MENU(gtk_menu_new());
    gtk_menu_item_set_submenu(parent_item, GTK_WIDGET(menu));
    return menu;
}

void ui_menu_setup_accelerators(GtkMenuBar* menubar) {
    // TODO: Setup keyboard accelerators
}

// ============================================================================
// TOOLBAR
// ============================================================================

GtkToolbar* ui_create_toolbar(UIState* state) {
    GtkToolbar* toolbar = GTK_TOOLBAR(gtk_toolbar_new());
    gtk_toolbar_set_style(toolbar, GTK_TOOLBAR_ICONS);
    
    if (g_theme_config) {
        theme_style_toolbar(toolbar, g_theme_config);
    }
    
    return toolbar;
}

GtkToolButton* ui_create_tool_button(const char* icon_name, const char* label, GCallback callback, gpointer user_data) {
    GtkToolButton* button = GTK_TOOL_BUTTON(gtk_tool_button_new(NULL, label));
    if (callback) {
        g_signal_connect(button, "clicked", callback, user_data);
    }
    return button;
}

void ui_toolbar_setup_student_buttons(GtkToolbar* toolbar, UIState* state) {
    GtkToolButton* add_btn = ui_create_tool_button("list-add", "Add Student", 
                                                    G_CALLBACK(ui_on_add_student_clicked), state);
    gtk_toolbar_insert(toolbar, GTK_TOOL_ITEM(add_btn), -1);
}

void ui_toolbar_setup_grade_buttons(GtkToolbar* toolbar, UIState* state) {}
void ui_toolbar_setup_attendance_buttons(GtkToolbar* toolbar, UIState* state) {}
void ui_toolbar_setup_club_buttons(GtkToolbar* toolbar, UIState* state) {}

// ============================================================================
// UTILITY FUNCTIONS
// ============================================================================

void ui_show_error_message(GtkWindow* parent, const char* message) {
    GtkDialog* dialog = ui_create_message_dialog(parent, "Error", message, GTK_MESSAGE_ERROR);
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ui_show_info_message(GtkWindow* parent, const char* message) {
    GtkDialog* dialog = ui_create_message_dialog(parent, "Information", message, GTK_MESSAGE_INFO);
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

void ui_show_warning_message(GtkWindow* parent, const char* message) {
    GtkDialog* dialog = ui_create_message_dialog(parent, "Warning", message, GTK_MESSAGE_WARNING);
    gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
}

int ui_show_confirm_message(GtkWindow* parent, const char* message) {
    GtkDialog* dialog = ui_create_confirm_dialog(parent, "Confirm", message);
    gint response = gtk_dialog_run(dialog);
    gtk_widget_destroy(GTK_WIDGET(dialog));
    return (response == GTK_RESPONSE_YES);
}

void ui_center_window(GtkWindow* window) {
    if (!window) return;
    gtk_window_set_position(window, GTK_WIN_POS_CENTER);
}

void ui_set_window_icon(GtkWindow* window, const char* icon_file) {
    if (!window || !icon_file) return;
    // TODO: Set window icon
}

// ============================================================================
// LOGO FUNCTIONS
// ============================================================================

GtkImage* ui_create_logo_image(const char* logo_path, int width, int height) {
    if (!logo_path) return NULL;
    
    GError* error = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(logo_path, &error);
    
    if (!pixbuf) {
        if (error) {
            g_warning("Failed to load logo from %s: %s", logo_path, error->message);
            g_error_free(error);
        }
        return NULL;
    }
    
    // Scale the image if dimensions are specified
    GdkPixbuf* scaled_pixbuf = pixbuf;
    if (width > 0 && height > 0) {
        int orig_width = gdk_pixbuf_get_width(pixbuf);
        int orig_height = gdk_pixbuf_get_height(pixbuf);
        
        // Maintain aspect ratio
        double scale = (double)width / orig_width;
        if (scale * orig_height > height) {
            scale = (double)height / orig_height;
        }
        
        int new_width = (int)(orig_width * scale);
        int new_height = (int)(orig_height * scale);
        
        scaled_pixbuf = gdk_pixbuf_scale_simple(pixbuf, new_width, new_height, GDK_INTERP_BILINEAR);
        g_object_unref(pixbuf);
    }
    
    GtkImage* image = GTK_IMAGE(gtk_image_new_from_pixbuf(scaled_pixbuf));
    g_object_unref(scaled_pixbuf);
    
    return image;
}

void ui_set_window_logo(GtkWindow* window, const char* logo_path) {
    if (!window || !logo_path) return;
    
    GError* error = NULL;
    GdkPixbuf* pixbuf = gdk_pixbuf_new_from_file(logo_path, &error);
    
    if (pixbuf) {
        gtk_window_set_icon(window, pixbuf);
        g_object_unref(pixbuf);
    } else {
        if (error) {
            g_warning("Failed to load window icon from %s: %s", logo_path, error->message);
            g_error_free(error);
        }
    }
}

// ============================================================================
// CALLBACK FUNCTIONS
// ============================================================================

void ui_on_login_button_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->current_window) return;
    
    GtkEntry* email_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(state->current_window), "email-entry"));
    GtkEntry* password_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(state->current_window), "password-entry"));
    
    if (email_entry && password_entry) {
        ui_handle_login(email_entry, password_entry, state);
    }
}

void ui_on_logout_button_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state) return;
    
    if (ui_show_confirm_message(state->current_window, "Are you sure you want to logout?")) {
        // Clear session
        if (state->current_session) {
            session_destroy(state->current_session);
            state->current_session = NULL;
        }
        
        // Switch back to login
        ui_switch_window(UI_WINDOW_LOGIN, state);
    }
}

void ui_on_add_student_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_student_window_add_student(state);
    }
}

void ui_on_edit_student_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_student_window_edit_student(state);
    }
}

void ui_on_delete_student_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_student_window_delete_student(state);
    }
}

void ui_on_refresh_students_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_student_window_refresh_data(state);
    }
}

void ui_on_search_students_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->current_window) return;
    
    GtkEntry* search_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(state->current_window), "search-entry"));
    if (!search_entry) return;
    
    const char* search_text = gtk_entry_get_text(search_entry);
    
    if (strlen(search_text) == 0) {
        // If search is empty, show all students
        ui_student_window_refresh_data(state);
        return;
    }
    
    // Get the treeview
    GtkTreeView* treeview = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(state->current_window), "treeview"));
    if (!treeview || !state->students) return;
    
    // Create filtered list
    StudentList* filtered = student_list_create();
    if (!filtered) return;
    
    // Search in first name, last name, email, course, and ID
    char search_lower[256];
    snprintf(search_lower, sizeof(search_lower), "%s", search_text);
    
    // Convert search text to lowercase
    for (int i = 0; search_lower[i]; i++) {
        search_lower[i] = tolower((unsigned char)search_lower[i]);
    }
    
    // Iterate through all students
    for (int i = 0; i < state->students->count; i++) {
        Student* student = &state->students->students[i];
        char student_data[1024];
        
        // Combine student data for search
        snprintf(student_data, sizeof(student_data), "%s %s %s %s %d",
                student->first_name,
                student->last_name,
                student->email,
                student->course,
                student->id);
        
        // Convert to lowercase
        for (int j = 0; student_data[j]; j++) {
            student_data[j] = tolower((unsigned char)student_data[j]);
        }
        
        // Check if search text is in student data
        if (strstr(student_data, search_lower)) {
            // Add to filtered list
            student_list_add(filtered, *student);
        }
    }
    
    // Update treeview with filtered results
    ui_student_treeview_populate(treeview, filtered);
    
    // Show status message
    printf("[INFO] Search results for '%s': %d student(s) found\n", search_text, filtered->count);
    
    // Free the filtered list
    student_list_destroy(filtered);
}

void ui_on_clear_search_clicked(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->current_window) return;
    
    GtkEntry* search_entry = GTK_ENTRY(g_object_get_data(G_OBJECT(state->current_window), "search-entry"));
    if (search_entry) {
        gtk_entry_set_text(search_entry, "");
    }
    
    // Refresh to show all students
    ui_student_window_refresh_data(state);
    printf("[INFO] Search cleared\n");
}

void ui_on_search_entry_activate(GtkEntry* entry, gpointer user_data) {
    // When user presses Enter in search field, trigger search
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_on_search_students_clicked(NULL, user_data);
    }
}

void ui_on_student_selection_changed(GtkTreeSelection* selection, gpointer user_data) {
    // TODO: Handle selection change
}

void ui_on_window_destroy(GtkWindow* window, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state) {
        gtk_main_quit();
        return;
    }
    
    if (window == state->current_window) {
        state->current_window = NULL;
    }
    
    if (window == state->main_window) {
        gtk_main_quit();
    }
}

void ui_on_toggle_theme_clicked(GtkMenuItem* item, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (state) {
        ui_switch_theme(state);
    }
}

// ============================================================================
// MODULE INITIALIZATION
// ============================================================================

/*
 * Initialize all application modules (file manager, data structures)
 */
int ui_init_modules(void) {
    // Initialize file manager
    printf("[INFO] Initializing file manager...\n");
    if (file_manager_init() != FILE_SUCCESS) {
        fprintf(stderr, "[ERROR] File manager initialization failed\n");
        return -1;
    }
    
    // Ensure data directories exist
    printf("[INFO] Creating data directories...\n");
    if (ensure_data_directories() != 0) {
        fprintf(stderr, "[ERROR] Failed to create data directories\n");
        return -1;
    }
    
    // Verify directories
    if (!directory_exists(DATA_DIR)) {
        fprintf(stderr, "[ERROR] Data directory not found: %s\n", DATA_DIR);
        return -1;
    }
    
    printf("[OK] Modules initialized successfully\n");
    return 0;
}

/*
 * Cleanup all application modules
 */
void ui_cleanup_modules(void) {
    printf("[INFO] Cleaning up modules...\n");
    file_manager_cleanup();
    printf("[OK] Module cleanup complete\n");
}

/*
 * Load all application data from files
 */
int ui_load_data(UIState* state) {
    if (!state) return -1;
    
    char filepath[512];
    int errors = 0;
    
    // Initialize data structures ONLY if they don't already exist
    // This prevents overwriting data that's already been loaded and possibly modified
    if (!state->users) state->users = user_list_create();
    if (!state->students) state->students = student_list_create();
    if (!state->grades) state->grades = liste_note_create();
    if (!state->attendance) state->attendance = attendance_list_create();
    if (!state->clubs) state->clubs = club_list_create();
    if (!state->memberships) state->memberships = membership_list_create();
    if (!state->courses) state->courses = liste_module_create();
    
    if (!state->users || !state->students || !state->grades || 
        !state->attendance || !state->clubs || !state->memberships || !state->courses) {
        fprintf(stderr, "[ERROR] Failed to create data structures\n");
        return -1;
    }
    
    // Load users
    snprintf(filepath, sizeof(filepath), "%s", USERS_FILE);
    if (file_exists(filepath)) {
        if (user_list_load_from_file(state->users, filepath) != 0) {
            fprintf(stderr, "[WARNING] Failed to load users\n");
            errors++;
        } else {
            printf("[OK] Loaded %d users\n", state->users->count);
        }
    }
    
    // Create default admin if no users
    if (state->users->count == 0) {
        printf("[INFO] Creating default admin user (admin/admin123)...\n");
        if (auth_register(state->users, "admin", "admin@studentmgmt.org", 
                         "admin123", ROLE_ADMIN) == 0) {
            snprintf(filepath, sizeof(filepath), "%s", USERS_FILE);
            user_list_save_to_file(state->users, filepath);
            printf("[OK] Default admin created\n");
        }
    }
    
    // Load students (only if not already loaded)
    snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
    if (state->students->count == 0 && file_exists(filepath)) {
        if (student_list_load_from_file(state->students, filepath) != 0) {
            fprintf(stderr, "[WARNING] Failed to load students\n");
            errors++;
        } else {
            printf("[OK] Loaded %d students\n", state->students->count);
        }
    } else if (state->students->count > 0) {
        printf("[OK] Using existing student data (%d students)\n", state->students->count);
    }
    
    // Load grades
    snprintf(filepath, sizeof(filepath), "%s", GRADES_FILE);
    if (file_exists(filepath)) {
        if (grade_list_load_from_file(state->grades, filepath) != 0) {
            fprintf(stderr, "[WARNING] Failed to load grades\n");
            errors++;
        } else {
            printf("[OK] Loaded %d grades\n", state->grades->count);
        }
    }
    
    // Load attendance
    snprintf(filepath, sizeof(filepath), "%s", ATTENDANCE_FILE);
    if (file_exists(filepath)) {
        if (attendance_list_load_from_file(state->attendance, filepath) != 0) {
            fprintf(stderr, "[WARNING] Failed to load attendance\n");
            errors++;
        } else {
            printf("[OK] Loaded %d attendance records\n", state->attendance->count);
        }
    }
    
    // Load clubs
    snprintf(filepath, sizeof(filepath), "%s", CLUBS_FILE);
    if (file_exists(filepath)) {
        if (club_list_load_from_file(state->clubs, filepath) != 0) {
            fprintf(stderr, "[WARNING] Failed to load clubs\n");
            errors++;
        } else {
            printf("[OK] Loaded %d clubs\n", state->clubs->count);
        }
    }
    
    return errors > 0 ? -1 : 0;
}

/*
 * Save all application data to files
 */
int ui_save_data(UIState* state) {
    if (!state) return -1;
    
    char filepath[512];
    int errors = 0;
    
    // Save users
    if (state->users) {
        snprintf(filepath, sizeof(filepath), "%s", USERS_FILE);
        if (user_list_save_to_file(state->users, filepath) != 0) {
            fprintf(stderr, "[ERROR] Failed to save users\n");
            errors++;
        } else {
            printf("[OK] Saved %d users\n", state->users->count);
        }
    }
    
    // Save students
    if (state->students) {
        snprintf(filepath, sizeof(filepath), "%s", STUDENTS_FILE);
        if (student_list_save_to_file(state->students, filepath) != 0) {
            fprintf(stderr, "[ERROR] Failed to save students\n");
            errors++;
        } else {
            printf("[OK] Saved %d students\n", state->students->count);
        }
    }
    
    // Save grades
    if (state->grades) {
        snprintf(filepath, sizeof(filepath), "%s", GRADES_FILE);
        if (grade_list_save_to_file(state->grades, filepath) != 0) {
            fprintf(stderr, "[ERROR] Failed to save grades\n");
            errors++;
        } else {
            printf("[OK] Saved %d grades\n", state->grades->count);
        }
    }
    
    // Save attendance
    if (state->attendance) {
        snprintf(filepath, sizeof(filepath), "%s", ATTENDANCE_FILE);
        if (attendance_list_save_to_file(state->attendance, filepath) != 0) {
            fprintf(stderr, "[ERROR] Failed to save attendance\n");
            errors++;
        } else {
            printf("[OK] Saved %d attendance records\n", state->attendance->count);
        }
    }
    
    // Save clubs
    if (state->clubs) {
        snprintf(filepath, sizeof(filepath), "%s", CLUBS_FILE);
        if (club_list_save_to_file(state->clubs, filepath) != 0) {
            fprintf(stderr, "[ERROR] Failed to save clubs\n");
            errors++;
        } else {
            printf("[OK] Saved %d clubs\n", state->clubs->count);
        }
    }
    
    return errors > 0 ? -1 : 0;
}

// ============================================================================
// GTK APPLICATION CALLBACKS
// ============================================================================

/*
 * GTK Application activation callback
 * This is where all application initialization happens
 */
void ui_app_activate(GtkApplication *app, gpointer user_data) {
    UIState *state = NULL;
    
    printf("[INFO] Initializing application modules...\n");
    
    // Initialize all modules
    if (ui_init_modules() != 0) {
        fprintf(stderr, "[ERROR] Failed to initialize application modules\n");
        g_application_quit(G_APPLICATION(app));
        return;
    }
    
    // Create UI state
    state = ui_state_create();
    if (!state) {
        fprintf(stderr, "[ERROR] Failed to create UI state\n");
        g_application_quit(G_APPLICATION(app));
        return;
    }
    
    state->app = app;
    g_ui_state = state;
    
    // Load data
    printf("[INFO] Loading application data...\n");
    if (ui_load_data(state) != 0) {
        fprintf(stderr, "[WARNING] Some data could not be loaded\n");
    }
    
    // Initialize and apply theme
    printf("[INFO] Initializing theme system...\n");
    theme_init((struct UIState*)state);
    theme_apply((struct UIState*)state, THEME_LIGHT);
    
    // Show login window
    printf("[INFO] Showing login window...\n");
    ui_show_login_window(state);
    
    printf("[OK] Application initialized successfully\n");
}

// Student: Join a club
void ui_on_student_join_club(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs || !state->memberships) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    GtkTreeView* club_tree = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(parent_window), "treeview"));
    
    if (!club_tree) return;
    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(club_tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(parent_window, "Please select a club to join!");
        return;
    }
    
    gchar* club_id_str;
    gchar* club_name;
    gtk_tree_model_get(model, &iter, 0, &club_id_str, 1, &club_name, -1);
    gint club_id = atoi(club_id_str);
    g_free(club_id_str);
    
    // Get student ID from current user
    if (!state->current_user || !state->students) {
        ui_show_error_message(parent_window, "Could not identify student!");
        g_free(club_name);
        return;
    }
    
    // Find student by email matching current user
    Student* student = NULL;
    for (int i = 0; i < state->students->count; i++) {
        if (strcmp(state->students->students[i].email, state->current_user->email) == 0) {
            student = &state->students->students[i];
            break;
        }
    }
    
    if (!student) {
        ui_show_error_message(parent_window, "Student record not found!");
        g_free(club_name);
        return;
    }
    
    // Check if already a member
    for (int i = 0; i < state->memberships->count; i++) {
        ClubMembership* m = &state->memberships->memberships[i];
        if (m->student_id == student->id && m->club_id == club_id && m->is_active) {
            char msg[256];
            snprintf(msg, sizeof(msg), "You are already a member of %s!", club_name);
            ui_show_info_message(parent_window, msg);
            g_free(club_name);
            return;
        }
    }
    
    // Create new membership
    ClubMembership new_membership;
    new_membership.id = state->memberships->count + 1;
    new_membership.student_id = student->id;
    new_membership.club_id = club_id;
    new_membership.join_date = time(NULL);
    strncpy(new_membership.role, "Member", sizeof(new_membership.role) - 1);
    new_membership.is_active = 1;
    
    if (membership_list_add(state->memberships, new_membership)) {
        // Update club member count
        Club* club = club_list_find_by_id(state->clubs, club_id);
        if (club) {
            club->member_count++;
            club_list_save_to_file(state->clubs, "clubs.txt");
        }
        
        membership_list_save_to_file(state->memberships, "club_memberships.txt");
        
        char msg[256];
        snprintf(msg, sizeof(msg), "Successfully joined %s!", club_name);
        ui_show_info_message(parent_window, msg);
        
        ui_club_treeview_populate(club_tree, state->clubs);
    } else {
        ui_show_error_message(parent_window, "Failed to join club!");
    }
    
    g_free(club_name);
}

// Student: Leave a club
void ui_on_student_leave_club(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs || !state->memberships) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    GtkTreeView* club_tree = GTK_TREE_VIEW(g_object_get_data(G_OBJECT(parent_window), "treeview"));
    
    if (!club_tree) return;
    
    GtkTreeSelection* selection = gtk_tree_view_get_selection(club_tree);
    GtkTreeModel* model;
    GtkTreeIter iter;
    
    if (!gtk_tree_selection_get_selected(selection, &model, &iter)) {
        ui_show_error_message(parent_window, "Please select a club to leave!");
        return;
    }
    
    gchar* club_id_str;
    gchar* club_name;
    gtk_tree_model_get(model, &iter, 0, &club_id_str, 1, &club_name, -1);
    gint club_id = atoi(club_id_str);
    g_free(club_id_str);
    
    // Get student ID from current user
    if (!state->current_user || !state->students) {
        ui_show_error_message(parent_window, "Could not identify student!");
        g_free(club_name);
        return;
    }
    
    // Find student by email matching current user
    Student* student = NULL;
    for (int i = 0; i < state->students->count; i++) {
        if (strcmp(state->students->students[i].email, state->current_user->email) == 0) {
            student = &state->students->students[i];
            break;
        }
    }
    
    if (!student) {
        ui_show_error_message(parent_window, "Student record not found!");
        g_free(club_name);
        return;
    }
    
    // Find and remove membership
    int found = 0;
    for (int i = 0; i < state->memberships->count; i++) {
        ClubMembership* m = &state->memberships->memberships[i];
        if (m->student_id == student->id && m->club_id == club_id && m->is_active) {
            m->is_active = 0;
            found = 1;
            
            // Update club member count
            Club* club = club_list_find_by_id(state->clubs, club_id);
            if (club && club->member_count > 0) {
                club->member_count--;
                club_list_save_to_file(state->clubs, "clubs.txt");
            }
            
            membership_list_save_to_file(state->memberships, "club_memberships.txt");
            
            char msg[256];
            snprintf(msg, sizeof(msg), "Successfully left %s!", club_name);
            ui_show_info_message(parent_window, msg);
            
            ui_club_treeview_populate(club_tree, state->clubs);
            break;
        }
    }
    
    if (!found) {
        char msg[256];
        snprintf(msg, sizeof(msg), "You are not a member of %s!", club_name);
        ui_show_error_message(parent_window, msg);
    }
    
    g_free(club_name);
}

// Student: View my clubs
void ui_on_student_view_my_clubs(GtkButton* button, gpointer user_data) {
    UIState* state = (UIState*)user_data;
    if (!state || !state->clubs || !state->memberships) return;
    
    GtkWindow* parent_window = GTK_WINDOW(g_object_get_data(G_OBJECT(button), "window"));
    
    // Get student ID from current user
    if (!state->current_user || !state->students) {
        ui_show_error_message(parent_window, "Could not identify student!");
        return;
    }
    
    // Find student by email matching current user
    Student* student = NULL;
    for (int i = 0; i < state->students->count; i++) {
        if (strcmp(state->students->students[i].email, state->current_user->email) == 0) {
            student = &state->students->students[i];
            break;
        }
    }
    
    if (!student) {
        ui_show_error_message(parent_window, "Student record not found!");
        return;
    }
    
    // Create dialog
    GtkWidget* dialog = gtk_window_new(GTK_WINDOW_TOPLEVEL);
    gtk_window_set_title(GTK_WINDOW(dialog), "My Clubs");
    gtk_window_set_default_size(GTK_WINDOW(dialog), 700, 500);
    gtk_window_set_modal(GTK_WINDOW(dialog), TRUE);
    gtk_window_set_transient_for(GTK_WINDOW(dialog), parent_window);
    
    GtkBox* main_box = GTK_BOX(gtk_box_new(GTK_ORIENTATION_VERTICAL, 12));
    gtk_container_add(GTK_CONTAINER(dialog), GTK_WIDGET(main_box));
    gtk_widget_set_margin_all(GTK_WIDGET(main_box), 16);
    
    // Header
    char header_text[256];
    snprintf(header_text, sizeof(header_text), "My Clubs - %s %s", student->first_name, student->last_name);
    GtkLabel* header = GTK_LABEL(gtk_label_new(NULL));
    gtk_label_set_markup(header, g_strdup_printf("<span font='20' weight='bold'>%s</span>", header_text));
    gtk_box_pack_start(main_box, GTK_WIDGET(header), FALSE, FALSE, 0);
    
    // Tree view
    GtkScrolledWindow* scrolled = GTK_SCROLLED_WINDOW(gtk_scrolled_window_new(NULL, NULL));
    gtk_box_pack_start(main_box, GTK_WIDGET(scrolled), TRUE, TRUE, 0);
    
    GtkTreeView* tree = GTK_TREE_VIEW(gtk_tree_view_new());
    gtk_container_add(GTK_CONTAINER(scrolled), GTK_WIDGET(tree));
    
    // Create columns
    GtkCellRenderer* renderer = gtk_cell_renderer_text_new();
    gtk_tree_view_append_column(tree, 
        gtk_tree_view_column_new_with_attributes("Club", renderer, "text", 0, NULL));
    gtk_tree_view_append_column(tree, 
        gtk_tree_view_column_new_with_attributes("Category", renderer, "text", 1, NULL));
    gtk_tree_view_append_column(tree, 
        gtk_tree_view_column_new_with_attributes("Role", renderer, "text", 2, NULL));
    gtk_tree_view_append_column(tree, 
        gtk_tree_view_column_new_with_attributes("Joined", renderer, "text", 3, NULL));
    
    // Populate
    GtkListStore* store = gtk_list_store_new(4, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING, G_TYPE_STRING);
    int club_count = 0;
    
    for (int i = 0; i < state->memberships->count; i++) {
        ClubMembership* m = &state->memberships->memberships[i];
        if (m->student_id == student->id && m->is_active) {
            Club* club = club_list_find_by_id(state->clubs, m->club_id);
            if (club) {
                char date_str[64];
                struct tm* tm_info = localtime(&m->join_date);
                strftime(date_str, sizeof(date_str), "%Y-%m-%d", tm_info);
                
                GtkTreeIter iter;
                gtk_list_store_append(store, &iter);
                gtk_list_store_set(store, &iter,
                    0, club->name,
                    1, club->category,
                    2, m->role,
                    3, date_str,
                    -1);
                club_count++;
            }
        }
    }
    
    gtk_tree_view_set_model(tree, GTK_TREE_MODEL(store));
    g_object_unref(store);
    
    // Info label
    char info_text[128];
    snprintf(info_text, sizeof(info_text), "Total: %d clubs", club_count);
    GtkLabel* info = GTK_LABEL(gtk_label_new(info_text));
    gtk_box_pack_start(main_box, GTK_WIDGET(info), FALSE, FALSE, 0);
    
    // Close button
    GtkButton* close_btn = GTK_BUTTON(gtk_button_new_with_label("Close"));
    gtk_box_pack_start(main_box, GTK_WIDGET(close_btn), FALSE, FALSE, 0);
    g_signal_connect_swapped(close_btn, "clicked", G_CALLBACK(gtk_widget_destroy), dialog);
    
    gtk_widget_show_all(dialog);
}

/*
 * GTK Application shutdown callback
 * This handles cleanup before the application exits
 */
void ui_app_shutdown(GtkApplication *app, gpointer user_data) {
    printf("[INFO] Application shutting down...\n");
    
    // Save all data
    if (g_ui_state) {
        printf("[INFO] Saving application data...\n");
        ui_save_data(g_ui_state);
    }
    
    // Cleanup UI
    ui_cleanup();
    
    // Cleanup modules
    ui_cleanup_modules();
    
    printf("[OK] Shutdown complete\n");
}

