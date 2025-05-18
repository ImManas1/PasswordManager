#include <stdio.h>
#include <stdlib.h>
#include <string.h>

#define MAX 50
#define MAX_CREDS 100

struct Credential {
    char site[MAX];
    char username[MAX];
    char password[MAX];
};

struct User {
    char username[MAX];
    char password[MAX];
};

void encryptDecrypt(char *input) {
    char key = 'K';
    for (int i = 0; input[i]; i++)
        input[i] ^= key;
}

char *getCredsFilename(const char *username) {
    static char filename[MAX];
    snprintf(filename, MAX, "%s_credentials.dat", username);
    return filename;
}

int usernameExists(const char *username) {
    FILE *fp = fopen("users.dat", "r");
    if (!fp) return 0;

    struct User u;
    while (fread(&u, sizeof(u), 1, fp)) {
        if (strcmp(u.username, username) == 0) {
            fclose(fp);
            return 1;
        }
    }
    fclose(fp);
    return 0;
}

int signup() {
    struct User u;
    printf("Signup - Enter username: ");
    scanf("%s", u.username);

    if (usernameExists(u.username)) {
        printf("Username already exists. Try login.\n");
        return 0;
    }

    printf("Enter password: ");
    scanf("%s", u.password);

    FILE *fp = fopen("users.dat", "a");
    if (!fp) {
        printf("Error saving user.\n");
        return 0;
    }

    fwrite(&u, sizeof(u), 1, fp);
    fclose(fp);
    printf("Signup successful!\n");
    return 1;
}

int login(char *loggedInUser) {
    struct User u;
    char user[MAX], pass[MAX];

    printf("Login - Username: ");
    scanf("%s", user);
    printf("Password: ");
    scanf("%s", pass);

    FILE *fp = fopen("users.dat", "r");
    if (!fp) return 0;

    while (fread(&u, sizeof(u), 1, fp)) {
        if (!strcmp(user, u.username) && !strcmp(pass, u.password)) {
            strcpy(loggedInUser, user);
            fclose(fp);
            printf("Login successful! Welcome, %s\n", loggedInUser);
            return 1;
        }
    }
    fclose(fp);
    printf("Invalid credentials.\n");
    return 0;
}

void addCredential(const char *loggedInUser) {
    FILE *fp = fopen(getCredsFilename(loggedInUser), "a");
    if (!fp) {
        printf("Could not open file.\n");
        return;
    }

    struct Credential c;
    printf("Site: ");
    scanf("%s", c.site);
    printf("Username: ");
    scanf("%s", c.username);
    printf("Password: ");
    scanf("%s", c.password);

    encryptDecrypt(c.password);
    fwrite(&c, sizeof(c), 1, fp);
    fclose(fp);
    printf("Credential added.\n");
}

void viewCredentials(const char *loggedInUser) {
    FILE *fp = fopen(getCredsFilename(loggedInUser), "r");
    if (!fp) {
        printf("No credentials found.\n");
        return;
    }

    struct Credential creds[MAX_CREDS];
    int count = 0;

    while (fread(&creds[count], sizeof(struct Credential), 1, fp) == 1) {
        encryptDecrypt(creds[count].password);
        count++;
    }
    fclose(fp);

    if (count == 0) {
        printf("No saved credentials.\n");
        return;
    }

    printf("\n--- Stored Credentials ---\n");
    for (int i = 0; i < count; i++) {
        printf("Site: %s\nUser: %s\nPassword: %s\n\n", creds[i].site, creds[i].username, creds[i].password);
    }
}

int main() {
    char loggedInUser[MAX] = {0};
    int choice;

    while (1) {
        if (strlen(loggedInUser) == 0) {
            printf("\n--- Welcome ---\n1. Signup\n2. Login\n0. Exit\nChoice: ");
            scanf("%d", &choice);

            switch (choice) {
                case 1: signup(); break;
                case 2: login(loggedInUser); break;
                case 0: return 0;
                default: printf("Invalid choice.\n");
            }
        } else {
            printf("\n--- Password Manager: %s ---\n1. Add Credential\n2. View Credentials\n3. Logout\n0. Exit\nChoice: ", loggedInUser);
            scanf("%d", &choice);

            switch (choice) {
                case 1: addCredential(loggedInUser); break;
                case 2: viewCredentials(loggedInUser); break;
                case 3: loggedInUser[0] = '\0'; break;
                case 0: return 0;
                default: printf("Invalid choice.\n");
            }
        }
    }
}
