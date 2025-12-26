#ifndef GRADE_H
#define GRADE_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include "config.h"

// Forward declarations
typedef struct liste_note_s liste_note;
typedef liste_note GradeList;
typedef struct ListeModules_s ListeModules;
typedef ListeModules CourseList;

typedef struct {
    int id;
    char nom[MAX_NAME_LENGTH];
    char description[MAX_DESC_LENGTH];
    int heures_cours;
    int heures_td;
    int heures_tp;
    int niveau;
    int  filiere;
    int semestre;
    char nom_prenom_enseignent[50];
   } Module;
typedef struct ListeModules_s {
    Module* cours;
    int count;
    int capacity;
    char filename[256];
} ListeModules;
typedef struct {
    int id_examen;
    int id_module;
    char nom_module[20];
    time_t date_examen;
    int duree;
} Examen;
typedef struct {
    Examen* exam;
    int count;
    int capacity;
    char filename[256];
}liste_examen;
typedef struct {
    int id_etudiant;
    int id_examen;
    float note_obtenue;
    int present;
} Note;

// Grade is an alias for Note for UI compatibility
typedef Note Grade;

typedef struct liste_note_s {
    Note *note;
    int count;
    int capacity;
    char file_name[256];
} liste_note;
//fct examen
Examen* creer_examen();
void afficher_examen(Examen *E);
int examen_ajouter(liste_examen* liste, Examen *ex);
int examen_supprimer_par_id(liste_examen* liste,int id_examen);
int examen_supprimer_par_nom(liste_examen* liste,char* nom_examen);
void afficher_liste_examens(liste_examen *liste) ;
liste_examen* cree_liste_examen();
Examen* chercher_examen_par_id(liste_examen* liste,int id);
Examen* chercher_examen_par_nom(liste_examen* liste,char* nom);
void modidier_examen(liste_examen liste);
int sauvegarder_liste_examen_ds_file(liste_examen *liste);
int liste_examen_a_partir_file(liste_examen *liste);
int trie_liste_examen_id(liste_examen liste ,int n );
int trie_liste_examen_nom(int n,liste_examen liste);
//fct note
liste_note* creer_liste_note(int capacite);
Note* cree_note() ;
int note_ajouter(liste_note *liste, Note *n);
void afficher_note(Note *n);
void afficher_liste_notes(liste_note *liste);
Note* chercher_note(liste_note *liste, int id_etudiant, int id_examen);
void afficher_notes_etudiant(liste_note *liste, int id_etudiant);
void afficher_notes_examen(liste_note *liste, int id_examen);
void modifier_note(liste_note *liste);
int note_supprimer(liste_note *liste, int id_etudiant, int id_examen);
float calculer_moyenne_etudiant(liste_note *liste, int id_etudiant);
float calculer_moyenne_examen(liste_note *liste, int id_examen);
void statistiques_examen(liste_note *liste, int id_examen);
int sauvegarder_notes_ds_file(liste_note *liste);
int charger_notes_depuis_file(liste_note *liste);
void trier_notes_par_etudiant(liste_note *liste);
void detruire_liste_notes(liste_note **liste);
//fct module
 Module* cree_module();
 ListeModules* liste_cours_creer();
 void liste_cours_detruire(ListeModules** liste);
 int cours_ajouter(ListeModules* liste, Module cours);
 int cours_supprimer_par_nom(ListeModules* liste, char* cours_nom);
 int cours_supprimer_par_id(ListeModules *liste, int cours_id);
 Module* cours_rechercher_par_id(ListeModules* liste, int cours_id);
 void cours_afficher(Module* m);
 void liste_cours_afficher(ListeModules* liste);
 void cours_afficher_depuis_un_liste_par_id(ListeModules liste,int id);
 void cours_afficher_depuis_un_liste_par_nom(ListeModules liste,char *nom);
 Module* chercher_module_par_nom(ListeModules liste,char* nom);
 Module* chercher_module_par_id(ListeModules liste,int id);
 void modidier_cours(ListeModules liste);
 int sauvegarder_modules_ds_file(ListeModules liste);
 int remplire_liste_appartit_file(ListeModules *liste);
 int trie_liste_id(ListeModules liste ,int n );
 int trie_par_nom(int n,ListeModules liste);
 void liste_cours_niveau(ListeModules liste,int niveaux);
 void liste_cours_filiere(ListeModules liste,int filiere);

// Additional functions for compatibility
liste_note* liste_note_create(void);
void liste_note_destroy(liste_note* list);
liste_examen* liste_examen_create(void);
void liste_examen_destroy(liste_examen* list);
ListeModules* liste_module_create(void);
void liste_module_destroy(ListeModules* list);
int grade_list_load_from_file(liste_note* list, const char* filename);
int grade_list_save_to_file(liste_note* list, const char* filename);

#endif // GRADE_H