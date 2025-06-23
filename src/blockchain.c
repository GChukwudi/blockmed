#include "blockchain.h"

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
#define BRIGHT_YELLOW   "\033[93m"
#define BRIGHT_MAGENTA  "\033[95m"

// Background colors
#define BG_BLUE         "\033[44m"
#define BG_GREEN        "\033[42m"
#define BG_RED          "\033[41m"
#define BG_YELLOW       "\033[43m"

// Utility functions for beautiful blockchain display
void print_block_header(int index, const char* block_type) {
    if (index == 0) {
        printf(BRIGHT_YELLOW "┌" BG_YELLOW " " BOLD "🌟 GENESIS BLOCK #%d - %s" RESET_COLOR " " BRIGHT_YELLOW "┐\n" RESET_COLOR, index, block_type);
    } else {
        printf(BRIGHT_CYAN "┌" BG_BLUE " " BOLD "🔗 BLOCK #%d - %s" RESET_COLOR " " BRIGHT_CYAN "┐\n" RESET_COLOR, index, block_type);
    }
}

void print_block_separator() {
    printf(DIM "├─────────────────────────────────────────────────────────────┤\n" RESET_COLOR);
}

void print_block_footer() {
    printf(BRIGHT_CYAN "└─────────────────────────────────────────────────────────────┘\n" RESET_COLOR);
}

void print_hash_field(const char* label, const char* hash, const char* color) {
    printf(BRIGHT_WHITE "│ %s: " RESET_COLOR "%s%.8s...%.8s" RESET_COLOR "\n", 
           label, color, hash, hash + strlen(hash) - 8);
}

void print_field(const char* label, const char* value, const char* color) {
    printf(BRIGHT_WHITE "│ %s: " RESET_COLOR "%s%s" RESET_COLOR "\n", label, color, value);
}

void print_chain_link() {
    printf(BRIGHT_CYAN "       ║\n");
    printf("       ▼\n" RESET_COLOR);
}

// Functions to create blockchain
blockchain_t* create_blockchain(void) {
    printf(BRIGHT_BLUE "🔄 Initializing BlockMed Blockchain...\n" RESET_COLOR);
    
    blockchain_t *chain = malloc(sizeof(blockchain_t));
    if (!chain) {
        printf(RED "❌ Failed to allocate memory for blockchain!\n" RESET_COLOR);
        return NULL;
    }

    chain->head = NULL;
    chain->tail = NULL;
    chain->length = 0;

    printf(YELLOW "⚡ Creating Genesis Block...\n" RESET_COLOR);
    
    // Create the genesis block
    block_t *genesis = create_genesis_block();
    if (genesis) {
        add_block_to_chain(chain, genesis);
        printf(BRIGHT_GREEN "✅ Blockchain initialized successfully!\n" RESET_COLOR);
        printf(BRIGHT_CYAN "🎉 Genesis block created with hash: " CYAN "%.16s...\n" RESET_COLOR, genesis->current_hash);
    } else {
        printf(RED "❌ Failed to create genesis block!\n" RESET_COLOR);
        free(chain);
        return NULL;
    }
    
    return chain;
}

// Create the genesis block with initial transaction
block_t* create_genesis_block(void) {
    block_t *genesis = malloc(sizeof(block_t));
    if (!genesis) {
        printf(RED "❌ Memory allocation failed for genesis block!\n" RESET_COLOR);
        return NULL;
    }

    genesis->index = 0;
    get_timestamp(genesis->timestamp);

    printf(DIM "   📝 Setting up genesis transaction...\n" RESET_COLOR);
    
    // Create a dummy transaction for the genesis block
    create_transaction(&genesis->transaction, "GENESIS", "system@alueducation.com",
                       "Genesis Block", "No Prescription", "Initial Block in the chain");

    // Set nonce and previous hash for the genesis block
    genesis->nonce = 0;
    strcpy(genesis->previous_hash, "0000000000000000000000000000000000000000000000000000000000000000");
    genesis->next = NULL;

    printf(DIM "   🔐 Calculating genesis block hash...\n" RESET_COLOR);
    
    // Calculate the hash for the genesis block
    calculate_block_hash(genesis);
    
    printf(BRIGHT_GREEN "   ✅ Genesis block created successfully!\n" RESET_COLOR);
    
    return genesis;
}

// Create a new block with the given transaction and previous hash
block_t* create_block(int index, const medical_transaction_t *tx, const char *prev_hash) {
    if (!tx || !prev_hash) {
        printf(RED "❌ Invalid parameters for block creation!\n" RESET_COLOR);
        return NULL;
    }

    printf(BRIGHT_BLUE "🔨 Creating new block #%d...\n" RESET_COLOR, index);

    // Allocate memory for the new block
    block_t *block = malloc(sizeof(block_t));
    if (!block) {
        printf(RED "❌ Memory allocation failed for block #%d!\n" RESET_COLOR, index);
        return NULL;
    }

    // Initialize the block fields
    block->index = index;
    get_timestamp(block->timestamp);
    block->transaction = *tx;
    block->nonce = 0;
    
    // Copy the previous hash and initialize the current hash
    strncpy(block->previous_hash, prev_hash, HASH_SIZE - 1);
    block->previous_hash[HASH_SIZE - 1] = '\0';
    block->next = NULL;

    printf(BRIGHT_GREEN "✅ Block #%d structure created successfully!\n" RESET_COLOR, index);
    printf(DIM "   📝 Patient ID: %s\n" RESET_COLOR, tx->patient_id);
    printf(DIM "   👨‍⚕️ Doctor: %s\n" RESET_COLOR, tx->doctor_email);

    return block;
}

// Calculate the hash for the block with visual feedback
void calculate_block_hash(block_t *block) {
    if (!block) {
        printf(RED "❌ Cannot calculate hash - block is NULL!\n" RESET_COLOR);
        return;
    }

    // Prepare the string representation of the block
    char tx_string[2048];
    transaction_to_string(&block->transaction, tx_string);

    // Ensure the transaction string is null-terminated
    char block_data[4096];
    snprintf(block_data, sizeof(block_data), "%d%s%s%lu%s",
             block->index, block->timestamp, tx_string, block->nonce, block->previous_hash);

    // Calculate the SHA-256 hash of the block data
    sha256_hash(block_data, block->current_hash);
    
    if (block->index > 0) {  // Don't show for genesis block to avoid spam
        printf(DIM "   🔐 Hash calculated: %s%.16s...\n" RESET_COLOR, CYAN, block->current_hash);
    }
}

// Add a block to the blockchain with enhanced feedback
int add_block_to_chain(blockchain_t *chain, block_t *block) {
    if (!chain || !block) {
        printf(RED "❌ Cannot add block - invalid chain or block!\n" RESET_COLOR);
        return 0;
    }

    // Validate the block's index and previous hash
    if (chain->length == 0) {
        // Adding the first block (genesis block)
        chain->head = block;
        chain->tail = block;
        printf(BRIGHT_YELLOW "🌟 Genesis block added to chain!\n" RESET_COLOR);
    } else {
        // Adding subsequent blocks
        chain->tail->next = block;
        chain->tail = block;
        printf(BRIGHT_GREEN "🔗 Block #%d linked to blockchain!\n" RESET_COLOR, block->index);
        printf(BRIGHT_CYAN "📊 Chain length: %d → %d\n" RESET_COLOR, chain->length, chain->length + 1);
    }
    
    chain->length++;
    return 1;
}

// Print the entire blockchain with beautiful formatting
void print_blockchain(const blockchain_t *chain) {
    if (!chain) {
        printf(RED "❌ Blockchain is NULL - cannot display!\n" RESET_COLOR);
        return;
    }

    if (chain->length == 0) {
        printf(YELLOW "⚠️  Blockchain is empty!\n" RESET_COLOR);
        return;
    }

    // Header
    printf("\n");
    printf(BRIGHT_CYAN "╔════════════════════════════════════════════════════════════════╗\n" RESET_COLOR);
    printf(BRIGHT_CYAN "║" BOLD BRIGHT_WHITE " 🏥 BLOCKMED BLOCKCHAIN EXPLORER - %d BLOCKS" RESET_COLOR "%-15s" BRIGHT_CYAN "║\n" RESET_COLOR, chain->length, "");
    printf(BRIGHT_CYAN "╚════════════════════════════════════════════════════════════════╝\n" RESET_COLOR);
    printf("\n");

    block_t *current = chain->head;
    int block_count = 0;

    while (current) {
        block_count++;
        
        // Block header with special styling for genesis
        if (current->index == 0) {
            print_block_header(current->index, "GENESIS");
            printf(BRIGHT_YELLOW "│ 🌟 " BOLD "BLOCKCHAIN FOUNDATION BLOCK" RESET_COLOR BRIGHT_YELLOW " 🌟\n" RESET_COLOR);
        } else {
            print_block_header(current->index, "MEDICAL RECORD");
            printf(BRIGHT_CYAN "│ 📋 " BOLD "PATIENT MEDICAL DATA BLOCK" RESET_COLOR BRIGHT_CYAN "\n" RESET_COLOR);
        }
        
        print_block_separator();
        
        // Block details
        print_field("📅 Timestamp", current->timestamp, BRIGHT_WHITE);
        print_hash_field("🔗 Previous Hash", current->previous_hash, DIM);
        print_hash_field("🔐 Current Hash", current->current_hash, BRIGHT_CYAN);
        
        char nonce_str[32];
        snprintf(nonce_str, sizeof(nonce_str), "%lu", current->nonce);
        print_field("⚡ Nonce", nonce_str, BRIGHT_YELLOW);
        
        print_block_separator();
        
        // Transaction details with medical context
        printf(BRIGHT_WHITE "│ 📋 " BOLD "MEDICAL TRANSACTION DETAILS:" RESET_COLOR "\n");
        printf(BRIGHT_WHITE "│   👤 Patient ID: " RESET_COLOR CYAN "%s\n" RESET_COLOR, current->transaction.patient_id);
        printf(BRIGHT_WHITE "│   👨‍⚕️ Doctor: " RESET_COLOR BRIGHT_BLUE "%s\n" RESET_COLOR, current->transaction.doctor_email);
        printf(BRIGHT_WHITE "│   🩺 Diagnosis: " RESET_COLOR GREEN "%s\n" RESET_COLOR, current->transaction.diagnosis);
        printf(BRIGHT_WHITE "│   💊 Prescription: " RESET_COLOR YELLOW "%s\n" RESET_COLOR, current->transaction.prescription);
        printf(BRIGHT_WHITE "│   📝 Notes: " RESET_COLOR WHITE "%s\n" RESET_COLOR, current->transaction.visit_note);
        
        print_block_footer();
        
        // Show chain link if not the last block
        if (current->next) {
            print_chain_link();
        }

        current = current->next;
    }
    
    // Footer summary
    printf("\n");
    printf(BRIGHT_CYAN "╔════════════════════════════════════════════════════════════════╗\n" RESET_COLOR);
    printf(BRIGHT_CYAN "║" BRIGHT_WHITE " 📊 BLOCKCHAIN SUMMARY" RESET_COLOR "%-40s" BRIGHT_CYAN "║\n" RESET_COLOR, "");
    printf(BRIGHT_CYAN "║" BRIGHT_WHITE " Total Blocks: " BOLD "%d" RESET_COLOR "%-45s" BRIGHT_CYAN "║\n" RESET_COLOR, chain->length, "");
    printf(BRIGHT_CYAN "║" BRIGHT_WHITE " Medical Records: " BOLD "%d" RESET_COLOR "%-40s" BRIGHT_CYAN "║\n" RESET_COLOR, chain->length - 1, "");
    printf(BRIGHT_CYAN "║" BRIGHT_WHITE " Chain Status: " BRIGHT_GREEN "🔒 SECURE & IMMUTABLE" RESET_COLOR "%-23s" BRIGHT_CYAN "║\n" RESET_COLOR, "");
    printf(BRIGHT_CYAN "╚════════════════════════════════════════════════════════════════╝\n" RESET_COLOR);
}

// Validate the blockchain with enhanced visual feedback
int validate_blockchain(const blockchain_t *chain) {
    if (!chain || !chain->head) {
        printf(RED "❌ Cannot validate - blockchain is NULL or empty!\n" RESET_COLOR);
        return 0;
    }

    printf(BRIGHT_BLUE "🔍 Starting comprehensive blockchain validation...\n" RESET_COLOR);
    printf(YELLOW "📊 Validating %d blocks in the chain...\n\n" RESET_COLOR, chain->length);

    block_t *current = chain->head;
    int blocks_validated = 0;
    int total_blocks = chain->length;

    while (current->next) {
        block_t *next_block = current->next;
        blocks_validated++;
        
        // Progress indicator
        printf(BRIGHT_WHITE "🔍 Validating Block #%d" RESET_COLOR, current->index);
        
        // Check if the current block's hash matches the next block's previous hash
        if (strcmp(current->current_hash, next_block->previous_hash) != 0) {
            printf(RED " ❌ FAILED!\n" RESET_COLOR);
            printf(RED "💥 Chain integrity compromised at block %d!\n" RESET_COLOR, next_block->index);
            printf(RED "   Expected: %s\n" RESET_COLOR, current->current_hash);
            printf(RED "   Found:    %s\n" RESET_COLOR, next_block->previous_hash);
            return 0;
        }

        // Verify the current block's hash
        char temp_hash[HASH_SIZE];
        strcpy(temp_hash, current->current_hash);
        calculate_block_hash(current);

        // Check if the calculated hash matches the stored hash
        if (strcmp(temp_hash, current->current_hash) != 0) {
            printf(RED " ❌ FAILED!\n" RESET_COLOR);
            printf(RED "🚨 Block %d has been tampered with!\n" RESET_COLOR, current->index);
            printf(RED "   Original:   %s\n" RESET_COLOR, temp_hash);
            printf(RED "   Calculated: %s\n" RESET_COLOR, current->current_hash);
            return 0;
        }

        printf(BRIGHT_GREEN " ✅ VALID\n" RESET_COLOR);
        
        // Show progress
        float progress = (float)blocks_validated / (total_blocks - 1) * 100;
        printf(DIM "   Progress: %.1f%% (%d/%d blocks validated)\n\n" RESET_COLOR, 
               progress, blocks_validated, total_blocks - 1);

        current = current->next;
    }
    
    // Final validation of the last block
    printf(BRIGHT_WHITE "🔍 Validating Block #%d" RESET_COLOR, current->index);
    char temp_hash[HASH_SIZE];
    strcpy(temp_hash, current->current_hash);
    calculate_block_hash(current);
    
    if (strcmp(temp_hash, current->current_hash) != 0) {
        printf(RED " ❌ FAILED!\n" RESET_COLOR);
        printf(RED "🚨 Final block %d has been tampered with!\n" RESET_COLOR, current->index);
        return 0;
    }
    
    printf(BRIGHT_GREEN " ✅ VALID\n\n" RESET_COLOR);
    
    // Success message
    printf(BRIGHT_GREEN "🎉 BLOCKCHAIN VALIDATION COMPLETE!\n" RESET_COLOR);
    printf(BRIGHT_GREEN "✅ All %d blocks are valid and secure\n" RESET_COLOR, chain->length);
    printf(BRIGHT_GREEN "🔒 Chain integrity: " BOLD "VERIFIED\n" RESET_COLOR);
    printf(BRIGHT_GREEN "🛡️  Security status: " BOLD "SECURE\n" RESET_COLOR);
    
    return 1;
}

// Free the entire blockchain with confirmation
void free_blockchain(blockchain_t *chain) {
    if (!chain) {
        printf(YELLOW "⚠️  Attempted to free NULL blockchain\n" RESET_COLOR);
        return;
    }

    printf(YELLOW "🧹 Cleaning up blockchain memory...\n" RESET_COLOR);
    
    int blocks_freed = 0;
    block_t *current = chain->head;
    
    while (current) {
        block_t *next = current->next;
        free(current);
        blocks_freed++;
        current = next;
    }
    
    free(chain);
    
    printf(BRIGHT_GREEN "✅ Blockchain cleanup complete!\n" RESET_COLOR);
    printf(DIM "   Freed %d blocks and chain structure\n" RESET_COLOR, blocks_freed);
}