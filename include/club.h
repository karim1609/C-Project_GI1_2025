#ifndef CLUB_H
#define CLUB_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// Club structure
typedef struct {
    int id;
    char name[MAX_CLUB_LENGTH];
    char description[500];
    char category[50];
    int president_id;
    int advisor_id;
    int member_count;
    int max_members;
    time_t founded_date;
    time_t last_meeting;
    char meeting_day[20];
    char meeting_time[20];
    char meeting_location[100];
    float budget;
    int is_active;
} Club;

// Club membership structure
typedef struct {
    int id;
    int student_id;
    int club_id;
    time_t join_date;
    char role[50];  // member, secretary, treasurer, president, etc.
    int is_active;
} ClubMembership;

// Club list structure
typedef struct {
    Club* clubs;
    int count;
    int capacity;
} ClubList;

// Membership list structure
typedef struct {
    ClubMembership* memberships;
    int count;
    int capacity;
} MembershipList;

// Principal Club management functions
ClubList* club_list_create(void);
void club_list_destroy(ClubList* list);
int club_list_add(ClubList* list, Club club);
int club_list_remove(ClubList* list, int club_id);
Club* club_list_find_by_id(ClubList* list, int club_id);
Club* club_list_find_by_name(ClubList* list, const char* name);
void club_list_display_all(ClubList* list);
void club_list_display_club(Club* club);

// Principal Membership management functions
MembershipList* membership_list_create(void);
void membership_list_destroy(MembershipList* list);
int membership_list_add(MembershipList* list, ClubMembership membership);
int membership_list_remove(MembershipList* list, int membership_id);
ClubMembership* membership_list_find_by_id(MembershipList* list, int membership_id);

// Principal Membership operations
int join_club(MembershipList* list, int student_id, int club_id, const char* role);
int leave_club(MembershipList* list, int student_id, int club_id);

// Principal File operations
int club_list_save_to_file(ClubList* list, const char* filename);
int club_list_load_from_file(ClubList* list, const char* filename);
int membership_list_save_to_file(MembershipList* list, const char* filename);
int membership_list_load_from_file(MembershipList* list, const char* filename);

// Principal Input/Output functions
Club club_input_new(void);
void club_input_edit(Club* club);
void club_display_summary(ClubList* list);

// Initialize predefined clubs
int club_list_init_predefined(ClubList* list);

// Club categories
#define CLUB_CATEGORY_ACADEMIC "Academic"
#define CLUB_CATEGORY_SPORTS "Sports"
#define CLUB_CATEGORY_ARTS "Arts"
#define CLUB_CATEGORY_SERVICE "Service"
#define CLUB_CATEGORY_CULTURAL "Cultural"
#define CLUB_CATEGORY_TECHNOLOGY "Technology"
#define CLUB_CATEGORY_SOCIAL "Social"
#define CLUB_CATEGORY_RELIGIOUS "Religious"

// Club roles
#define CLUB_ROLE_MEMBER "Member"
#define CLUB_ROLE_SECRETARY "Secretary"
#define CLUB_ROLE_TREASURER "Treasurer"
#define CLUB_ROLE_VICE_PRESIDENT "Vice President"
#define CLUB_ROLE_PRESIDENT "President"

#endif // CLUB_H
