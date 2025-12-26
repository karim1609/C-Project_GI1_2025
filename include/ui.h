#ifndef UI_H
#define UI_H

#include <gtk/gtk.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "config.h"
#include "auth.h"
#include "student.h"
#include "grade.h"
#include "attendance.h"
#include "club.h"
#include "stats.h"

// Type aliases are defined in grade.h (already included above)
// GradeList = liste_note, CourseList = ListeModules, Grade = Note

// UI window types
typedef enum {
    UI_WINDOW_LOGIN = 0,
    UI_WINDOW_MAIN = 1,
    UI_WINDOW_STUDENT_MANAGEMENT = 2,
    UI_WINDOW_GRADE_MANAGEMENT = 3,
    UI_WINDOW_ATTENDANCE_MANAGEMENT = 4,
    UI_WINDOW_CLUB_MANAGEMENT = 5,
    UI_WINDOW_STATISTICS = 6,
    UI_WINDOW_SETTINGS = 7,
    UI_WINDOW_ABOUT = 8,
    UI_WINDOW_ADMIN_VIEW = 9
} UIWindowType;

// UI state structure
typedef struct {
    GtkApplication* app;
    GtkWindow* main_window;
    GtkWindow* current_window;
    Session* current_session;
    User* current_user;  // Currently logged in user for role checking
    UserList* users;
    StudentList* students;
    GradeList* grades;
    AttendanceList* attendance;
    ClubList* clubs;
    MembershipList* memberships;
    CourseList* courses;
    UIWindowType current_window_type;
    int is_dark_theme;
    char current_language[10];
} UIState;

// UI management functions
UIState* ui_state_create(void);
void ui_state_destroy(UIState* state);
int ui_init(int argc, char* argv[]);
void ui_cleanup(void);
int ui_run(void);

// GTK Application callbacks
void ui_app_activate(GtkApplication *app, gpointer user_data);
void ui_app_shutdown(GtkApplication *app, gpointer user_data);

// Module initialization functions
int ui_init_modules(void);
void ui_cleanup_modules(void);
int ui_load_data(UIState* state);
int ui_save_data(UIState* state);

// Window management
void ui_show_login_window(UIState* state);
GtkWindow* ui_create_login_window(UIState* state);

// Window management
GtkWindow* ui_create_window(UIWindowType type, UIState* state);
void ui_show_window(GtkWindow* window);
void ui_hide_window(GtkWindow* window);
void ui_close_window(GtkWindow* window);
void ui_switch_window(UIWindowType new_type, UIState* state);

// Login window
GtkWindow* ui_create_login_window(UIState* state);
void ui_login_window_setup_callbacks(GtkWindow* window, UIState* state);
int ui_handle_login(GtkEntry* username_entry, GtkEntry* password_entry, UIState* state);
void ui_login_success(UIState* state);
void ui_login_failed(const char* error_message);

// Main window
GtkWindow* ui_create_main_window(UIState* state);
void ui_main_window_setup_menu(GtkWindow* window, UIState* state);
void ui_main_window_setup_toolbar(GtkWindow* window, UIState* state);
void ui_main_window_setup_statusbar(GtkWindow* window, UIState* state);
void ui_main_window_update_user_info(UIState* state);

// Student management window
GtkWindow* ui_create_student_window(UIState* state);
void ui_student_window_setup_treeview(GtkWindow* window, UIState* state);
void ui_student_window_setup_buttons(GtkWindow* window, UIState* state);
void ui_student_window_refresh_data(UIState* state);
void ui_student_window_add_student(UIState* state);
void ui_student_window_edit_student(UIState* state);
void ui_student_window_delete_student(UIState* state);
void ui_student_window_search_student(UIState* state);

// Grade management window
GtkWindow* ui_create_grade_window(UIState* state);
void ui_grade_window_setup_treeview(GtkWindow* window, UIState* state);
void ui_grade_window_setup_buttons(GtkWindow* window, UIState* state);
void ui_grade_window_refresh_data(UIState* state);
void ui_grade_window_add_grade(UIState* state);
void ui_grade_window_edit_grade(UIState* state);
void ui_grade_window_delete_grade(UIState* state);
void ui_grade_window_calculate_gpa(UIState* state);

// Attendance management window
GtkWindow* ui_create_attendance_window(UIState* state);
void ui_attendance_window_setup_treeview(GtkWindow* window, UIState* state);
void ui_attendance_window_setup_buttons(GtkWindow* window, UIState* state);
void ui_attendance_window_refresh_data(UIState* state);
void ui_attendance_window_mark_attendance(UIState* state);
void ui_attendance_window_view_attendance(UIState* state);
void ui_attendance_window_generate_report(UIState* state);

// Club management window
GtkWindow* ui_create_club_window(UIState* state);
void ui_club_window_setup_treeview(GtkWindow* window, UIState* state);
void ui_club_window_setup_buttons(GtkWindow* window, UIState* state);
void ui_club_window_refresh_data(UIState* state);
void ui_club_window_add_club(UIState* state);
void ui_club_window_edit_club(UIState* state);
void ui_club_window_delete_club(UIState* state);
void ui_club_window_manage_membership(UIState* state);

// Statistics window
GtkWindow* ui_create_statistics_window(UIState* state);
void ui_statistics_window_setup_charts(GtkWindow* window, UIState* state);
void ui_statistics_window_setup_buttons(GtkWindow* window, UIState* state);
void ui_statistics_window_refresh_data(UIState* state);
void ui_statistics_window_generate_report(UIState* state);
void ui_statistics_window_export_data(UIState* state);

// Settings window
GtkWindow* ui_create_settings_window(UIState* state);
void ui_settings_window_setup_theme(GtkWindow* window, UIState* state);
void ui_settings_window_setup_language(GtkWindow* window, UIState* state);

void ui_settings_window_save_settings(UIState* state);
void ui_settings_window_load_settings(UIState* state);

// Admin view window (admin only)
GtkWindow* ui_create_admin_view_window(UIState* state);

// Dialog boxes
GtkDialog* ui_create_message_dialog(GtkWindow* parent, const char* title, const char* message, GtkMessageType type);
GtkDialog* ui_create_confirm_dialog(GtkWindow* parent, const char* title, const char* message);
GtkDialog* ui_create_input_dialog(GtkWindow* parent, const char* title, const char* message, GtkEntry** entry);
GtkDialog* ui_create_file_dialog(GtkWindow* parent, const char* title, GtkFileChooserAction action);

// Student input dialog
GtkDialog* ui_create_student_input_dialog(GtkWindow* parent, Student* student, int is_edit);
int ui_student_input_dialog_get_data(GtkDialog* dialog, Student* student);
void ui_student_input_dialog_set_data(GtkDialog* dialog, Student* student);

// Grade input dialog
GtkDialog* ui_create_grade_input_dialog(GtkWindow* parent, Grade* grade, int is_edit);
int ui_grade_input_dialog_get_data(GtkDialog* dialog, Grade* grade);
void ui_grade_input_dialog_set_data(GtkDialog* dialog, Grade* grade);

// Club input dialog
GtkDialog* ui_create_club_input_dialog(GtkWindow* parent, Club* club, int is_edit);
int ui_club_input_dialog_get_data(GtkDialog* dialog, Club* club);
void ui_club_input_dialog_set_data(GtkDialog* dialog, Club* club);

// Theme management
void ui_apply_theme(UIState* state, int is_dark);
void ui_switch_theme(UIState* state);
void ui_load_theme_css(const char* css_file);
void ui_set_dark_theme(GtkWindow* window);
void ui_set_light_theme(GtkWindow* window);

// Language management
void ui_set_language(UIState* state, const char* language);
void ui_load_language_file(const char* language_file);
const char* ui_get_translated_text(const char* key);

// TreeView helpers
GtkTreeView* ui_create_treeview_with_columns(const char* column_titles[], int column_count);
void ui_treeview_add_row(GtkTreeView* treeview, const char* data[], int data_count);
void ui_treeview_clear(GtkTreeView* treeview);
void ui_treeview_set_selection_callback(GtkTreeView* treeview, GCallback callback, gpointer user_data);

// Student TreeView
GtkTreeView* ui_create_student_treeview(void);
void ui_student_treeview_populate(GtkTreeView* treeview, StudentList* students);
void ui_student_treeview_add_student(GtkTreeView* treeview, Student* student);
void ui_student_treeview_update_student(GtkTreeView* treeview, Student* student);
void ui_student_treeview_remove_student(GtkTreeView* treeview, int student_id);

// Grade TreeView
GtkTreeView* ui_create_grade_treeview(void);
void ui_grade_treeview_populate(GtkTreeView* treeview, GradeList* grades);
void ui_grade_treeview_add_grade(GtkTreeView* treeview, Grade* grade);
void ui_grade_treeview_update_grade(GtkTreeView* treeview, Grade* grade);
void ui_grade_treeview_remove_grade(GtkTreeView* treeview, int grade_id);

// Attendance TreeView
GtkTreeView* ui_create_attendance_treeview(void);
void ui_attendance_treeview_populate(GtkTreeView* treeview, AttendanceList* attendance);
void ui_attendance_treeview_add_record(GtkTreeView* treeview, AttendanceRecord* record);
void ui_attendance_treeview_update_record(GtkTreeView* treeview, AttendanceRecord* record);
void ui_attendance_treeview_remove_record(GtkTreeView* treeview, int record_id);

// Club TreeView
GtkTreeView* ui_create_club_treeview(void);
void ui_club_treeview_populate(GtkTreeView* treeview, ClubList* clubs);
void ui_club_treeview_add_club(GtkTreeView* treeview, Club* club);
void ui_club_treeview_update_club(GtkTreeView* treeview, Club* club);
void ui_club_treeview_remove_club(GtkTreeView* treeview, int club_id);

// Status bar helpers
GtkStatusbar* ui_create_statusbar(void);
void ui_statusbar_set_message(GtkStatusbar* statusbar, const char* message);
void ui_statusbar_set_user_info(GtkStatusbar* statusbar, Session* session);
void ui_statusbar_set_data_count(GtkStatusbar* statusbar, int student_count, int grade_count);

// Menu helpers
GtkMenuBar* ui_create_main_menu(UIState* state);
GtkMenuItem* ui_create_menu_item(const char* label, GCallback callback, gpointer user_data);
GtkMenu* ui_create_submenu(GtkMenuItem* parent_item);
void ui_menu_setup_accelerators(GtkMenuBar* menubar);

// Toolbar helpers
GtkToolbar* ui_create_toolbar(UIState* state);
GtkToolButton* ui_create_tool_button(const char* icon_name, const char* label, GCallback callback, gpointer user_data);
void ui_toolbar_setup_student_buttons(GtkToolbar* toolbar, UIState* state);
void ui_toolbar_setup_grade_buttons(GtkToolbar* toolbar, UIState* state);
void ui_toolbar_setup_attendance_buttons(GtkToolbar* toolbar, UIState* state);
void ui_toolbar_setup_club_buttons(GtkToolbar* toolbar, UIState* state);

// Utility functions
void ui_show_error_message(GtkWindow* parent, const char* message);
void ui_show_info_message(GtkWindow* parent, const char* message);
void ui_show_warning_message(GtkWindow* parent, const char* message);
int ui_show_confirm_message(GtkWindow* parent, const char* message);
void ui_center_window(GtkWindow* window);
void ui_set_window_icon(GtkWindow* window, const char* icon_file);

// Callback functions
void ui_on_login_button_clicked(GtkButton* button, gpointer user_data);
void ui_on_logout_button_clicked(GtkButton* button, gpointer user_data);
void ui_on_add_student_clicked(GtkButton* button, gpointer user_data);
void ui_on_edit_student_clicked(GtkButton* button, gpointer user_data);
void ui_on_delete_student_clicked(GtkButton* button, gpointer user_data);
void ui_on_search_students_clicked(GtkButton* button, gpointer user_data);
void ui_on_clear_search_clicked(GtkButton* button, gpointer user_data);
void ui_on_search_entry_activate(GtkEntry* entry, gpointer user_data);
void ui_on_student_selection_changed(GtkTreeSelection* selection, gpointer user_data);
void ui_on_window_destroy(GtkWindow* window, gpointer user_data);
void ui_on_forgot_password_clicked(GtkWidget* widget, GdkEventButton* event, gpointer user_data);
void ui_on_nav_button_clicked(GtkButton* button, gpointer user_data);
void ui_on_theme_light_clicked(GtkButton* button, gpointer user_data);
void ui_on_theme_dark_clicked(GtkButton* button, gpointer user_data);
void ui_on_toggle_theme_clicked(GtkMenuItem* item, gpointer user_data);
void ui_on_refresh_students_clicked(GtkButton* button, gpointer user_data);

// Constants
#define UI_WINDOW_TITLE "Student Management System"
#define UI_WINDOW_DEFAULT_WIDTH WINDOW_WIDTH
#define UI_WINDOW_DEFAULT_HEIGHT WINDOW_HEIGHT
#define UI_ICON_SIZE 24
#define UI_TREEVIEW_MIN_WIDTH 600
#define UI_TREEVIEW_MIN_HEIGHT 400

// Theme files
#define UI_THEME_DARK_CSS "assets/css/dark.css"
#define UI_THEME_LIGHT_CSS "assets/css/light.css"

// Language files
#define UI_LANGUAGE_ENGLISH "assets/lang/en.json"
#define UI_LANGUAGE_FRENCH "assets/lang/fr.json"
#define UI_LANGUAGE_SPANISH "assets/lang/es.json"

// Icon files
#define UI_ICON_APP "assets/icons/app.png"
#define UI_ICON_STUDENT "assets/icons/student.png"
#define UI_ICON_GRADE "assets/icons/grade.png"
#define UI_ICON_ATTENDANCE "assets/icons/attendance.png"
#define UI_ICON_CLUB "assets/icons/club.png"
#define UI_ICON_STATS "assets/icons/stats.png"

// Logo file
#define UI_LOGO_FILE "src/t1.png"

// Logo functions
GtkImage* ui_create_logo_image(const char* logo_path, int width, int height);
void ui_set_window_logo(GtkWindow* window, const char* logo_path);

#endif // UI_H
