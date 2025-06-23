#include "cli.h"
#include "auth.h"

// ANSI Color codes for beautiful terminal output
#define RESET_COLOR     "\033[0m"
#define BOLD            "\033[1m"
#define DIM             "\033[2m"
#define UNDERLINE       "\033[4m"

// Colors
#define RED             "\033[31m"
#define GREEN           "\033[32m"
#define YELLOW          "\033[33m"
#define BLUE            "\033[34m"
#define MAGENTA         "\033[35m"
#define CYAN            "\033[36m"
#define WHITE           "\033[37m"
#define BRIGHT_GREEN    "\033[92m"
#define BRIGHT_BLUE     "\033[94m"
#define BRIGHT_CYAN     "\033[96m"
#define BRIGHT_WHITE    "\033[97m"

// Background colors
#define BG_BLUE         "\033[44m"
#define BG_GREEN        "\033[42m"
#define BG_RED          "\033[41m"

// Include necessary constants
static medical_transaction_t pending_transaction;
static int has_pending_transaction = 0;

// Utility functions for beautiful UI
void print_header(const char* title) {
    printf("\n");
    printf(CYAN "╔══════════════════════════════════════════════════════════════╗\n" RESET_COLOR);
    printf(CYAN "║" BOLD BRIGHT_WHITE "%-62s" RESET_COLOR CYAN "║\n" RESET_COLOR, title);
    printf(CYAN "╚══════════════════════════════════════════════════════════════╝\n" RESET_COLOR);
    printf("\n");
}

void print_section_header(const char* title) {
    printf("\n" BRIGHT_CYAN "▓▓▓ " BOLD "%s" RESET_COLOR BRIGHT_CYAN " ▓▓▓" RESET_COLOR "\n\n");
}

void print_success(const char* message) {
    printf(BRIGHT_GREEN "✓ " BOLD "%s" RESET_COLOR "\n", message);
}

void print_error(const char* message) {
    printf(RED "✗ " BOLD "%s" RESET_COLOR "\n", message);
}

void print_warning(const char* message) {
    printf(YELLOW "⚠ " BOLD "%s" RESET_COLOR "\n", message);
}

void print_info(const char* message) {
    printf(BRIGHT_BLUE "ℹ " BOLD "%s" RESET_COLOR "\n", message);
}

void print_separator() {
    printf(DIM "─────────────────────────────────────────────────────────────\n" RESET_COLOR);
}

void print_menu_option(int number, const char* option, const char* description) {
    printf(BRIGHT_WHITE "  [" BRIGHT_CYAN "%d" BRIGHT_WHITE "] " 
           BOLD "%s" RESET_COLOR DIM " - %s" RESET_COLOR "\n", 
           number, option, description);
}

// Function to display the main menu based on user role
void show_menu(user_role_t role) {
    system("clear"); // Clear screen for better presentation
    
    print_header("🏥 BLOCKMED - MEDICAL RECORD BLOCKCHAIN SYSTEM 🏥");
    
    printf(BRIGHT_WHITE "Current User Role: " BOLD);
    switch(role) {
        case ROLE_STAFF:
            printf(GREEN "👨‍⚕️ DOCTOR" RESET_COLOR);
            break;
        case ROLE_INTERN:
            printf(BLUE "👩‍⚕️ NURSE" RESET_COLOR);
            break;
        case ROLE_STUDENT:
            printf(MAGENTA "👑 ADMINISTRATOR" RESET_COLOR);
            break;
        default:
            printf(RED "❌ INVALID" RESET_COLOR);
    }
    printf("\n\n");
    
    print_section_header("📋 MAIN MENU OPTIONS");
    
    print_menu_option(1, "📝 Add Medical Record", "Create new patient medical record");
    print_menu_option(2, "⛏️  Mine Pending Block", "Process and secure pending transactions");
    print_menu_option(3, "🔗 View Blockchain", "Display complete blockchain history");
    print_menu_option(4, "🔍 Validate Chain", "Check blockchain integrity and security");
    print_menu_option(5, "💾 Save Blockchain", "Export blockchain to file");
    print_menu_option(6, "📂 Load Blockchain", "Import blockchain from file");
    print_menu_option(7, "⚙️  Mining Difficulty", "Adjust blockchain mining parameters");
    print_menu_option(8, "🚪 Exit System", "Logout and close application");
    
    print_separator();
    printf(BRIGHT_WHITE "Enter your choice: " CYAN);
}

// Function to handle adding a new medical record
void handle_add_record(blockchain_t *chain, const user_t *user) {
    print_header("📝 ADD NEW MEDICAL RECORD");
    
    if (!has_write_permission(user->role)) {
        print_error("Access Denied: You do not have permission to add records.");
        log_security_event(user->email, "Attempted to add record without permission");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }

    char patient_id[50], diagnosis[MAX_DIAGNOSIS_SIZE];
    char prescription[MAX_PRESCRIPTION_SIZE], visit_note[MAX_NOTES_SIZE];

    printf(BRIGHT_WHITE "Patient ID: " CYAN);
    secure_input(patient_id, sizeof(patient_id));
    
    printf(BRIGHT_WHITE "Diagnosis: " CYAN);
    secure_input(diagnosis, sizeof(diagnosis));
    
    printf(BRIGHT_WHITE "Prescription: " CYAN);
    secure_input(prescription, sizeof(prescription));
    
    printf(BRIGHT_WHITE "Visit Notes: " CYAN);
    secure_input(visit_note, sizeof(visit_note));
    
    printf(RESET_COLOR);

    // Create a new transaction
    create_transaction(&pending_transaction, patient_id, user->email, diagnosis, prescription, visit_note);
    has_pending_transaction = 1;

    print_separator();
    print_success("Medical record created successfully!");
    print_info("Record is now pending and ready to be mined.");
    log_operation(LOG_INFO, user->email, "Created new medical record");
    
    printf("\nPress Enter to continue...");
    getchar();
}

// Function to handle mining a block
void handle_mine_block(blockchain_t *chain, const user_t *user) {
    print_header("⛏️ MINE PENDING BLOCK");
    
    if (!has_write_permission(user->role)) {
        print_error("Access Denied: You do not have permission to mine blocks.");
        log_security_event(user->email, "Attempted to mine block without permission");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }

    if (!has_pending_transaction) {
        print_warning("No pending transactions available for mining.");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }

    printf(YELLOW "🔄 Initializing mining process...\n" RESET_COLOR);
    printf(BRIGHT_WHITE "Mining difficulty: " CYAN "%d\n" RESET_COLOR, get_mining_difficulty());
    
    block_t *new_block = create_block(chain->length, &pending_transaction, chain->tail->current_hash);

    if (!new_block) {
        print_error("Failed to create block structure.");
        printf("\nPress Enter to continue...");
        getchar();
        return;
    }

    printf(YELLOW "⚡ Mining in progress" RESET_COLOR);
    for(int i = 0; i < 3; i++) {
        printf(YELLOW "." RESET_COLOR);
        fflush(stdout);
        usleep(500000); // 0.5 second delay for visual effect
    }
    printf("\n");

    if (mine_block(new_block, get_mining_difficulty())) {
        add_block_to_chain(chain, new_block);
        has_pending_transaction = 0;
        print_success("Block successfully mined and added to blockchain!");
        printf(BRIGHT_GREEN "🎉 New block hash: " CYAN "%s\n" RESET_COLOR, new_block->current_hash);
        log_operation(LOG_INFO, user->email, "Successfully mined a block");
    } else {
        free(new_block);
        print_error("Mining failed. Please try again.");
    }
    
    printf("\nPress Enter to continue...");
    getchar();
}

// Function to handle viewing the blockchain
void handle_view_blockchain(const blockchain_t *chain, const user_t *user) {
    print_header("🔗 BLOCKCHAIN EXPLORER");
    
    printf(BRIGHT_WHITE "Total Blocks: " BRIGHT_CYAN "%d\n" RESET_COLOR, chain->length);
    print_separator();
    
    print_blockchain(chain);
    log_operation(LOG_INFO, user->email, "Viewed blockchain");
    
    printf("\nPress Enter to continue...");
    getchar();
}

// Function to handle validating the blockchain integrity
void handle_validate_chain(const blockchain_t *chain, const user_t *user) {
    print_header("🔍 BLOCKCHAIN INTEGRITY VALIDATOR");
    
    printf(YELLOW "🔄 Performing comprehensive blockchain validation...\n" RESET_COLOR);
    
    // Visual validation progress
    printf(BRIGHT_WHITE "Checking blocks" RESET_COLOR);
    for(int i = 0; i < 5; i++) {
        printf(CYAN "█" RESET_COLOR);
        fflush(stdout);
        usleep(200000); // 0.2 second delay
    }
    printf("\n\n");

    if (validate_blockchain(chain)) {
        print_success("Blockchain integrity verified - All blocks are valid!");
        printf(BRIGHT_GREEN "🛡️  Security Status: " BOLD "SECURE\n" RESET_COLOR);
    } else {
        print_error("Blockchain integrity compromised!");
        printf(RED "⚠️  Security Status: " BOLD "COMPROMISED\n" RESET_COLOR);
        log_security_event(user->email, "Blockchain integrity check failed");
    }
    
    log_operation(LOG_INFO, user->email, "Performed blockchain integrity validation");
    
    printf("\nPress Enter to continue...");
    getchar();
}

// Function to handle user login
void handle_user_login(user_t *user) {
    print_header("🔐 BLOCKMED AUTHENTICATION");
    
    char email[MAX_EMAIL_SIZE], password[MAX_PASSWORD_SIZE];
    
    printf(BRIGHT_WHITE "📧 Email Address: " CYAN);
    secure_input(email, sizeof(email));
    
    printf(BRIGHT_WHITE "🔑 Password: " CYAN);
    secure_input(password, sizeof(password));
    printf(RESET_COLOR);
    
    printf(YELLOW "\n🔄 Authenticating user...\n" RESET_COLOR);
    
    if (authenticate_user(email, password, user)) {
        print_success("Authentication successful!");
        printf(BRIGHT_GREEN "Welcome back, " BOLD "%s" RESET_COLOR BRIGHT_GREEN " (%s)\n" RESET_COLOR, 
               user->email, role_to_string(user->role));
        log_operation(LOG_INFO, user->email, "Successful login");
        
        printf("\nPress Enter to continue to main menu...");
        getchar();
    } else {
        print_error("Authentication failed - Invalid credentials");
        log_security_event(email, "Failed login attempt");
        memset(user, 0, sizeof(user_t));
        user->role = ROLE_INVALID;
        
        printf("\nPress Enter to try again...");
        getchar();
    }
}

// Function to handle user registration
void handle_user_registration(void) {
    print_header("📋 USER REGISTRATION");
    
    char email[MAX_EMAIL_SIZE], password[MAX_PASSWORD_SIZE];

    printf(BRIGHT_WHITE "📧 Email (ALU domain required): " CYAN);
    secure_input(email, sizeof(email));
    
    printf(BRIGHT_WHITE "🔑 Create Password: " CYAN);
    secure_input(password, sizeof(password));
    printf(RESET_COLOR);

    printf(YELLOW "\n🔄 Processing registration...\n" RESET_COLOR);

    if (register_user(email, password)) {
        print_success("Registration completed successfully!");
        print_info("You can now login with your credentials.");
    } else {
        print_error("Registration failed - Please check your details and try again.");
        log_security_event(email, "Failed registration attempt");
    }
    
    printf("\nPress Enter to continue...");
    getchar();
}

// Enhanced authentication menu
void show_auth_menu() {
    system("clear");
    print_header("🏥 BLOCKMED SYSTEM - SECURE ACCESS PORTAL 🏥");
    
    printf(BRIGHT_WHITE "Welcome to the " BOLD "BlockMed Medical Blockchain System\n" RESET_COLOR);
    printf(DIM "Secure • Transparent • Immutable Medical Records\n\n" RESET_COLOR);
    
    print_section_header("🔐 AUTHENTICATION OPTIONS");
    
    print_menu_option(1, "🔑 Login", "Access your existing account");
    print_menu_option(2, "📝 Register", "Create a new user account");
    print_menu_option(3, "🚪 Exit", "Close the application");
    
    print_separator();
    printf(BRIGHT_WHITE "Select an option: " CYAN);
}

// Main CLI function to run the application
int run_cli(blockchain_t *chain) {
    user_t current_user = {0};
    
    // Main application loop - outer loop handles authentication
    while (1) {
        current_user.role = ROLE_INVALID;
        
        // Authentication loop
        while (current_user.role == ROLE_INVALID) {
            show_auth_menu();
            
            char choice[10];
            secure_input(choice, sizeof(choice));
            printf(RESET_COLOR);
            
            switch (choice[0]) {
                case '1':
                    handle_user_login(&current_user);
                    break;
                case '2':
                    handle_user_registration();
                    break;
                case '3':
                    printf(BRIGHT_CYAN "\n👋 Thank you for using BlockMed. Goodbye!\n" RESET_COLOR);
                    return 0;
                default:
                    print_error("Invalid selection. Please choose 1, 2, or 3.");
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
            }
        }

        // Main menu loop - inner loop handles main application functionality
        char choice[10];
        int logout_requested = 0;
        
        while (!logout_requested) {
            show_menu(current_user.role);
            secure_input(choice, sizeof(choice));
            printf(RESET_COLOR);

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
                    print_header("💾 SAVE BLOCKCHAIN");
                    printf(YELLOW "🔄 Saving blockchain to file...\n" RESET_COLOR);
                    if (save_blockchain(chain, "data/blockchain.dat")) {
                        print_success("Blockchain saved successfully to data/blockchain.dat");
                        log_operation(LOG_INFO, current_user.email, "Saved blockchain to file");
                    } else {
                        print_error("Failed to save blockchain to file");
                    }
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
                case '6':
                    if (has_write_permission(current_user.role)) {
                        print_header("📂 LOAD BLOCKCHAIN");
                        printf(YELLOW "🔄 Loading blockchain from file...\n" RESET_COLOR);
                        blockchain_t *loaded_chain = load_blockchain("data/blockchain.dat");
                        if (loaded_chain) {
                            print_success("Blockchain loaded successfully from data/blockchain.dat");
                            print_info("Note: Changes will take effect on next application restart");
                            log_operation(LOG_INFO, current_user.email, "Loaded blockchain from file");
                            // Free the loaded chain since we can't replace the main one
                            // free_blockchain(&loaded_chain);
                        } else {
                            print_error("Failed to load blockchain from file");
                        }
                    } else {
                        print_error("Access Denied: You do not have permission to load blockchain data");
                        log_security_event(current_user.email, "Attempted to load blockchain without permission");
                    }
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
                case '7':
                    if (has_full_permission(current_user.role)) {
                        print_header("⚙️ MINING DIFFICULTY SETTINGS");
                        printf(BRIGHT_WHITE "Current Mining Difficulty: " BRIGHT_CYAN "%d\n\n" RESET_COLOR, get_mining_difficulty());
                        printf(BRIGHT_WHITE "Enter new difficulty level (1-8): " CYAN);
                        int new_difficulty;

                        if (scanf("%d", &new_difficulty) == 1 && new_difficulty >= 1 && new_difficulty <= 8) {
                            set_mining_difficulty(new_difficulty);
                            print_success("Mining difficulty updated successfully!");
                            printf(BRIGHT_GREEN "New difficulty level: " BOLD "%d\n" RESET_COLOR, new_difficulty);
                            log_operation(LOG_INFO, current_user.email, "Set mining difficulty");
                        } else {
                            print_error("Invalid difficulty level. Must be between 1 and 8.");
                        }
                        while (getchar() != '\n'); // clear input buffer
                    } else {
                        print_error("Access Denied: You do not have permission to modify mining settings");
                        log_security_event(current_user.email, "Attempted to change mining difficulty without permission");
                    }
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
                case '8':
                    // just log the logout and set the flag
                    log_operation(LOG_INFO, current_user.email, "User logged out");
                    print_success("Successfully logged out. Returning to login screen...");
                    printf("\nPress Enter to continue...");
                    getchar();
                    logout_requested = 1;  // This will exit the inner loop and return to auth menu
                    break;
                default:
                    print_error("Invalid selection. Please choose a number between 1-8.");
                    printf("\nPress Enter to continue...");
                    getchar();
                    break;
            }
        }
        
        // Clear user data after logout
        memset(&current_user, 0, sizeof(user_t));
    }
    
    return 0;
}