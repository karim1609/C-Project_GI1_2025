#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "attendance.h"
#include "config.h"
#include "ui.h"
#include "file_manager.h"
#include "utils.h"
#include "stats.h"
#include "auth.h"
#include "club.h"



AttendanceList* attendance_list_create() {
    AttendanceList* list = (AttendanceList*)malloc(sizeof(AttendanceList));
    if (!list) return NULL;

    list->count = 0;
    list->capacity = 40;
    list->records = (AttendanceRecord*)malloc(sizeof(AttendanceRecord) * list->capacity);
    if (!list->records) {
        free(list);
        return NULL;
    }
    return list;
}

void attendance_list_destroy(AttendanceList* list) {
    if (list == NULL)
        return;
    if (list->records) {
        free(list->records);
        list->records = NULL;
    }
    list->count = 0;
    list->capacity = 0;
    free(list);
}

int attendance_list_add(AttendanceList *list, AttendanceRecord record) {
    if (list == NULL)
        return 0;

    if (list->count >= list->capacity)
        return 0;

    list->records[list->count++] = record;
    return 1;
}

int attendance_list_remove(AttendanceList* list, int record_id) {
    if (list == NULL || list->count == 0)
        return 0;

    for (int i = 0; i < list->count; i++) {
        if (list->records[i].id == record_id) {
            for (int j = i; j < list->count - 1; j++) {
                list->records[j] = list->records[j + 1];
            }
            list->count--;
            return 1;
        }
    }
    return 0;
}

AttendanceRecord* attendance_list_find_by_id(AttendanceList* list, int record_id) {
    if (list == NULL)
        return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].id == record_id) {
            return &(list->records[i]);
        }
    }
    return NULL;
}

AttendanceRecord* attendance_list_find_by_student_date(AttendanceList* list, int student_id, time_t date) {
    if (list == NULL)
        return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].student_id == student_id) {
            struct tm tm1 = *localtime(&date);
            struct tm tm2 = *localtime(&(list->records[i].recorded_time));

            if (tm1.tm_year == tm2.tm_year &&
                tm1.tm_mon == tm2.tm_mon &&
                tm1.tm_mday == tm2.tm_mday) {
                return &(list->records[i]);
            }
        }
    }
    return NULL;
}

AttendanceRecord* attendance_list_find_by_course_date(AttendanceList* list, int course_id, time_t date) {
    if (list == NULL)
        return NULL;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].course_id == course_id) {
            struct tm tm1 = *localtime(&date);
            struct tm tm2 = *localtime(&(list->records[i].date));

            if (tm1.tm_year == tm2.tm_year &&
                tm1.tm_mon == tm2.tm_mon &&
                tm1.tm_mday == tm2.tm_mday) {
                return &(list->records[i]);
            }
        }
    }
    return NULL;
}

int mark_attendance(AttendanceList* list, int student_id, int course_id, time_t date, int status, int teacher_id){
    if(list == NULL){
        printf("erreur la list est nulle ");
        return -1;
    }
    
    // Check if we need to reallocate
    if(list->count >= list->capacity){
        int new_capacity = (list->capacity == 0) ? 10 : list->capacity * 2;
        AttendanceRecord* newblock = (AttendanceRecord *)realloc(list->records, new_capacity * sizeof(AttendanceRecord));
        if(!newblock){
            printf("erreur de reallocation ");
            return -1;
        }
        list->records = newblock;
        list->capacity = new_capacity;
    }
    
    AttendanceRecord newrecord;
    
    // Find the next available ID
    int max_id = 0;
    for(int i = 0; i < list->count; i++){
        if(list->records[i].id > max_id){
            max_id = list->records[i].id;
        }
    }
    
    newrecord.id = max_id + 1;
    newrecord.student_id = student_id;
    newrecord.course_id = course_id;
    newrecord.date = date;
    newrecord.status = status;
    newrecord.teacher_id = teacher_id;
    strcpy(newrecord.reason, ""); 
    newrecord.recorded_time = time(NULL); // temps exacte d'enregistrement 

    list->records[list->count] = newrecord;
    list->count++;

    return 0;
}
int update_attendance(AttendanceList* list, int record_id, int new_status, const char* reason){
    if(list == NULL){
        printf("erreur la liste est nulle !!");
        return -1;
    }
    
    for(int i = 0 ; i < list->count ; i++){
        if(list->records[i].id == record_id){
            list->records[i].status = new_status ;
            if(reason != NULL){
            strncpy(list->records[i].reason , reason , 199);
            list->records[i].reason[199] = '\0';
            }
            return 0;
        }
    }
   return -1 ; //record non trouver 
}

int get_attendance_for_date(AttendanceList* list,int course_id,time_t date,AttendanceRecord** records,int* count){

    if (list == NULL || records == NULL || count == NULL) {
        return -1; 
    }

    *count = 0;     
    *records = NULL; 
    

   
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].course_id == course_id && list->records[i].date == date){
            (*count)++;
        }
    }


    if (*count == 0) {
        return 0; 
    }

  
    *records = malloc(sizeof(AttendanceRecord) * (*count));
    if (*records == NULL) {
        return -1; 
    }

    int index = 0;
    for (int i = 0; i < list->count; i++) {
        if (list->records[i].course_id == course_id &&
            list->records[i].date == date)
        {
            (*records)[index] = list->records[i];
            index++;
        }
    }

    return 0; 
}

void attendance_list_display_all(AttendanceList* list) {
    if (list == NULL || list->records == NULL) {
        printf("Error: Invalid attendance list\n");
        return;
    }
    
    if (list->count == 0) {
        printf("Attendance list is empty\n");
        return;
    }
    
    printf("\n=== ALL ATTENDANCE RECORDS ===\n");
    printf("Total records: %d\n\n", list->count);
    
    for (int i = 0; i < list->count; i++) {
        printf("Record %d:\n", i + 1);
        attendance_display_record(&(list->records[i]));
        printf("--------------------\n");
    }
}

void attendance_display_record(AttendanceRecord* record) {
    if (record == NULL) {
        printf("Error: Invalid attendance record\n");
        return;
    }
    
    char date_str[64];
    char recorded_time_str[64];
    struct tm* date_tm = localtime(&(record->date));
    struct tm* recorded_tm = localtime(&(record->recorded_time));
    
    strftime(date_str, sizeof(date_str), "%Y-%m-%d", date_tm);
    strftime(recorded_time_str, sizeof(recorded_time_str), "%Y-%m-%d %H:%M:%S", recorded_tm);
    
    const char* status_str;
    switch (record->status) {
        case ATTENDANCE_ABSENT:
            status_str = "Absent";
            break;
        case ATTENDANCE_PRESENT:
            status_str = "Present";
            break;
        case ATTENDANCE_LATE:
            status_str = "Late";
            break;
        case ATTENDANCE_EXCUSED:
            status_str = "Excused";
            break;
        default:
            status_str = "Unknown";
            break;
    }
    
    printf("  ID: %d\n", record->id);
    printf("  Student ID: %d\n", record->student_id);
    printf("  Course ID: %d\n", record->course_id);
    printf("  Date: %s\n", date_str);
    printf("  Status: %s (%d)\n", status_str, record->status);
    printf("  Teacher ID: %d\n", record->teacher_id);
    printf("  Recorded Time: %s\n", recorded_time_str);
    if (strlen(record->reason) > 0) {
        printf("  Reason: %s\n", record->reason);
    }
}

int attendance_list_save_to_file(AttendanceList* list, const char* filename) {
    if (!list || !filename)
        return -1;

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return -1;
    }

    FILE* fp = fopen(full_path, "w");
    if (!fp) {
        printf("[ERROR] Failed to open attendance file for writing: %s\n", full_path);
        return -1;
    }

    for (int i = 0; i < list->count; i++) {
        AttendanceRecord* rec = &list->records[i];
        fprintf(fp, "%d,%d,%d,%lld,%d\n",
                rec->student_id,
                rec->course_id,
                rec->status,
                (long long)rec->date,
                rec->teacher_id);
    }

    fclose(fp);
    printf("[OK] Saved %d attendance records to %s\n", list->count, full_path);
    return 1;
}

int attendance_list_load_from_file(AttendanceList* list, const char* filename){
    if (!list || !filename)
        return -1;

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return -1;
    }

    FILE* fp = fopen(full_path, "r");
    if (!fp) {
        printf("[WARNING] Failed to open attendance file for reading: %s (might not exist yet)\n", full_path);
        return -1;
    }

    list->count = 0;
    char line[256];
    while (fgets(line, sizeof(line), fp)) {
        if (list->count >= list->capacity) {
            int new_cap = list->capacity * 2;
            AttendanceRecord* new_records = (AttendanceRecord*)realloc(list->records,
                                                sizeof(AttendanceRecord) * new_cap);
            if (!new_records) {
                fclose(fp);
                return -1;
            }
            list->records = new_records;
            list->capacity = new_cap;
        }

        AttendanceRecord rec;
        long long date_tmp;
        if (sscanf(line, "%d,%d,%d,%lld,%d",
                   &rec.student_id,
                   &rec.course_id,
                   &rec.status,
                   &date_tmp,
                   &rec.teacher_id) == 5) {
            rec.date = (time_t)date_tmp;
            list->records[list->count++] = rec;
        }
    }

    fclose(fp);
    printf("[OK] Loaded %d attendance records from %s\n", list->count, filename);
    return 1;
}
