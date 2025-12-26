#ifndef UTILS_H
#define UTILS_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <ctype.h>
#include <math.h>
#include "config.h"

// Utility result codes
typedef enum {
    UTILS_SUCCESS = 0,
    UTILS_ERROR_INVALID_INPUT = -1,
    UTILS_ERROR_MEMORY_ALLOCATION = -2,
    UTILS_ERROR_FILE_OPERATION = -3,
    UTILS_ERROR_INVALID_FORMAT = -4,
    UTILS_ERROR_OUT_OF_RANGE = -5
} UtilsResult;

// String utilities
char* utils_string_trim(char* str);
char* utils_string_trim_left(char* str);
char* utils_string_trim_right(char* str);
char* utils_string_to_lower(char* str);
char* utils_string_to_upper(char* str);
char* utils_string_replace(char* str, const char* find, const char* replace);
char* utils_string_remove_char(char* str, char c);
int utils_string_contains(const char* str, const char* substr);
int utils_string_starts_with(const char* str, const char* prefix);
int utils_string_ends_with(const char* str, const char* suffix);
int utils_string_equals(const char* str1, const char* str2);
int utils_string_equals_ignore_case(const char* str1, const char* str2);
char* utils_string_copy(const char* src);
char* utils_string_concat(const char* str1, const char* str2);
char* utils_string_format(const char* format, ...);
int utils_string_is_empty(const char* str);
int utils_string_is_whitespace(const char* str);
int utils_string_length(const char* str);

// String validation
int utils_string_is_numeric(const char* str);
int utils_string_is_alpha(const char* str);
int utils_string_is_alphanumeric(const char* str);
int utils_string_is_email(const char* str);
int utils_string_is_phone(const char* str);
int utils_string_is_url(const char* str);
int utils_string_is_valid_filename(const char* str);
int utils_string_has_valid_length(const char* str, int min_length, int max_length);

// Number utilities
int utils_number_is_in_range(int number, int min, int max);
int utils_number_is_positive(int number);
int utils_number_is_negative(int number);
int utils_number_is_zero(int number);
float utils_number_round(float number, int decimal_places);
float utils_number_ceil(float number);
float utils_number_floor(float number);
int utils_number_random(int min, int max);
float utils_number_random_float(float min, float max);

// Date and time utilities
time_t utils_date_create(int year, int month, int day);
time_t utils_date_create_with_time(int year, int month, int day, int hour, int minute, int second);
int utils_date_get_year(time_t date);
int utils_date_get_month(time_t date);
int utils_date_get_day(time_t date);
int utils_date_get_hour(time_t date);
int utils_date_get_minute(time_t date);
int utils_date_get_second(time_t date);
int utils_date_get_day_of_week(time_t date);
int utils_date_get_day_of_year(time_t date);
int utils_date_is_leap_year(int year);
int utils_date_days_in_month(int year, int month);
int utils_date_days_between(time_t date1, time_t date2);
time_t utils_date_add_days(time_t date, int days);
time_t utils_date_add_months(time_t date, int months);
time_t utils_date_add_years(time_t date, int years);
int utils_date_is_weekend(time_t date);
int utils_date_is_holiday(time_t date);
char* utils_date_format(time_t date, const char* format);
time_t utils_date_parse(const char* date_str, const char* format);

// File utilities
int utils_file_exists(const char* filename);
int utils_file_is_readable(const char* filename);
int utils_file_is_writable(const char* filename);
int utils_file_is_executable(const char* filename);
long utils_file_get_size(const char* filename);
time_t utils_file_get_modified_time(const char* filename);
time_t utils_file_get_created_time(const char* filename);
int utils_file_copy(const char* source, const char* destination);
int utils_file_move(const char* source, const char* destination);
int utils_file_delete(const char* filename);
int utils_file_create_directory(const char* path);
int utils_file_directory_exists(const char* path);
int utils_file_get_extension(const char* filename, char* extension, size_t size);
int utils_file_get_filename(const char* path, char* filename, size_t size);
int utils_file_get_directory(const char* path, char* directory, size_t size);
char* utils_file_read_all(const char* filename);
int utils_file_write_all(const char* filename, const char* content);
int utils_file_append(const char* filename, const char* content);

// Path utilities for data files
char* utils_get_data_file_path(const char* filename, char* buffer, size_t buffer_size);
char* utils_get_executable_dir(char* buffer, size_t buffer_size);

// Memory utilities
void* utils_memory_allocate(size_t size);
void* utils_memory_reallocate(void* ptr, size_t size);
void utils_memory_free(void* ptr);
void utils_memory_clear(void* ptr, size_t size);
void utils_memory_copy(void* dest, const void* src, size_t size);
void utils_memory_move(void* dest, const void* src, size_t size);
int utils_memory_compare(const void* ptr1, const void* ptr2, size_t size);
void* utils_memory_find(const void* haystack, size_t haystack_size, const void* needle, size_t needle_size);

// Array utilities
void utils_array_sort_int(int* array, int count);
void utils_array_sort_float(float* array, int count);
void utils_array_sort_string(char** array, int count);
int utils_array_find_int(const int* array, int count, int value);
int utils_array_find_float(const float* array, int count, float value);
int utils_array_find_string(const char** array, int count, const char* value);
int utils_array_remove_int(int* array, int* count, int value);
int utils_array_remove_float(float* array, int* count, float value);
int utils_array_remove_string(char** array, int* count, const char* value);
int utils_array_add_int(int* array, int* count, int capacity, int value);
int utils_array_add_float(float* array, int* count, int capacity, float value);
int utils_array_add_string(char** array, int* count, int capacity, const char* value);

// Validation utilities
int utils_validate_email(const char* email);
int utils_validate_phone(const char* phone);
int utils_validate_url(const char* url);
int utils_validate_ip_address(const char* ip);
int utils_validate_credit_card(const char* card_number);
int utils_validate_postal_code(const char* postal_code, const char* country);
int utils_validate_age(int age);
int utils_validate_gpa(float gpa);
int utils_validate_percentage(float percentage);
int utils_validate_id(int id);
int utils_validate_year(int year);
int utils_validate_month(int month);
int utils_validate_day(int day, int month, int year);

// Format utilities
char* utils_format_currency(float amount, const char* currency_code);
char* utils_format_percentage(float value);
char* utils_format_number_with_commas(long number);
char* utils_format_file_size(long size);
char* utils_format_duration(int seconds);
char* utils_format_phone_number(const char* phone);
char* utils_format_date(time_t date);
char* utils_format_time(time_t time);
char* utils_format_datetime(time_t datetime);

// Conversion utilities
int utils_string_to_int(const char* str, int* result);
int utils_string_to_float(const char* str, float* result);
int utils_string_to_long(const char* str, long* result);
int utils_string_to_double(const char* str, double* result);
char* utils_int_to_string(int value);
char* utils_float_to_string(float value);
char* utils_long_to_string(long value);
char* utils_double_to_string(double value);
char* utils_bool_to_string(int value);
int utils_string_to_bool(const char* str, int* result);

// Hash utilities
unsigned long utils_hash_string(const char* str);
unsigned long utils_hash_int(int value);
unsigned long utils_hash_float(float value);
unsigned long utils_hash_combine(unsigned long hash1, unsigned long hash2);

// Random utilities
void utils_random_seed(unsigned int seed);
int utils_random_int(int min, int max);
float utils_random_float(float min, float max);
double utils_random_double(double min, double max);
char utils_random_char(void);
char* utils_random_string(int length);
int utils_random_choice(const int* choices, int count);

// Math utilities
float utils_math_round(float value, int decimal_places);
float utils_math_ceil(float value);
float utils_math_floor(float value);
float utils_math_abs(float value);
float utils_math_min(float a, float b);
float utils_math_max(float a, float b);
float utils_math_clamp(float value, float min, float max);
float utils_math_lerp(float a, float b, float t);
float utils_math_sqrt(float value);
float utils_math_pow(float base, float exponent);
float utils_math_log(float value);
float utils_math_sin(float angle);
float utils_math_cos(float angle);
float utils_math_tan(float angle);

// Error handling utilities
const char* utils_error_to_string(UtilsResult result);
void utils_set_error_callback(void (*callback)(const char* error_message));
void utils_log_error(const char* function, const char* message);
void utils_log_warning(const char* function, const char* message);
void utils_log_info(const char* function, const char* message);
void utils_log_debug(const char* function, const char* message);

// System utilities
int utils_system_get_current_time(time_t* current_time);
int utils_system_get_uptime(long* uptime);
int utils_system_get_memory_usage(size_t* memory_usage);
int utils_system_get_cpu_usage(float* cpu_usage);
int utils_system_get_disk_usage(const char* path, size_t* total, size_t* free);
int utils_system_get_username(char* username, size_t size);
int utils_system_get_hostname(char* hostname, size_t size);
int utils_system_get_platform(char* platform, size_t size);

// Constants
#define UTILS_MAX_STRING_LENGTH 1024
#define UTILS_MAX_PATH_LENGTH 512
#define UTILS_MAX_FILENAME_LENGTH 256
#define UTILS_MAX_ERROR_MESSAGE_LENGTH 256
#define UTILS_DEFAULT_BUFFER_SIZE 1024
#define UTILS_MAX_DECIMAL_PLACES 6

// Date format constants
#define UTILS_DATE_FORMAT_ISO "YYYY-MM-DD"
#define UTILS_DATE_FORMAT_US "MM/DD/YYYY"
#define UTILS_DATE_FORMAT_EU "DD/MM/YYYY"
#define UTILS_DATE_FORMAT_FULL "YYYY-MM-DD HH:MM:SS"

// Currency codes
#define UTILS_CURRENCY_USD "USD"
#define UTILS_CURRENCY_EUR "EUR"
#define UTILS_CURRENCY_GBP "GBP"
#define UTILS_CURRENCY_JPY "JPY"
#define UTILS_CURRENCY_CAD "CAD"
#define UTILS_CURRENCY_AUD "AUD"

#endif // UTILS_H
