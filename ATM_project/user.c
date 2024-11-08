#include "user.h"
#include "transaction.h"


users *head = NULL;

static void save_to_csv(users *users_list, const char *filename);
static int chek_user(users *users_list, char *ID);
static void add_user(users **list, users data);
static void print_list(users *user);


/**
 * @brief Function to check if the user exists in the linked list
 * @param users_list
 * @param ID
 */
static int chek_user(users *users_list, char *ID) {
    users *current = users_list;
    while (current != NULL) {
        if (strcmp(current->ID, ID) == 0) {
            return 1; // User found
        }
        current = current -> next_user; // Move to the next user
    }
    return 0; // No user found
}


/**
 * @brief Function to add a user to the linked list
 * @param list 
 * @param data 
 */
static void add_user(users **list, users data) {
    system("cls");
    users *temp = (users *)malloc(sizeof(users));
    if (temp == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Check if the user already exists
    if (chek_user(*list, data.ID) == 0) {
        // Encrypt the password
        char *encrypted_password = strdup(data.password); // Duplicate the original password
        if (encrypted_password == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(temp);
            return;
        }
        encryption(encrypted_password); // Encrypt the password
        temp->password = encrypted_password; // Store the encrypted password

        // Store the user data
        temp->ID = strdup(data.ID);
        temp->name = strdup(data.name);
        temp->current_balance = data.current_balance;

        // Check memory allocation for ID and name
        if (temp->ID == NULL || temp->name == NULL) {
            fprintf(stderr, "Memory allocation failed\n");
            free(temp->ID);
            free(temp->name);
            free(temp->password);
            free(temp);
            return;
        }

        // Set up the doubly linked list pointers
        temp->next_user = (*list); // Point to the current head
        temp->priv_user = NULL;    // New user will be the first user, no previous user

        // If the list is not empty, update the previous head's priv_user pointer
        if (*list != NULL) {
            (*list)->priv_user = temp;
        }

        // Update the head of the list to the new user
        *list = temp;

        // Save to CSV
        save_to_csv(*list, "data.csv");
    } else {
        printf("User with ID %s already exists.\n", data.ID);
        free(temp);
    }
}


/**
 * @brief Function to read from the csv file and add the users to the linked list
 *@param filename
 */
void read_from_csv(const char *filename) {
    system("cls");
    users temp_user;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    char line[200];
    int count = 0;

    // Skip header line
    if (fgets(line, sizeof(line), file) == NULL) {
        perror("Error reading header line from file");
        fclose(file);
        return;
    }

    // Read each line
    while (fgets(line, sizeof(line), file) != NULL) {
        // Allocate memory for each field in the struct
        temp_user.ID = malloc(50 * sizeof(char));
        temp_user.name = malloc(50 * sizeof(char));
        temp_user.password = malloc(50 * sizeof(char));

        if (temp_user.ID == NULL || temp_user.name == NULL || temp_user.password == NULL) {
            perror("Memory allocation failed");
            fclose(file);
            return;
        }

        // Parse the line
        int fields_parsed = sscanf(line, "%49[^,],%49[^,],%49[^,],%d",
                                    temp_user.ID, temp_user.name,
                                    temp_user.password, &temp_user.current_balance);

        if (fields_parsed != 4) {
            fprintf(stderr, "Warning: Malformed line %d: %s", count + 1, line);
            free(temp_user.ID);
            free(temp_user.name);
            free(temp_user.password);
            continue;
        }

        // Add users from csv to the linked list
        add_user(&head, temp_user);

        // Free the allocated memory for the temporary user
        free(temp_user.ID);
        free(temp_user.name);
        free(temp_user.password);
        count++;
    }

    fclose(file);
}


/**
 * @brief Function to save the users to the csv file
 * @param users_list
 * @param filename
 */
static void save_to_csv(users *users_list, const char *filename) {
    system("cls");
    FILE *file = fopen(filename, "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Write the header
    fprintf(file, "ID,Name,Password,Balance\n");

    // Write each user, but avoid adding an extra newline at the end
    users *current = users_list;
    while (current != NULL) {
        // Check if this is the last user in the list
        if (current->next_user == NULL) {
            // Last user: no newline after their data
            fprintf(file, "%s,%s,%s,%d", current->ID, current->name, current->password, current->current_balance);
        } else {
            // Not the last user: add a newline after their data
            fprintf(file, "%s,%s,%s,%d\n", current->ID, current->name, current->password, current->current_balance);
        }
        current = current->next_user;
    }

    fclose(file);
}


/**
 * @brief Function to display the window for the user to check their balance
 * @param users_list
 */
void balance_inquiry(users *users_list){
    users *current = users_list;
    system("cls");
    int back = 0;
    printf("Balance: %d\n", users_list -> current_balance);
    printf("=============================\n");
    printf("1. Go back\n");
    scanf("%d", &back);
    if (back == 1){
        display_menu(current);
    }
    
}


/**
 * @brief Function to display the window for the user to deposit funds
 * @param users_list
 */
void display_menu(users *users_list){
    system("cls");
    int choice = 0;
    printf("%s, Welcome to the bank\n", users_list -> name);
    printf("=============================\n");
    printf("1. balance_inquiry\n");
    printf("2. Deposit\n");
    printf("3. Withdraw\n");
    printf("4. Transfer\n");
    printf("5. Deposit History\n");
    printf("6. Exit\n");
    printf("=============================\n");
    printf("Enter your choice: ");
    if (scanf("%d", &choice) != 1) {
        printf(ANSI_COLOR_RED "Invalid input. Please enter an integer.\n" ANSI_COLOR_RESET);
    } else {
        switch (choice){
            case 1: balance_inquiry(users_list); break;
            case 2: deposit_funds(&trans_head, users_list); break;
            case 3: withdraw_funds(&trans_head, users_list); break;
            case 4: transfer_funds(&trans_head, users_list); break;
            case 5:
                char file_name[20];
                snprintf(file_name, sizeof(file_name), "%s.csv", users_list->ID);
                deposit_history(file_name, users_list); 
                break;
            case 6: printf("Exiting...\n"); break;
            default: printf(ANSI_COLOR_RED "Invalid choice. Please enter a number between 1 and 6.\n" ANSI_COLOR_RESET);
        }
    }
    
}


/**
 * @brief Function to authenticate the user
 * @param users_list
 */
void authenticate_user(users *users_list) {
    system("cls");
    char ID[50];
    char password[50];
    int authenticated = 0;

    printf("Enter your ID: ");
    scanf("%49s", ID);  // Prevent overflow
    printf("Enter your password: ");
    scanf("%49s", password);  // Prevent overflow

    // Duplicate the entered password
    char *encrypted_input_password = strdup(password);
    if (encrypted_input_password == NULL) {
        fprintf(stderr, "Memory allocation failed\n");
        return;
    }

    // Encrypt the entered password
    encryption(encrypted_input_password);

    // Check credentials
    users *current = users_list;
    while (current != NULL) {
        if (strcmp(current->ID, ID) == 0 && strcmp(current->password, encrypted_input_password) == 0) {
            printf(ANSI_COLOR_GREEN "Authentication successful\n" ANSI_COLOR_RESET);
            display_menu(current);
            authenticated = 1;
            break;
        }
        current = current->next_user;
    }

    free(encrypted_input_password); // Free the duplicated encrypted password

    if (!authenticated) {
        printf(ANSI_COLOR_RED "Authentication failed\n" ANSI_COLOR_RESET);
    }
}


// depugging function to be removed
/**
 * @brief Function to print the linked list
 * @param user
 */
static void print_list(users *user) {
    //system("cls");
    users *current = head;
    while (current != NULL) {
        printf("ID: '%s', Password: '%s', Balance: %d\n", current->ID, current->password, current->current_balance);
        current = current -> next_user;
    }
}
