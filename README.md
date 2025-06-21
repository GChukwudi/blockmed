# BlockMed - Blockchain Medical Record Management System

## Overview
BlockMed is a secure, blockchain-based medical record management system developed for African Leadership University (ALU) coursework. The system implements a complete blockchain with proof-of-work mining, role-based access control, and secure data persistence.

## Features

### 🔐 Security Features
- **Role-based Access Control**: 
  - Staff (`@alueducation.com`): Full access
  - Interns (`@si.alueducation.com`): Limited write access  
  - Students (`@alustudent.com`): View-only access
- **SHA-256 Cryptographic Hashing**: All blocks and passwords secured
- **Input Sanitization**: Protection against buffer overflow attacks  
- **Audit Logging**: Complete operation tracking in `access.log`
- **File Integrity Checking**: Tamper detection for blockchain files

### ⛓️ Blockchain Features
- **Complete Blockchain Implementation**: Genesis block + linked chain
- **Proof of Work Mining**: Adjustable difficulty (1-8 leading zeros)
- **Medical Transaction Storage**: Patient records with full metadata
- **Chain Validation**: Integrity verification across entire chain
- **Data Persistence**: Save/load blockchain to encrypted files

### 🏥 Medical Record Management
- **Structured Medical Transactions**: Patient ID, doctor, diagnosis, prescription, notes
- **Timestamped Records**: Automatic timestamp generation
- **Immutable Audit Trail**: Blockchain ensures record integrity
- **Privacy Protection**: Access controls prevent unauthorized viewing

## System Architecture

```
blockmed/
├── src/
│   ├── main.c          # Entry point and system initialization
│   ├── blockchain.c/.h # Core blockchain logic and block management  
│   ├── transaction.c/.h# Medical transaction handling
│   ├── pow.c/.h        # Proof of Work mining implementation
│   ├── cli.c/.h        # Command line interface and menus
│   ├── auth.c/.h       # Authentication and role management
│   ├── storage.c/.h    # File I/O and data persistence
│   ├── utils.c/.h      # SHA-256, timestamping, input validation
│   └── log.c/.h        # Security and operation logging
├── data/
│   ├── blockchain.dat  # Serialized blockchain storage
│   ├── users.csv       # User credentials database
│   └── access.log      # System audit log
├── Makefile
└── README.md
```

## Installation & Setup

### Prerequisites
```bash
# Ubuntu/Debian
sudo apt-get update
sudo apt-get install build-essential libssl-dev

# CentOS/RHEL
sudo yum install gcc openssl-devel

# macOS
brew install openssl
```

### Build Instructions
```bash
# Clone and build
git clone <repository>
cd blockmed
make

# Run the system
./blockmed
```

### Default Admin Account
- Email: `admin@alueducation.com`  
- Password: `admin123`

## Usage Guide

### 1. Authentication
Users must log in with valid ALU email addresses:
- `@alueducation.com` - Full system access
- `@si.alueducation.com` - Can add records and mine blocks
- `@alustudent.com` - View blockchain only

### 2. Adding Medical Records
1. Login with staff/intern credentials
2. Select "Add new medical record"
3. Enter patient details:
   - Patient ID
   - Diagnosis  
   - Prescription
   - Visit notes
4. Record is created and ready for mining

### 3. Mining Blocks
1. Ensure you have a pending transaction
2. Select "Mine pending block"
3. System will perform proof-of-work mining
4. Block is added to chain when valid hash found

### 4. Viewing Records
- Select "View entire blockchain"
- All blocks and transactions displayed
- Available to all authenticated users

### 5. Chain Validation
- Select "Validate chain integrity"
- System verifies all block hashes and links
- Detects any tampering or corruption

## Security Implementation

### Cryptographic Security
- **SHA-256 Hashing**: OpenSSL implementation for all cryptographic operations
- **Salted Password Hashing**: Passwords stored as SHA-256 hashes with salt
- **Block Hash Chaining**: Each block contains cryptographic hash of previous block
- **Proof of Work**: Computational difficulty prevents easy block manipulation

### Access Control
- **Email Domain Validation**: Only ALU email addresses accepted
- **Role-based Permissions**: Different access levels based on email domain
- **Operation Logging**: All user actions logged with timestamps
- **Session Management**: Users must authenticate for each session

### Input Validation
- **Buffer Overflow Protection**: All inputs bounded and sanitized
- **Email Format Validation**: Proper email format checking
- **SQL Injection Prevention**: No SQL used, CSV-based storage
- **Path Traversal Protection**: Restricted file access paths

## Technical Specifications

### Block Structure
```c
typedef struct block {
    int index;                      // Block number in chain
    char timestamp[20];             // Creation timestamp  
    medical_transaction_t transaction; // Medical record data
    unsigned long nonce;            // Proof of work nonce
    char previous_hash[65];         // Previous block hash
    char current_hash[65];          // This block's hash
    struct block *next;             // Pointer to next block
} block_t;
```

### Transaction Structure  
```c
typedef struct {
    char patient_id[50];            // Patient identifier
    char doctor_email[100];         // Doctor's email
    char diagnosis[500];            // Medical diagnosis
    char prescription[500];         // Prescribed treatment
    char visit_notes[1000];         // Additional notes
    char timestamp[20];             // Transaction timestamp
} medical_transaction_t;
```

### Mining Algorithm
1. Create block with transaction data
2. Set nonce to 0
3. Calculate SHA-256 hash of block data + nonce
4. Check if hash has required leading zeros
5. If not, increment nonce and repeat
6. When valid hash found, block is mined

## Development Guidelines

### Code Standards
- **C99 Standard**: Compatible with modern C compilers
- **Memory Management**: Proper malloc/free with leak prevention
- **Error Handling**: Comprehensive error checking and logging
- **Documentation**: Inline comments and function documentation
- **Modularity**: Clear separation of concerns across modules

### Testing
```bash
# Build with debug symbols
make debug

# Run with memory leak detection
make valgrind

# Clean build environment
make clean
```

### Contributing
1. Follow existing code style and patterns
2. Add comprehensive comments for new functions
3. Update documentation for new features
4. Test thoroughly before committing
5. Ensure all security checks pass

## Security Considerations

### Blockchain Security
- **Immutability**: Once mined, blocks cannot be altered without detection
- **Hash Chain Integrity**: Any modification breaks the cryptographic chain
- **Proof of Work**: Computational cost prevents easy chain manipulation
- **Validation**: Regular integrity checks detect tampering

### Data Privacy
- **Access Controls**: Role-based permissions protect sensitive data
- **Audit Trails**: Complete logging of all data access
- **Encryption**: File storage uses cryptographic integrity checks
- **Authentication**: Strong password requirements and hashing

### System Security
- **Input Sanitization**: All user inputs validated and cleaned
- **Buffer Protection**: Bounds checking prevents overflow attacks
- **File Security**: Restricted file system access and validation
- **Network Security**: Local operation, no network exposure

## Troubleshooting

### Common Issues
1. **OpenSSL not found**: Install libssl-dev package
2. **Permission denied**: Check file permissions in data/ directory
3. **Blockchain corrupt**: Delete blockchain.dat to start fresh
4. **Mining too slow**: Reduce difficulty setting (default: 4)
5. **Login failures**: Check users.csv file format

### System Requirements
- Linux/Unix/macOS operating system
- GCC compiler with C99 support
- OpenSSL development libraries
- Minimum 512MB RAM for mining operations
- 100MB disk space for blockchain storage

## License
Educational use only - African Leadership University coursework project.

## Authors
ALU Computer Science Department - Blockchain Systems Course

---
*This system is designed for educational purposes and demonstrates blockchain concepts in healthcare data management. For production use, additional security hardening and compliance measures would be required.*