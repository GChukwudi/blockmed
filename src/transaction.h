#ifndef TRANSACTION_H
#define TRANSACTION_H

#include "utils.h"

// structure to hold transaction details
typedef struct {
    char patient_id[50]; // Patient ID
    char doctor_email[MAX_EMAIL_SIZE];
    char diagnosis[MAX_DIAGNOSIS_SIZE];
    char prescription[MAX_PRESCRIPTION_SIZE];
    char timestamp[20]; // Timestamp of the transaction
    char visit_note[MAX_NOTES_SIZE];
} medical_transaction_t;

// Function prototypes
void create_transaction(medical_transaction_t *tx, const char *patient_id,
                        const char *doctor_email, const char *diagnosis,
                        const char *prescription, const char *visit_note);
void transaction_to_string(const medical_transaction_t *tx, char *output);
void print_transaction(const medical_transaction_t *tx);


#endif