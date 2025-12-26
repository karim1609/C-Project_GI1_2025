#ifndef AUTH_H
#define AUTH_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// User structure
typedef struct {
    int id;
    char username[50];
    char email[MAX_EMAIL_LENGTH];
    char password_hash[129]; // 128 character hash + null terminator
    char salt[33];
    UserRole role;
    time_t created_at;
    time_t last_login;
    int is_active;
} User;

// Session structure
typedef struct {
    int user_id;
    char username[50];
    UserRole role;
    time_t login_time;
    int is_valid;
} Session;

// User list structure
typedef struct {
    User* users;
    int count;
    int capacity;
} UserList;

// Function declarations
UserList* user_list_create(void);
void user_list_destroy(UserList* list);
int user_list_add(UserList* list, User user);
int user_list_remove(UserList* list, int user_id);
User* user_list_find_by_username(UserList* list, const char* username);
User* user_list_find_by_email(UserList* list, const char* email);
User* user_list_find_by_id(UserList* list, int user_id);

// Authentication functions
int auth_register(UserList* list, const char* username, const char* email, const char* password, UserRole role);
int auth_register_student(UserList* list, const char* username, const char* email, const char* password);
int auth_login(UserList* list, const char* username, const char* password, Session* session);
int auth_request_password_reset(UserList* list, const char* email, char* reset_code);
int auth_reset_password_with_code(UserList* list, const char* email, const char* reset_code, const char* new_password);
int auth_verify_email_format(const char* email);
int auth_verify_username_format(const char* username);

// File operations
int user_list_load_from_file(UserList* list, const char* filename);
int user_list_save_to_file(UserList* list, const char* filename);
int auth_logout(Session* session);
int auth_validate_session(Session* session);
int auth_change_password(UserList* list, int user_id, const char* old_password, const char* new_password);
int auth_reset_password(UserList* list, const char* email);

// Password functions
void auth_hash_password(const char* password, const char* salt, char* hash);
int auth_verify_password(const char* password, const char* hash, const char* salt);
void auth_generate_salt(char* salt);
int auth_validate_password_strength(const char* password);

// Session management
Session* session_create(void);
void session_destroy(Session* session);
int session_is_admin(Session* session);
int session_is_teacher(Session* session);
int session_is_student(Session* session);

// File operations
int auth_save_users(UserList* list, const char* filename);
int auth_load_users(UserList* list, const char* filename);

// Utility functions
const char* auth_role_to_string(UserRole role);
UserRole auth_string_to_role(const char* role_str);
void auth_display_user(User* user);
void auth_display_all_users(UserList* list);

#endif // AUTH_H
