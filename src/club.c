#include "student.h"
#include "config.h"
#include "ui.h"
#include "file_manager.h"
#include "stats.h"
#include "auth.h"
#include "attendance.h"
#include "grade.h"
#include "club.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <sys/time.h>

ClubList* club_list_create(void){
    ClubList* list = (ClubList*)malloc(sizeof(ClubList));
    if(list == NULL){
        printf("error: failed to create club list\n");
        return NULL;
    }
    Club *clubs = (Club*)malloc(sizeof(Club)*MAX_CLUBS);
    if(clubs == NULL){
        printf("error: failed to allocate memory for clubs\n");
        free(list);
        return NULL;
    }
    list->clubs = clubs;
    list->count = 0;
    list->capacity = MAX_CLUBS;
    return list;    

}
void club_list_destroy(ClubList* list){
    if(list == NULL){
        return;
    }
    if(list->clubs != NULL){
        free(list->clubs);
    }
    free(list);
}

int club_list_add(ClubList* list, Club new_club){
    if(list == NULL || list->clubs == NULL){
        return 0;
    }
    if(list->count >= list->capacity){
        printf("error: club list is full\n");
        return 0;
    }
    list->clubs[list->count] = new_club;
    list->count++;
    return 1;
}
int club_list_remove(ClubList* list, int club_id){
    if(list == NULL || list->clubs == NULL){
        return 0;
    }
    for(int i = 0; i < list->count; i++){
        if(list->clubs[i].id == club_id){
            for(int j = i; j < list->count - 1; j++){
                list->clubs[j] = list->clubs[j + 1];
            }
            memset(&list->clubs[list->count - 1], 0, sizeof(Club));
            list->count--;
            return 1;
        }
    }
    return 0;
}
Club* club_list_find_by_id(ClubList* list, int club_id){
    if(list == NULL || list->clubs == NULL){
        printf("list is null\n");
        return NULL;
    }
    for(int i = 0; i < list->count; i++){
        if(list->clubs[i].id == club_id){
            return &list->clubs[i];
        }
    }
    printf("club with id %d not found\n", club_id);
    return NULL;
}
Club* club_list_find_by_name(ClubList* list, const char* name){
    if(list == NULL || list->clubs == NULL){
        printf("list is null\n");
        return NULL;
    }
    for(int i = 0; i < list->count; i++){
        if(strcmp(list->clubs[i].name, name) == 0){
            return &list->clubs[i];
        }
    }
    printf("club with name %s not found\n", name);
    return NULL;
} 
void club_list_display_all(ClubList* list){
    if(list == NULL || list->clubs == NULL){
        printf("list is null\n");
        return;
    }
  for(int i = 0; i < list->count; i++){
    printf("\nClub %d:\n", i + 1);
    printf("ID: %d\n", list->clubs[i].id);
    printf("Name: %s\n", list->clubs[i].name);
    printf("Description: %s\n", list->clubs[i].description);
    printf("Category: %s\n", list->clubs[i].category);
    printf("President ID: %d\n", list->clubs[i].president_id);
    printf("Advisor ID: %d\n", list->clubs[i].advisor_id);
    printf("Member Count: %d\n", list->clubs[i].member_count);
    printf("Meeting Day: %s\n", list->clubs[i].meeting_day);
    printf("Meeting Time: %s\n", list->clubs[i].meeting_time);
    printf("Meeting Location: %s\n", list->clubs[i].meeting_location);
    printf("Is Active: %d\n", list->clubs[i].is_active);
    printf("--------------------\n");
  }
}
void club_list_display_club(Club* club){
    if(club == NULL){
        printf("club is null");
        return;
    }
    printf("\nClub information:\n");
    printf("ID: %d\n", club->id);
    printf("Name: %s\n", club->name);
    printf("Description: %s\n", club->description);
    printf("Category: %s\n", club->category);
    printf("President ID: %d\n", club->president_id);
    printf("Advisor ID: %d\n", club->advisor_id);
    printf("Member Count: %d\n", club->member_count);
    printf("Meeting Day: %s\n", club->meeting_day);
    printf("Meeting Time: %s\n", club->meeting_time);
    printf("Meeting Location: %s\n", club->meeting_location);
    printf("Is Active: %d\n", club->is_active);
    printf("--------------------\n");
}






MembershipList* membership_list_create(void) {
    MembershipList* list = (MembershipList*)malloc(sizeof(MembershipList));
    if (list == NULL) {
        printf("error: could not allocate memory for membership list\n");
        return NULL;
    }
    list->capacity = 16;
    list->count = 0;
    list->memberships = (ClubMembership*)malloc(sizeof(ClubMembership) * list->capacity);
    if (list->memberships == NULL) {
        printf("error: could not allocate memory for memberships array\n");
        free(list);
        return NULL;
    }
    return list;
}

void membership_list_destroy(MembershipList* list) {
    if (list == NULL) {
        return;
    }
    if (list->memberships != NULL) {
        free(list->memberships);
    }
    free(list);
}

int membership_list_add(MembershipList* list, ClubMembership membership) {
    if (list == NULL || list->memberships == NULL) {
        printf("error: invalid arguments to membership_list_add\n");
        return 0;
    }
    
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        ClubMembership* new_memberships = (ClubMembership*)realloc(list->memberships, sizeof(ClubMembership) * new_capacity);
        if (new_memberships == NULL) {
            printf("error: could not allocate more memory for memberships\n");
            return 0;
        }
        list->memberships = new_memberships;
        list->capacity = new_capacity;
    }
    
    list->memberships[list->count++] = membership;
    return 1;
}

int membership_list_remove(MembershipList* list, int membership_id) {
    if (list == NULL || list->memberships == NULL) {
        printf("error: invalid arguments to membership_list_remove\n");
        return 0;
    }
    
    for (int i = 0; i < list->count; i++) {
        if (list->memberships[i].id == membership_id) {
            for (int j = i; j < list->count - 1; j++) {
                list->memberships[j] = list->memberships[j + 1];
            }
            list->count--;
            return 1;
        }
    }
    printf("error: membership with id %d not found\n", membership_id);
    return 0;
}

ClubMembership* membership_list_find_by_id(MembershipList* list, int membership_id) {
    if (list == NULL || list->memberships == NULL) {
        printf("error: invalid arguments to membership_list_find_by_id\n");
        return NULL;
    }
    
    for (int i = 0; i < list->count; i++) {
        if (list->memberships[i].id == membership_id) {
            return &list->memberships[i];
        }
    }
    return NULL;
}








int club_list_save_to_file(ClubList* list, const char* filename) {
    if (list == NULL || list->clubs == NULL || filename == NULL) {
        printf("error: invalid arguments to club_list_save_to_file\n");
        return 0;
    }
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "w");
    if (!file) {
        printf("error: could not open file %s for writing\n", full_path);
        return 0;
    }

    // Write all clubs in text format
    for (int i = 0; i < list->count; i++) {
        Club* c = &list->clubs[i];
        fprintf(file, "%d|%s|%s|%s|%d|%d|%d|%d|%lld|%lld|%s|%s|%s|%.2f|%d\n",
            c->id,
            c->name,
            c->description,
            c->category,
            c->president_id,
            c->advisor_id,
            c->member_count,
            c->max_members,
            (long long)c->founded_date,
            (long long)c->last_meeting,
            c->meeting_day,
            c->meeting_time,
            c->meeting_location,
            c->budget,
            c->is_active
        );
    }

    fclose(file);
    printf("[OK] Saved %d clubs to %s\n", list->count, full_path);
    return 1;
}
int club_list_load_from_file(ClubList* list, const char* filename){
    if(list == NULL || list->clubs == NULL || filename == NULL){
        printf("error: invalid arguments to club_list_load_from_file\n");
        return 0;
    }

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }

    FILE* file = fopen(full_path, "r");
    if (!file) {
        printf("error: could not open file %s for reading\n", full_path);
        return 0;
    }

    list->count = 0;
    char line[2048];
    
    // Read all clubs in text format
    while (fgets(line, sizeof(line), file)) {
        // Resize if needed
        if (list->count >= list->capacity) {
            int new_capacity = list->capacity * 2;
            Club* new_clubs = realloc(list->clubs, sizeof(Club) * new_capacity);
            if (!new_clubs) {
                printf("error: could not allocate more memory for clubs\n");
                fclose(file);
                return 0;
            }
            list->clubs = new_clubs;
            list->capacity = new_capacity;
        }

        Club* c = &list->clubs[list->count];
        memset(c, 0, sizeof(Club));
        
        long long founded, last_meet;
        int fields = sscanf(line, "%d|%99[^|]|%499[^|]|%49[^|]|%d|%d|%d|%d|%lld|%lld|%19[^|]|%19[^|]|%99[^|]|%f|%d",
            &c->id,
            c->name,
            c->description,
            c->category,
            &c->president_id,
            &c->advisor_id,
            &c->member_count,
            &c->max_members,
            &founded,
            &last_meet,
            c->meeting_day,
            c->meeting_time,
            c->meeting_location,
            &c->budget,
            &c->is_active
        );

        if (fields == 15) {
            c->founded_date = (time_t)founded;
            c->last_meeting = (time_t)last_meet;
            list->count++;
        } else {
            printf("[WARNING] Failed to parse line: %s", line);
        }
    }

    fclose(file);
    printf("[OK] Loaded %d clubs from %s\n", list->count, full_path);
    return 1;
}
int membership_list_save_to_file(MembershipList* list, const char* filename) {
    if (list == NULL || list->memberships == NULL || filename == NULL) {
        printf("error: invalid arguments to membership_list_save_to_file\n");
        return 0;
    }
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "w");
    if (file == NULL) {
        printf("error: could not open file %s for writing\n", full_path);
        return 0;
    }
    for (int i = 0; i < list->count; i++) {
        ClubMembership* mmbsh = &list->memberships[i];
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n",
            mmbsh->id,
            mmbsh->student_id,
            mmbsh->club_id,
            (long long)mmbsh->join_date,
            mmbsh->role,
            mmbsh->is_active
        );
    }
    fclose(file);
    return 1;
}
int membership_list_load_from_file(MembershipList* list, const char* filename){
    if (list == NULL || list->memberships == NULL || filename == NULL) {
        printf("error: invalid arguments to membership_list_load_from_file\n");
        return 0;
    }
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", filename);
        return 0;
    }
    
    FILE* file = fopen(full_path, "r");
    if (!file) {
        printf("warning: could not open file %s for reading (will start with empty list)\n", full_path);
        list->count = 0;
        return 1;
    }

    list->count = 0;
    char line[256];
    
    // Read memberships line by line
    while (fgets(line, sizeof(line), file)) {
        // Resize if needed
        if (list->count >= list->capacity) {
            int new_capacity = list->capacity * 2;
            ClubMembership* new_memberships = realloc(list->memberships, sizeof(ClubMembership) * new_capacity);
            if (!new_memberships) {
                printf("error: could not allocate more memory for memberships\n");
                fclose(file);
                return 0;
            }
            list->memberships = new_memberships;
            list->capacity = new_capacity;
        }

        ClubMembership* m = &list->memberships[list->count];
        memset(m, 0, sizeof(ClubMembership));
        
        long long join_date;
        int fields = sscanf(line, "%d,%d,%d,%lld,%49[^,],%d",
            &m->id,
            &m->student_id,
            &m->club_id,
            &join_date,
            m->role,
            &m->is_active
        );

        if (fields == 6) {
            m->join_date = (time_t)join_date;
            list->count++;
        } else {
            printf("[WARNING] Failed to parse membership line: %s", line);
        }
    }

    fclose(file);
    printf("[OK] Loaded %d memberships from %s\n", list->count, filename);
    return 1;
}

// Improved version, fixing many critical issues and aligning with your structures.

// Function to create a new club (asks user for input)
Club club_input_new(void) {
    Club c;
    printf("Les informations du club :\n");
    printf("Id: ");
    scanf("%d", &c.id);

    printf("Name: ");
    scanf(" %[^\n]", c.name);

    printf("Description: ");
    scanf(" %[^\n]", c.description);

    printf("Category: ");
    scanf(" %[^\n]", c.category);

    printf("President_id: ");
    scanf("%d", &c.president_id);

    printf("Advisor_id: ");
    scanf("%d", &c.advisor_id);

    printf("Member_count: ");
    scanf("%d", &c.member_count);

    // If founded_date is time_t in struct, user must enter year,month,day and convert
    int year, month, day;
    struct tm tm_date = {0};
    printf("Founded date (jour-mois-annee, e.g. 30-05-2024): ");
    scanf("%d-%d-%d", &day, &month, &year);
    tm_date.tm_mday = day;
    tm_date.tm_mon = month - 1; // struct tm months are 0-11
    tm_date.tm_year = year - 1900;
    c.founded_date = mktime(&tm_date);

    printf("Budget: ");
    scanf("%f", &c.budget);

    c.is_active = 1;
    return c;
}

// Function to edit a club's information
void club_input_edit(Club* club) {
    int choice;
    printf("\nQue voulez-vous modifier ?\n");
    printf(" 1 -> Id\n");
    printf(" 2 -> Nom\n");
    printf(" 3 -> Description\n");
    printf(" 4 -> Category\n");
    printf(" 5 -> President_id\n");
    printf(" 6 -> Advisor_id\n");
    printf(" 7 -> Member_count\n");
    printf(" 8 -> Founded_date\n");
    printf(" 9 -> Budget\n");
    printf("10 -> Is_active\n");
    printf(" 0 -> Annuler\n");
    printf("Choix: ");
    scanf("%d", &choice);

    switch (choice) {
    case 0:
        printf("Rien n'a été modifié.\n");
        break;
    case 1:
        printf("Nouveau Id: ");
        scanf("%d", &club->id);
        printf("Id modifié.\n");
        break;
    case 2:
        printf("Nouveau Name: ");
        scanf(" %[^\n]", club->name);
        printf("Name modifié.\n");
        break;
    case 3:
        printf("Nouveau Description: ");
        scanf(" %[^\n]", club->description);
        printf("Description modifiée.\n");
        break;
    case 4:
        printf("Nouveau Category: ");
        scanf(" %[^\n]", club->category);
        printf("Category modifiée.\n");
        break;
    case 5:
        printf("Nouveau President_Id: ");
        scanf("%d", &club->president_id);
        printf("President_Id modifié.\n");
        break;
    case 6:
        printf("Nouveau Advisor_Id: ");
        scanf("%d", &club->advisor_id);
        printf("Advisor_Id modifié.\n");
        break;
    case 7:
        printf("Nouveau Member_count: ");
        scanf("%d", &club->member_count);
        printf("Member_count modifié.\n");
        break;
    case 8: {
        int day, month, year;
        struct tm tm_date = {0};
        printf("Nouvelle Founded_date (jour-mois-annee): ");
        scanf("%d-%d-%d", &day, &month, &year);
        tm_date.tm_mday = day;
        tm_date.tm_mon = month - 1;
        tm_date.tm_year = year - 1900;
        club->founded_date = mktime(&tm_date);
        printf("Founded_date modifiée.\n");
        break;
    }
    case 9:
        printf("Nouveau Budget: ");
        scanf("%f", &club->budget);
        printf("Budget modifié.\n");
        break;
    case 10:
        printf("Nouvelle Situation (0: not active / 1: active): ");
        scanf("%d", &club->is_active);
        printf("Situation modifiée.\n");
        break;
    default:
        printf("Choix invalide.\n");
        break;
    }
}

// Function to display a summary of all clubs
void club_display_summary(ClubList* list) {
    if (list == NULL || list->clubs == NULL) {
        printf("Error: Invalid club list\n");
        return;
    }
    
    if (list->count == 0) {
        printf("No clubs available.\n");
        return;
    }
    
    printf("\n=== CLUB SUMMARY ===\n");
    printf("%-5s %-30s %-20s %-8s %-8s %-10s\n", 
           "ID", "Name", "Category", "Members", "Max", "Status");
    printf("----------------------------------------------------------------------------\n");
    
    for (int i = 0; i < list->count; i++) {
        Club* club = &list->clubs[i];
        printf("%-5d %-30s %-20s %-8d %-8d %-10s\n",
               club->id,
               club->name,
               club->category,
               club->member_count,
               club->max_members,
               club->is_active ? "Active" : "Inactive");
    }
    
    printf("----------------------------------------------------------------------------\n");
    printf("Total clubs: %d\n\n", list->count);
}

// Function for a student to join a club (creates a new membership)
int join_club(MembershipList* list, int student_id, int club_id, const char* role) {
    if (!list || !role) return 0;

    ClubMembership mmbsh;
    mmbsh.id = 0; // Should be assigned properly elsewhere (e.g., unique id system)
    mmbsh.student_id = student_id;
    mmbsh.club_id = club_id;
    strncpy(mmbsh.role, role, sizeof(mmbsh.role) - 1);
    mmbsh.role[sizeof(mmbsh.role) - 1] = '\0';
    mmbsh.is_active = 1;

    int day, month, year;
    struct tm tm_date = {0};
    printf("Date d'adhesion (jour-mois-annee): ");
    scanf("%d-%d-%d", &day, &month, &year);
    tm_date.tm_mday = day;
    tm_date.tm_mon = month - 1;
    tm_date.tm_year = year - 1900;
    mmbsh.join_date = mktime(&tm_date);

    int result = membership_list_add(list, mmbsh);
    return result;
}

// Function for a student to leave a club (removes membership by membership id)
// Note: This implementation uses membership_id, but header declares student_id and club_id
// Keeping this as a helper function, the header function should call membership_list functions
int leave_club_by_membership_id(MembershipList* list, int membership_id) {
    if (list == NULL)
        return 0;
    return membership_list_remove(list, membership_id);
}

// Implementation matching header signature
int leave_club(MembershipList* list, int student_id, int club_id) {
    if (list == NULL) return 0;
    // Find membership by student_id and club_id, then remove
    for (int i = 0; i < list->count; i++) {
        if (list->memberships[i].student_id == student_id && 
            list->memberships[i].club_id == club_id) {
            return membership_list_remove(list, list->memberships[i].id);
        }
    }
    return 0;
}

// Initialize predefined clubs: quran, sport, chess, 01, tech, css, tgd
int club_list_init_predefined(ClubList* list) {
    if (list == NULL) return 0;
    
    time_t now = time(NULL);
    Club predefined_clubs[] = {
        {1, "Quran Club", "Islamic studies and Quran recitation", CLUB_CATEGORY_RELIGIOUS, 
         0, 0, 0, 50, now, 0, "Friday", "14:00", "Room 101", 0.0, 1},
        {2, "Sport Club", "Physical activities and sports competitions", CLUB_CATEGORY_SPORTS, 
         0, 0, 0, 100, now, 0, "Wednesday", "16:00", "Gymnasium", 0.0, 1},
        {3, "Chess Club", "Strategic thinking and chess tournaments", CLUB_CATEGORY_ACADEMIC, 
         0, 0, 0, 30, now, 0, "Tuesday", "15:00", "Room 205", 0.0, 1},
        {4, "01 Club", "Binary and computer science fundamentals", CLUB_CATEGORY_TECHNOLOGY, 
         0, 0, 0, 40, now, 0, "Thursday", "14:00", "Lab 1", 0.0, 1},
        {5, "Tech Club", "Technology innovation and programming", CLUB_CATEGORY_TECHNOLOGY, 
         0, 0, 0, 60, now, 0, "Monday", "15:00", "Lab 2", 0.0, 1},
        {6, "CSS Club", "Web design and creative styling", CLUB_CATEGORY_TECHNOLOGY, 
         0, 0, 0, 35, now, 0, "Wednesday", "14:00", "Lab 3", 0.0, 1},
        {7, "TGD Club", "Team game development and collaboration", CLUB_CATEGORY_TECHNOLOGY, 
         0, 0, 0, 45, now, 0, "Friday", "16:00", "Room 305", 0.0, 1}
    };
    
    // Only add clubs that don't already exist
    int added = 0;
    for (int i = 0; i < 7; i++) {
        Club* existing = club_list_find_by_name(list, predefined_clubs[i].name);
        if (existing == NULL) {
            if (club_list_add(list, predefined_clubs[i])) {
                added++;
            }
        }
    }
    
    return added;
}

