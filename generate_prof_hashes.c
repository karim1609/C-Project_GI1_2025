#include <stdio.h>
#include <string.h>
#include <stdint.h>

void auth_hash_password(const char *password, const char *salt, char *hash) {
    char combined[256];
    snprintf(combined, sizeof(combined), "%s%s", salt, password);
    
    uint64_t hash_value = 5381;
    for (int i = 0; combined[i] != '\0'; i++) {
        hash_value = (gi(hash_value << 5) + hash_value) + (unsigned char)combined[i];
    }
    
    sprintf(hash, "%016llx%016llx%016llx%016llx",
            hash_value,
            hash_value ^ 0xAAAAAAAA,
            hash_value ^ 0x55555555,
            hash_value ^ 0xFFFFFFFF);
}

int main() {
    const char *salts[] = {
        "profSalt111", "profSalt112", "profSalt113", "profSalt114", "profSalt115",
        "profSalt116", "profSalt117", "profSalt118", "profSalt119", "profSalt120",
        "profSalt121", "profSalt122", "profSalt123", "profSalt124", "profSalt125",
        "profSalt126", "profSalt127", "profSalt128", "profSalt129", "profSalt130",
        "profSalt131", "profSalt132", "profSalt133", "profSalt134", "profSalt135",
        "profSalt136", "profSalt137", "profSalt138", "profSalt139", "profSalt140",
        "profSalt141", "profSalt142", "profSalt143", "profSalt144", "profSalt145",
        "profSalt146", "profSalt147", "profSalt148", "profSalt201", "profSalt202",
        "profSalt203", "profSalt204", "profSalt205", "profSalt206", "profSalt207",
        "profSalt208", "profSalt209", "profSalt210", "profSalt211", "profSalt212",
        "profSalt213", "profSalt214", "profSalt215", "profSalt216", "profSalt217",
        "profSalt218", "profSalt11111", "profSalt11112", "profSalt11113", "profSalt11114",
        "profSalt11115", "profSalt11116", "profSalt11117", "profSalt11118", "profSalt11119",
        "profSalt11120", "profSalt11121", "profSalt11122", "profSalt11123", "profSalt11124",
        "profSalt11125", "profSalt11126", "profSalt11127", "profSalt11128", "profSalt11129",
        "profSalt11130", "profSalt11131", "profSalt11132", "profSalt11133", "profSalt11134",
        "profSalt11135", "profSalt11136", "profSalt11137", "profSalt20111", "profSalt20112",
        "profSalt20113", "profSalt20114", "profSalt20115", "profSalt20116", "profSalt20117",
        "profSalt20118", "profSalt20119", "profSalt20120", "profSalt20121"
    };
    
    const char *usernames[] = {
        "f.bouhafeer", "f.moradi", "m.addam", "h.bassidi", "a.elouahmani",
        "m.cherradi", "y.lakhlifi", "k.messaoudi", "m.amraoui", "s.elmanssouri",
        "r.khattab", "n.bouabid", "f.zahraoui", "a.bennis", "h.elmouden",
        "y.tazi", "k.benjelloun", "s.alami", "m.elgharbi", "f.senhaji",
        "a.lahlou", "h.benali", "y.messaoudi", "k.idrissi", "r.fassi",
        "n.elhassani", "m.azizi", "s.berrada", "f.ouazzani", "a.chraibi",
        "h.kadiri", "y.tahiri", "k.rifai", "m.skalli", "s.mouline",
        "r.amrani", "n.zniber", "f.bennani", "a.squalli", "h.sefrioui",
        "y.alaoui", "k.andaloussi", "m.tounsi", "s.filali", "f.nejjar",
        "a.kettani", "h.benabdellah", "y.elmansouri", "k.elmorabit", "r.elghazi",
        "n.elmahdi", "m.elbaz", "s.bouazza", "f.elhassani", "a.elkhattabi", 
        "h.elmansouri", "m.boulaich", "k.rachidi", "s.elmajdoub", "f.elboukhari",
        "a.elmalki", "h.elyaacoubi", "y.elbachiri", "k.elhilali", "r.elmoutawakkil",
        "n.elomari", "m.elfadili", "s.elguelzim", "f.eljabri", "a.elgueddari",
        "h.elmernissi", "y.elouardi", "k.elharrak", "m.elkhayati", "s.elmarkhi",
        "r.elmoutaouakil", "n.eloualidi", "f.elqorchi", "a.elrhazi", "h.elsaadi",
        "y.eltazi", "k.elyazidi", "m.habibi", "s.hamzaoui", "f.hachimi",
        "a.haddad", "h.hajji", "y.hamdaoui", "k.hannaoui", "r.harrak",
        "n.hassani", "m.himmi", "s.idrissi"
    };
    
    const char *emails[] = {
        "f.bouhafeer@university.edu", "f.moradi@university.edu", "m.addam@university.edu",
        "h.bassidi@university.edu", "a.elouahmani@university.edu", "m.cherradi@university.edu",
        "y.lakhlifi@university.edu", "k.messaoudi@university.edu", "m.amraoui@university.edu",
        "s.elmanssouri@university.edu", "r.khattab@university.edu", "n.bouabid@university.edu",
        "f.zahraoui@university.edu", "a.bennis@university.edu", "h.elmouden@university.edu",
        "y.tazi@university.edu", "k.benjelloun@university.edu", "s.alami@university.edu",
        "m.elgharbi@university.edu", "f.senhaji@university.edu", "a.lahlou@university.edu",
        "h.benali@university.edu", "y.messaoudi@university.edu", "k.idrissi@university.edu",
        "r.fassi@university.edu", "n.elhassani@university.edu", "m.azizi@university.edu",
        "s.berrada@university.edu", "f.ouazzani@university.edu", "a.chraibi@university.edu",
        "h.kadiri@university.edu", "y.tahiri@university.edu", "k.rifai@university.edu",
        "m.skalli@university.edu", "s.mouline@university.edu", "r.amrani@university.edu",
        "n.zniber@university.edu", "f.bennani@university.edu", "a.squalli@university.edu",
        "h.sefrioui@university.edu", "y.alaoui@university.edu", "k.andaloussi@university.edu",
        "m.tounsi@university.edu", "s.filali@university.edu", "f.nejjar@university.edu",
        "a.kettani@university.edu", "h.benabdellah@university.edu", "y.elmansouri@university.edu",
        "k.elmorabit@university.edu", "r.elghazi@university.edu", "n.elmahdi@university.edu",
        "m.elbaz@university.edu", "s.bouazza@university.edu", "f.elhassani@university.edu",
        "a.elkhattabi@university.edu", "h.elmansouri@university.edu", "m.boulaich@university.edu",
        "k.rachidi@university.edu", "s.elmajdoub@university.edu", "f.elboukhari@university.edu",
        "a.elmalki@university.edu", "h.elyaacoubi@university.edu", "y.elbachiri@university.edu",
        "k.elhilali@university.edu", "r.elmoutawakkil@university.edu", "n.elomari@university.edu",
        "m.elfadili@university.edu", "s.elguelzim@university.edu", "f.eljabri@university.edu",
        "a.elgueddari@university.edu", "h.elmernissi@university.edu", "y.elouardi@university.edu",
        "k.elharrak@university.edu", "m.elkhayati@university.edu", "s.elmarkhi@university.edu",
        "r.elmoutaouakil@university.edu", "n.eloualidi@university.edu", "f.elqorchi@university.edu",
        "a.elrhazi@university.edu", "h.elsaadi@university.edu", "y.eltazi@university.edu",
        "k.elyazidi@university.edu", "m.habibi@university.edu", "s.hamzaoui@university.edu",
        "f.hachimi@university.edu", "a.haddad@university.edu", "h.hajji@university.edu",
        "y.hamdaoui@university.edu", "k.hannaoui@university.edu", "r.harrak@university.edu",
        "n.hassani@university.edu", "m.himmi@university.edu", "s.idrissi@university.edu"
    };
    
    const char *password = "Professor123!";
    char hash[129];
    
    for (int i = 0; i < 93; i++) {
        auth_hash_password(password, salts[i], hash);
        printf("%s,%s,2,%s,%s,1733875200,1733875200,1\n", 
               usernames[i], emails[i], salts[i], hash);
    }
    
    return 0;
}
