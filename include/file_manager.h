#ifndef FILE_MANAGER_H
#define FILE_MANAGER_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/stat.h>
#include "config.h"

// File operation result codes
typedef enum {
    FILE_SUCCESS = 0,
    FILE_ERROR_NOT_FOUND = -1,
    FILE_ERROR_PERMISSION_DENIED = -2,
    FILE_ERROR_DISK_FULL = -3,
    FILE_ERROR_CORRUPTED = -4,
    FILE_ERROR_INVALID_FORMAT = -5
} FileResult;

// File information structure
typedef struct {
    char filename[256];
    size_t file_size;
    time_t last_modified;
    time_t created;
    char checksum[65];  // SHA-256 checksum
} FileInfo;

// File manager functions
FileResult file_manager_init(void);
void file_manager_cleanup(void);

// Directory operations
int create_directory(const char* path);
int directory_exists(const char* path);
int ensure_data_directories(void);
int cleanup_temp_files(void);

// File existence and validation
int file_exists(const char* filename);
int validate_file_integrity(const char* filename);
FileInfo* get_file_info(const char* filename);
void free_file_info(FileInfo* info);

// Basic file operations
FileResult read_file_content(const char* filename, char** content, size_t* content_size);
FileResult write_file_content(const char* filename, const char* content, size_t content_size);
FileResult append_to_file(const char* filename, const char* content);
FileResult copy_file(const char* source, const char* destination);
FileResult move_file(const char* source, const char* destination);
FileResult delete_file(const char* filename);

// Data serialization/deserialization (plain text files)
FileResult save_data_to_file(const void* data, size_t data_size, const char* filename);
FileResult load_data_from_file(void* data, size_t data_size, const char* filename);
FileResult save_struct_to_file(const void* struct_data, size_t struct_size, int count, const char* filename);
FileResult load_struct_from_file(void** struct_data, size_t struct_size, int* count, const char* filename);

// File monitoring
typedef struct {
    char filename[256];
    time_t last_check;
    size_t last_size;
    char last_checksum[65];
} FileMonitor;

FileMonitor* create_file_monitor(const char* filename);
void destroy_file_monitor(FileMonitor* monitor);
int check_file_changes(FileMonitor* monitor);

// Utility functions
const char* file_result_to_string(FileResult result);
int get_file_size(const char* filename);
time_t get_file_modification_time(const char* filename);
int calculate_file_checksum(const char* filename, char* checksum);
int compare_files(const char* file1, const char* file2);

// Configuration file operations
FileResult load_config_file(const char* filename);
FileResult save_config_file(const char* filename);
FileResult update_config_value(const char* filename, const char* key, const char* value);
FileResult get_config_value(const char* filename, const char* key, char* value, size_t value_size);

// Log file operations
FileResult append_to_log(const char* log_file, const char* message);
FileResult rotate_log_file(const char* log_file, int max_size);
FileResult cleanup_log_files(const char* log_dir, int days_to_keep);

#endif // FILE_MANAGER_H
