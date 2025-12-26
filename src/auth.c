#include "auth.h"
#include "utils.h"
#include "config.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// Static variable to track next user ID
static int next_user_id = 1000;

// Static function for resizing user list
static int user_list_resize(UserList* list, int new_capacity) {
    if (!list || new_capacity < list->count) {
        return 0;
    }

    User* new_users = (User*)realloc(list->users, sizeof(User) * new_capacity);
    if (!new_users) {
        return 0;
    }

    list->users = new_users;
    list->capacity = new_capacity;
    return 1;
}
UserList* user_list_create(void) {
    UserList* liste = (UserList*)malloc(sizeof(UserList));
    if (!liste) {
        return NULL;
    }

    liste->capacity = 1500;
    liste->count = 0;
    liste->users = (User*)malloc(sizeof(User) * liste->capacity);

    if (!liste->users) {
        free(liste);
        return NULL;
    }

    return liste;
}
void user_list_destroy(UserList* list) {
    if (!list) {
        return;
    }

    if (list->users) {
        free(list->users);
    }

    free(list);
}
int user_list_add(UserList* list, User user) {
    if (list == NULL) {
        return 0;
    }

    // Check if username already exists
    if (user_list_find_by_username(list, user.username) != NULL) {
        return 0;
    }

    // Check if email already exists
    if (user_list_find_by_email(list, user.email) != NULL) {
        return 0;
    }

    // Resize if needed
    if (list->count >= list->capacity) {
        if (!user_list_resize(list, list->capacity * 2)) {
            return 0;
        }
    }

    // Assign ID if not set
    if (user.id == 0) {
        user.id = next_user_id++;
    }

    // Set creation time if not set
    if (user.created_at == 0) {
        user.created_at = time(NULL);
    }

    list->users[list->count] = user;
    list->count++;

    return 1;
}
int user_list_remove(UserList* list, int user_id) {
    if (list == NULL || list->count == 0) {
        return 0;
    }

    int index = -1;
    for (int i = 0; i < list->count; i++) {
        if (list->users[i].id == user_id) {
            index = i;
            break;
        }
    }

    if (index == -1) {
        return 0;
    }

    // Shift remaining users
    for (int i = index; i < list->count - 1; i++) {
        list->users[i] = list->users[i + 1];
    }

    list->count--;

    // Shrink if capacity is much larger than count
    if (list->capacity > 10 && list->count < list->capacity / 4) {
        user_list_resize(list, list->capacity / 2);
    }

    return 1;
}
User* user_list_find_by_username(UserList* list, const char* username) {
    if (list == NULL || !username) {
        return NULL;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->users[i].username, username) == 0) {
            return &list->users[i];
        }
    }

    return NULL;
}
User* user_list_find_by_id(UserList* list, int user_id) {
    if (list == NULL) {
        return NULL;
    }

    for (int i = 0; i < list->count; i++) {
        if (list->users[i].id == user_id) {
            return &list->users[i];
        }
    }

    return NULL;
}
User* user_list_find_by_email(UserList* list, const char* email) {
    if (list == NULL || !email) {
        return NULL;
    }

    for (int i = 0; i < list->count; i++) {
        if (strcmp(list->users[i].email, email) == 0) {
            return &list->users[i];
        }
    }

    return NULL;
}
int auth_register(UserList* list, const char* username, const char* email,
                  const char* password, UserRole role) {
    if (!list || !username || !email || !password) {
        return 0;
    }

    // Validate inputs
    if (strlen(username) == 0 || strlen(username) >= 50) {
        return 0;
    }

    if (strlen(email) == 0 || strlen(email) >= MAX_EMAIL_LENGTH) {
        return 0;
    }

    if (!utils_string_is_email(email)) {
        return 0;
    }

    if (strlen(password) < 8) {
        return 0;
    }

    // Check if user already exists
    if (user_list_find_by_username(list, username) != NULL) {
        return 0;
    }

    if (user_list_find_by_email(list, email) != NULL) {
        return 0;
    }

    // Create new user
    User new_user = {0};
    new_user.id = next_user_id++;
    strncpy(new_user.username, username, sizeof(new_user.username) - 1);
    strncpy(new_user.email, email, sizeof(new_user.email) - 1);
    new_user.role = role;
    new_user.created_at = time(NULL);
    new_user.last_login = 0;
    new_user.is_active = 1;

    // Generate salt and hash password
    auth_generate_salt(new_user.salt);
    auth_hash_password(password, new_user.salt, new_user.password_hash);

    // Add to list
    return user_list_add(list, new_user);
}
Session* session_create(void) {
    Session* session = (Session*)malloc(sizeof(Session));
    if (!session) {
        return NULL;
    }

    session->user_id = 0;
    session->username[0] = '\0';
    session->role = ROLE_STUDENT;
    session->login_time = 0;
    session->is_valid = 0;

    return session;
}

int auth_logout(Session* session) {
    if (session == NULL) return 0;
    session_destroy(session);
    return 1;
}

void session_destroy(Session* session) {
    if (session) {
        free(session);
    }
}

int auth_login(UserList* list, const char* username, const char* password, Session* session) {
    if (list == NULL || username == NULL || password == NULL || session == NULL) {
        return 0;
    }
    for (int i = 0; i < list->count; i++) {
        if ((strcmp(list->users[i].username, username) == 0 || 
             strcmp(list->users[i].email, username) == 0) &&
            list->users[i].is_active == 1) {
            if (auth_verify_password(password, list->users[i].password_hash, list->users[i].salt)) {
                strcpy(session->username, list->users[i].username);
                session->user_id = list->users[i].id;
                session->role = list->users[i].role;
                session->login_time = time(NULL);
                session->is_valid = 1;
                list->users[i].last_login = time(NULL);

                return 1;
            }
        }
    }
    return 0;
}
int auth_validate_session(Session* session) {
    if (!session) {
        return 0;
    }

    if (!session->is_valid) {
        return 0;
    }

    // Check if session is expired (24 hours)
    time_t now = time(NULL);
    time_t session_age = now - session->login_time;

    if (session_age > 24 * 60 * 60) { // 24 hours
        session->is_valid = 0;
        return 0;
    }

    return 1;
}
int auth_change_password(UserList* list, int user_id, const char* old_password,
                         const char* new_password) {
    if (!list || !old_password || !new_password) {
        return 0;
    }

    // Find user
    User* user = user_list_find_by_id(list, user_id);
    if (!user) {
        return 0;
    }

    // Verify old password
    if (!auth_verify_password(old_password, user->password_hash, user->salt)) {
        return 0;
    }

    // Validate new password strength
    if (!auth_validate_password_strength(new_password)) {
        return 0;
    }

    // Generate new salt and hash new password
    auth_generate_salt(user->salt);
    auth_hash_password(new_password, user->salt, user->password_hash);

    return 1;
}

int auth_reset_password(UserList* list, const char* email) {
    if (!list || !email) {
        return 0;
    }
    
    // Find user by email
    User* user = user_list_find_by_email(list, email);
    if (!user) {
        return 0;
    }
    
    // TODO: Implement actual password reset logic
    // This could involve:
    // 1. Generating a temporary password
    // 2. Sending reset email
    // 3. Allowing user to set new password
    
    // For now, just verify email exists
    return 1;
}
void auth_hash_password(const char* password, const char* salt, char* hash) {
    // Simple hash implementation: combine password and salt
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);
    
    // Create a simple hash from the combined string
    unsigned long long hash_value = 5381;
    for (int i = 0; combined[i] != '\0'; i++) {
        hash_value = ((hash_value << 5) + hash_value) + combined[i];
    }
    
    // Convert to hex string (128 chars)
    sprintf(hash, "%016llx%016llx%016llx%016llx", hash_value, hash_value ^ 0xAAAAAAAA, 
            hash_value ^ 0x55555555, hash_value ^ 0xFFFFFFFF);
}

void auth_generate_salt(char* salt) {
    // Generate a simple salt using timestamp and random values
    srand((unsigned int)time(NULL));
    for (int i = 0; i < 32; i++) {
        int r = rand() % 62;
        if (r < 10) {
            salt[i] = '0' + r;
        } else if (r < 36) {
            salt[i] = 'a' + (r - 10);
        } else {
            salt[i] = 'A' + (r - 36);
        }
    }
    salt[32] = '\0';
}
int auth_verify_password(const char* password, const char* hash, const char* salt) {
    char computed_hash[129];
    auth_hash_password(password, salt, computed_hash);
    printf("[DEBUG] Password verification:\n");
    printf("  Salt: %s\n", salt);
    printf("  Expected hash: %s\n", hash);
    printf("  Computed hash: %s\n", computed_hash);
    printf("  Match: %s\n", strcmp(computed_hash, hash) == 0 ? "YES" : "NO");
    return strcmp(computed_hash, hash) == 0;
}
int auth_validate_password_strength(const char* password) {
    if (!password) return 0;
    
    if (strlen(password) < 8) return 0;
    if (!strpbrk(password, "ABCDEFGHIJKLMNOPQRSTUVWXYZ")) return 0;
    if (!strpbrk(password, "abcdefghijklmnopqrstuvwxyz")) return 0;
    if (!strpbrk(password, "0123456789")) return 0;
    if (!strpbrk(password, "!@#$%^&*()_+-=[]{}|;:,.<>?")) return 0;
    return 1;
}
int session_is_admin(Session* session) {
    if (!session || !session->is_valid) {
        return 0;
    }
    return session->role == ROLE_ADMIN;
}

int session_is_teacher(Session* session) {
    if (!session || !session->is_valid) {
        return 0;
    }
    return session->role == ROLE_TEACHER;
}

int session_is_student(Session* session) {
    if (!session || !session->is_valid) {
        return 0;
    }
    return session->role == ROLE_STUDENT;
}

int auth_save_users(UserList* list, const char* filename) {
    if (!list || !filename) {
        return 0;
    }

    FILE* file = fopen(filename, "w");
    if (!file) {
        printf("[ERROR] Failed to open file for writing: %s\n", filename);
        return 0;
    }

    // Write users in text format: username,email,role,salt,password_hash,created_at,last_login,is_active
    printf("[DEBUG] Saving %d users to %s:\n", list->count, filename);
    for (int i = 0; i < list->count; i++) {
        User* user = &list->users[i];
        printf("[DEBUG]   %d. %s (%s) - Role: %d\n", i+1, user->username, user->email, user->role);
        int written = fprintf(file, "%s,%s,%d,%s,%s,%ld,%ld,%d\n",
                user->username,
                user->email,
                user->role,
                user->salt,
                user->password_hash,
                (long)user->created_at,
                (long)user->last_login,
                user->is_active);
        if (written < 0) {
            printf("[ERROR] Failed to write user %d to file\n", i+1);
            fclose(file);
            return 0;
        }
    }

    // Flush the buffer to ensure all data is written
    fflush(file);
    
    fclose(file);
    printf("[OK] Saved %d users to %s\n", list->count, filename);
    return 1;
}

int auth_load_users(UserList* list, const char* filename) {
    if (!list || !filename) {
        return 0;
    }

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "r");
    if (!file) {
        printf("[WARNING] Failed to open file for reading: %s (might not exist yet)\n", full_path);
        return 0;
    }

    char line[1024];
    int count = 0;

    // Read users in text format: username,email,role,salt,password_hash,created_at,last_login,is_active
    while (fgets(line, sizeof(line), file)) {
        // Remove newline
        line[strcspn(line, "\r\n")] = '\0';
        
        // Skip empty lines
        if (strlen(line) == 0) continue;
        
        // Resize if needed
        if (count >= list->capacity) {
            if (!user_list_resize(list, list->capacity * 2)) {
                fclose(file);
                return 0;
            }
        }

        User* user = &list->users[count];
        
        // Make a working copy of the line for strtok
        char linecopy[1024];
        strncpy(linecopy, line, sizeof(linecopy) - 1);
        linecopy[sizeof(linecopy) - 1] = '\0';
        
        // Parse CSV line: id,username,email,password_hash,salt,role,created_at,last_login,is_active
        char* token = strtok(linecopy, ",");
        if (!token) {
            printf("[WARNING] Skipping malformed user line (no id)\n");
            continue;
        }
        user->id = atoi(token);

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user (no username)\n");
            continue;
        }
        strncpy(user->username, token, sizeof(user->username) - 1);
        user->username[sizeof(user->username) - 1] = '\0';

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no email)\n", user->username);
            continue;
        }
        strncpy(user->email, token, sizeof(user->email) - 1);
        user->email[sizeof(user->email) - 1] = '\0';

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no password_hash)\n", user->username);
            continue;
        }
        strncpy(user->password_hash, token, sizeof(user->password_hash) - 1);
        user->password_hash[sizeof(user->password_hash) - 1] = '\0';

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no salt)\n", user->username);
            continue;
        }
        strncpy(user->salt, token, sizeof(user->salt) - 1);
        user->salt[sizeof(user->salt) - 1] = '\0';

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no role)\n", user->username);
            continue;
        }
        user->role = atoi(token);

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no created_at)\n", user->username);
            continue;
        }
        user->created_at = (time_t)atol(token);

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no last_login)\n", user->username);
            continue;
        }
        user->last_login = (time_t)atol(token);

        token = strtok(NULL, ",");
        if (!token) {
            printf("[WARNING] Skipping user %s (no is_active)\n", user->username);
            continue;
        }
        user->is_active = atoi(token);

        // Update next_user_id if needed
        if (user->id >= next_user_id) {
            next_user_id = user->id + 1;
        }

        count++;
    }

    list->count = count;
    fclose(file);
    printf("[OK] Loaded %d users from %s\n", count, full_path);
    return 1;
}


const char* auth_role_to_string(UserRole role) {
    switch (role) {
        case ROLE_ADMIN:
            return "Admin";
        case ROLE_TEACHER:
            return "Teacher";
        case ROLE_STUDENT:
            return "Student";
        default:
            return "Unknown";
    }
}

UserRole auth_string_to_role(const char* role_str) {
    if (!role_str) {
        return ROLE_STUDENT;
    }

    if (strcmp(role_str, "Admin") == 0 || strcmp(role_str, "admin") == 0) {
        return ROLE_ADMIN;
    } else if (strcmp(role_str, "Teacher") == 0 || strcmp(role_str, "teacher") == 0) {
        return ROLE_TEACHER;
    } else if (strcmp(role_str, "Student") == 0 || strcmp(role_str, "student") == 0) {
        return ROLE_STUDENT;
    }

    return ROLE_STUDENT;
}

void auth_display_user(User* user) {
    if (!user) {
        return;
    }

    printf("User ID: %d\n", user->id);
    printf("Username: %s\n", user->username);
    printf("Email: %s\n", user->email);
    printf("Role: %s\n", auth_role_to_string(user->role));
    printf("Active: %s\n", user->is_active ? "Yes" : "No");

    char* created_str = utils_date_format(user->created_at, "%Y-%m-%d %H:%M:%S");
    if (created_str) {
        printf("Created: %s\n", created_str);
        free(created_str);
    }

    if (user->last_login > 0) {
        char* login_str = utils_date_format(user->last_login, "%Y-%m-%d %H:%M:%S");
        if (login_str) {
            printf("Last Login: %s\n", login_str);
            free(login_str);
        }
    } else {
        printf("Last Login: Never\n");
    }

    printf("---\n");
}

void auth_display_all_users(UserList* list) {
    if (!list) {
        return;
    }

    printf("Total Users: %d\n\n", list->count);

    for (int i = 0; i < list->count; i++) {
        auth_display_user(&list->users[i]);
    }
}

// File operations
int user_list_save_to_file(UserList* list, const char* filename) {
    if (!list || !filename) {
        return -1;
    }

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return -1;
    }

    FILE* file = fopen(full_path, "w");
    if (!file) {
        printf("[ERROR] Failed to open file for writing: %s\n", full_path);
        return -1;
    }

    for (int i = 0; i < list->count; i++) {
        User* u = &list->users[i];
        fprintf(file, "%d,%s,%s,%s,%s,%d,%lld,%lld,%d\n",
                u->id,
                u->username,
                u->email,
                u->password_hash,
                u->salt,
                u->role,
                (long long)u->created_at,
                (long long)u->last_login,
                u->is_active);
    }
    
    fclose(file);
    printf("[OK] Saved %d users to %s\n", list->count, full_path);
    return 0;
}

int user_list_load_from_file(UserList* list, const char* filename) {
    if (!list || !filename) {
        return -1;
    }

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return -1;
    }

    FILE* file = fopen(full_path, "r");
    if (!file) {
        printf("[WARNING] Failed to open file for reading: %s (might not exist yet)\n", full_path);
        return -1;
    }

    char line[1024];
    int index = 0;
    while (fgets(line, sizeof(line), file)) {
        if (index >= list->capacity) {
            int new_cap = list->capacity * 2;
            if (!user_list_resize(list, new_cap)) {
                fclose(file);
                return -1;
            }
        }

        User u;
        long long created_at_tmp, last_login_tmp;
        int fields = sscanf(line, "%d,%49[^,],%99[^,],%127[^,],%32[^,],%d,%lld,%lld,%d",
                            &u.id,
                            u.username,
                            u.email,
                            u.password_hash,
                            u.salt,
                            &u.role,
                            &created_at_tmp,
                            &last_login_tmp,
                            &u.is_active);

        if (fields == 9) {
            u.created_at = (time_t)created_at_tmp;
            u.last_login = (time_t)last_login_tmp;
            list->users[index++] = u;
            if (u.id >= next_user_id) {
                next_user_id = u.id + 1;
            }
        }
    }

    list->count = index;
    fclose(file);
    printf("[OK] Loaded %d users from %s\n", index, filename);
    return 0;
}

// New student registration function (simplified, no admin approval needed)
int auth_register_student(UserList* list, const char* username, const char* email, const char* password) {
    if (!list || !username || !email || !password) {
        return 0; // Invalid input
    }

    // Validate email format
    if (!auth_verify_email_format(email)) {
        return -1; // Invalid email format
    }

    // Validate username format
    if (!auth_verify_username_format(username)) {
        return -2; // Invalid username format
    }

    // Validate password strength
    if (!auth_validate_password_strength(password)) {
        return -3; // Weak password
    }

    // Check if username already exists
    if (user_list_find_by_username(list, username) != NULL) {
        return -4; // Username already exists
    }

    // Check if email already exists
    if (user_list_find_by_email(list, email) != NULL) {
        return -5; // Email already exists
    }

    // Register as student role
    int result = auth_register(list, username, email, password, ROLE_STUDENT);
    return result ? 1 : 0; // 1 = success, 0 = failed
}

// Request password reset (generates a reset code)
int auth_request_password_reset(UserList* list, const char* email, char* reset_code) {
    if (!list || !email || !reset_code) {
        return 0;
    }

    // Find user by email
    User* user = user_list_find_by_email(list, email);
    if (!user) {
        return -1; // Email not found
    }

    // Generate a simple 6-digit reset code
    srand((unsigned int)time(NULL));
    int code = 100000 + (rand() % 900000);
    sprintf(reset_code, "%06d", code);

    // In a real system, you would:
    // 1. Store this code in the database with an expiration time
    // 2. Send it via email to the user
    // For now, we'll just return it and display it to the user
    
    return 1; // Success
}

// Reset password using the reset code
int auth_reset_password_with_code(UserList* list, const char* email, const char* reset_code, const char* new_password) {
    if (!list || !email || !reset_code || !new_password) {
        return 0;
    }

    // Find user by email
    User* user = user_list_find_by_email(list, email);
    if (!user) {
        return -1; // Email not found
    }

    // In a real system, you would verify the reset code here
    // For this demo, we'll accept any 6-digit code
    if (strlen(reset_code) != 6) {
        return -2; // Invalid code format
    }

    // Validate new password strength
    if (!auth_validate_password_strength(new_password)) {
        return -3; // Weak password
    }

    // Generate new salt and hash new password
    auth_generate_salt(user->salt);
    auth_hash_password(new_password, user->salt, user->password_hash);

    return 1; // Success
}

// Verify email format
int auth_verify_email_format(const char* email) {
    if (!email || strlen(email) == 0) {
        return 0;
    }

    // Check for @ symbol
    const char* at = strchr(email, '@');
    if (!at || at == email) {
        return 0;
    }

    // Check for dot after @
    const char* dot = strchr(at, '.');
    if (!dot || dot == at + 1 || dot[1] == '\0') {
        return 0;
    }

    // Check length
    if (strlen(email) >= MAX_EMAIL_LENGTH) {
        return 0;
    }

    return 1;
}

// Verify username format
int auth_verify_username_format(const char* username) {
    if (!username) {
        return 0;
    }

    size_t len = strlen(username);
    
    // Check length (3-49 characters)
    if (len < 3 || len >= 50) {
        return 0;
    }

    // Check first character is alphanumeric
    if (!((username[0] >= 'a' && username[0] <= 'z') ||
          (username[0] >= 'A' && username[0] <= 'Z') ||
          (username[0] >= '0' && username[0] <= '9'))) {
        return 0;
    }

    // Check all characters are alphanumeric or underscore
    for (size_t i = 0; i < len; i++) {
        char c = username[i];
        if (!((c >= 'a' && c <= 'z') ||
              (c >= 'A' && c <= 'Z') ||
              (c >= '0' && c <= '9') ||
              (c == '_'))) {
            return 0;
        }
    }

    return 1;
}