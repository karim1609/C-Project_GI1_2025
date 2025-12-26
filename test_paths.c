// Test program to verify data file paths
#include <stdio.h>
#include <stdlib.h>
#include "include/utils.h"
#include "include/file_manager.h"

int main() {
    printf("=== Data Path Resolution Test ===\n\n");
    
    // Test getting data directory
    char data_dir[512];
    if (utils_get_data_file_path("", data_dir, sizeof(data_dir))) {
        printf("Data directory: %s\n", data_dir);
        
        // Check if it exists
        if (directory_exists(data_dir)) {
            printf("✓ Data directory EXISTS\n");
        } else {
            printf("✗ Data directory DOES NOT EXIST\n");
        }
    } else {
        printf("✗ Failed to get data directory path\n");
    }
    
    printf("\n");
    
    // Test getting specific file paths
    const char* test_files[] = {
        "students.txt",
        "users.txt",
        "grades.txt",
        "clubs.txt"
    };
    
    for (int i = 0; i < 4; i++) {
        char file_path[512];
        if (utils_get_data_file_path(test_files[i], file_path, sizeof(file_path))) {
            printf("File: %s\n", test_files[i]);
            printf("  Full path: %s\n", file_path);
            
            if (file_exists(file_path)) {
                printf("  Status: ✓ EXISTS\n");
            } else {
                printf("  Status: ✗ DOES NOT EXIST\n");
            }
        } else {
            printf("✗ Failed to get path for: %s\n", test_files[i]);
        }
        printf("\n");
    }
    
    // Test file manager initialization
    printf("=== File Manager Initialization ===\n");
    FileResult result = file_manager_init();
    if (result == FILE_SUCCESS) {
        printf("✓ File manager initialized successfully\n");
    } else {
        printf("✗ File manager initialization failed: %d\n", result);
    }
    
    return 0;
}
