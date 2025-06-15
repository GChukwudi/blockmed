#include "transaction.h"

void create_transaction(medical_transaction_t *tx, const char *patient_id,
                         const char *doctor_email, const char *diagnosis,
                         const char *prescription, const char *visit_note) {

    if (!tx) return;

    strncpy(tx->patient_id, patient_id ? patient_id : "", sizeof(tx->patient_id) - 1);
    strncpy(tx->doctor_email, doctor_email ? doctor_email : "", sizeof(tx->doctor_email) - 1);
    strncpy(tx->diagnosis, diagnosis ? diagnosis : "", sizeof(tx->diagnosis) - 1);
    strncpy(tx->prescription, prescription ? prescription : "", sizeof(tx->prescription) - 1);
    strncpy(tx->visit_note, visit_note ? visit_note : "", sizeof(tx->visit_note) - 1);

    // ensure null termination
    tx->patient_id[sizeof(tx->patient_id) - 1] = '\0';
    tx->doctor_email[sizeof(tx->doctor_email) - 1] = '\0';
    tx->diagnosis[sizeof(tx->diagnosis) - 1] = '\0';
    tx->prescription[sizeof(tx->prescription) - 1] = '\0';
    tx->visit_note[sizeof(tx->visit_note) - 1] = '\0';

    // Get current timestamp
    get_timestamp(tx->timestamp);
}

void transaction_to_string(const medical_transaction_t *tx, char *output) {
    if (!tx || !output) return;

    snprintf(output, 2048, "Patient ID: %s/Doctor: %s/Diagnosis: %s/Prescription: %s/Visit Note: %s/Timestamp: %s",
             tx->patient_id, tx->doctor_email, tx->diagnosis,
             tx->prescription, tx->visit_note, tx->timestamp);
}

void print_transaction(const medical_transaction_t *tx) {
    if (!tx) return;

    printf("Patient ID: %s\n", tx->patient_id);
    printf("Doctor: %s\n", tx->doctor_email);
    printf("Diagnosis: %s\n", tx->diagnosis);
    printf("Prescription: %s\n", tx->prescription);
    printf("Visit Note: %s\n", tx->visit_note);
    printf("Timestamp: %s\n", tx->timestamp);
    printf("------------------------------\n");
}
