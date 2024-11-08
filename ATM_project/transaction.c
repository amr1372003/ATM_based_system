#include "transaction.h"

transactions *trans_head;

static void save_transaction(transactions *list, char *ID);
static void save_to_csv(transactions *list, const char *filename);
static void update_data_file(users *users_list, const char *selected_user_id);
static users *find_user_by_id(transactions *users_list, const char *user_id);
static int transaction_exists(const char *filename, const char *transaction_id);
static void generate_random_id(char *id);
static int is_id_unique(const char *id);


#define ID_LENGTH 4
#define MAX_IDS 1000

char generated_ids[MAX_IDS][ID_LENGTH];


/**
 * @brief Function to check if an ID is unique
 * @param id
 */
static int is_id_unique(const char *id) {
    for (int i = 0; i < MAX_IDS; i++) {
        if (strcmp(generated_ids[i], id) == 0) {
            return 0; // ID already exists
        }
    }
    return 1; // ID is unique
}


/**
 * @brief Function to generate a random ID
 * @param id
 */
static void generate_random_id(char *id) {
    // Define characters to use in the random ID
    char chars[] = "ABCDEFGHIJKLMNOPQRSTUVWXYZabcdefghijklmnopqrstuvwxyz0123456789";
    int num_chars = sizeof(chars) - 1;

    srand(time(NULL));

    do {
        for (int i = 0; i < ID_LENGTH - 1; i++) {
            id[i] = chars[rand() % num_chars];
        }
        id[ID_LENGTH - 1] = '\0';
    } while (!is_id_unique(id));

    for (int i = 0; i < MAX_IDS; i++) {
        if (generated_ids[i][0] == '\0'){
            strcpy(generated_ids[i], id);
            break;
        }
    }
}


/**
 * @brief Function to check if a transaction already exists
 * @param filename
 * @param transaction_id
 */
static int transaction_exists(const char *filename, const char *transaction_id) {
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        return 0; // File doesn't exist, so return 0 (not found)
    }

    char line[256];
    while (fgets(line, sizeof(line), file)) {
        // If the transaction ID already exists, return 1 (found)
        if (strstr(line, transaction_id) != NULL) {
            fclose(file);
            return 1;
        }
    }

    fclose(file);
    return 0; // Transaction ID not found
}


/**
 * @brief Function to find a user by their ID
 * @param users_list
 * @param user_id
 * @return current user
 */
static users *find_user_by_id(transactions *users_list, const char *user_id) {
    users *current = users_list;
    while (current != NULL) {
        if (strcmp(current -> ID, user_id) == 0){
            return current;
        }
        current = current->next_user;
    }
    return NULL;
}


/**
 * @brief Function to update the data file with the new user balance
 * @param users_list
 * @param selected_user_id
 */
static void update_data_file(users *users_list, const char *selected_user_id) {
    FILE *file = fopen("data.csv", "r");
    if (file == NULL) {
        perror("Error opening file for reading");
        return;
    }

    // Read the file content into memory (array of lines)
    char lines[100][200];  // Array to hold lines (adjust as necessary)
    int line_count = 0;

    // Read the header line
    if (fgets(lines[line_count], sizeof(lines[line_count]), file) == NULL) {
        perror("Error reading file");
        fclose(file);
        return;
    }
    line_count++;

    // Read the rest of the file into memory
    while (fgets(lines[line_count], sizeof(lines[line_count]), file) != NULL) {
        line_count++;
    }

    fclose(file);  // Close file after reading

    // Now update the data for the selected user in the memory (array of lines)
    int updated = 0;
    for (int i = 1; i < line_count; i++){
        char id[50];
        sscanf(lines[i], "%49[^,],", id);  // Extract ID from the line

        if (strcmp(id, selected_user_id) == 0) {
            // Update the user's data if the ID matches
            users *current = users_list;
            while (current != NULL) {
                if (strcmp(current->ID, selected_user_id) == 0) {
                    // Found the user, update their information
                    if (current->next_user == NULL) {
                        // Last user: no newline after their data
                        sprintf(lines[i], "%s,%s,%s,%d", current->ID, current->name, current->password, current->current_balance);
                    } else {
                        // Not the last user: add a newline after their data
                        sprintf(lines[i], "%s,%s,%s,%d\n", current->ID, current->name, current->password, current->current_balance);
                    }
                    updated = 1;
                    break;
                }
                current = current->next_user;
            }
            if (updated) break;
        }
    }

    if (!updated) {
        printf("User with ID %s not found.\n", selected_user_id);
        return;
    }

    // Write the updated data back to the file
    file = fopen("data.csv", "w");
    if (file == NULL) {
        perror("Error opening file for writing");
        return;
    }

    // Write the header only once (only at the start of the file)
    fprintf(file, "ID,Name,Password,Balance\n");

    // Write all the lines (including the updated one)
    for (int i = 1; i < line_count; i++) {
        fprintf(file, "%s", lines[i]);
    }

    fclose(file);
}


/**
 * @brief Function to initialize the transaction list
 * @param trans_head
 * @param users_list
 */
void initialize_transaction(transactions **trans_head, users *users_list){
    if (*trans_head == NULL) {
        *trans_head = (transactions *)malloc(sizeof(transactions));
        if (*trans_head != NULL) {
            (*trans_head)->next_transaction = NULL;
            (*trans_head)->prev_transaction = NULL;
            strcpy((*trans_head)->transaction_type, "-");  // Set default values
            strcpy((*trans_head)->user_id, "-");
            strcpy((*trans_head)->transaction_id, "-");
            (*trans_head)->amount = 0;
        } else {
            perror(ANSI_COLOR_RED "Error initializing transaction list\n" ANSI_COLOR_RESET);
        }
    }
}


/**
 * @brief Function to save the users to the csv file
 * @param users_list
 * @param filename
 */
static void save_to_csv(transactions *list, const char *filename) {
    transactions *current = list->next_transaction; // Skip dummy head node

    // Loop through transactions and process each one
    while (current != NULL) {
        char user_filename[50];
        snprintf(user_filename, sizeof(user_filename), "%s.csv", current->user_id); // Unique file for each user

        // Open the file in append mode
        FILE *file = fopen(user_filename, "a");
        if (file == NULL) {
            perror("Error opening file for writing");
            return;
        }

        // Check if the file is new by checking if it's empty
        fseek(file, 0, SEEK_END);
        if (ftell(file) == 0) {
            // File is empty, so add the header
            fprintf(file, "Transaction ID,Transaction Type,User ID,Amount\n");
        }

        // Check if the transaction has already been written to avoid duplicates
        // (In this case, we check based on the transaction ID, which should be unique)
        if (!transaction_exists(user_filename, current->transaction_id)) {
            // Write the transaction data for the current user
            if (current->next_transaction == NULL) {
            // Last user: no newline after their data
                fprintf(file, "%s,%s,%s,%d\n",
                        current->transaction_id,
                        current->transaction_type,
                        current->user_id,
                        current->amount);
            } else {
                fprintf(file, "%s,%s,%s,%d",
                    current->transaction_id,
                    current->transaction_type,
                    current->user_id,
                    current->amount);
            }
        }

        fclose(file);
        current = current->next_transaction;
    }
}


/**
 * @brief Function to deposit funds to the user's account
 * @param list
 * @param users_list
 */
void deposit_funds(transactions **list, users *users_list) {
    system("cls");
    int back = 0;

    // Ensure the list is initialized
    if (*list == NULL) {
        printf(ANSI_COLOR_RED "Warning: Transaction list is empty, creating a new list.\n" ANSI_COLOR_RESET);
    }else{
        // Allocate memory for a new transaction
        transactions *new_transaction = (transactions *)malloc(sizeof(transactions));
        if (new_transaction == NULL) {
            perror(ANSI_COLOR_RED "Error allocating memory" ANSI_COLOR_RESET);
            return;
        }else{
            // Prompt for deposit amount
            printf("Enter the amount to deposit: ");
            scanf("%d", &(new_transaction->amount));

            // Set transaction type and other details
            strcpy(new_transaction->transaction_type, "Deposit");
            strcpy(new_transaction->user_id, users_list->ID);
            generate_random_id(new_transaction->transaction_id);
            //strcpy(new_transaction->transaction_id, "AAA");    // Replace with unique ID generation

            // Initialize pointers for the transaction
            new_transaction->next_transaction = NULL;
            new_transaction->prev_transaction = NULL;

            // Append transaction to the list
            if (*list == NULL) {
                // Start a new list with this transaction
                *list = new_transaction;
            } else {
                // Traverse to the end of the list
                transactions *current = *list;
                while (current->next_transaction != NULL) {
                    current = current->next_transaction;
                }

                // Link the new transaction at the end of the list
                current->next_transaction = new_transaction;
                new_transaction->prev_transaction = current;
            }

            // Save the transaction list to a CSV file or database
            save_transaction(*list, users_list -> ID);

            // Print completion message
            printf(ANSI_COLOR_GREEN "Transaction saved successfully!\n" ANSI_COLOR_RESET);

            // Update the user's balance
            users_list->current_balance += new_transaction->amount;  // Update the balance
            printf("Your new balance is: %d\n", users_list->current_balance);

            // Prompt user to go back
            update_data_file(users_list, users_list -> ID);
        }
    }
    printf("=============================\n");
    printf("1. Go back\n");
    scanf("%d", &back);
    if (back == 1) {
        display_menu(users_list);
    }
}


/**
 * @brief Function to withdraw funds from the user's account
 * @param list
 * @param users_list
 */
void withdraw_funds(transactions **list, users *users_list){
    system("cls");
    int back = 0;

    // Ensure the list is initialized
    if (*list == NULL) {
        printf(ANSI_COLOR_RED "Warning: withdraw list is empty, creating a new list.\n" ANSI_COLOR_RESET);
    }else{
        // Allocate memory for a new transaction
        transactions *new_transaction = (transactions *)malloc(sizeof(transactions));
        if (new_transaction == NULL) {
            perror(ANSI_COLOR_RED "Error allocating memory" ANSI_COLOR_RESET);
        }else{
            // Prompt for deposit amount
            printf("Enter the amount to withdraw: ");
            scanf("%d", &(new_transaction->amount));

            // Set transaction type and other details
            strcpy(new_transaction->transaction_type, "withdraw");
            strcpy(new_transaction->user_id, users_list->ID);
            generate_random_id(new_transaction->transaction_id);
            //strcpy(new_transaction->transaction_id, "BBB");    // Replace with unique ID generation

            // Initialize pointers for the transaction
            new_transaction->next_transaction = NULL;
            new_transaction->prev_transaction = NULL;

            // Append transaction to the list
            if (*list == NULL) {
                // Start a new list with this transaction
                *list = new_transaction;
            } else {
                // Traverse to the end of the list
                transactions *current = *list;
                while (current->next_transaction != NULL) {
                    current = current->next_transaction;
                }

                // Link the new transaction at the end of the list
                current->next_transaction = new_transaction;
                new_transaction->prev_transaction = current;
            }

            // Save the transaction list to a CSV file or database
            save_transaction(*list, users_list -> ID);

            // Print completion message
            printf(ANSI_COLOR_GREEN "withdraw saved successfully!\n" ANSI_COLOR_RESET);

            // Update the user's balance
            users_list->current_balance -= new_transaction->amount;  // Update the balance
            printf("Your new balance is: %d\n", users_list->current_balance);

            // Prompt user to go back
            update_data_file(users_list, users_list -> ID);
        }
    }
    printf("=============================\n");
    printf("1. Go back\n");
    scanf("%d", &back);
    if (back == 1) {
        display_menu(users_list);
    }
}


/**
 * @brief Function to transfer funds from the user's account
 * @param list
 * @param users_list
 */
void transfer_funds(transactions **list, users *users_list) {
    system("cls");
    int back = 0;
    char receive_ID[20];
    int amount = 0;
    transactions *temp = *list;
    users *temp_users = users_list;

    printf("Enter the recipient's ID: ");
    scanf("%s", receive_ID);
    // Find recipient in the user list and create transaction for them (deposit)
    users *recipient_user = find_user_by_id(head ,receive_ID);

    if (*list == NULL) {
        printf(ANSI_COLOR_RED "Warning: transaction list is empty, creating a new list.\n" ANSI_COLOR_RESET);
        printf("=============================\n");
        printf("1. Go back\n");
        scanf("%d", &back);
        if (back == 1) {
            display_menu(users_list);
        }
    }

    if (recipient_user != NULL)
    {
        // Prompt for transfer amount
        printf("Enter the amount to send: ");
        scanf("%d", &amount);

        // Check if balance is sufficient
        if (users_list->current_balance < amount) {
            printf(ANSI_COLOR_RED "Error: Insufficient funds.\n" ANSI_COLOR_RESET);
            printf("=============================\n");
            printf("1. Go back\n");
            scanf("%d", &back);
            if (back == 1) {
                display_menu(users_list);
            }
        }

        // Create transaction for sender (transfer)
        transactions *sender_transaction = (transactions *)malloc(sizeof(transactions));
        if (sender_transaction == NULL) {
            perror(ANSI_COLOR_RED "Error allocating memory" ANSI_COLOR_RESET);
            printf("=============================\n");
            printf("1. Go back\n");
            scanf("%d", &back);
            if (back == 1) {
                display_menu(users_list);
            }
        }

        // Fill in sender transaction details
        sender_transaction->amount = amount;
        strcpy(sender_transaction->transaction_type, "transfer");
        strcpy(sender_transaction->user_id, users_list->ID);
        generate_random_id(sender_transaction->transaction_id);
        //strcpy(sender_transaction->transaction_id, "TTT");  // Replace with unique ID generation
        sender_transaction->next_transaction = NULL;
        sender_transaction->prev_transaction = NULL;

        // Append sender transaction to the list
        if (*list == NULL) {
            *list = sender_transaction;
        } else {
            transactions *current = *list;
            while (current->next_transaction != NULL) {
                current = current->next_transaction;
            }
            current->next_transaction = sender_transaction;
            sender_transaction->prev_transaction = current;
        }

        // Update the sender's balance
        users_list->current_balance -= amount;
        printf("Your new balance is: %d\n", users_list->current_balance);
        save_transaction(*list, users_list->ID);
        update_data_file(users_list, users_list->ID);
        printf(ANSI_COLOR_GREEN "Transfer saved successfully!\n" ANSI_COLOR_RESET);

        // Create transaction for recipient (deposit)
        transactions *recipient_transaction = (transactions *)malloc(sizeof(transactions));
        if (recipient_transaction == NULL) {
            perror(ANSI_COLOR_RED "Error allocating memory" ANSI_COLOR_RESET);
            printf("=============================\n");
            printf("1. Go back\n");
            scanf("%d", &back);
            if (back == 1) {
                display_menu(users_list);
            }
        }

        // Fill in recipient transaction details
        recipient_transaction->amount = amount;
        strcpy(recipient_transaction->transaction_type, "deposit");
        strcpy(recipient_transaction->user_id, receive_ID);
        strcpy(recipient_transaction->transaction_id, sender_transaction->transaction_id);
        recipient_transaction->next_transaction = NULL;
        recipient_transaction->prev_transaction = NULL;

        // Append recipient transaction to the list
        transactions *current = *list;
        while (current->next_transaction != NULL) {
            current = current->next_transaction;
        }
        current->next_transaction = recipient_transaction;
        recipient_transaction->prev_transaction = current;

        // Update the recipient's balance
        recipient_user->current_balance += amount;
        printf("Recipient's new balance updated.\n");
        save_transaction(*list, receive_ID);
        update_data_file(recipient_user, receive_ID);
        printf(ANSI_COLOR_GREEN "Transaction saved successfully!\n" ANSI_COLOR_RESET);
    } else {
        printf(ANSI_COLOR_RED "Error: Recipient not found.\n" ANSI_COLOR_RESET);
    }

    printf("=============================\n");
    printf("1. Go back\n");
    scanf("%d", &back);
    if (back == 1) {
        display_menu(users_list);
    }
}


/**
 * @brief Function to display the transaction history
 * @param filename
 */ 
void deposit_history(const char *filename, users *users_list) {
    // Open file for reading
    system("cls");
    int back = 0;
    FILE *file = fopen(filename, "r");
    if (file == NULL) {
        perror("\033[0;31mError opening file for reading\033[0m"); // Red text for error
    }else{
        char line[200];
        int count = 0;

        // Print table header
        printf("Transaction ID   Transaction Type    User ID    Amount\n");
        printf("-------------------------------------------------------\n");

        // Skip the first line if it’s the header in the CSV
        if (fgets(line, sizeof(line), file) == NULL) {
            printf("The file is empty or an error occurred while reading.\n");
            fclose(file);
            return;
        }

        // Print transaction data
        while (fgets(line, sizeof(line), file) != NULL) {
            line[strcspn(line, "\n")] = '\0';

            // Variables to hold each field
            char transaction_id[50];
            char transaction_type[50];
            char user_id[50];
            int amount;

            // Parse the line into fields
            int fields_parsed = sscanf(line, "%49[^,],%49[^,],%49[^,],%d",
                                        transaction_id, transaction_type, user_id, &amount);

            // Check for correct parsing of four fields
            if (fields_parsed == 4) {
                // Print each field in a fixed-width format for better readability
                printf("%-16s %-19s %-10s %d\n", transaction_id, transaction_type, user_id, amount);
                count++;
            } else {
                fprintf(stderr,ANSI_COLOR_RED "Warning: Malformed line %d: %s\n" ANSI_COLOR_RESET, count + 1, line);
            }
        }

        fclose(file);

        // If no transactions were found after the header
        if (count == 0) {
            printf(ANSI_COLOR_RED "No transaction data found in the file.\n" ANSI_COLOR_RESET);
        }
    }

    printf("=============================\n");
    printf("1. Go back\n");
    scanf("%d", &back);
    if (back == 1) {
        display_menu(users_list);
    }
}


/**
 * @brief Function to save the transaction list to a CSV file
 * @param list
 */
static void save_transaction(transactions *list, char *ID) {
    if (list == NULL || list->next_transaction == NULL) {
        printf(ANSI_COLOR_RED "Transaction list is empty. Nothing to save.\n" ANSI_COLOR_RESET);
        return;
    }else{
        char filename[100];
        snprintf(filename, sizeof(filename), "%s.csv", ID);
        save_to_csv(list, filename);
    }
}
