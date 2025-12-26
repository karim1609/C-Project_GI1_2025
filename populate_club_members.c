#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "include/club.h"

int main() {
    srand(time(NULL));
    
    FILE* file = fopen("data/club_memberships.txt", "w");
    if (!file) {
        printf("Error: Could not create data/club_memberships.txt\n");
        return 1;
    }

    int membership_id = 1;
    time_t now = time(NULL);
    
    // Populate each club with random students
    // Club 1 (Quran Club): 15 random students
    for (int i = 0; i < 15; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 1, (long long)now, "Member", 1);
    }
    
    // Club 2 (Sport Club): 25 random students
    for (int i = 0; i < 25; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 2, (long long)now, "Member", 1);
    }
    
    // Club 3 (Chess Club): 10 random students
    for (int i = 0; i < 10; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 3, (long long)now, "Member", 1);
    }
    
    // Club 4 (01 Club): 20 random students
    for (int i = 0; i < 20; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 4, (long long)now, "Member", 1);
    }
    
    // Club 5 (Tech Club): 18 random students
    for (int i = 0; i < 18; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 5, (long long)now, "Member", 1);
    }
    
    // Club 6 (CSS Club): 12 random students
    for (int i = 0; i < 12; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 6, (long long)now, "Member", 1);
    }
    
    // Club 7 (TGD Club): 16 random students
    for (int i = 0; i < 16; i++) {
        int student_id = (rand() % 100) + 1;
        fprintf(file, "%d,%d,%d,%lld,%s,%d\n", 
            membership_id++, student_id, 7, (long long)now, "Member", 1);
    }
    
    fclose(file);
    printf("✓ Successfully generated %d club memberships\n", membership_id - 1);
    printf("  Quran Club: 15 members\n");
    printf("  Sport Club: 25 members\n");
    printf("  Chess Club: 10 members\n");
    printf("  01 Club: 20 members\n");
    printf("  Tech Club: 18 members\n");
    printf("  CSS Club: 12 members\n");
    printf("  TGD Club: 16 members\n");
    
    return 0;
}
