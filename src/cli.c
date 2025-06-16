#include "cli.h"
#include "auth.h"

// Include necessary constants
static medical_transaction_t pending_transaction;
static int has_pending_transaction = 0;

// Function to display the main menu based on user role
void show_menu(user_role_t role) {
    printf("\n=== BlockMed - Medical Record Blockchain ===\n");
    printf("Role: %s\n", role_to_string(role));
    printf("1. Add new Medical Record\n");
    printf("2. Mine pending block\n");
    printf("3. View Blockchain\n");
    printf("4. Validate chain integrity\n");
    printf("5. Save Blockchain\n");
    printf("6. Load Blockchain\n");
    printf("7. Set Mining Difficulty\n");
    printf("8. Exit\n");
    printf("Select an option: ");
}

// Function to handle adding a new medical record
void handle_add_record(blockchain_t *chain, const user_t *user) {
    if (!has_write_permission(user->role)) {
        printf("You do not have permission to add records.\n");
        log_security_event(user->email, "Attempted to add record without permission");
        return;
    }

    // Ensure the blockchain is initialized
    char patient_id[50], diagnosis[MAX_DIAGNOSIS_SIZE];
    char prescription[MAX_PRESCRIPTION_SIZE], visit_note[MAX_NOTES_SIZE];

    printf("\n=== Add New Medical Record ===\n");
    printf("Patient ID: ");
    secure_input(patient_id, sizeof(patient_id));

    printf("Diagnosis: ");
    secure_input(diagnosis, sizeof(diagnosis));

    printf("Prescription: ");
    secure_input(prescription, sizeof(prescription));

    printf("Visit Note: ");
    secure_input(visit_note, sizeof(visit_note));

    // Create a new transaction
    create_transaction(&pending_transaction, patient_id, user->email, diagnosis, prescription, visit_note);

    has_pending_transaction = 1;

    // Create a new block with the transaction
    printf("Medical record created and ready to be mined.\n");
    log_operation(LOG_INFO, user->email, "Created new medical record");
}

// Function to handle mining a block
void handle_mine_block(blockchain_t *chain, const user_t *user) {
    if (!has_write_permission(user->role)) {
        printf("You do not have permission to mine blocks.\n");
        log_security_event(user->email, "Attempted to mine block without permission");
        return;
    }

    // Check if there is a pending transaction to mine
    if (!has_pending_transaction) {
        printf("No pending transactions to mine.\n");
        return;
    }

    // Ensure the blockchain is initialized
    block_t *new_block = create_block(chain->length, &pending_transaction, chain->tail->current_hash);

    // Check if the block was created successfully
    if (!new_block) {
        printf("Failed to create block.\n");
        return;
    }

    // Set the timestamp for the new block
    if (mine_block(new_block, get_mining_difficulty())) {
        add_block_to_chain(chain, new_block);
        has_pending_transaction = 0;
        printf("Block successfully mined and added to the blockchain.\n");
        log_operation(LOG_INFO, user->email, "Successfully mined a block");
    } else {
        free(new_block);
        printf("Failed to mine block. Please try again.\n");
    }
}

// Function to handle viewing the blockchain
void handle_view_blockchain(const blockchain_t *chain, const user_t *user) {
    print_blockchain(chain);
    log_operation(LOG_INFO, user->email, "Viewed blockchain");
}

// Function to handle validating the blockchain integrity
void handle_validate_chain(const blockchain_t *chain, const user_t *user) {
    printf("\n Validating Blockchain Integrity..\n");

    // Ensure the blockchain is initialized
    if (validate_blockchain(chain)) {
        printf("✓ Blockchain is valid and secure.\n");
    } else {
        printf("✗ Blockchain integrity compromised!\n");
        log_security_event(user->email, "Blockchain integrity check failed");
    }
    log_operation(LOG_INFO, user->email, "Performed blockchain integrity validation");
}

// Function to handle user login
void handle_user_login(user_t *user) {
    char email[MAX_EMAIL_SIZE], password[HASH_SIZE];

    printf("\n=== BlockMed Login ===\n");
    printf("Email: ");
    secure_input(email, sizeof(email));

    printf("Password: ");
    secure_input(password, sizeof(password));

    if (authenticate_user(email, password, user)) {
        printf("Login successful! Welcome %s (%s)\n", user->email, role_to_string(user->role));
    } else {
        printf("Login failed! Invalid email or password.\n");
        log_security_event(email, "Failed login attempt");
        memset(user, 0, sizeof(user_t));
        user->role = ROLE_INVALID;
    }
}

// Function to handle user registration
void handle_user_registration(void) {
    char email[MAX_EMAIL_SIZE], password[MAX_PASSWORD_SIZE];

    printf("\n=== User Registration ===\n");
    printf("Email (must be ALU domain): ");
    secure_input(email, sizeof(email));

    printf("Password: ");
    secure_input(password, sizeof(password));

    // registration checks
    if (register_user(email, password)) {
        printf("Registration successful! You can now log in.\n");
    } else {
        printf("Registration failed! Please try again.\n");
        log_security_event(email, "Failed registration attempt");
    }
}

// Main CLI function to run the application
int run_cli(blockchain_t *chain) {
    user_t current_user = {0};
    current_user.role = ROLE_INVALID;

    while (1) {
        printf("\n1. Login\n2. Register\n3. Exit\nChoice: ");

        char choice[10];
        secure_input(choice, sizeof(choice));

        switch (choice[0]) {
            case '1':
                handle_user_login(&current_user);
                break;
            case '2':
                handle_user_registration();
                break;
            case '3':
                printf("Exiting BlockMed CLI. Goodbye!\n");
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
        }
    }

    //main application loop
    char choice[10];
    while (1) {
        show_menu(current_user.role);
        secure_input(choice, sizeof(choice));

        switch (choice[0]) {
            case '1':
                handle_add_record(chain, &current_user);
                break;
            case '2':
                handle_mine_block(chain, &current_user);
                break;
            case '3':
                handle_view_blockchain(chain, &current_user);
                break;
            case '4':
                handle_validate_chain(chain, &current_user);
                break;
            case '5':
                if (save_blockchain(chain, "data/blockchain.dat")) {
                    printf("Blockchain saved successfully.\n");
                    log_operation(LOG_INFO, current_user.email, "Saved blockchain to file");
                } else {
                    printf("Failed to save blockchain.\n");
                }
                break;
            case '6':
                if (has_full_permission(current_user.role)) {
                    blockchain_t *loaded_chain = load_blockchain("data/blockchain.dat");
                    if (loaded_chain) {
                        free_blockchain(chain);
                        chain = loaded_chain;
                        printf("Blockchain loaded successfully.\n");
                        log_operation(LOG_INFO, current_user.email, "Loaded blockchain from file");
                    } else {
                        printf("Failed to load blockchain.\n");
                    }
                } else {
                    printf("You do not have permission to load the blockchain.\n");
                    log_security_event(current_user.email, "Attempted to load blockchain without permission");
                }
                break;
            case '7':
                if (has_full_permission(current_user.role)) {
                    printf("Current Mining Difficulty: %d\n", get_mining_difficulty());
                    printf("Enter new mining difficulty (1-8): ");
                    int new_difficulty;

                    if (scanf("%d", &new_difficulty) == 1) {
                        set_mining_difficulty(new_difficulty);
                        printf("Mining difficulty set to %d.\n", new_difficulty);
                        log_operation(LOG_INFO, current_user.email, "Set mining difficulty");
                    }
                    while (getchar() != '\n'); // clear input buffer
                } else {
                    printf("You do not have permission to change mining difficulty.\n");
                    log_security_event(current_user.email, "Attempted to change mining difficulty without permission");
                }
                break;
            case '8':
                printf("Exiting BlockMed CLI. Goodbye!\n");
                free_blockchain(chain);
                return 0;
            default:
                printf("Invalid choice. Please try again.\n");
                break;
            }
        }
    return 0;
}


