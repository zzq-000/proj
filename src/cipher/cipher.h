#include "cipher_impl.h"


class Cipher {
private:
    EVP_CIPHER_CTX *ctx_;
public:
    Cipher() {
        ctx_ = EVP_CIPHER_CTX_new();
    }
    ~Cipher() {
        EVP_CIPHER_CTX_free(ctx_);
    }
    int AES_Encrypt(unsigned char *plaintext, int plaintext_len, unsigned char *key,
            unsigned char *iv, unsigned char *ciphertext, unsigned char *tag);

    int AES_Decrypt(unsigned char *ciphertext, int ciphertext_len, unsigned char *key,
                unsigned char *iv, unsigned char *tag, unsigned char *plaintext);

    int ChaCha20_Encrypt(unsigned char *plaintext, int plaintext_len,
                unsigned char *key, unsigned char *iv, unsigned char *ciphertext);
    int ChaCha20_Decrypt(unsigned char *ciphertext, int ciphertext_len,
                unsigned char *key, unsigned char *iv, unsigned char *plaintext);
};