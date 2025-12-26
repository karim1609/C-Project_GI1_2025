#include "file_manager.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <dirent.h>
#include <unistd.h>
#include <time.h>
#include <direct.h>
#include <sys/stat.h>
#include <windows.h>
#include "config.h"
#include "utils.h"

// Local utility helpers
static const char* fm_basename(const char* path) {
    const char* last_slash = strrchr(path, '/');
    const char* last_backslash = strrchr(path, '\\');
    const char* base = path;

    if (last_slash && (!last_backslash || last_slash > last_backslash)) {
        base = last_slash + 1;
    } else if (last_backslash) {
        base = last_backslash + 1;
    }
    return base;
}

FileResult file_manager_init(void){
    // Ensure required data directories exist
    if (ensure_data_directories() != 0) {
        fprintf(stderr, "Error: Failed to create data directories.\n");
        return FILE_ERROR_PERMISSION_DENIED;
    }

    // Perform cleanup of any lingering temp files
    if (cleanup_temp_files() != 0) {
        fprintf(stderr, "Warning: Failed to clean up temp files.\n");
        // Not critical enough to abort initialization, so continue
    }

    // File system initialization complete

    return FILE_SUCCESS;
}
void file_manager_cleanup(void){
    // Perform any file manager cleanup here

    // Attempt to clean up temporary files (again, at shutdown)
    if (cleanup_temp_files() != 0) {
        fprintf(stderr, "Warning: Failed to clean up temp files during shutdown.\n");
    }

    // In the future, add more cleanup logic if file manager allocates resources

    // For now, nothing else to free -- most resources are owned/managed elsewhere
}
int create_directory(const char* path) {
    // Windows version
    return _mkdir(path);
}

int directory_exists(const char* path) {
    struct _stat st;
    if (_stat(path, &st) == 0 && (st.st_mode & _S_IFDIR)) {
        return 1; // Directory exists
    }
    return 0; // Not found or not a directory
}
int ensure_data_directories(void){
    int error_flag = 0;
    
    // Get the full path to the data directory
    char full_data_dir[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path("", full_data_dir, sizeof(full_data_dir))) {
        fprintf(stderr, "Error: Unable to construct data directory path\n");
        return 1;
    }
    
    // List of required directories
    const char* required_dirs[] = {
        full_data_dir
    };
    int num_dirs = sizeof(required_dirs) / sizeof(required_dirs[0]);

    // List of core data files (using relative names, will be converted to full paths)
    const char* required_file_names[] = {
        STUDENTS_FILE,
        USERS_FILE,
        GRADES_FILE,
        CLUBS_FILE,
        ATTENDANCE_FILE,
        LOGS_FILE
    };
    int num_files = sizeof(required_file_names) / sizeof(required_file_names[0]);

    // Create required directories if missing
    for (int i = 0; i < num_dirs; ++i) {
        int exists = directory_exists(required_dirs[i]);
        printf("[DEBUG] Directory '%s' exists: %d\n", required_dirs[i], exists);
        if (!exists) {
            int result = create_directory(required_dirs[i]);
            printf("[DEBUG] create_directory('%s') returned: %d\n", required_dirs[i], result);
            if (result != 0 && errno != EEXIST) {
                fprintf(stderr, "Error: Unable to create required directory: %s (errno: %d)\n", required_dirs[i], errno);
                error_flag = 1;
            }
        }
    }

    // Ensure required data and key files exist
    for (int i = 0; i < num_files; ++i) {
        char full_file_path[UTILS_MAX_PATH_LENGTH];
        if (!utils_get_data_file_path(required_file_names[i], full_file_path, sizeof(full_file_path))) {
            fprintf(stderr, "Error: Unable to construct path for file: %s\n", required_file_names[i]);
            error_flag = 1;
            continue;
        }
        
        FILE* f = fopen(full_file_path, "rb");
        if (!f) {
            // Try to create if missing
            f = fopen(full_file_path, "wb");
            if (!f) {
                fprintf(stderr, "Error: Unable to create required data/key file: %s\n", full_file_path);
                error_flag = 1;
            } else {
                fclose(f);
            }
        } else {
            fclose(f);
        }
    }

    return error_flag;
}
int cleanup_temp_files(void){
    // Cleans up any files in DATA_DIR ending with .tmp or .temp
    // Returns the number of files removed, or -1 on error
    
    char full_data_dir[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path("", full_data_dir, sizeof(full_data_dir))) {
        fprintf(stderr, "Warning: Unable to construct data directory path for cleanup\n");
        return -1;
    }
  
    const char* dirs[] = { full_data_dir, NULL };
    int removed_count = 0;

    for (int d = 0; dirs[d] != NULL; ++d) {
        DIR* dir = opendir(dirs[d]);
        if (!dir) {
            fprintf(stderr, "Warning: Could not open directory for temp file cleanup: %s\n", dirs[d]);
            continue; // Don't return - just skip this dir
        }
        struct dirent* entry;
        while ((entry = readdir(dir)) != NULL) {
            // Skip . and ..
            if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0)
                continue;
            // Check for .tmp or .temp at end of filename
            size_t len = strlen(entry->d_name);
            if ((len > 4 && strcmp(entry->d_name + len - 4, ".tmp") == 0) ||
                (len > 5 && strcmp(entry->d_name + len - 5, ".temp") == 0)) {

                char full_path[512];
                snprintf(full_path, sizeof(full_path), "%s%s", dirs[d], entry->d_name);

                if (remove(full_path) == 0) {
                    ++removed_count;
                } else {
                    fprintf(stderr, "Warning: Failed to remove temp file: %s\n", full_path);
                }
            }
        }
        closedir(dir);
    }

    return removed_count;
}
int file_exists(const char* filename){
    struct _stat st ;
    if(_stat(filename,&st)==0 && (st.st_mode & _S_IFREG)) {
        return 1; // File exists
    }
    return 0; // Not found or not a file
}

int validate_file_integrity(const char* filename){
    char expected_checksum[65] = {0};
    char actual_checksum[65] = {0};

    // Attempt to calculate the file's checksum
    if (calculate_file_checksum(filename, actual_checksum) != 0) {
        // Failed to read or hash file, or file not found
        return 0;
    }

    // Try to find a sidecar checksum file (e.g., filename.sha256)
    char checksum_file[512];
    snprintf(checksum_file, sizeof(checksum_file), "%s.sha256", filename);

    FILE* chkf = fopen(checksum_file, "r");
    if (chkf) {
        if (fgets(expected_checksum, sizeof(expected_checksum), chkf) != NULL) {
            // Remove trailing newline if any
            size_t len = strlen(expected_checksum);
            if (len > 0 && expected_checksum[len-1] == '\n')
                expected_checksum[len-1] = '\0';
        }
        fclose(chkf);
    } else {
        // If checksum file doesn't exist, assume integrity is not guaranteed, return false
        return 0;
    }

    // Compare
    if (strcmp(expected_checksum, actual_checksum) == 0) {
        return 1; // File is OK
    }
    return 0; // Invalid/corrupted
}
FileInfo* get_file_info(const char* filename){
    if (!filename || !file_exists(filename)) {
        return NULL;
    }

    FileInfo* info = (FileInfo*)malloc(sizeof(FileInfo));
    if (!info) {
        return NULL;
    }
    memset(info, 0, sizeof(FileInfo));
    strncpy(info->filename, filename, sizeof(info->filename)-1);

    struct _stat st;
    if (_stat(filename, &st) != 0) {
        free(info);
        return NULL;
    }
    info->file_size = st.st_size;
    info->last_modified = st.st_mtime;
    info->created = st.st_ctime;

    if (calculate_file_checksum(filename, info->checksum) != 0) {
        // Leave checksum as empty string
        info->checksum[0] = '\0';
    }

    return info;
}
void free_file_info(FileInfo* info){
    if (info) {
        free(info);
    }
}
FileResult read_file_content(const char* filename, char** content, size_t* content_size){
    if (!filename || !content || !content_size) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    *content = NULL;
    *content_size = 0;

    FILE* fp = fopen(filename, "rb");
    if (!fp) {
        return FILE_ERROR_NOT_FOUND;
    }

    fseek(fp, 0, SEEK_END);
    long fsize = ftell(fp);
    if (fsize < 0) {
        fclose(fp);
        return FILE_ERROR_CORRUPTED;
    }
    fseek(fp, 0, SEEK_SET);

    char* buf = (char*)malloc(fsize + 1);
    if (!buf) {
        fclose(fp);
        return FILE_ERROR_DISK_FULL;
    }

    size_t read_size = fread(buf, 1, fsize, fp);
    fclose(fp);

    if ((long)read_size != fsize) {
        free(buf);
        return FILE_ERROR_CORRUPTED;
    }

    buf[fsize] = '\0'; // Null-terminate for convenience (even for binary)
    *content = buf;
    *content_size = fsize;

    return FILE_SUCCESS;
}
FileResult write_file_content(const char* filename, const char* content, size_t content_size) {
    if (!filename || !content || content_size == 0) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    FILE* fp = fopen(filename, "wb");
    if (!fp) {
        return FILE_ERROR_PERMISSION_DENIED;
    }

    size_t written = fwrite(content, 1, content_size, fp);
    fclose(fp);

    if (written != content_size) {
        return FILE_ERROR_DISK_FULL;
    }

    return FILE_SUCCESS;
}
FileResult append_to_file(const char* filename, const char* content) {
    if (!filename || !content) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    FILE* fp = fopen(filename, "ab");
    if (!fp) {
        return FILE_ERROR_PERMISSION_DENIED;
    }

    size_t content_size = strlen(content);
    size_t written = fwrite(content, 1, content_size, fp);
    fclose(fp);

    if (written != content_size) {
        return FILE_ERROR_DISK_FULL;
    }

    return FILE_SUCCESS;
}
FileResult delete_file(const char* filename) {
    if (!filename || strlen(filename) == 0) {
        return FILE_ERROR_INVALID_FORMAT;
    }
    if (remove(filename) == 0) {
        // Remove sidecar checksum file if present
        char checksum_file[512];
        snprintf(checksum_file, sizeof(checksum_file), "%s.sha256", filename);
        remove(checksum_file);
        return FILE_SUCCESS;
    } else {
        // Error: check if file exists or it's a permission problem
        if (!file_exists(filename)) {
            return FILE_ERROR_NOT_FOUND;
        }
        return FILE_ERROR_PERMISSION_DENIED;
    }
}

FileResult save_data_to_file(const void* data, size_t data_size, const char* filename){
    if (!data || data_size == 0 || !filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }
    return write_file_content(filename, (const char*)data, data_size);
}
FileResult load_data_from_file(void* data, size_t data_size, const char* filename){
    if (!data || data_size == 0 || !filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    char* buffer = NULL;
    size_t file_size = 0;
    FileResult res = read_file_content(filename, &buffer, &file_size);

    if (res != FILE_SUCCESS || !buffer) {
        if (buffer) {
            free(buffer);
        }
        return res;
    }

    // Ensure the destination buffer is large enough
    if (file_size > data_size) {
        // Copy only what fits to avoid overflow, but signal invalid format
        memcpy(data, buffer, data_size);
        free(buffer);
        return FILE_ERROR_INVALID_FORMAT;
    }

    memcpy(data, buffer, file_size);
    free(buffer);
    return FILE_SUCCESS;
}
FileResult save_struct_to_file(const void* struct_data, size_t struct_size, int count, const char* filename){
    if (!struct_data || struct_size == 0 || count < 0 || !filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    size_t total_size = sizeof(int) + (size_t)count * struct_size;
    unsigned char* buffer = (unsigned char*)malloc(total_size);
    if (!buffer) {
        return FILE_ERROR_DISK_FULL;
    }

    // Store count followed by raw struct array bytes
    memcpy(buffer, &count, sizeof(int));
    if (count > 0) {
        memcpy(buffer + sizeof(int), struct_data, (size_t)count * struct_size);
    }

    FileResult result = save_data_to_file(buffer, total_size, filename);
    free(buffer);
    return result;
}
FileResult load_struct_from_file(void** struct_data, size_t struct_size, int* count, const char* filename){
    if (!struct_data || !count || struct_size == 0 || !filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    *struct_data = NULL;
    *count = 0;

    char* buffer = NULL;
    size_t file_size = 0;
    FileResult res = read_file_content(filename, &buffer, &file_size);

    if (res != FILE_SUCCESS || !buffer) {
        if (buffer) {
            free(buffer);
        }
        return res;
    }

    if (file_size < sizeof(int)) {
        free(buffer);
        return FILE_ERROR_INVALID_FORMAT;
    }

    int local_count = 0;
    memcpy(&local_count, buffer, sizeof(int));
    if (local_count < 0) {
        free(buffer);
        return FILE_ERROR_INVALID_FORMAT;
    }

    size_t expected_size = (size_t)local_count * struct_size;
    if (file_size < sizeof(int) + expected_size) {
        free(buffer);
        return FILE_ERROR_INVALID_FORMAT;
    }

    void* data_buf = NULL;
    if (expected_size > 0) {
        data_buf = malloc(expected_size);
        if (!data_buf) {
            free(buffer);
            return FILE_ERROR_DISK_FULL;
        }
        memcpy(data_buf, buffer + sizeof(int), expected_size);
    }

    free(buffer);

    *struct_data = data_buf;
    *count = local_count;

    return FILE_SUCCESS;
}
FileResult create_backup(const char* source_file, const char* backup_dir){
    if (!source_file || !backup_dir) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(source_file)) {
        return FILE_ERROR_NOT_FOUND;
    }

    // Ensure backup directory exists
    if (!directory_exists(backup_dir)) {
        if (create_directory(backup_dir) != 0) {
            return FILE_ERROR_PERMISSION_DENIED;
        }
    }

    const char* base_name = fm_basename(source_file);

    // Create time-based backup name
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[64];
    if (tm_info) {
        strftime(timestamp, sizeof(timestamp), "%Y%m%d_%H%M%S", tm_info);
    } else {
        snprintf(timestamp, sizeof(timestamp), "unknown_time");
    }

    char backup_path[512];
    snprintf(backup_path, sizeof(backup_path), "%s%s_%s.bak", backup_dir, base_name, timestamp);

    FILE* src = fopen(source_file, "rb");
    if (!src) {
        return FILE_ERROR_PERMISSION_DENIED;
    }

    FILE* dst = fopen(backup_path, "wb");
    if (!dst) {
        fclose(src);
        return FILE_ERROR_DISK_FULL;
    }

    char buffer[4096];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            remove(backup_path);
            return FILE_ERROR_DISK_FULL;
        }
    }

    fclose(src);
    fclose(dst);

    return FILE_SUCCESS;
}


int calculate_file_checksum(const char* filename, char* checksum){
    // Simple checksum implementation using file size and modification time
    if (!filename || !checksum) {
        return -1;
    }
    
    struct _stat st;
    if (_stat(filename, &st) != 0) {
        return -1;
    }
    
    // Create a simple hash from file size and modification time
    sprintf(checksum, "%016llx%016llx", (unsigned long long)st.st_size, (unsigned long long)st.st_mtime);
    
    return 0;
}

// File monitoring functions
FileMonitor* create_file_monitor(const char* filename) {
    if (!filename || !file_exists(filename)) {
        return NULL;
    }

    FileMonitor* monitor = (FileMonitor*)malloc(sizeof(FileMonitor));
    if (!monitor) {
        return NULL;
    }

    strncpy(monitor->filename, filename, sizeof(monitor->filename) - 1);
    monitor->filename[sizeof(monitor->filename) - 1] = '\0';

    struct _stat st;
    if (_stat(filename, &st) == 0) {
        monitor->last_size = st.st_size;
        monitor->last_check = time(NULL);
    } else {
        monitor->last_size = 0;
        monitor->last_check = time(NULL);
    }

    if (calculate_file_checksum(filename, monitor->last_checksum) != 0) {
        monitor->last_checksum[0] = '\0';
    }

    return monitor;
}

void destroy_file_monitor(FileMonitor* monitor) {
    if (monitor) {
        free(monitor);
    }
}

int check_file_changes(FileMonitor* monitor) {
    if (!monitor || !file_exists(monitor->filename)) {
        return -1; // Error or file deleted
    }

    struct _stat st;
    if (_stat(monitor->filename, &st) != 0) {
        return -1;
    }

    // Check size change
    if (st.st_size != monitor->last_size) {
        monitor->last_size = st.st_size;
        monitor->last_check = time(NULL);
        if (calculate_file_checksum(monitor->filename, monitor->last_checksum) == 0) {
            return 1; // Changed
        }
        return 1; // Changed (size)
    }

    // Check checksum change
    char current_checksum[65];
    if (calculate_file_checksum(monitor->filename, current_checksum) == 0) {
        if (strcmp(current_checksum, monitor->last_checksum) != 0) {
            strncpy(monitor->last_checksum, current_checksum, sizeof(monitor->last_checksum) - 1);
            monitor->last_checksum[sizeof(monitor->last_checksum) - 1] = '\0';
            monitor->last_check = time(NULL);
            return 1; // Changed
        }
    }

    monitor->last_check = time(NULL);
    return 0; // No change
}

// Utility functions
const char* file_result_to_string(FileResult result) {
    switch (result) {
        case FILE_SUCCESS:
            return "Success";
        case FILE_ERROR_NOT_FOUND:
            return "File not found";
        case FILE_ERROR_PERMISSION_DENIED:
            return "Permission denied";
        case FILE_ERROR_DISK_FULL:
            return "Disk full";
        case FILE_ERROR_CORRUPTED:
            return "File corrupted";
        case FILE_ERROR_INVALID_FORMAT:
            return "Invalid format";
        default:
            return "Unknown error";
    }
}

int get_file_size(const char* filename) {
    if (!filename) {
        return -1;
    }

    struct _stat st;
    if (_stat(filename, &st) != 0) {
        return -1;
    }

    return (int)st.st_size;
}

time_t get_file_modification_time(const char* filename) {
    if (!filename) {
        return (time_t)-1;
    }

    struct _stat st;
    if (_stat(filename, &st) != 0) {
        return (time_t)-1;
    }

    return st.st_mtime;
}

int compare_files(const char* file1, const char* file2) {
    if (!file1 || !file2) {
        return -1;
    }

    char checksum1[65], checksum2[65];

    if (calculate_file_checksum(file1, checksum1) != 0) {
        return -1;
    }

    if (calculate_file_checksum(file2, checksum2) != 0) {
        return -1;
    }

    return strcmp(checksum1, checksum2) == 0 ? 0 : 1;
}

// Configuration file operations
FileResult load_config_file(const char* filename) {
    if (!filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(filename)) {
        // Create empty config file if it doesn't exist
        FILE* fp = fopen(filename, "w");
        if (!fp) {
            return FILE_ERROR_PERMISSION_DENIED;
        }
        fclose(fp);
        return FILE_SUCCESS;
    }

    // Config file exists, validation only
    return FILE_SUCCESS;
}

FileResult save_config_file(const char* filename) {
    if (!filename) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    // This is a placeholder - actual config saving would be handled by config.h functions
    // Just ensure the file can be written to
    FILE* fp = fopen(filename, "a");
    if (!fp) {
        return FILE_ERROR_PERMISSION_DENIED;
    }
    fclose(fp);
    return FILE_SUCCESS;
}

FileResult update_config_value(const char* filename, const char* key, const char* value) {
    if (!filename || !key || !value) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    // Read entire config file
    char* content = NULL;
    size_t content_size = 0;
    FileResult res = read_file_content(filename, &content, &content_size);
    if (res != FILE_SUCCESS && res != FILE_ERROR_NOT_FOUND) {
        return res;
    }

    // If file doesn't exist, create it
    if (res == FILE_ERROR_NOT_FOUND) {
        content = (char*)malloc(1);
        if (!content) {
            return FILE_ERROR_DISK_FULL;
        }
        content[0] = '\0';
        content_size = 0;
    }

    // Simple key=value format parser
    char* new_content = NULL;
    size_t new_size = 0;
    int found = 0;

    // Build new content line by line
    char temp_line[1024];
    size_t pos = 0;

    while (pos < content_size) {
        size_t line_len = 0;
        while (pos + line_len < content_size && content[pos + line_len] != '\n' && content[pos + line_len] != '\r') {
            line_len++;
        }

        if (line_len > 0 && line_len < sizeof(temp_line) - 1) {
            memcpy(temp_line, content + pos, line_len);
            temp_line[line_len] = '\0';

            // Check if this line has our key
            char* eq_pos = strchr(temp_line, '=');
            if (eq_pos) {
                *eq_pos = '\0';
                if (strcmp(temp_line, key) == 0) {
                    // Replace this line
                    size_t needed = strlen(key) + strlen(value) + 10;
                    char* new_line = (char*)malloc(needed);
                    if (new_line) {
                        snprintf(new_line, needed, "%s=%s\n", key, value);
                        size_t new_line_len = strlen(new_line);
                        new_content = (char*)realloc(new_content, new_size + new_line_len);
                        if (new_content) {
                            memcpy(new_content + new_size, new_line, new_line_len);
                            new_size += new_line_len;
                        }
                        free(new_line);
                    }
                    found = 1;
                } else {
                    // Keep original line
                    size_t needed = line_len + 2;
                    new_content = (char*)realloc(new_content, new_size + needed);
                    if (new_content) {
                        memcpy(new_content + new_size, temp_line, line_len);
                        new_content[new_size + line_len] = '\n';
                        new_size += needed;
                    }
                }
            } else {
                // Keep line as-is (no = sign)
                size_t needed = line_len + 2;
                new_content = (char*)realloc(new_content, new_size + needed);
                if (new_content) {
                    memcpy(new_content + new_size, temp_line, line_len);
                    new_content[new_size + line_len] = '\n';
                    new_size += needed;
                }
            }
        }

        pos += line_len;
        // Skip newline characters
        while (pos < content_size && (content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
    }

    // If key not found, append it
    if (!found) {
        size_t needed = strlen(key) + strlen(value) + 10;
        new_content = (char*)realloc(new_content, new_size + needed);
        if (new_content) {
            snprintf(new_content + new_size, needed, "%s=%s\n", key, value);
            new_size += strlen(new_content + new_size);
        }
    }

    free(content);

    // Write new content
    res = write_file_content(filename, new_content, new_size);
    free(new_content);

    return res;
}

FileResult get_config_value(const char* filename, const char* key, char* value, size_t value_size) {
    if (!filename || !key || !value || value_size == 0) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(filename)) {
        return FILE_ERROR_NOT_FOUND;
    }

    char* content = NULL;
    size_t content_size = 0;
    FileResult res = read_file_content(filename, &content, &content_size);
    if (res != FILE_SUCCESS) {
        return res;
    }

    // Simple key=value parser
    size_t pos = 0;
    while (pos < content_size) {
        size_t line_start = pos;
        size_t line_len = 0;

        // Find end of line
        while (pos + line_len < content_size && content[pos + line_len] != '\n' && content[pos + line_len] != '\r') {
            line_len++;
        }

        if (line_len > 0) {
            char line[512];
            if (line_len < sizeof(line) - 1) {
                memcpy(line, content + line_start, line_len);
                line[line_len] = '\0';

                char* eq_pos = strchr(line, '=');
                if (eq_pos) {
                    *eq_pos = '\0';
                    if (strcmp(line, key) == 0) {
                        // Found the key
                        eq_pos++;
                        size_t val_len = strlen(eq_pos);
                        if (val_len >= value_size) {
                            val_len = value_size - 1;
                        }
                        strncpy(value, eq_pos, val_len);
                        value[val_len] = '\0';
                        free(content);
                        return FILE_SUCCESS;
                    }
                }
            }
        }

        pos += line_len;
        // Skip newline characters
        while (pos < content_size && (content[pos] == '\n' || content[pos] == '\r')) {
            pos++;
        }
    }

    free(content);
    return FILE_ERROR_NOT_FOUND;
}

// Log file operations
FileResult append_to_log(const char* log_file, const char* message) {
    if (!log_file || !message) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    FILE* fp = fopen(log_file, "a");
    if (!fp) {
        return FILE_ERROR_PERMISSION_DENIED;
    }

    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char timestamp[64];
    strftime(timestamp, sizeof(timestamp), "%Y-%m-%d %H:%M:%S", tm_info);

    fprintf(fp, "[%s] %s\n", timestamp, message);
    fclose(fp);

    return FILE_SUCCESS;
}

FileResult rotate_log_file(const char* log_file, int max_size) {
    if (!log_file || max_size <= 0) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    if (!file_exists(log_file)) {
        return FILE_SUCCESS; // Nothing to rotate
    }

    int current_size = get_file_size(log_file);
    if (current_size < 0) {
        return FILE_ERROR_NOT_FOUND;
    }

    if (current_size < max_size) {
        return FILE_SUCCESS; // No rotation needed
    }

    // Create rotated filename with timestamp
    time_t now = time(NULL);
    struct tm* tm_info = localtime(&now);
    char rotated_name[512];
    snprintf(rotated_name, sizeof(rotated_name), "%s.%04d%02d%02d_%02d%02d%02d",
             log_file,
             tm_info->tm_year + 1900,
             tm_info->tm_mon + 1,
             tm_info->tm_mday,
             tm_info->tm_hour,
             tm_info->tm_min,
             tm_info->tm_sec);

    // Rename current log to rotated name
    if (rename(log_file, rotated_name) != 0) {
        return FILE_ERROR_PERMISSION_DENIED;
    }

    // Create new empty log file
    FILE* fp = fopen(log_file, "w");
    if (!fp) {
        return FILE_ERROR_PERMISSION_DENIED;
    }
    fclose(fp);

    return FILE_SUCCESS;
}

FileResult cleanup_log_files(const char* log_dir, int days_to_keep) {
    if (!log_dir || days_to_keep < 0) {
        return FILE_ERROR_INVALID_FORMAT;
    }

    DIR* dir = opendir(log_dir);
    if (!dir) {
        return FILE_ERROR_NOT_FOUND;
    }

    time_t now = time(NULL);
    time_t cutoff = now - (time_t)days_to_keep * 24 * 60 * 60;

    struct dirent* entry;
    while ((entry = readdir(dir)) != NULL) {
        if (strcmp(entry->d_name, ".") == 0 || strcmp(entry->d_name, "..") == 0) {
            continue;
        }

        // Check if it's a log file (ends with .log or matches log pattern)
        size_t len = strlen(entry->d_name);
        int is_log = 0;
        if (len > 4 && strcmp(entry->d_name + len - 4, ".log") == 0) {
            is_log = 1;
        } else if (len > 4 && strstr(entry->d_name, ".log.") != NULL) {
            is_log = 1;
        }

        if (!is_log) {
            continue;
        }

        char full_path[512];
        snprintf(full_path, sizeof(full_path), "%s%s", log_dir, entry->d_name);

        struct _stat st;
        if (_stat(full_path, &st) != 0) {
            continue;
        }

        if (st.st_mtime < cutoff) {
            remove(full_path);
        }
    }

    closedir(dir);
    return FILE_SUCCESS;
}