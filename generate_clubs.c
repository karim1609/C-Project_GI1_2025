#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/club.h"

int main() {
    FILE* file = fopen("data/clubs.txt", "w");
    if (!file) {
        printf("Error: Could not create data/clubs.txt\n");
        return 1;
    }

    Club clubs[7];
    memset(clubs, 0, sizeof(clubs));
    time_t now = time(NULL);
    
    // Club 1: Quran Club
    clubs[0].id = 1;
    strcpy(clubs[0].name, "Quran Club");
    strcpy(clubs[0].description, "A club dedicated to Quran study and memorization");
    strcpy(clubs[0].category, "Religious");
    clubs[0].president_id = 0;
    clubs[0].advisor_id = 0;
    clubs[0].member_count = 0;
    clubs[0].max_members = 30;
    clubs[0].founded_date = now;
    clubs[0].last_meeting = now;
    strcpy(clubs[0].meeting_day, "Monday");
    strcpy(clubs[0].meeting_time, "14:00");
    strcpy(clubs[0].meeting_location, "Room 101");
    clubs[0].budget = 500.0f;
    clubs[0].is_active = 1;
    
    // Club 2: Sport Club
    clubs[1].id = 2;
    strcpy(clubs[1].name, "Sport Club");
    strcpy(clubs[1].description, "Promotes physical fitness and sports activities");
    strcpy(clubs[1].category, "Sports");
    clubs[1].president_id = 0;
    clubs[1].advisor_id = 0;
    clubs[1].member_count = 0;
    clubs[1].max_members = 50;
    clubs[1].founded_date = now;
    clubs[1].last_meeting = now;
    strcpy(clubs[1].meeting_day, "Wednesday");
    strcpy(clubs[1].meeting_time, "15:00");
    strcpy(clubs[1].meeting_location, "Gymnasium");
    clubs[1].budget = 1000.0f;
    clubs[1].is_active = 1;
    
    // Club 3: Chess Club
    clubs[2].id = 3;
    strcpy(clubs[2].name, "Chess Club");
    strcpy(clubs[2].description, "Strategic thinking and chess tournaments");
    strcpy(clubs[2].category, "Academic");
    clubs[2].president_id = 0;
    clubs[2].advisor_id = 0;
    clubs[2].member_count = 0;
    clubs[2].max_members = 20;
    clubs[2].founded_date = now;
    clubs[2].last_meeting = now;
    strcpy(clubs[2].meeting_day, "Tuesday");
    strcpy(clubs[2].meeting_time, "16:00");
    strcpy(clubs[2].meeting_location, "Room 202");
    clubs[2].budget = 300.0f;
    clubs[2].is_active = 1;
    
    // Club 4: 01 Club
    clubs[3].id = 4;
    strcpy(clubs[3].name, "01 Club");
    strcpy(clubs[3].description, "Technology and binary enthusiasts");
    strcpy(clubs[3].category, "Technology");
    clubs[3].president_id = 0;
    clubs[3].advisor_id = 0;
    clubs[3].member_count = 0;
    clubs[3].max_members = 40;
    clubs[3].founded_date = now;
    clubs[3].last_meeting = now;
    strcpy(clubs[3].meeting_day, "Thursday");
    strcpy(clubs[3].meeting_time, "14:00");
    strcpy(clubs[3].meeting_location, "Lab 1");
    clubs[3].budget = 800.0f;
    clubs[3].is_active = 1;
    
    // Club 5: Tech Club
    clubs[4].id = 5;
    strcpy(clubs[4].name, "Tech Club");
    strcpy(clubs[4].description, "Programming and technology innovation");
    strcpy(clubs[4].category, "Technology");
    clubs[4].president_id = 0;
    clubs[4].advisor_id = 0;
    clubs[4].member_count = 0;
    clubs[4].max_members = 35;
    clubs[4].founded_date = now;
    clubs[4].last_meeting = now;
    strcpy(clubs[4].meeting_day, "Friday");
    strcpy(clubs[4].meeting_time, "15:00");
    strcpy(clubs[4].meeting_location, "Lab 2");
    clubs[4].budget = 750.0f;
    clubs[4].is_active = 1;
    
    // Club 6: CSS Club
    clubs[5].id = 6;
    strcpy(clubs[5].name, "CSS Club");
    strcpy(clubs[5].description, "Web design and styling workshops");
    strcpy(clubs[5].category, "Technology");
    clubs[5].president_id = 0;
    clubs[5].advisor_id = 0;
    clubs[5].member_count = 0;
    clubs[5].max_members = 25;
    clubs[5].founded_date = now;
    clubs[5].last_meeting = now;
    strcpy(clubs[5].meeting_day, "Monday");
    strcpy(clubs[5].meeting_time, "16:00");
    strcpy(clubs[5].meeting_location, "Lab 3");
    clubs[5].budget = 400.0f;
    clubs[5].is_active = 1;
    
    // Club 7: TGD Club
    clubs[6].id = 7;
    strcpy(clubs[6].name, "TGD Club");
    strcpy(clubs[6].description, "Graphic Design and Digital Arts");
    strcpy(clubs[6].category, "Arts");
    clubs[6].president_id = 0;
    clubs[6].advisor_id = 0;
    clubs[6].member_count = 0;
    clubs[6].max_members = 30;
    clubs[6].founded_date = now;
    clubs[6].last_meeting = now;
    strcpy(clubs[6].meeting_day, "Wednesday");
    strcpy(clubs[6].meeting_time, "14:00");
    strcpy(clubs[6].meeting_location, "Lab 4");
    clubs[6].budget = 600.0f;
    clubs[6].is_active = 1;

    // Write clubs in text format
    for (int i = 0; i < 7; i++) {
        Club* c = &clubs[i];
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
    printf("✓ Successfully generated 7 clubs in data/clubs.txt\n");
    return 0;
}
