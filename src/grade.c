#include "student.h"
#include "config.h"
#include "ui.h"
#include "file_manager.h"
// #include "report.h"  // Commented out - file doesn't exist
#include "stats.h"
#include "auth.h"
#include "attendance.h"
#include "grade.h"
#include "utils.h"
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

// All type definitions are in grade.h - no duplicate definitions needed

Examen* creer_examen() {
    Examen* E = (Examen*)malloc(sizeof(Examen));
    if (E == NULL) {
        printf("Error: memory allocation failed!\n");
        return NULL;
    }

    printf("Exam ID: ");
    scanf("%d", &E->id_examen);

    printf("Module ID: ");
    scanf("%d", &E->id_module);

    getchar();
    printf("Module Name: ");
    scanf("%[^\n]", E->nom_module);

    getchar();

    printf("Duration: ");
    scanf("%d", &E->duree);

    struct tm tm_date = {0};

    printf("Date (day month year): ");
    scanf("%d %d %d", &tm_date.tm_mday, &tm_date.tm_mon, &tm_date.tm_year);
    tm_date.tm_mon -= 1;
    tm_date.tm_year -= 1900;

    printf("Time (h m s): ");
    scanf("%d %d %d", &tm_date.tm_hour, &tm_date.tm_min, &tm_date.tm_sec);

    E->date_examen = mktime(&tm_date);

    if (E->date_examen == -1) {
        printf("Error: invalid date!\n");
        free(E);
        return NULL;
    }

    return E;
}
void afficher_examen(Examen *E) {
    struct tm *info = localtime(&E->date_examen);

    printf("| %-8d | %-8d | %-18s | %02d/%02d/%04d | %02d:%02d:%02d | %-8d |\n",
           E->id_examen,
           E->id_module,
           E->nom_module,
           info->tm_mday,
           info->tm_mon + 1,
           info->tm_year + 1900,
           info->tm_hour,
           info->tm_min,
           info->tm_sec,
           E->duree);
}
int examen_ajouter(liste_examen* liste, Examen *ex){
   if(liste->count<liste->capacity){
    (liste)->exam[liste->count++] =*ex;
    return(1);}

return(0);
}
int examen_supprimer_par_id(liste_examen* liste,int id_examen){
if(liste->count!=0){
        for(int i=0;i<liste->count;i++){
            if((liste)->exam[i].id_examen==id_examen){
                 for(int j=i;j<liste->count-1;j++)
                   (liste)->exam[j]=(liste)->exam[j+1];
                 liste->count--;
                 return(1);
            }
        }

    }

return(0);
}
int examen_supprimer_par_nom(liste_examen* liste,char* nom_examen){
if(liste->count!=0){
        for(int i=0;i<liste->count;i++){
            if(strcmp((liste)->exam[i].nom_module,nom_examen)==0){
                 for(int j=i;j<liste->count-1;j++)
                   (liste)->exam[j]=(liste)->exam[j+1];
                 liste->count--;
                 return(1);
            }
        }

    }

return(0);
}

void afficher_liste_examens(liste_examen *liste) {
    if (liste == NULL || liste->exam == NULL) {
        printf("Error: Invalid list\n\n");
        return;
    }

    if (liste->count == 0) {
        printf("No exams to display.\n");
        return;
    }
    printf("+-----------+-----------+-------------------------+------------+----------+-------------+----------+\n");
    printf("| EXAM ID   | MODULE ID | MODULE NAME             | DATE       | TIME     | DURATION    |\n");
    printf("+-----------+-----------+-------------------------+------------+----------+-------------+----------+\n");

    for (int i = 0; i < liste->count; i++) {
            printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
        Examen * E = &liste->exam[i];
        struct tm *info = localtime(&E->date_examen);
        printf("| %-8d | %-8d | %-18s | %02d/%02d/%04d | %02d:%02d:%02d | %-8d |\n",
           E->id_examen,
           E->id_module,
           E->nom_module,
           info->tm_mday,
           info->tm_mon + 1,
           info->tm_year + 1900,
           info->tm_hour,
           info->tm_min,
           info->tm_sec,
           E->duree);
       printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");

    }  printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");}

liste_examen* cree_liste_examen(){
    liste_examen * liste=(liste_examen*)malloc(sizeof(liste_examen));
    liste->capacity=3000;
    liste->count=0;
    liste->exam=(Examen*)malloc(liste->capacity*sizeof(Examen));
    strcpy(liste->filename,"liste_des_examen.txt");
    return(liste);
}
Examen* chercher_examen_par_id(liste_examen* liste,int id){
for(int i=0;i<liste->count;i++){
    if(liste->exam[i].id_examen==id)
        return(&liste->exam[i]);
}
return(NULL);
}
Examen* chercher_examen_par_nom(liste_examen* liste,char* nom){
for(int i=0;i<liste->count;i++){
    if(strcmp(liste->exam[i].nom_module,nom)==0)
        return(&liste->exam[i]);
}
return(NULL);
}
void modidier_examen(liste_examen liste){
    int choix,id;
    printf("Enter exam ID: "); scanf("%d",&id);
    Examen *m=chercher_examen_par_id(&liste,id);
    if(m==NULL){
        printf("Exam not found!\n");
        return;
    }
    printf("\n-------------------------------------------------------------------------\n");
    printf("Choose the element you want to change:\n");
    printf("1 - Id\n");
    printf("2 - Id_module\n");
    printf("3 - Name module\n");
    printf("4 - Exam date\n");
    printf("5 - Duration \n");
    printf("Enter your choice: ");
    scanf("%d", &choix);
    switch (choix){
    case 1:
        {
            printf("Enter new exam ID: "); scanf("%d",&m->id_examen);
              break;
        }
        case 2:
 printf("Enter new module ID: "); scanf("%d",&m->id_module);
              break;
        case 3:
            printf("Enter new module name: ");
             scanf(" %s",m->nom_module);
            break;
        case 4:
            printf("New exam date: ");
                struct tm tm_date = {0};
    printf("Date (day month year): ");
    scanf("%d %d %d", &tm_date.tm_mday, &tm_date.tm_mon, &tm_date.tm_year);
    tm_date.tm_mon -= 1;
    tm_date.tm_year -= 1900;

    printf("Time (h m s): ");
    scanf("%d %d %d", &tm_date.tm_hour, &tm_date.tm_min, &tm_date.tm_sec);
    m->date_examen = mktime(&tm_date);
            break;
        case 5:
            printf("New duration: ");
            scanf("%d", &m->duree);
            break;

        default:
            printf("Invalid choice!\n");
    }
}
int sauvegarder_liste_examen_ds_file(liste_examen *liste){
    if(liste->count==0) return(0);

char full_path[UTILS_MAX_PATH_LENGTH];
if (!utils_get_data_file_path(liste->filename, full_path, sizeof(full_path))) {
    printf("[ERROR] Failed to construct path for: %s\n", liste->filename);
    return 0;
}
FILE *p=fopen(full_path,"w");
for(int i=0;i<liste->count;i++){
        struct tm *info = localtime(&liste->exam[i].date_examen);
    fprintf(p,"| %d | %d | %s | %d/%d/%d | %d:%d:%d | %d |",
           liste->exam[i].id_examen,
           liste->exam[i].id_module,
           liste->exam[i].nom_module,
           info->tm_mday,
           info->tm_mon + 1,
           info->tm_year + 1900,
           info->tm_hour,
           info->tm_min,
           info->tm_sec,
           liste->exam[i].duree);
}
fclose(p);
return(1);
}
int liste_examen_a_partir_file(liste_examen *liste){
char full_path[UTILS_MAX_PATH_LENGTH];
if (!utils_get_data_file_path(liste->filename, full_path, sizeof(full_path))) {
    printf("[ERROR] Failed to construct path for: %s\n", liste->filename);
    return 0;
}
FILE *p=fopen(full_path,"r");
if(p==NULL) return(0);
int i=0;
while(i<liste->capacity){
         struct tm info ={0};
         int day,mon,year,hour,min,sec;
    int n=fscanf(p,"| %d | %d | %19[^|] | %d/%d/%d | %d:%d:%d | %d |",
           &liste->exam[i].id_examen,
           &liste->exam[i].id_module,
           liste->exam[i].nom_module,
           &day,
           &mon,
           &year,
           &hour,
           &min,
           &sec,
           &liste->exam[i].duree);
    if(n!=10) break;
    info.tm_mday=day;
    info.tm_mon=mon-1;
    info.tm_year=year-1900;
    info.tm_hour=hour;
    info.tm_min=min;
    info.tm_sec=sec;
    liste->exam[i].date_examen = mktime(&info);
           i++;
}
liste->count=i;
fclose(p);
return(1);
}
int trie_liste_examen_id(liste_examen liste ,int n ){
if(n==1){
    for(int i=0;i<liste.count;i++){
            Examen min=liste.exam[i];
        for(int j=1+i;j<liste.count;j++){
            if(liste.exam[j].id_examen<min.id_examen){
                      Examen h=liste.exam[j];
               liste.exam[j]=min;
            min=h;
            }
        }liste.exam[i]=min;
    }
    return(1);
}
else{
     for(int i=0;i<liste.count;i++){
            Examen max=liste.exam[i];
        for(int j=1+i;j<liste.count;j++){
            if(liste.exam[j].id_examen>max.id_examen){
                  Examen h=liste.exam[j];
               liste.exam[j]=max;
            max=h;
            }
        }liste.exam[i]=max;
    }
    return(1);
}
return(0);


}
int trie_liste_examen_nom(int n,liste_examen liste){
    if(n==1){
    for(int i=0;i<liste.count;i++){
             Examen min=liste.exam[i];
        for(int j=1+i;j<liste.count;j++){
            if(strcmp(liste.exam[j].nom_module,min.nom_module)>0){
                       Examen h=liste.exam[j];
               liste.exam[j]=min;
            min=h;
            }
        }liste.exam[i]=min;
    }
    return(1);
}
else{
     for(int i=0;i<liste.count;i++){
             Examen max=liste.exam[i];
        for(int j=1+i;j<liste.count;j++){
            if(strcmp(liste.exam[j].nom_module,max.nom_module)<0){
                  Examen h=liste.exam[j];
               liste.exam[j]=max;
            max=h;
            }
        }liste.exam[i]=max;
    }
    return(1);
}
return(0);
}
liste_note* creer_liste_note(int capacite) {
    liste_note *liste = (liste_note*)malloc(sizeof(liste_note));
    if (liste == NULL) {
        printf("Error: memory allocation failed!\n");
        return NULL;
    }

    liste->note = (Note*)malloc(capacite * sizeof(Note));
    if (liste->note == NULL) {
        free(liste);
        return NULL;
    }

    liste->count = 0;
    liste->capacity = capacite;
    strcpy(liste->file_name, "liste_des_notes.txt");

    return liste;
}


Note* cree_note() {
    Note *n = (Note*)malloc(sizeof(Note));
    if (n == NULL) {
        printf("Error: allocation failed!\n");
        return NULL;
    }

    printf("\n=== Create Note ===\n");
    printf("Student ID: ");
    scanf("%d", &n->id_etudiant);

    printf("Exam ID: ");
    scanf("%d", &n->id_examen);

    do {
        printf("Obtained grade (0-20): ");
        scanf("%f", &n->note_obtenue);
        if (n->note_obtenue < 0 || n->note_obtenue > 20) {
            printf(" Grade must be between 0 and 20!\n");
        }
    } while (n->note_obtenue < 0 || n->note_obtenue > 20);

    printf("Present (1=yes, 0=no): ");
    scanf("%d", &n->present);

    return n;
}

int note_ajouter(liste_note *liste, Note *n) {
    if (liste == NULL || n == NULL) return 0;

    if (liste->count >= liste->capacity) {
        liste->capacity *= 2;
        liste->note = (Note*)realloc(liste->note,
                                     liste->capacity * sizeof(Note));
        if (liste->note == NULL) return 0;
    }

    liste->note[liste->count++] = *n;
    free(n);
    return 1;
}

void afficher_note(Note *n) {
    if (n == NULL) {
        printf("Invalid grade!\n");
        return;
    }

    printf("| %-12d | %-10d | %-12.2f | %-8s |\n",
           n->id_etudiant,
           n->id_examen,
           n->note_obtenue,
           n->present ? "yes" : "no");
}

void afficher_liste_notes(liste_note *liste) {
    if (liste == NULL || liste->note == NULL) {
        printf("Error: Invalid list\n");
        return;
    }

    if (liste->count == 0) {
        printf("\nNo grades to display.\n");
        return;
    }

    printf("\n+--------------+------------+--------------+----------+\n");
    printf("| Student ID   | Exam ID    | Grade        | Present  |\n");
    printf("+--------------+------------+--------------+----------+\n");

    for (int i = 0; i < liste->count; i++) {
        afficher_note(&liste->note[i]);
    }

    printf("+--------------+------------+--------------+----------+\n");
    printf("Total: %d note(s)\n\n", liste->count);
}

Note* chercher_note(liste_note *liste, int id_etudiant, int id_examen) {
    if (liste == NULL) return NULL;

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_etudiant == id_etudiant &&
            liste->note[i].id_examen == id_examen) {
            return &liste->note[i];
        }
    }
    return NULL;
}

void afficher_notes_etudiant(liste_note *liste, int id_etudiant) {
    if (liste == NULL || liste->count == 0) {
        printf("No grades!\n");
        return;
    }

    int trouve = 0;
    printf("\n=== Grades of student %d ===\n", id_etudiant);
    printf("+--------------+------------+--------------+----------+\n");
    printf("| Student ID   | Exam ID    | Grade        | Present  |\n");
    printf("+--------------+------------+--------------+----------+\n");

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_etudiant == id_etudiant) {
            afficher_note(&liste->note[i]);
            trouve = 1;
        }
    }

    if (!trouve) {
        printf("No grades found for this student.\n");
    }
    printf("+--------------+------------+--------------+----------+\n\n");
}

void afficher_notes_examen(liste_note *liste, int id_examen) {
    if (liste == NULL || liste->count == 0) {
        printf("No grades!\n");
        return;
    }

    int trouve = 0;
    printf("\n=== Grades of exam %d ===\n", id_examen);
    printf("+--------------+------------+--------------+----------+\n");
    printf("| Student ID   | Exam ID    | Grade        | Present  |\n");
    printf("+--------------+------------+--------------+----------+\n");

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_examen == id_examen) {
            afficher_note(&liste->note[i]);
            trouve = 1;
        }
    }

    if (!trouve) {
        printf("No grades found for this exam.\n");
    }
    printf("+--------------+------------+--------------+----------+\n\n");
}


void modifier_note(liste_note *liste) {
    int id_etudiant, id_examen, choix;

    printf("Student ID: ");
    scanf("%d", &id_etudiant);
    printf("Exam ID: ");
    scanf("%d", &id_examen);

    Note *n = chercher_note(liste, id_etudiant, id_examen);

    if (n == NULL) {
        printf("Grade not found!\n");
        return;
    }

    printf("\n--- Current Grade ---\n");
    printf("+--------------+------------+--------------+----------+\n");
    printf("| Student ID   | Exam ID    | Grade        | Present  |\n");
    printf("+--------------+------------+--------------+----------+\n");
    afficher_note(n);
    printf("+--------------+------------+--------------+----------+\n");

    printf("\nWhat do you want to modify?\n");
    printf("1 - Obtained grade\n");
    printf("2 - Presence\n");
    printf("3 - Both\n");
    printf("Choice: ");
    scanf("%d", &choix);

    switch (choix) {
        case 1:
            do {
                printf("New grade (0-20): ");
                scanf("%f", &n->note_obtenue);
                if (n->note_obtenue < 0 || n->note_obtenue > 20) {
                    printf(" Invalid grade!\n");
                }
            } while (n->note_obtenue < 0 || n->note_obtenue > 20);
            break;

        case 2:
            printf("Present (1=yes, 0=no): ");
            scanf("%d", &n->present);
            break;

        case 3:
            do {
                printf("New grade (0-20): ");
                scanf("%f", &n->note_obtenue);
            } while (n->note_obtenue < 0 || n->note_obtenue > 20);

            printf("Present (1=yes, 0=no): ");
            scanf("%d", &n->present);
            break;

        default:
            printf("Invalid choice!\n");
            return;
    }

    printf(" Grade successfully modified!\n");
}


int note_supprimer(liste_note *liste, int id_etudiant, int id_examen) {
    if (liste == NULL || liste->count == 0) return 0;

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_etudiant == id_etudiant &&
            liste->note[i].id_examen == id_examen) {

            for (int j = i; j < liste->count - 1; j++) {
                liste->note[j] = liste->note[j + 1];
            }
            liste->count--;
            return 1;
        }
    }
    return 0;
}
float calculer_moyenne_etudiant(liste_note *liste, int id_etudiant) {
    if (liste == NULL || liste->count == 0) return -1;

    float somme = 0;
    int count = 0;

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_etudiant == id_etudiant &&
            liste->note[i].present == 1) {
            somme += liste->note[i].note_obtenue;
            count++;
        }
    }

    if (count == 0) return -1;
    return somme / count;
}
float calculer_moyenne_examen(liste_note *liste, int id_examen) {
    if (liste == NULL || liste->count == 0) return -1;

    float somme = 0;
    int count = 0;

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_examen == id_examen &&
            liste->note[i].present == 1) {
            somme += liste->note[i].note_obtenue;
            count++;
        }
    }

    if (count == 0) return -1;
    return somme / count;
}
void statistiques_examen(liste_note *liste, int id_examen) {
    if (liste == NULL || liste->count == 0) {
        printf("No grades!\n");
        return;
    }

    float somme = 0, min = 20, max = 0;
    int count = 0, presents = 0, absents = 0, admis = 0;

    for (int i = 0; i < liste->count; i++) {
        if (liste->note[i].id_examen == id_examen) {
            if (liste->note[i].present) {
                presents++;
                somme += liste->note[i].note_obtenue;
                count++;

                if (liste->note[i].note_obtenue < min)
                    min = liste->note[i].note_obtenue;
                if (liste->note[i].note_obtenue > max)
                    max = liste->note[i].note_obtenue;
                if (liste->note[i].note_obtenue >= 10)
                    admis++;
            } else {
                absents++;
            }
        }
    }

    if (count == 0) {
        printf("No grades for this exam.\n");
        return;
    }

    printf("\n========== EXAM STATISTICS %d ==========\n", id_examen);
    printf("Number of students    : %d\n", presents + absents);
    printf("Presents              : %d\n", presents);
    printf("Absents               : %d\n", absents);
    printf("Average               : %.2f\n", somme / count);
    printf("Minimum grade         : %.2f\n", min);
    printf("Maximum grade         : %.2f\n", max);
    printf("Passed (>=10)         : %d\n", admis);
    printf("Success rate          : %.2f%%\n", (admis * 100.0) / count);
    printf("Absence rate          : %.2f%%\n", (absents * 100.0) / (presents + absents));
    printf("==========================================\n\n");
}
int sauvegarder_notes_ds_file(liste_note *liste) {
    if (liste == NULL || liste->count == 0) return 0;

    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(liste->file_name, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", liste->file_name);
        return 0;
    }

    FILE *p = fopen(full_path, "w");
    if (p == NULL) {
        printf("Error opening file!\n");
        return 0;
    }

    for (int i = 0; i < liste->count; i++) {
        int written = fprintf(p, "%d,%d,%.2f,%d\n",
                liste->note[i].id_etudiant,
                liste->note[i].id_examen,
                liste->note[i].note_obtenue,
                liste->note[i].present);
        if (written < 0) {
            printf("Error: Failed to write grade %d to file\n", i+1);
            fclose(p);
            return 0;
        }
    }

    fflush(p);
    fclose(p);
    printf(" %d note(s) sauvegardee(s)\n", liste->count);
    return 1;
}
int charger_notes_depuis_file(liste_note *liste) {
    if (liste == NULL || liste->note == NULL) return 0;
    
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(liste->file_name, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", liste->file_name);
        return 0;
    }
    
    FILE *p = fopen(full_path, "r");
    if (p == NULL) {
        printf("Error opening file!\n");
        return 0;
    }
int i=0;
   while(i<liste->capacity){
        int n=fscanf(p, "%d,%d,%f,%d",
                           &liste->note[i].id_etudiant,
                           &liste->note[i].id_examen,
                           &liste->note[i].note_obtenue,
                           &liste->note[i].present);
        if(n!=4) break;
        i++;
   }
  liste->count=i;
    fclose(p);
    printf(" %d grade(s) loaded\n", liste->count);
    return 1;
}

void trier_notes_par_etudiant(liste_note *liste) {
    if (liste == NULL || liste->count <= 1) return;

    for (int i = 0; i < liste->count - 1; i++) {
        for (int j = i + 1; j < liste->count; j++) {
            if (liste->note[j].id_etudiant < liste->note[i].id_etudiant) {
                Note temp = liste->note[i];
                liste->note[i] = liste->note[j];
                liste->note[j] = temp;
            }
        }
    }
    printf("List sorted by student ID\n");
}


void detruire_liste_notes(liste_note **liste) {
    if (*liste == NULL) return;

    free((*liste)->note);
    (*liste)->note = NULL;
    (*liste)->count = 0;
    (*liste)->capacity = 0;
    free(*liste);
    *liste = NULL;

    printf("Grades list destroyed\n");
}

 Module* cree_module(){
    Module* cour=(Module*)malloc(sizeof(Module));
    printf("Module ID number:");scanf("%d",&cour->id);
    printf("Enter the module name:\t");scanf("  %[^\n]",cour->nom);
    printf("Enter the module description:\t");scanf("  %[^\n]",cour-> description);
    printf("Enter the number of course hours:\t");scanf("%d",&cour->heures_cours);
    printf("Enter the number of tutorial hours:\t");scanf("%d",&cour->heures_td);
    printf("Enter the number of practical hours:\t");scanf("%d",&cour->heures_tp);
    do {
    printf("Which level does it concern:\n1-FIRST_YEAR_LEVEL.\n2-SECOND_YEAR_LEVEL.\n3-FIRST_YEAR_CYCLE_LEVEL.\n4-SECOND_YEAR_CYCLE_LEVEL.\n5-SECOND_YEAR_CYCLE_LEVEL.\n6-THIRD_YEAR_CYCLE_LEVEL.\n7-DOCTORATE_LEVEL\n num:");scanf("%d",&cour->niveau);}
    while((cour->niveau>7)||(cour->niveau<1));
          do{
    printf("Which major does it concern:\n1-PREPARATORY_YEARS_TO_ENGINEERING_CYCLE.\n2-COMPUTER_ENGINEERING.\n3-CIVIL_ENGINEERING.\n4-WATER_ENVIRONMENT_ENGINEERING.\n5-ENERGETIC_ENGINEERING.\n6-MECHANICAL_ENGINEERING \n num:");scanf("%d",&cour->filiere);
          }  while((cour->filiere>6)||(cour->filiere<1));
   printf("For which semester:\t");scanf("%d",&cour->semestre);
       printf("Teacher's full name:\t");scanf(" %[^\n]",cour->nom_prenom_enseignent);
       return(cour);

}
//les fcts :
ListeModules* liste_cours_creer(){
    ListeModules* coursliste=(ListeModules*)malloc(sizeof(ListeModules));
    coursliste->cours=(Module*)malloc(sizeof(Module)*300);
    coursliste->count=0;
   coursliste->capacity=300;
   strcpy(coursliste->filename,"liste_des_modules.txt");
   return(coursliste);
}
void liste_cours_detruire(ListeModules** liste){
    if(liste==NULL || *liste==NULL) return;
    free((*liste)->cours);
    (*liste)->cours=NULL;
    (*liste)->count=0;
    (*liste)->capacity=0;
    free(*liste);
    (*liste)=NULL;
}
int cours_ajouter(ListeModules* liste, Module cours){
   if(liste->count<liste->capacity){
    (liste)->cours[liste->count++] =cours;
    return(1);}

return(0);
}
int cours_supprimer_par_nom(ListeModules* liste, char* cours_nom){
if(liste->count!=0){
        for(int i=0;i<liste->count;i++){
            if(strcmp((liste)->cours[i].nom,cours_nom)==0){
                 for(int j=i;j<liste->count-1;j++)
                   (liste)->cours[j]=(liste)->cours[j+1];
                 liste->count--;
                 return(1);
            }
        }

    }

return(0);
}
int cours_supprimer_par_id(ListeModules *liste, int cours_id){
if(liste->count!=0){
        for(int i=0;i<liste->count;i++){
            if(liste->cours[i].id==cours_id){
                 for(int j=i;j<liste->count-1;j++)
                   (liste)->cours[j]=(liste)->cours[j+1];
                 liste->count--;
                 return(1);
            }
        }

    }

return(0);
}
Module* cours_rechercher_par_id(ListeModules* liste, int cours_id){
    for(int i=0;i<liste->count;i++){
        if(liste->cours[i].id==cours_id)
            return(&liste->cours[i]);
    }
return(NULL);
}
void cours_afficher(Module* m) {
if(m==NULL){
    printf("IMPOSSIBLE");
}
else{
    printf("\n+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
    printf("| ID  | Name                     | Course(h) | Tutorial(h) | Practical(h) | Sem | Teacher                    | Major     | Level     |\n");
    printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");

    printf("| %-3d | %-23s | %-8d | %-8d | %-8d | %-3d | %-27s | %-9d | %-9d |\n",
        m->id,
        m->nom,
        m->heures_cours,
        m->heures_td,
        m->heures_tp,
        m->semestre,
        m->nom_prenom_enseignent,
        m->filiere,
        m->niveau
    );

    printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");}
}

void liste_cours_afficher(ListeModules* liste){
    if (liste == NULL || liste->cours == NULL) {
        printf("Error: Invalid list\n");
        return;
    }

    if (liste->count == 0) {
        printf("No modules to display.\n");
        return;
    }

    printf("\n+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
    printf("| ID  | Name                     | Course(h) | Tutorial(h) | Practical(h) | Sem | Teacher                    | Major     | Level     |\n");
    printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
    for (int i = 0; i < liste->count; i++) {
        Module* m = &liste->cours[i];
        printf("| %-3d | %-23s | %-8d | %-8d | %-8d | %-3d | %-27s | %-9d | %-9d |\n",
            m->id,
            m->nom,
            m->heures_cours,
            m->heures_td,
            m->heures_tp,
            m->semestre,
            m->nom_prenom_enseignent,
            m->filiere,
            m->niveau
        );
    }  printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");}
void cours_afficher_depuis_un_liste_par_id(ListeModules liste,int id){

    if (liste.count == 0) {
        printf("No modules to display.\n");
        return;
    }
    for (int i = 0; i < liste.count; i++) {
            if(liste.cours[i].id==id){ Module m = liste.cours[i];
             printf("\n+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
    printf("| ID  | Name                     | Course(h) | Tutorial(h) | Practical(h) | Sem | Teacher                    | Major     | Level     |\n");
    printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
        printf("| %-3d | %-23s | %-8d | %-8d | %-8d | %-3d | %-27s | %-9d | %-9d |\n",
            m.id,
            m.nom,
            m.heures_cours,
            m.heures_td,
            m.heures_tp,
            m.semestre,
            m.nom_prenom_enseignent,
            m.filiere,
            m.niveau
        ); return;}
    }
    printf("The course does not exist in the list");
}
void cours_afficher_depuis_un_liste_par_nom(ListeModules liste,char *nom){

    if (liste.count == 0) {
        printf("No modules to display.\n");
        return;
    }
    for (int i = 0; i < liste.count; i++) {
            if(strcmp(liste.cours[i].nom,nom)==0){ Module m = liste.cours[i];
             printf("\n+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
    printf("| ID  | Name                     | Course(h) | Tutorial(h) | Practical(h) | Sem | Teacher                    | Major     | Level     |\n");
    printf("+-----+-------------------------+----------+----------+----------+-----+-----------------------------+-----------+-----------+\n");
        printf("| %-3d | %-23s | %-8d | %-8d | %-8d | %-3d | %-27s | %-9d | %-9d |\n",
            m.id,
            m.nom,
            m.heures_cours,
            m.heures_td,
            m.heures_tp,
            m.semestre,
            m.nom_prenom_enseignent,
            m.filiere,
            m.niveau
        ); return;}
    }
    printf("The course does not exist in the list");
}
Module* chercher_module_par_nom(ListeModules liste,char* nom){
  for(int i=0;i<liste.count;i++){
    if(strcmp((liste.cours[i].nom),nom)==0)
    {
        return(&liste.cours[i]);
    }

  }
  return(NULL);
}
Module* chercher_module_par_id(ListeModules liste,int id){
  for(int i=0;i<liste.count;i++){
    if(liste.cours[i].id==id)
    {
        return(&liste.cours[i]);
    }

  }
  return(NULL);
}
void modidier_cours(ListeModules liste){
    int choix,id;
    printf("Enter the module ID:"); scanf("%d",&id);
   Module *m= chercher_module_par_id(liste,id);
    printf("\n-------------------------------------------------------------------------\n");
    printf("Choose the element you want to change:\n");
    printf("1 - Id\n");
    printf("2 - Name\n");
    printf("3 - Description\n");
    printf("4 - Course hours\n");
    printf("5 - Tutorial hours\n");
    printf("6 - Practical hours\n");
    printf("7 - Level\n");
    printf("8 - Major\n");
    printf("9 - Semester\n");
    printf("10- Teacher's Name and Surname\n");
    printf("Enter your choice: ");
    scanf("%d", &choix);
    switch (choix){
    case 1:
        {
            printf("Enter the new id:");scanf("%d",&m->id);
              break;
        }
        case 2:
            printf("New name: ");
            scanf(" %[^\n]", m->nom);
            break;
        case 3:
            printf("New description: ");
             scanf(" %s",m->description);
            break;
        case 4:
            printf("New course hours: ");
            scanf("%d", &m->heures_cours);
            break;
        case 5:
            printf("New tutorial hours: ");
            scanf("%d", &m->heures_td);
            break;
        case 6:
            printf("New practical hours: ");
            scanf("%d", &m->heures_tp);
            break;
        case 7:
            printf("New level: ");
            scanf("%d", &m->niveau);
            break;
        case 8:
            printf("New major: ");
            scanf("%d", &m->filiere);
            break;
        case 9:
            printf("New semester: ");
            scanf("%d", &m->semestre);
            break;
        case 10:
            printf("New teacher's name and surname: ");
             scanf(" %[^\n]",m->nom_prenom_enseignent);
            break;
        default:
            printf("Invalid choice!\n");
    }
}
int sauvegarder_modules_ds_file(ListeModules liste){
    if(liste.count==0) return(0);

char full_path[UTILS_MAX_PATH_LENGTH];
if (!utils_get_data_file_path(liste.filename, full_path, sizeof(full_path))) {
    printf("[ERROR] Failed to construct path for: %s\n", liste.filename);
    return 0;
}
FILE *p=fopen(full_path,"w");
for(int i=0;i<liste.count;i++){
    fprintf(p,"%d,%s,%s,%d,%d,%d,%d,%d,%d,%s\n",liste.cours[i].id,
            liste.cours[i].nom,
            liste.cours[i].description,
            liste.cours[i].heures_cours,
            liste.cours[i].heures_td,
            liste.cours[i].heures_tp,
            liste.cours[i].niveau,
            liste.cours[i].filiere,
            liste.cours[i].semestre,
            liste.cours[i].nom_prenom_enseignent
);
}
fclose(p);
return(1);

}
int remplire_liste_appartit_file(ListeModules *liste){
    char full_path[UTILS_MAX_PATH_LENGTH];
    if (!utils_get_data_file_path(liste->filename, full_path, sizeof(full_path))) {
        printf("[ERROR] Failed to construct path for: %s\n", liste->filename);
        return 0;
    }
    
    FILE *p = fopen(full_path,"r");
    if(p == NULL) return 0;

    while (!feof(p)) {
        Module m;

        int n = fscanf(p,"%d,%[^,],%[^,],%d,%d,%d,%d,%d,%d,%[^\n]\n",
            &m.id,
            m.nom,
            m.description,
            &m.heures_cours,
            &m.heures_td,
            &m.heures_tp,
            &m.niveau,
            &m.filiere,
            &m.semestre,
            m.nom_prenom_enseignent
        );

       if(n == 10) {
            if(liste->count < liste->capacity) {
                liste->cours[liste->count] = m;
                liste->count++;            }
                else {
                printf(" List full!\n");
                break;
            }
        } else {
            printf(" Read error (only %d fields out of 10)\n", n);
        }
        printf("---\n");
    }
    fclose(p);
    return 1;
}
int trie_liste_id(ListeModules liste ,int n ){
if(n==1){
    for(int i=0;i<liste.count;i++){
            Module min=liste.cours[i];
        for(int j=1+i;j<liste.count;j++){
            if(liste.cours[j].id<min.id){
                      Module h=liste.cours[j];
               liste.cours[j]=min;
            min=h;
            }
        }liste.cours[i]=min;
    }
    return(1);
}
else{
     for(int i=0;i<liste.count;i++){
            Module max=liste.cours[i];
        for(int j=1+i;j<liste.count;j++){
            if(liste.cours[j].id>max.id){
                 Module h=liste.cours[j];
               liste.cours[j]=max;
            max=h;
            }
        }liste.cours[i]=max;
    }
    return(1);
}
return(0);
}
int trie_par_nom(int n,ListeModules liste){
    if(n==1){
    for(int i=0;i<liste.count;i++){
            Module min=liste.cours[i];
        for(int j=1+i;j<liste.count;j++){
            if(strcmp(liste.cours[j].nom,min.nom)>0){
                      Module h=liste.cours[j];
               liste.cours[j]=min;
            min=h;
            }
        }liste.cours[i]=min;
    }
    return(1);
}
else{
     for(int i=0;i<liste.count;i++){
            Module max=liste.cours[i];
        for(int j=1+i;j<liste.count;j++){
            if(strcmp(liste.cours[j].nom,max.nom)<0){
                 Module h=liste.cours[j];
               liste.cours[j]=max;
            max=h;
            }
        }liste.cours[i]=max;
    }
    return(1);
}
return(0);
}
void liste_cours_niveau(ListeModules liste,int niveaux){
for(int i=0;i<liste.count;i++){
    if(liste.cours[i].niveau==niveaux){
        cours_afficher(&liste.cours[i]);
    }
}
}
void liste_cours_filiere(ListeModules liste,int filiere){
for(int i=0;i<liste.count;i++){
    if(liste.cours[i].filiere==filiere){
        cours_afficher(&liste.cours[i]);
    }
}
}

// Wrapper functions for compatibility
liste_note* liste_note_create(void) {
    return creer_liste_note(100);
}

void liste_note_destroy(liste_note* list) {
    if (list) {
        detruire_liste_notes(&list);
    }
}

liste_examen* liste_examen_create(void) {
    return cree_liste_examen();
}

void liste_examen_destroy(liste_examen* list) {
    if (list) {
        free(list->exam);
        free(list);
    }
}

ListeModules* liste_module_create(void) {
    return liste_cours_creer();
}

void liste_module_destroy(ListeModules* list) {
    if (list) {
        liste_cours_detruire(&list);
    }
}

int grade_list_load_from_file(liste_note* list, const char* filename) {
    if (!list || !filename) return -1;
    strncpy(list->file_name, filename, sizeof(list->file_name) - 1);
    list->file_name[sizeof(list->file_name) - 1] = '\0';
    return charger_notes_depuis_file(list);
}

int grade_list_save_to_file(liste_note* list, const char* filename) {
    if (!list || !filename) return -1;
    strncpy(list->file_name, filename, sizeof(list->file_name) - 1);
    list->file_name[sizeof(list->file_name) - 1] = '\0';
    return sauvegarder_notes_ds_file(list);
}