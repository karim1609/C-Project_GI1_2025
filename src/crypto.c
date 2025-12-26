#include "crypto.h"
#include <stdio.h>
#include <stdlib.h>
int aes_encrypt(const unsigned char* plaintext, int plaintext_len,
    unsigned char* ciphertext, const unsigned char* key)
{
    AES_KEY enc_key;
    int num_blocks = plaintext_len / AES_BLOCK_SIZE;
    int remainder = plaintext_len % AES_BLOCK_SIZE;
    int padded_len;
    unsigned char in_block[AES_BLOCK_SIZE];
    unsigned char out_block[AES_BLOCK_SIZE];

    if(AES_set_encrypt_key(key, 256, &enc_key) < 0) {
        return -1;
    }

    // AES uses block size of 16 bytes; we need to pad if not multiple of block size
    // PKCS7 padding: always add padding, even if data is exact multiple of block size
    // 
    // Examples:
    // - plaintext_len = 15 bytes: remainder=15, need 1 block (15 bytes + 1 padding byte = 16)
    // - plaintext_len = 16 bytes: remainder=0, need 2 blocks (16 bytes + 16 padding bytes = 32)
    // - plaintext_len = 31 bytes: remainder=15, need 2 blocks (31 bytes + 1 padding byte = 32)
    // - plaintext_len = 32 bytes: remainder=0, need 3 blocks (32 bytes + 16 padding bytes = 48)
    //
    // Calculation:
    // - padded_len = num_blocks * AES_BLOCK_SIZE (size of complete blocks)
    // - if remainder > 0: add 1 more block for remainder + padding
    // - if remainder == 0: add 1 more block for full block of padding (PKCS7 requirement)
    padded_len = num_blocks * AES_BLOCK_SIZE;
    if(remainder > 0) {
        // We need one additional block to fit the remainder + padding bytes
        padded_len += AES_BLOCK_SIZE;
    } else {
        // If exact multiple, we still need one more full block for padding (PKCS7)
        padded_len += AES_BLOCK_SIZE;
    }

    // Copy plaintext to local buffer and add PKCS7 padding
    unsigned char* padded_plaintext = (unsigned char*)malloc(padded_len);
    if(!padded_plaintext) return -1;

    memcpy(padded_plaintext, plaintext, plaintext_len);
    
    // Add PKCS7 padding
    unsigned char pad_value;
    if(remainder > 0) {
        // Partial block: pad with (block_size - remainder) bytes
        pad_value = AES_BLOCK_SIZE - remainder;
        memset(padded_plaintext + plaintext_len, pad_value, pad_value);
    } else {
        // Full block: pad with a full block of padding bytes (value = block_size)
        pad_value = AES_BLOCK_SIZE;
        memset(padded_plaintext + plaintext_len, pad_value, AES_BLOCK_SIZE);
    }

    for(int i = 0; i < padded_len; i += AES_BLOCK_SIZE) {
        memcpy(in_block, padded_plaintext + i, AES_BLOCK_SIZE);
        AES_ecb_encrypt(in_block, out_block, &enc_key, AES_ENCRYPT);
        memcpy(ciphertext + i, out_block, AES_BLOCK_SIZE);
    }

    free(padded_plaintext);

    return padded_len;
}
int aes_decrypt(const unsigned char* ciphertext, int ciphertext_len,
    unsigned char* plaintext, const unsigned char* key){
    if (!ciphertext || !plaintext || !key || ciphertext_len % AES_BLOCK_SIZE != 0 || ciphertext_len <= 0)
        return -1;

    AES_KEY dec_key;
    if(AES_set_decrypt_key(key, 128, &dec_key) < 0) {
        return -1;
    }

    unsigned char in_block[AES_BLOCK_SIZE];
    unsigned char out_block[AES_BLOCK_SIZE];

    // Decrypt each block
    for(int i = 0; i < ciphertext_len; i += AES_BLOCK_SIZE) {
        memcpy(in_block, ciphertext + i, AES_BLOCK_SIZE);
        AES_ecb_encrypt(in_block, out_block, &dec_key, AES_DECRYPT);
        memcpy(plaintext + i, out_block, AES_BLOCK_SIZE);
    }

    // Remove PKCS7 padding (last byte is value of padding length)
    unsigned char pad_value = plaintext[ciphertext_len - 1];
    if (pad_value == 0 || pad_value > AES_BLOCK_SIZE) {
        // Invalid padding
        return -1;
    }
    // Check all the padding bytes (security: avoid padding oracle)
    for (int i = 0; i < pad_value; ++i) {
        if (plaintext[ciphertext_len - 1 - i] != pad_value) return -1;
    }

    int unpadded_len = ciphertext_len - pad_value;
    // Optional: null-terminate if plaintext is a string (not required for binary)
    // plaintext[unpadded_len] = '\0'; // uncomment if needed

    return unpadded_len;
}
int aes_encrypt_with_iv(const unsigned char* plaintext, int plaintext_len,
    unsigned char* ciphertext, const unsigned char* key,
    const unsigned char* iv)
{
    if (!plaintext || !ciphertext || !key || !iv || plaintext_len <= 0)
        return -1;

    AES_KEY enc_key;
    if (AES_set_encrypt_key(key, 128, &enc_key) < 0) {
        return -1;
    }

    int pad_value = AES_BLOCK_SIZE - (plaintext_len % AES_BLOCK_SIZE);
    int padded_len = plaintext_len + pad_value;
    unsigned char* padded_plaintext = (unsigned char*)malloc(padded_len);
    if (!padded_plaintext)
        return -1;

    memcpy(padded_plaintext, plaintext, plaintext_len);
    memset(padded_plaintext + plaintext_len, pad_value, pad_value);

    unsigned char block_in[AES_BLOCK_SIZE];
    unsigned char block_out[AES_BLOCK_SIZE];
    unsigned char iv_cpy[AES_BLOCK_SIZE];
    memcpy(iv_cpy, iv, AES_BLOCK_SIZE);

    for (int i = 0; i < padded_len; i += AES_BLOCK_SIZE) {
        // XOR plaintext block with IV or previous ciphertext (CBC mode)
        for (int j = 0; j < AES_BLOCK_SIZE; ++j) {
            block_in[j] = padded_plaintext[i + j] ^ iv_cpy[j];
        }
        AES_encrypt(block_in, block_out, &enc_key);
        memcpy(ciphertext + i, block_out, AES_BLOCK_SIZE);
        // Next IV is current ciphertext
        memcpy(iv_cpy, block_out, AES_BLOCK_SIZE);
    }

    free(padded_plaintext);
    return padded_len;
}
int aes_decrypt_with_iv(const unsigned char* ciphertext, int ciphertext_len,
    unsigned char* plaintext, const unsigned char* key,
    const unsigned char* iv)
{
    if (!ciphertext || !plaintext || !key || !iv || ciphertext_len <= 0)
        return -1;

    if (ciphertext_len % AES_BLOCK_SIZE != 0)
        return -1;

    AES_KEY dec_key;
    if (AES_set_decrypt_key(key, 128, &dec_key) < 0) {
        return -1;
    }

    unsigned char block_in[AES_BLOCK_SIZE];
    unsigned char block_out[AES_BLOCK_SIZE];
    unsigned char iv_cpy[AES_BLOCK_SIZE];
    memcpy(iv_cpy, iv, AES_BLOCK_SIZE);

    for (int i = 0; i < ciphertext_len; i += AES_BLOCK_SIZE) {
        memcpy(block_in, ciphertext + i, AES_BLOCK_SIZE);
        AES_decrypt(block_in, block_out, &dec_key);
        // XOR with IV or previous ciphertext
        for (int j = 0; j < AES_BLOCK_SIZE; ++j) {
            plaintext[i + j] = block_out[j] ^ iv_cpy[j];
        }
        memcpy(iv_cpy, block_in, AES_BLOCK_SIZE);
    }

    // Remove PKCS#7 padding
    int pad_value = plaintext[ciphertext_len - 1];
    if (pad_value < 1 || pad_value > AES_BLOCK_SIZE)
        return -1;
    for (int i = 0; i < pad_value; ++i) {
        if (plaintext[ciphertext_len - 1 - i] != pad_value)
            return -1;
    }
    int unpadded_len = ciphertext_len - pad_value;
    return unpadded_len;
}
int generate_random_key(unsigned char* key, int key_size) {
    if (!key || key_size <= 0)
        return -1;

    // Use OpenSSL's RAND_bytes for cryptographically secure random generation
    if (RAND_bytes(key, key_size) != 1) {
        // Random generation failed
        return -1;
    }
    return 0; // Success
}
int generate_random_iv(unsigned char* iv, int iv_size) {
    if (!iv || iv_size <= 0)
        return -1;

    // Use OpenSSL's RAND_bytes to generate a cryptographically secure IV
    if (RAND_bytes(iv, iv_size) != 1) {
        // Random generation failed
        return -1;
    }
    return 0; // Success
}
int generate_random_salt(unsigned char* salt, int salt_size) {
    if (!salt || salt_size <= 0) {
        return -1;
    }
    if (RAND_bytes(salt, salt_size) != 1) {
        return -1;
    }
    return 0;
}

int load_master_key(const char* filename, unsigned char* key) {
    if (!filename || !key) {
        return -1;
    }

    FILE* f = fopen(filename, "rb");
    if (!f)
        return -1;

    size_t read = fread(key, 1, AES_BLOCK_SIZE, f);
    fclose(f);

    if (read != AES_BLOCK_SIZE) {
        return -1;
    }
    return 0;
}

int save_master_key(const char* filename, const unsigned char* key) {
    if (!filename || !key) {
        return -1;
    }

    FILE* f = fopen(filename, "wb");
    if (!f)
        return -1;

    size_t written = fwrite(key, 1, AES_BLOCK_SIZE, f);
    fclose(f);

    if (written != AES_BLOCK_SIZE) {
        return -1;
    }
    return 0;
}
void hash_password_with_salt(const char* password, const char* salt, char* hash){
    // Combine password and salt
    char combined[512];
    snprintf(combined, sizeof(combined), "%s%s", password, salt);

    // Hash using SHA-256
    unsigned char hash_bin[SHA256_DIGEST_LENGTH];
    SHA256((unsigned char*)combined, strlen(combined), hash_bin);

    // Convert hash to hex string
    for (int i = 0; i < SHA256_DIGEST_LENGTH; ++i) {
        sprintf(hash + (i * 2), "%02x", hash_bin[i]);
    }
    hash[SHA256_DIGEST_LENGTH * 2] = '\0';
}
// Duplicate functions removed - already defined above

    FILE* f = fopen(filename, "wb");
    if (!f)
        return -1;

    size_t written = fwrite(key, 1, AES_BLOCK_SIZE, f);
    fclose(f);

    if (written != AES_BLOCK_SIZE) {
        return -1;
    }
    return 0;

}
