
#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define DATA_FILE "students.dat"
#define NAME_LEN 50
#define COURSE_LEN 50

typedef struct {
    int id;
    char name[NAME_LEN];
    int age;
    char gender;            // 'M' / 'F' / 'O'
    char course[COURSE_LEN];
    int year;               // current year/semester
} Student;

void read_line(char *buffer, int length) {
    if (fgets(buffer, length, stdin) != NULL) {
        size_t ln = strlen(buffer);
        if (ln > 0 && buffer[ln - 1] == '\n') buffer[ln - 1] = '\0';
    }
}

int id_exists(int id) {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) return 0;
    Student s;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) { fclose(fp); return 1; }
    }
    fclose(fp);
    return 0;
}

void add_student() {
    Student s;
    printf("\n--- Add New Student ---\n");
    printf("Enter ID (integer): ");
    if (scanf("%d", &s.id) != 1) {
        printf("Invalid ID.\n");
        while (getchar() != '\n'); return;
    }
    while (getchar() != '\n'); 

    if (id_exists(s.id)) {
        printf("Student with ID %d already exists.\n", s.id);
        return;
    }

    printf("Enter Name: ");
    read_line(s.name, NAME_LEN);

    printf("Enter Age: ");
    if (scanf("%d", &s.age) != 1) { printf("Invalid Age.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    printf("Enter Gender (M/F/O): ");
    s.gender = getchar();
    while (getchar() != '\n');

    printf("Enter Course: ");
    read_line(s.course, COURSE_LEN);

    printf("Enter Year/Semester (integer): ");
    if (scanf("%d", &s.year) != 1) { printf("Invalid Year.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "ab"); 
    if (!fp) { perror("Unable to open data file"); return; }
    fwrite(&s, sizeof(Student), 1, fp);
    fclose(fp);
    printf("Student added successfully.\n");
}

void display_students() {
    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf("\nNo records found.\n"); return; }

    printf("\n--- All Students ---\n");
    Student s;
    int count = 0;
    printf("%-6s %-20s %-4s %-6s %-20s %-4s\n", "ID", "Name", "Age", "Gen", "Course", "Year");
    printf("-----------------------------------------------------------------------------\n");
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        printf("%-6d %-20s %-4d %-6c %-20s %-4d\n", s.id, s.name, s.age, s.gender, s.course, s.year);
        count++;
    }
    if (count == 0) printf("No records to display.\n");
    fclose(fp);
}

void search_student() {
    int id;
    printf("\nEnter ID to search: ");
    if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf("No records found.\n"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) {
            printf("\n--- Student Found ---\n");
            printf("ID: %d\nName: %s\nAge: %d\nGender: %c\nCourse: %s\nYear: %d\n",
                   s.id, s.name, s.age, s.gender, s.course, s.year);
            found = 1; break;
        }
    }
    if (!found) printf("Student with ID %d not found.\n", id);
    fclose(fp);
}

void update_student() {
    int id;
    printf("\nEnter ID to update: ");
    if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "r+b"); // read+write binary
    if (!fp) { printf("No records found.\n"); return; }

    Student s;
    int found = 0;
    long pos;
    while ((pos = ftell(fp)), fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) {
            found = 1;
            printf("\nCurrent details:\n");
            printf("Name: %s\nAge: %d\nGender: %c\nCourse: %s\nYear: %d\n",
                   s.name, s.age, s.gender, s.course, s.year);

            printf("\nEnter new Name (leave blank to keep): ");
            char buffer[NAME_LEN];
            read_line(buffer, NAME_LEN);
            if (strlen(buffer) > 0) strncpy(s.name, buffer, NAME_LEN);

            printf("Enter new Age (0 to keep): ");
            int newAge; if (scanf("%d", &newAge) != 1) { while (getchar() != '\n'); newAge = 0; }
            while (getchar() != '\n');
            if (newAge > 0) s.age = newAge;

            printf("Enter new Gender (M/F/O or 0 to keep): ");
            char g = getchar();
            while (getchar() != '\n');
            if (g != '0' && g != '\n') s.gender = g;

            printf("Enter new Course (leave blank to keep): ");
            char buffer2[COURSE_LEN];
            read_line(buffer2, COURSE_LEN);
            if (strlen(buffer2) > 0) strncpy(s.course, buffer2, COURSE_LEN);

            printf("Enter new Year (0 to keep): ");
            int newYear; if (scanf("%d", &newYear) != 1) { while (getchar() != '\n'); newYear = 0; }
            while (getchar() != '\n');
            if (newYear > 0) s.year = newYear;

            // move file pointer back to start of this record
            fseek(fp, pos, SEEK_SET);
            fwrite(&s, sizeof(Student), 1, fp);
            printf("Student updated successfully.\n");
            break;
        }
    }

    if (!found) printf("Student with ID %d not found.\n", id);
    fclose(fp);
}

/* Delete student by ID (copy all except target to temp file) */
void delete_student() {
    int id;
    printf("\nEnter ID to delete: ");
    if (scanf("%d", &id) != 1) { printf("Invalid ID.\n"); while (getchar() != '\n'); return; }
    while (getchar() != '\n');

    FILE *fp = fopen(DATA_FILE, "rb");
    if (!fp) { printf("No records found.\n"); return; }

    FILE *temp = fopen("temp.dat", "wb");
    if (!temp) { fclose(fp); perror("Unable to create temp file"); return; }

    Student s;
    int found = 0;
    while (fread(&s, sizeof(Student), 1, fp) == 1) {
        if (s.id == id) { found = 1; continue; } // skip writing this record
        fwrite(&s, sizeof(Student), 1, temp);
    }
    fclose(fp);
    fclose(temp);

    if (!found) {
        remove("temp.dat");
        printf("Student with ID %d not found. No deletion performed.\n", id);
        return;
    }

    // replace original file
    remove(DATA_FILE);
    rename("temp.dat", DATA_FILE);
    printf("Student with ID %d deleted successfully.\n", id);
}

/* Main menu */
void menu() {
    while (1) {
        printf("\n===== STUDENT MANAGEMENT MENU =====\n");
        printf("1. Add Student\n");
        printf("2. Display All Students\n");
        printf("3. Search Student by ID\n");
        printf("4. Update Student by ID\n");
        printf("5. Delete Student by ID\n");
        printf("6. Exit\n");
        printf("Choose an option (1-6): ");

        int choice;
        if (scanf("%d", &choice) != 1) {
            printf("Invalid input. Please enter a number 1-6.\n");
            while (getchar() != '\n');
            continue;
        }
        while (getchar() != '\n');

        switch (choice) {
            case 1: add_student(); break;
            case 2: display_students(); break;
            case 3: search_student(); break;
            case 4: update_student(); break;
            case 5: delete_student(); break;
            case 6: printf("Exiting. Goodbye!\n"); return;
            default: printf("Invalid choice. Choose 1-6.\n");
        }
    }
}

int main() {
    printf("Student Management System (CodeAlpha Task 3)\n");
    menu();
    return 0;
}
