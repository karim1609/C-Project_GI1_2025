#ifndef CRYPTO_H
#define CRYPTO_H

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>
#include <openssl/aes.h>
#include <openssl/rand.h>
#include <openssl/sha.h>
#include <openssl/evp.h>
#include "config.h"

// AES encryption/decryption functions
int aes_encrypt(const unsigned char* plaintext, int plaintext_len,
                unsigned char* ciphertext, const unsigned char* key);
int aes_decrypt(const unsigned char* ciphertext, int ciphertext_len,
                unsigned char* plaintext, const unsigned char* key);

// AES with IV (Initialization Vector)
int aes_encrypt_with_iv(const unsigned char* plaintext, int plaintext_len,
                        unsigned char* ciphertext, const unsigned char* key,
                        const unsigned char* iv);
int aes_decrypt_with_iv(const unsigned char* ciphertext, int ciphertext_len,
                        unsigned char* plaintext, const unsigned char* key,
                        const unsigned char* iv); 

// Key generation and management
int generate_random_key(unsigned char* key, int key_size);
int generate_random_iv(unsigned char* iv, int iv_size);
int generate_random_salt(unsigned char* salt, int salt_size);
int load_master_key(const char* filename, unsigned char* key);
int save_master_key(const char* filename, const unsigned char* key);

// Password hashing functions
void hash_password_with_salt(const char* password, const char* salt, char* hash);
int verify_password_hash(const char* password, const char* hash, const char* salt);
void generate_password_salt(char* salt);

// File encryption/decryption
int encrypt_file(const char* input_file, const char* output_file, const unsigned char* key);
int decrypt_file(const char* input_file, const char* output_file, const unsigned char* key);
int encrypt_data_to_file(const void* data, size_t data_size, const char* filename, const unsigned char* key);
int decrypt_data_from_file(void* data, size_t data_size, const char* filename, const unsigned char* key);

// Memory encryption/decryption
int encrypt_memory(const void* plaintext, size_t plaintext_size,
                   unsigned char** ciphertext, size_t* ciphertext_size,
                   const unsigned char* key);
int decrypt_memory(const unsigned char* ciphertext, size_t ciphertext_size,
                   void** plaintext, size_t* plaintext_size,
                   const unsigned char* key);

// Utility functions
int is_encrypted_file(const char* filename);
int validate_key_strength(const unsigned char* key, int key_size);
void secure_memory_clear(void* ptr, size_t size);
int crypto_init(void);
void crypto_cleanup(void);

// Error handling
const char* crypto_get_error_string(int error_code);
int crypto_set_error_callback(void (*callback)(const char* error_message));

// Constants for error codes
#define CRYPTO_SUCCESS 0
#define CRYPTO_ERROR_INVALID_KEY -1
#define CRYPTO_ERROR_INVALID_IV -2
#define CRYPTO_ERROR_ENCRYPTION_FAILED -3
#define CRYPTO_ERROR_DECRYPTION_FAILED -4
#define CRYPTO_ERROR_FILE_IO -5
#define CRYPTO_ERROR_MEMORY_ALLOCATION -6
#define CRYPTO_ERROR_INVALID_INPUT -7

#endif // CRYPTO_H
