#include "cli.h"
#include "storage.h"
#include "log.h"
#include <sys/stat.h>


void create_data_directory(void) {
    struct stat st = {0};

    // 
    if (stat("data", &st) == -1) {
        #ifdef _WIN32
            mkdir("data");
        #else
            mkdir("data", 0700);
        #endif
    }
}

int main(void) {
    printf("\n=== BlockMed - Medical Records Blockchain ===\n");
    printf("African Leadership University (ALU) Project\n");
    printf("Secure Medical Records Management System\n");

    //initialize the data directory
    create_data_directory();
    init_logging();

    // try to load the blockchain from storage
    blockchain_t *chain = load_blockchain("data/blockchain.dat");
    if (!chain) {
        printf("Creating a new blockchain...\n");
        chain = create_blockchain();

        if (!chain) {
            fprintf(stderr, "Failed to create a new blockchain.\n");
            return 1;
        }
    } else {
        printf("Loading existing blockchain with %d blocks.\n", chain->length);
    }

    // Run the CLI interface for interacting with the blockchain
    int result = run_cli(chain);

    // save the blockchain before exiting
    save_blockchain(chain, "data/blockchain.dat");

    // Free the blockchain resources
    free_blockchain(&chain);
    printf("System shutting down. Goodbye!\n");
    return result;
}

