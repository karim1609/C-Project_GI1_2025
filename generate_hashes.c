#include <stdio.h>
#include <string.h>

void auth_hash_password(const char* password, const char* salt, char* hash) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);
    unsigned long long hash_value = 5381;
    for (int i = 0; combined[i] != '\0'; i++) {
        hash_value = ((hash_value << 5) + hash_value) + combined[i];
    }
    sprintf(hash, "%016llx%016llx%016llx%016llx", hash_value, hash_value ^ 0xAAAAAAAA, 
            hash_value ^ 0x55555555, hash_value ^ 0xFFFFFFFF);
}

int main() {
    char hash[129];
    const char* salts[] = {
        "profSalt111", "profSalt112", "profSalt113", "profSalt114", "profSalt115",
        "profSalt116", "profSalt117", "profSalt118", "profSalt119", "profSalt1110",
        "profSalt121", "profSalt122", "profSalt123", "profSalt124", "profSalt125",
        "profSalt126", "profSalt231", "profSalt232", "profSalt233", "profSalt234",
        "profSalt235", "profSalt236", "profSalt237", "profSalt241", "profSalt242",
        "profSalt243", "profSalt244", "profSalt245", "profSalt246", "profSalt351",
        "profSalt352", "profSalt353", "profSalt354", "profSalt355", "profSalt356",
        "profSalt10111", "profSalt10112", "profSalt10113", "profSalt10114", "profSalt10115",
        "profSalt10116", "profSalt10117", "profSalt10118", "profSalt10121", "profSalt10122",
        "profSalt10123", "profSalt10124", "profSalt10125", "profSalt10126", "profSalt10211",
        "profSalt10212", "profSalt10213", "profSalt10214", "profSalt10215", "profSalt10216",
        "profSalt10217", "profSalt10218", "profSalt10221", "profSalt10222", "profSalt10223",
        "profSalt10224", "profSalt10225", "profSalt10226", "profSalt10227", "profSalt10311",
        "profSalt10312", "profSalt10313", "profSalt10314", "profSalt10315", "profSalt10316",
        "profSalt10317", "profSalt20111", "profSalt20112", "profSalt20113", "profSalt20114",
        "profSalt20115", "profSalt20116", "profSalt20117", "profSalt20118", "profSalt20121",
        "profSalt20122", "profSalt20123", "profSalt20124", "profSalt20125", "profSalt20126",
        "profSalt20211", "profSalt20212", "profSalt20213", "profSalt20214", "profSalt20215",
        "profSalt20216", "profSalt20217", "profSalt20218"
    };
    
    for (int i = 0; i < 93; i++) {
        auth_hash_password("password", salts[i], hash);
        printf("%s\n", hash);
    }
    return 0;
}
