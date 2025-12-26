#include "../include/prof_note.h"

ProfessorNoteList* prof_note_list_create(void) {
    ProfessorNoteList* list = (ProfessorNoteList*)malloc(sizeof(ProfessorNoteList));
    if (!list) return NULL;
    
    list->capacity = 100;
    list->count = 0;
    list->notes = (ProfessorNote*)malloc(sizeof(ProfessorNote) * list->capacity);
    list->filename[0] = '\0';
    
    if (!list->notes) {
        free(list);
        return NULL;
    }
    
    return list;
}

void prof_note_list_destroy(ProfessorNoteList* list) {
    if (list) {
        if (list->notes) {
            free(list->notes);
        }
        free(list);
    }
}

int prof_note_create(ProfessorNoteList* list, int student_id, int module_id, int prof_id, const char* content) {
    if (!list || !content) return 0;
    
    // Resize if needed
    if (list->count >= list->capacity) {
        int new_capacity = list->capacity * 2;
        ProfessorNote* new_notes = (ProfessorNote*)realloc(list->notes, sizeof(ProfessorNote) * new_capacity);
        if (!new_notes) return 0;
        
        list->notes = new_notes;
        list->capacity = new_capacity;
    }
    
    ProfessorNote* note = &list->notes[list->count];
    
    // Generate simple ID (max current ID + 1)
    int max_id = 0;
    for (int i = 0; i < list->count; i++) {
        if (list->notes[i].id > max_id) max_id = list->notes[i].id;
    }
    note->id = max_id + 1;
    
    note->student_id = student_id;
    note->module_id = module_id;
    note->professor_id = prof_id;
    
    // Helper to safely copy content
    strncpy(note->content, content, sizeof(note->content) - 1);
    note->content[sizeof(note->content) - 1] = '\0';
    
    // Set current date
    time_t now = time(NULL);
    struct tm *t = localtime(&now);
    strftime(note->date, sizeof(note->date), "%Y-%m-%d", t);
    
    list->count++;
    return 1;
}

int prof_note_save(ProfessorNoteList* list, const char* filename) {
    if (!list || !filename) return 0;
    
    FILE* f = fopen(filename, "w");
    if (!f) return 0;
    
    for (int i = 0; i < list->count; i++) {
        ProfessorNote* n = &list->notes[i];
        // CSV format: id,student_id,module_id,prof_id,date,message
        // Note: Using a unique delimiter if needed, but assuming message doesn't contain newlines/special chars for now.
        // Or using a fixed format. Let's stick to the CSV style used in other files.
        // We replace commas in content with spaces to avoid CSV issues, or just handle it carefully.
        
        char safe_content[512];
        strncpy(safe_content, n->content, sizeof(safe_content)-1);
        safe_content[sizeof(safe_content)-1] = '\0';
        
        // Simple sanitization
        for(int j=0; safe_content[j]; j++) {
            if(safe_content[j] == ',') safe_content[j] = ';';
            if(safe_content[j] == '\n') safe_content[j] = ' ';
        }
        
        fprintf(f, "%d,%d,%d,%d,%s,%s\n", 
            n->id, n->student_id, n->module_id, n->professor_id, n->date, safe_content);
    }
    
    fclose(f);
    return 1;
}

int prof_note_load(ProfessorNoteList* list, const char* filename) {
    if (!list || !filename) return 0;
    
    FILE* f = fopen(filename, "r");
    if (!f) return 0;
    
    char line[1024];
    while (fgets(line, sizeof(line), f)) {
        if (list->count >= list->capacity) {
            int new_capacity = list->capacity * 2;
            ProfessorNote* new_notes = (ProfessorNote*)realloc(list->notes, sizeof(ProfessorNote) * new_capacity);
            if (!new_notes) {
                fclose(f);
                return 0;
            }
            list->notes = new_notes;
            list->capacity = new_capacity;
        }
        
        ProfessorNote* n = &list->notes[list->count];
        // Format: id,student_id,module_id,prof_id,date,content
        // Content is the last field, so we can use %[^,\n] to read until newline
        // But sscanf is tricky with the last string.
        
        // Let's implement manual parsing for reliability like in other files if needed, 
        // or just standard sscanf if simple.
        
        char content_buffer[512];
        if (sscanf(line, "%d,%d,%d,%d,%19[^,],%511[^\n]", 
            &n->id, &n->student_id, &n->module_id, &n->professor_id, n->date, content_buffer) == 6) {
                
            strncpy(n->content, content_buffer, sizeof(n->content)-1);
            n->content[sizeof(n->content)-1] = '\0';
            list->count++;
        }
    }
    
    fclose(f);
    return 1;
}

ProfessorNote** prof_note_find_by_student(ProfessorNoteList* list, int student_id, int* count) {
    if (!list || !count) return NULL;
    
    *count = 0;
    // temporary array, could be optimized
    ProfessorNote** result = (ProfessorNote**)malloc(sizeof(ProfessorNote*) * list->count);
    if (!result) return NULL;
    
    for (int i = 0; i < list->count; i++) {
        if (list->notes[i].student_id == student_id) {
            result[*count] = &list->notes[i];
            (*count)++;
        }
    }
    
    if (*count == 0) {
        free(result);
        return NULL;
    }
    
    return result;
}

ProfessorNote** prof_note_find_by_module(ProfessorNoteList* list, int module_id, int* count) {
    if (!list || !count) return NULL;
    
    *count = 0;
    ProfessorNote** result = (ProfessorNote**)malloc(sizeof(ProfessorNote*) * list->count);
    if (!result) return NULL;
    
    for (int i = 0; i < list->count; i++) {
        if (list->notes[i].module_id == module_id) {
            result[*count] = &list->notes[i];
            (*count)++;
        }
    }
    
    if (*count == 0) {
        free(result);
        return NULL;
    }
    
    return result;
}
