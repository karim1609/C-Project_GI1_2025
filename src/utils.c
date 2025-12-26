#include "utils.h"
#include <stdarg.h>
#include <errno.h>
#include <limits.h>

#if defined(_WIN32) || defined(_WIN64)
#include <windows.h>
#include <io.h>
#include <direct.h>
#include <sys/stat.h>
#include <string.h>
#define access _access
#define mkdir _mkdir
#define stat _stat
#define strcasecmp _stricmp
#else
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <pwd.h>
#endif

// Static variables for error handling
static void (*error_callback)(const char*) = NULL;

// ============================================================================
// STRING UTILITIES
// ============================================================================

char* utils_string_trim(char* str) {
    if (!str) return NULL;
    return utils_string_trim_left(utils_string_trim_right(str));
}

char* utils_string_trim_left(char* str) {
    if (!str) return NULL;
    char* start = str;
    while (*start && isspace((unsigned char)*start)) {
        start++;
    }
    if (start != str) {
        size_t len = strlen(start);
        memmove(str, start, len + 1);
    }
    return str;
}

char* utils_string_trim_right(char* str) {
    if (!str) return NULL;
    char* end = str + strlen(str) - 1;
    while (end >= str && isspace((unsigned char)*end)) {
        *end = '\0';
        end--;
    }
    return str;
}

char* utils_string_to_lower(char* str) {
    if (!str) return NULL;
    for (char* p = str; *p; p++) {
        *p = (char)tolower((unsigned char)*p);
    }
    return str;
}

char* utils_string_to_upper(char* str) {
    if (!str) return NULL;
    for (char* p = str; *p; p++) {
        *p = (char)toupper((unsigned char)*p);
    }
    return str;
}

char* utils_string_replace(char* str, const char* find, const char* replace) {
    if (!str || !find || !replace) return str;
    
    size_t find_len = strlen(find);
    if (find_len == 0) return str;
    
    size_t replace_len = strlen(replace);
    (void)replace_len;  // Suppress unused variable warning
    char* result = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!result) return str;
    
    result[0] = '\0';
    char* pos = str;
    char* found;
    
    while ((found = strstr(pos, find)) != NULL) {
        size_t len = found - pos;
        strncat(result, pos, len);
        strcat(result, replace);
        pos = found + find_len;
    }
    strcat(result, pos);
    strcpy(str, result);
    free(result);
    return str;
}

char* utils_string_remove_char(char* str, char c) {
    if (!str) return NULL;
    char* read = str;
    char* write = str;
    while (*read) {
        if (*read != c) {
            *write++ = *read;
        }
        read++;
    }
    *write = '\0';
    return str;
}

int utils_string_contains(const char* str, const char* substr) {
    if (!str || !substr) return 0;
    return strstr(str, substr) != NULL;
}

int utils_string_starts_with(const char* str, const char* prefix) {
    if (!str || !prefix) return 0;
    size_t prefix_len = strlen(prefix);
    if (strlen(str) < prefix_len) return 0;
    return strncmp(str, prefix, prefix_len) == 0;
}

int utils_string_ends_with(const char* str, const char* suffix) {
    if (!str || !suffix) return 0;
    size_t str_len = strlen(str);
    size_t suffix_len = strlen(suffix);
    if (str_len < suffix_len) return 0;
    return strcmp(str + str_len - suffix_len, suffix) == 0;
}

int utils_string_equals(const char* str1, const char* str2) {
    if (!str1 || !str2) return str1 == str2;
    return strcmp(str1, str2) == 0;
}

int utils_string_equals_ignore_case(const char* str1, const char* str2) {
    if (!str1 || !str2) return str1 == str2;
    return strcasecmp(str1, str2) == 0;
}

char* utils_string_copy(const char* src) {
    if (!src) return NULL;
    size_t len = strlen(src) + 1;
    char* dest = (char*)malloc(len);
    if (!dest) return NULL;
    strcpy(dest, src);
    return dest;
}

char* utils_string_concat(const char* str1, const char* str2) {
    if (!str1 && !str2) return NULL;
    if (!str1) return utils_string_copy(str2);
    if (!str2) return utils_string_copy(str1);
    
    size_t len = strlen(str1) + strlen(str2) + 1;
    char* result = (char*)malloc(len);
    if (!result) return NULL;
    strcpy(result, str1);
    strcat(result, str2);
    return result;
}

char* utils_string_format(const char* format, ...) {
    if (!format) return NULL;
    
    va_list args;
    va_start(args, format);
    
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) {
        va_end(args);
        return NULL;
    }
    
    vsnprintf(buffer, UTILS_MAX_STRING_LENGTH, format, args);
    va_end(args);
    
    return buffer;
}

int utils_string_is_empty(const char* str) {
    return !str || *str == '\0';
}

int utils_string_is_whitespace(const char* str) {
    if (!str) return 1;
    while (*str) {
        if (!isspace((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int utils_string_length(const char* str) {
    return str ? (int)strlen(str) : 0;
}

// ============================================================================
// STRING VALIDATION
// ============================================================================

int utils_string_is_numeric(const char* str) {
    if (!str || *str == '\0') return 0;
    if (*str == '-' || *str == '+') str++;
    int has_digit = 0;
    while (*str) {
        if (!isdigit((unsigned char)*str) && *str != '.') return 0;
        if (isdigit((unsigned char)*str)) has_digit = 1;
        str++;
    }
    return has_digit;
}

int utils_string_is_alpha(const char* str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isalpha((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int utils_string_is_alphanumeric(const char* str) {
    if (!str || *str == '\0') return 0;
    while (*str) {
        if (!isalnum((unsigned char)*str)) return 0;
        str++;
    }
    return 1;
}

int utils_string_is_email(const char* str) {
    if (!str) return 0;
    const char* at = strchr(str, '@');
    if (!at || at == str) return 0;
    const char* dot = strrchr(at, '.');
    if (!dot || dot == at + 1 || dot == str + strlen(str) - 1) return 0;
    return 1;
}

int utils_string_is_phone(const char* str) {
    if (!str) return 0;
    int digit_count = 0;
    while (*str) {
        if (isdigit((unsigned char)*str)) {
            digit_count++;
        } else if (*str != '-' && *str != ' ' && *str != '(' && *str != ')' && *str != '+') {
            return 0;
        }
        str++;
    }
    return digit_count >= 7 && digit_count <= 15;
}

int utils_string_is_url(const char* str) {
    if (!str) return 0;
    return utils_string_starts_with(str, "http://") || 
           utils_string_starts_with(str, "https://") ||
           utils_string_starts_with(str, "ftp://");
}

int utils_string_is_valid_filename(const char* str) {
    if (!str || *str == '\0') return 0;
    const char* invalid_chars = "<>:\"|?*\\/";
    while (*str) {
        if (strchr(invalid_chars, *str) != NULL) return 0;
        str++;
    }
    return 1;
}

int utils_string_has_valid_length(const char* str, int min_length, int max_length) {
    if (!str) return min_length == 0;
    int len = (int)strlen(str);
    return len >= min_length && len <= max_length;
}

// ============================================================================
// NUMBER UTILITIES
// ============================================================================

int utils_number_is_in_range(int number, int min, int max) {
    return number >= min && number <= max;
}

int utils_number_is_positive(int number) {
    return number > 0;
}

int utils_number_is_negative(int number) {
    return number < 0;
}

int utils_number_is_zero(int number) {
    return number == 0;
}

float utils_number_round(float number, int decimal_places) {
    float multiplier = powf(10.0f, (float)decimal_places);
    return roundf(number * multiplier) / multiplier;
}

float utils_number_ceil(float number) {
    return ceilf(number);
}

float utils_number_floor(float number) {
    return floorf(number);
}

int utils_number_random(int min, int max) {
    if (min > max) {
        int temp = min;
        min = max;
        max = temp;
    }
    return min + (rand() % (max - min + 1));
}

float utils_number_random_float(float min, float max) {
    if (min > max) {
        float temp = min;
        min = max;
        max = temp;
    }
    float range = max - min;
    return min + ((float)rand() / (float)RAND_MAX) * range;
}

// ============================================================================
// DATE AND TIME UTILITIES
// ============================================================================

time_t utils_date_create(int year, int month, int day) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = 0;
    timeinfo.tm_min = 0;
    timeinfo.tm_sec = 0;
    return mktime(&timeinfo);
}

time_t utils_date_create_with_time(int year, int month, int day, int hour, int minute, int second) {
    struct tm timeinfo = {0};
    timeinfo.tm_year = year - 1900;
    timeinfo.tm_mon = month - 1;
    timeinfo.tm_mday = day;
    timeinfo.tm_hour = hour;
    timeinfo.tm_min = minute;
    timeinfo.tm_sec = second;
    return mktime(&timeinfo);
}

int utils_date_get_year(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_year + 1900 : 0;
}

int utils_date_get_month(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_mon + 1 : 0;
}

int utils_date_get_day(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_mday : 0;
}

int utils_date_get_hour(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_hour : 0;
}

int utils_date_get_minute(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_min : 0;
}

int utils_date_get_second(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_sec : 0;
}

int utils_date_get_day_of_week(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_wday : 0;
}

int utils_date_get_day_of_year(time_t date) {
    struct tm* timeinfo = localtime(&date);
    return timeinfo ? timeinfo->tm_yday + 1 : 0;
}

int utils_date_is_leap_year(int year) {
    return (year % 4 == 0 && year % 100 != 0) || (year % 400 == 0);
}

int utils_date_days_in_month(int year, int month) {
    int days[] = {31, 28, 31, 30, 31, 30, 31, 31, 30, 31, 30, 31};
    if (month == 2 && utils_date_is_leap_year(year)) return 29;
    if (month >= 1 && month <= 12) return days[month - 1];
    return 0;
}

int utils_date_days_between(time_t date1, time_t date2) {
    double diff = difftime(date2, date1);
    return (int)(diff / (24 * 60 * 60));
}

time_t utils_date_add_days(time_t date, int days) {
    return date + (days * 24 * 60 * 60);
}

time_t utils_date_add_months(time_t date, int months) {
    struct tm* timeinfo = localtime(&date);
    if (!timeinfo) return date;
    
    timeinfo->tm_mon += months;
    while (timeinfo->tm_mon > 11) {
        timeinfo->tm_mon -= 12;
        timeinfo->tm_year++;
    }
    while (timeinfo->tm_mon < 0) {
        timeinfo->tm_mon += 12;
        timeinfo->tm_year--;
    }
    
    int max_day = utils_date_days_in_month(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
    if (timeinfo->tm_mday > max_day) {
        timeinfo->tm_mday = max_day;
    }
    
    return mktime(timeinfo);
}

time_t utils_date_add_years(time_t date, int years) {
    struct tm* timeinfo = localtime(&date);
    if (!timeinfo) return date;
    
    timeinfo->tm_year += years;
    
    int max_day = utils_date_days_in_month(timeinfo->tm_year + 1900, timeinfo->tm_mon + 1);
    if (timeinfo->tm_mday > max_day) {
        timeinfo->tm_mday = max_day;
    }
    
    return mktime(timeinfo);
}

int utils_date_is_weekend(time_t date) {
    int day = utils_date_get_day_of_week(date);
    return day == 0 || day == 6;
}

int utils_date_is_holiday(time_t date) {
    // Simple implementation - can be extended
    int month = utils_date_get_month(date);
    int day = utils_date_get_day(date);
    // New Year's Day
    if (month == 1 && day == 1) return 1;
    // Christmas
    if (month == 12 && day == 25) return 1;
    return 0;
}

char* utils_date_format(time_t date, const char* format) {
    if (!format) return NULL;
    
    struct tm* timeinfo = localtime(&date);
    if (!timeinfo) return NULL;
    
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    
    strftime(buffer, UTILS_MAX_STRING_LENGTH, format, timeinfo);
    return buffer;
}

time_t utils_date_parse(const char* date_str, const char* format) {
    if (!date_str || !format) return 0;
    
    struct tm timeinfo = {0};
#if defined(_WIN32) || defined(_WIN64)
    // Windows doesn't have strptime, use sscanf for common formats
    if (strcmp(format, "%Y-%m-%d") == 0) {
        if (sscanf(date_str, "%d-%d-%d", &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday) == 3) {
            timeinfo.tm_year -= 1900;
            timeinfo.tm_mon -= 1;
        } else {
            return 0;
        }
    } else if (strcmp(format, "%Y-%m-%d %H:%M:%S") == 0) {
        if (sscanf(date_str, "%d-%d-%d %d:%d:%d", 
                   &timeinfo.tm_year, &timeinfo.tm_mon, &timeinfo.tm_mday,
                   &timeinfo.tm_hour, &timeinfo.tm_min, &timeinfo.tm_sec) == 6) {
            timeinfo.tm_year -= 1900;
            timeinfo.tm_mon -= 1;
        } else {
            return 0;
        }
    } else {
        return 0;
    }
#else
    if (strptime(date_str, format, &timeinfo) == NULL) return 0;
#endif
    return mktime(&timeinfo);
}

// ============================================================================
// FILE UTILITIES
// ============================================================================

int utils_file_exists(const char* filename) {
    if (!filename) return 0;
    return access(filename, 0) == 0;
}

int utils_file_is_readable(const char* filename) {
    if (!filename) return 0;
    return access(filename, 4) == 0;
}

int utils_file_is_writable(const char* filename) {
    if (!filename) return 0;
    return access(filename, 2) == 0;
}

int utils_file_is_executable(const char* filename) {
    if (!filename) return 0;
#if defined(_WIN32) || defined(_WIN64)
    return access(filename, 0) == 0;
#else
    return access(filename, 1) == 0;
#endif
}

long utils_file_get_size(const char* filename) {
    if (!filename) return -1;
    struct stat st;
    if (stat(filename, &st) == 0) {
        return (long)st.st_size;
    }
    return -1;
}

time_t utils_file_get_modified_time(const char* filename) {
    if (!filename) return 0;
    struct stat st;
    if (stat(filename, &st) == 0) {
        return st.st_mtime;
    }
    return 0;
}

time_t utils_file_get_created_time(const char* filename) {
    if (!filename) return 0;
    struct stat st;
    if (stat(filename, &st) == 0) {
#if defined(_WIN32) || defined(_WIN64)
        return st.st_ctime;
#else
        return st.st_ctime;
#endif
    }
    return 0;
}

int utils_file_copy(const char* source, const char* destination) {
    if (!source || !destination) return 0;
    
    FILE* src = fopen(source, "rb");
    if (!src) return 0;
    
    FILE* dst = fopen(destination, "wb");
    if (!dst) {
        fclose(src);
        return 0;
    }
    
    char buffer[UTILS_DEFAULT_BUFFER_SIZE];
    size_t bytes;
    while ((bytes = fread(buffer, 1, sizeof(buffer), src)) > 0) {
        if (fwrite(buffer, 1, bytes, dst) != bytes) {
            fclose(src);
            fclose(dst);
            return 0;
        }
    }
    
    fclose(src);
    fclose(dst);
    return 1;
}

int utils_file_move(const char* source, const char* destination) {
    if (!source || !destination) return 0;
    if (rename(source, destination) == 0) return 1;
    if (utils_file_copy(source, destination)) {
        return remove(source) == 0;
    }
    return 0;
}

int utils_file_delete(const char* filename) {
    if (!filename) return 0;
    return remove(filename) == 0;
}

int utils_file_create_directory(const char* path) {
    if (!path) return 0;
#if defined(_WIN32) || defined(_WIN64)
    return mkdir(path) == 0;
#else
    return mkdir(path, 0755) == 0;
#endif
}

int utils_file_directory_exists(const char* path) {
    if (!path) return 0;
    struct stat st;
    if (stat(path, &st) == 0) {
        return (st.st_mode & S_IFDIR) != 0;
    }
    return 0;
}

int utils_file_get_extension(const char* filename, char* extension, size_t size) {
    if (!filename || !extension || size == 0) return 0;
    
    const char* dot = strrchr(filename, '.');
    if (!dot || dot == filename) {
        extension[0] = '\0';
        return 0;
    }
    
    size_t len = strlen(dot + 1);
    if (len >= size) len = size - 1;
    strncpy(extension, dot + 1, len);
    extension[len] = '\0';
    return 1;
}

int utils_file_get_filename(const char* path, char* filename, size_t size) {
    if (!path || !filename || size == 0) return 0;
    
    const char* slash = strrchr(path, '/');
#if defined(_WIN32) || defined(_WIN64)
    const char* backslash = strrchr(path, '\\');
    if (backslash && (!slash || backslash > slash)) {
        slash = backslash;
    }
#endif
    const char* start = slash ? slash + 1 : path;
    
    size_t len = strlen(start);
    if (len >= size) len = size - 1;
    strncpy(filename, start, len);
    filename[len] = '\0';
    return 1;
}

int utils_file_get_directory(const char* path, char* directory, size_t size) {
    if (!path || !directory || size == 0) return 0;
    
    const char* slash = strrchr(path, '/');
#if defined(_WIN32) || defined(_WIN64)
    const char* backslash = strrchr(path, '\\');
    if (backslash && (!slash || backslash > slash)) {
        slash = backslash;
    }
#endif
    
    if (!slash) {
        directory[0] = '\0';
        return 0;
    }
    
    size_t len = slash - path;
    if (len >= size) len = size - 1;
    strncpy(directory, path, len);
    directory[len] = '\0';
    return 1;
}

char* utils_file_read_all(const char* filename) {
    if (!filename) return NULL;
    
    FILE* file = fopen(filename, "rb");
    if (!file) return NULL;
    
    fseek(file, 0, SEEK_END);
    long size = ftell(file);
    fseek(file, 0, SEEK_SET);
    
    if (size < 0) {
        fclose(file);
        return NULL;
    }
    
    char* buffer = (char*)malloc((size_t)size + 1);
    if (!buffer) {
        fclose(file);
        return NULL;
    }
    
    size_t read = fread(buffer, 1, (size_t)size, file);
    buffer[read] = '\0';
    fclose(file);
    
    return buffer;
}

int utils_file_write_all(const char* filename, const char* content) {
    if (!filename || !content) return 0;
    
    FILE* file = fopen(filename, "wb");
    if (!file) return 0;
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    return written == len;
}

int utils_file_append(const char* filename, const char* content) {
    if (!filename || !content) return 0;
    
    FILE* file = fopen(filename, "ab");
    if (!file) return 0;
    
    size_t len = strlen(content);
    size_t written = fwrite(content, 1, len, file);
    fclose(file);
    
    return written == len;
}

// ============================================================================
// PATH UTILITIES FOR DATA FILES
// ============================================================================

char* utils_get_executable_dir(char* buffer, size_t buffer_size) {
    if (!buffer || buffer_size == 0) return NULL;
    
#if defined(_WIN32) || defined(_WIN64)
    // Get the full path to the executable
    DWORD result = GetModuleFileNameA(NULL, buffer, (DWORD)buffer_size);
    if (result == 0 || result >= buffer_size) {
        // Failed or buffer too small
        return NULL;
    }
    
    // Find the last backslash to get directory
    char* last_slash = strrchr(buffer, '\\');
    if (last_slash) {
        *last_slash = '\0';  // Terminate at directory
    }
#else
    // Unix-like systems
    ssize_t result = readlink("/proc/self/exe", buffer, buffer_size - 1);
    if (result == -1) {
        return NULL;
    }
    buffer[result] = '\0';
    
    // Find the last slash to get directory
    char* last_slash = strrchr(buffer, '/');
    if (last_slash) {
        *last_slash = '\0';  // Terminate at directory
    }
#endif
    
    return buffer;
}

char* utils_get_data_file_path(const char* filename, char* buffer, size_t buffer_size) {
    if (!filename || !buffer || buffer_size == 0) return NULL;
    
    // Use absolute path for data directory
    const char* data_dir_base = "c:\\Users\\Karim erradi\\Documents\\c-project1\\data";
    
    // Check if filename already contains "data/" prefix
    const char* actual_filename = filename;
    if (strncmp(filename, "data/", 5) == 0) {
        actual_filename = filename + 5;  // Skip "data/" prefix
    } else if (strncmp(filename, "data\\", 5) == 0) {
        actual_filename = filename + 5;  // Skip "data\" prefix
    }
    
    // Construct the full path using absolute data directory
    int written;
    if (strlen(actual_filename) == 0) {
        // Just the data directory
        written = snprintf(buffer, buffer_size, "%s", data_dir_base);
    } else {
        written = snprintf(buffer, buffer_size, "%s\\%s", data_dir_base, actual_filename);
    }
    
    if (written > 0 && (size_t)written < buffer_size) {
        return buffer;
    }
    
    return NULL;
}

// ============================================================================
// MEMORY UTILITIES
// ============================================================================

void* utils_memory_allocate(size_t size) {
    return malloc(size);
}

void* utils_memory_reallocate(void* ptr, size_t size) {
    return realloc(ptr, size);
}

void utils_memory_free(void* ptr) {
    free(ptr);
}

void utils_memory_clear(void* ptr, size_t size) {
    if (ptr) {
        memset(ptr, 0, size);
    }
}

void utils_memory_copy(void* dest, const void* src, size_t size) {
    if (dest && src) {
        memcpy(dest, src, size);
    }
}

void utils_memory_move(void* dest, const void* src, size_t size) {
    if (dest && src) {
        memmove(dest, src, size);
    }
}

int utils_memory_compare(const void* ptr1, const void* ptr2, size_t size) {
    if (!ptr1 || !ptr2) return ptr1 == ptr2 ? 0 : 1;
    return memcmp(ptr1, ptr2, size);
}

void* utils_memory_find(const void* haystack, size_t haystack_size, const void* needle, size_t needle_size) {
    if (!haystack || !needle || needle_size == 0 || haystack_size < needle_size) {
        return NULL;
    }
    
    const unsigned char* h = (const unsigned char*)haystack;
    const unsigned char* n = (const unsigned char*)needle;
    
    for (size_t i = 0; i <= haystack_size - needle_size; i++) {
        if (memcmp(h + i, n, needle_size) == 0) {
            return (void*)(h + i);
        }
    }
    
    return NULL;
}

// ============================================================================
// ARRAY UTILITIES
// ============================================================================

static int compare_int(const void* a, const void* b) {
    int ia = *(const int*)a;
    int ib = *(const int*)b;
    return (ia > ib) - (ia < ib);
}

static int compare_float(const void* a, const void* b) {
    float fa = *(const float*)a;
    float fb = *(const float*)b;
    return (fa > fb) - (fa < fb);
}

static int compare_string(const void* a, const void* b) {
    const char* sa = *(const char**)a;
    const char* sb = *(const char**)b;
    if (!sa && !sb) return 0;
    if (!sa) return -1;
    if (!sb) return 1;
    return strcmp(sa, sb);
}

void utils_array_sort_int(int* array, int count) {
    if (array && count > 0) {
        qsort(array, (size_t)count, sizeof(int), compare_int);
    }
}

void utils_array_sort_float(float* array, int count) {
    if (array && count > 0) {
        qsort(array, (size_t)count, sizeof(float), compare_float);
    }
}

void utils_array_sort_string(char** array, int count) {
    if (array && count > 0) {
        qsort(array, (size_t)count, sizeof(char*), compare_string);
    }
}

int utils_array_find_int(const int* array, int count, int value) {
    if (!array) return -1;
    for (int i = 0; i < count; i++) {
        if (array[i] == value) return i;
    }
    return -1;
}

int utils_array_find_float(const float* array, int count, float value) {
    if (!array) return -1;
    const float epsilon = 0.0001f;
    for (int i = 0; i < count; i++) {
        if (fabsf(array[i] - value) < epsilon) return i;
    }
    return -1;
}

int utils_array_find_string(const char** array, int count, const char* value) {
    if (!array || !value) return -1;
    for (int i = 0; i < count; i++) {
        if (array[i] && strcmp(array[i], value) == 0) return i;
    }
    return -1;
}

int utils_array_remove_int(int* array, int* count, int value) {
    if (!array || !count) return 0;
    int index = utils_array_find_int(array, *count, value);
    if (index < 0) return 0;
    
    for (int i = index; i < *count - 1; i++) {
        array[i] = array[i + 1];
    }
    (*count)--;
    return 1;
}

int utils_array_remove_float(float* array, int* count, float value) {
    if (!array || !count) return 0;
    int index = utils_array_find_float(array, *count, value);
    if (index < 0) return 0;
    
    for (int i = index; i < *count - 1; i++) {
        array[i] = array[i + 1];
    }
    (*count)--;
    return 1;
}

int utils_array_remove_string(char** array, int* count, const char* value) {
    if (!array || !count || !value) return 0;
    int index = utils_array_find_string((const char**)array, *count, value);
    if (index < 0) return 0;
    
    free(array[index]);
    for (int i = index; i < *count - 1; i++) {
        array[i] = array[i + 1];
    }
    (*count)--;
    return 1;
}

int utils_array_add_int(int* array, int* count, int capacity, int value) {
    if (!array || !count || *count >= capacity) return 0;
    array[*count] = value;
    (*count)++;
    return 1;
}

int utils_array_add_float(float* array, int* count, int capacity, float value) {
    if (!array || !count || *count >= capacity) return 0;
    array[*count] = value;
    (*count)++;
    return 1;
}

int utils_array_add_string(char** array, int* count, int capacity, const char* value) {
    if (!array || !count || !value || *count >= capacity) return 0;
    array[*count] = utils_string_copy(value);
    if (!array[*count]) return 0;
    (*count)++;
    return 1;
}

// ============================================================================
// VALIDATION UTILITIES
// ============================================================================

int utils_validate_email(const char* email) {
    return utils_string_is_email(email);
}

int utils_validate_phone(const char* phone) {
    return utils_string_is_phone(phone);
}

int utils_validate_url(const char* url) {
    return utils_string_is_url(url);
}

int utils_validate_ip_address(const char* ip) {
    if (!ip) return 0;
    int parts[4];
    int count = sscanf(ip, "%d.%d.%d.%d", &parts[0], &parts[1], &parts[2], &parts[3]);
    if (count != 4) return 0;
    for (int i = 0; i < 4; i++) {
        if (parts[i] < 0 || parts[i] > 255) return 0;
    }
    return 1;
}

int utils_validate_credit_card(const char* card_number) {
    if (!card_number) return 0;
    
    // Luhn algorithm
    int len = (int)strlen(card_number);
    if (len < 13 || len > 19) return 0;
    
    int sum = 0;
    int alternate = 0;
    for (int i = len - 1; i >= 0; i--) {
        if (!isdigit((unsigned char)card_number[i])) return 0;
        int digit = card_number[i] - '0';
        if (alternate) {
            digit *= 2;
            if (digit > 9) digit -= 9;
        }
        sum += digit;
        alternate = !alternate;
    }
    return (sum % 10) == 0;
}

int utils_validate_postal_code(const char* postal_code, const char* country) {
    if (!postal_code) return 0;
    // Simple validation - can be extended for specific countries
    int len = (int)strlen(postal_code);
    return len >= 4 && len <= 10;
}

int utils_validate_age(int age) {
    return age >= 0 && age <= 150;
}

int utils_validate_percentage(float percentage) {
    return percentage >= 0.0f && percentage <= 100.0f;
}

int utils_validate_gpa(float gpa) {
    return gpa >= 0.0f && gpa <= 4.0f;
}

int utils_validate_id(int id) {
    return id > 0;
}

int utils_validate_year(int year) {
    return year >= 1900 && year <= 2100;
}

int utils_validate_month(int month) {
    return month >= 1 && month <= 12;
}

int utils_validate_day(int day, int month, int year) {
    if (day < 1 || day > 31) return 0;
    if (month < 1 || month > 12) return 0;
    int max_day = utils_date_days_in_month(year, month);
    return day <= max_day;
}

// ============================================================================
// FORMAT UTILITIES
// ============================================================================

char* utils_format_currency(float amount, const char* currency_code) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    
    const char* symbol = "$";
    if (currency_code) {
        if (strcmp(currency_code, "EUR") == 0) symbol = "€";
        else if (strcmp(currency_code, "GBP") == 0) symbol = "£";
        else if (strcmp(currency_code, "JPY") == 0) symbol = "¥";
    }
    
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%s%.2f", symbol, amount);
    return buffer;
}

char* utils_format_percentage(float value) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%.2f%%", value);
    return buffer;
}

char* utils_format_number_with_commas(long number) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    
    char temp[UTILS_MAX_STRING_LENGTH];
    snprintf(temp, sizeof(temp), "%ld", number);
    
    int len = (int)strlen(temp);
    int pos = 0;
    int comma_pos = len % 3;
    if (comma_pos == 0) comma_pos = 3;
    
    for (int i = 0; i < len; i++) {
        if (i > 0 && i == comma_pos) {
            buffer[pos++] = ',';
            comma_pos += 3;
        }
        buffer[pos++] = temp[i];
    }
    buffer[pos] = '\0';
    
    return buffer;
}

char* utils_format_file_size(long size) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    
    const char* units[] = {"B", "KB", "MB", "GB", "TB"};
    int unit_index = 0;
    double size_d = (double)size;
    
    while (size_d >= 1024.0 && unit_index < 4) {
        size_d /= 1024.0;
        unit_index++;
    }
    
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%.2f %s", size_d, units[unit_index]);
    return buffer;
}

char* utils_format_duration(int seconds) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    
    int hours = seconds / 3600;
    int minutes = (seconds % 3600) / 60;
    int secs = seconds % 60;
    
    if (hours > 0) {
        snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%d:%02d:%02d", hours, minutes, secs);
    } else {
        snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%d:%02d", minutes, secs);
    }
    
    return buffer;
}

char* utils_format_phone_number(const char* phone) {
    if (!phone) return NULL;
    // Simple formatting - can be enhanced
    return utils_string_copy(phone);
}

char* utils_format_date(time_t date) {
    return utils_date_format(date, "%Y-%m-%d");
}

char* utils_format_time(time_t time) {
    return utils_date_format(time, "%H:%M:%S");
}

char* utils_format_datetime(time_t datetime) {
    return utils_date_format(datetime, "%Y-%m-%d %H:%M:%S");
}

// ============================================================================
// CONVERSION UTILITIES
// ============================================================================

int utils_string_to_int(const char* str, int* result) {
    if (!str || !result) return 0;
    char* endptr;
    long value = strtol(str, &endptr, 10);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) return 0;
    if (value < INT_MIN || value > INT_MAX) return 0;
    *result = (int)value;
    return 1;
}

int utils_string_to_float(const char* str, float* result) {
    if (!str || !result) return 0;
    char* endptr;
    float value = strtof(str, &endptr);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) return 0;
    *result = value;
    return 1;
}

int utils_string_to_long(const char* str, long* result) {
    if (!str || !result) return 0;
    char* endptr;
    long value = strtol(str, &endptr, 10);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) return 0;
    *result = value;
    return 1;
}

int utils_string_to_double(const char* str, double* result) {
    if (!str || !result) return 0;
    char* endptr;
    double value = strtod(str, &endptr);
    if (*endptr != '\0' && !isspace((unsigned char)*endptr)) return 0;
    *result = value;
    return 1;
}

char* utils_int_to_string(int value) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%d", value);
    return buffer;
}

char* utils_float_to_string(float value) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%f", value);
    return buffer;
}

char* utils_long_to_string(long value) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%ld", value);
    return buffer;
}

char* utils_double_to_string(double value) {
    char* buffer = (char*)malloc(UTILS_MAX_STRING_LENGTH);
    if (!buffer) return NULL;
    snprintf(buffer, UTILS_MAX_STRING_LENGTH, "%lf", value);
    return buffer;
}

char* utils_bool_to_string(int value) {
    return utils_string_copy(value ? "true" : "false");
}

int utils_string_to_bool(const char* str, int* result) {
    if (!str || !result) return 0;
    if (utils_string_equals_ignore_case(str, "true") || 
        utils_string_equals_ignore_case(str, "1") ||
        utils_string_equals_ignore_case(str, "yes")) {
        *result = 1;
        return 1;
    }
    if (utils_string_equals_ignore_case(str, "false") || 
        utils_string_equals_ignore_case(str, "0") ||
        utils_string_equals_ignore_case(str, "no")) {
        *result = 0;
        return 1;
    }
    return 0;
}

// ============================================================================
// HASH UTILITIES
// ============================================================================

unsigned long utils_hash_string(const char* str) {
    if (!str) return 0;
    unsigned long hash = 5381;
    int c;
    while ((c = *str++)) {
        hash = ((hash << 5) + hash) + c;
    }
    return hash;
}

unsigned long utils_hash_int(int value) {
    return (unsigned long)value * 2654435761UL;
}

unsigned long utils_hash_float(float value) {
    union { float f; unsigned long i; } u;
    u.f = value;
    return utils_hash_int((int)u.i);
}

unsigned long utils_hash_combine(unsigned long hash1, unsigned long hash2) {
    return hash1 ^ (hash2 + 0x9e3779b9 + (hash1 << 6) + (hash1 >> 2));
}

// ============================================================================
// RANDOM UTILITIES
// ============================================================================

void utils_random_seed(unsigned int seed) {
    srand(seed);
}

int utils_random_int(int min, int max) {
    return utils_number_random(min, max);
}

float utils_random_float(float min, float max) {
    return utils_number_random_float(min, max);
}

double utils_random_double(double min, double max) {
    if (min > max) {
        double temp = min;
        min = max;
        max = temp;
    }
    double range = max - min;
    return min + ((double)rand() / (double)RAND_MAX) * range;
}

char utils_random_char(void) {
    return (char)('a' + (rand() % 26));
}

char* utils_random_string(int length) {
    if (length <= 0) return NULL;
    char* str = (char*)malloc((size_t)length + 1);
    if (!str) return NULL;
    
    for (int i = 0; i < length; i++) {
        str[i] = utils_random_char();
    }
    str[length] = '\0';
    return str;
}

int utils_random_choice(const int* choices, int count) {
    if (!choices || count <= 0) return 0;
    return choices[rand() % count];
}

// ============================================================================
// MATH UTILITIES
// ============================================================================

float utils_math_round(float value, int decimal_places) {
    return utils_number_round(value, decimal_places);
}

float utils_math_ceil(float value) {
    return ceilf(value);
}

float utils_math_floor(float value) {
    return floorf(value);
}

float utils_math_abs(float value) {
    return fabsf(value);
}

float utils_math_min(float a, float b) {
    return a < b ? a : b;
}

float utils_math_max(float a, float b) {
    return a > b ? a : b;
}

float utils_math_clamp(float value, float min, float max) {
    if (value < min) return min;
    if (value > max) return max;
    return value;
}

float utils_math_lerp(float a, float b, float t) {
    return a + (b - a) * t;
}

float utils_math_sqrt(float value) {
    return sqrtf(value);
}

float utils_math_pow(float base, float exponent) {
    return powf(base, exponent);
}

float utils_math_log(float value) {
    return logf(value);
}

float utils_math_sin(float angle) {
    return sinf(angle);
}

float utils_math_cos(float angle) {
    return cosf(angle);
}

float utils_math_tan(float angle) {
    return tanf(angle);
}

// ============================================================================
// ERROR HANDLING UTILITIES
// ============================================================================

const char* utils_error_to_string(UtilsResult result) {
    switch (result) {
        case UTILS_SUCCESS: return "Success";
        case UTILS_ERROR_INVALID_INPUT: return "Invalid input";
        case UTILS_ERROR_MEMORY_ALLOCATION: return "Memory allocation error";
        case UTILS_ERROR_FILE_OPERATION: return "File operation error";
        case UTILS_ERROR_INVALID_FORMAT: return "Invalid format";
        case UTILS_ERROR_OUT_OF_RANGE: return "Out of range";
        default: return "Unknown error";
    }
}

void utils_set_error_callback(void (*callback)(const char* error_message)) {
    error_callback = callback;
}

void utils_log_error(const char* function, const char* message) {
    if (error_callback) {
        char buffer[UTILS_MAX_ERROR_MESSAGE_LENGTH];
        snprintf(buffer, sizeof(buffer), "[ERROR] %s: %s", function ? function : "unknown", message ? message : "");
        error_callback(buffer);
    }
}

void utils_log_warning(const char* function, const char* message) {
    if (error_callback) {
        char buffer[UTILS_MAX_ERROR_MESSAGE_LENGTH];
        snprintf(buffer, sizeof(buffer), "[WARNING] %s: %s", function ? function : "unknown", message ? message : "");
        error_callback(buffer);
    }
}

void utils_log_info(const char* function, const char* message) {
    if (error_callback) {
        char buffer[UTILS_MAX_ERROR_MESSAGE_LENGTH];
        snprintf(buffer, sizeof(buffer), "[INFO] %s: %s", function ? function : "unknown", message ? message : "");
        error_callback(buffer);
    }
}

void utils_log_debug(const char* function, const char* message) {
    if (error_callback) {
        char buffer[UTILS_MAX_ERROR_MESSAGE_LENGTH];
        snprintf(buffer, sizeof(buffer), "[DEBUG] %s: %s", function ? function : "unknown", message ? message : "");
        error_callback(buffer);
    }
}

// ============================================================================
// SYSTEM UTILITIES
// ============================================================================

int utils_system_get_current_time(time_t* current_time) {
    if (!current_time) return 0;
    *current_time = time(NULL);
    return 1;
}

int utils_system_get_uptime(long* uptime) {
    if (!uptime) return 0;
    // Platform-specific implementation would go here
    *uptime = 0;
    return 0;
}

int utils_system_get_memory_usage(size_t* memory_usage) {
    if (!memory_usage) return 0;
    // Platform-specific implementation would go here
    *memory_usage = 0;
    return 0;
}

int utils_system_get_cpu_usage(float* cpu_usage) {
    if (!cpu_usage) return 0;
    // Platform-specific implementation would go here
    *cpu_usage = 0.0f;
    return 0;
}

int utils_system_get_disk_usage(const char* path, size_t* total, size_t* free) {
    if (!path || !total || !free) return 0;
    // Platform-specific implementation would go here
    *total = 0;
    *free = 0;
    return 0;
}

int utils_system_get_username(char* username, size_t size) {
    if (!username || size == 0) return 0;
#if defined(_WIN32) || defined(_WIN64)
    DWORD len = (DWORD)size;
    if (GetUserNameA(username, &len)) {
        return 1;
    }
#else
    struct passwd* pw = getpwuid(getuid());
    if (pw && pw->pw_name) {
        strncpy(username, pw->pw_name, size - 1);
        username[size - 1] = '\0';
        return 1;
    }
#endif
    return 0;
}

int utils_system_get_hostname(char* hostname, size_t size) {
    if (!hostname || size == 0) return 0;
#if defined(_WIN32) || defined(_WIN64)
    DWORD len = (DWORD)size;
    if (GetComputerNameA(hostname, &len)) {
        return 1;
    }
#else
    if (gethostname(hostname, size) == 0) {
        return 1;
    }
#endif
    return 0;
}

int utils_system_get_platform(char* platform, size_t size) {
    if (!platform || size == 0) return 0;
#if defined(_WIN32) || defined(_WIN64)
    strncpy(platform, "Windows", size - 1);
#elif defined(__linux__)
    strncpy(platform, "Linux", size - 1);
#elif defined(__APPLE__)
    strncpy(platform, "macOS", size - 1);
#else
    strncpy(platform, "Unknown", size - 1);
#endif
    platform[size - 1] = '\0';
    return 1;
}

