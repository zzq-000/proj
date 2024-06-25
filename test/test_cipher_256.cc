#include <gtest/gtest.h>
#include "cipher/cipher.h"
#include "transport_util.h"

TEST(Cipher, AES) {
    uint8_t plain[BUFSIZ];
    uint8_t encrypted[BUFSIZ];
    uint8_t tmp[BUFSIZ];

    uint8_t key[32];
    uint8_t iv[12];
    uint8_t tag[16];
    Cipher cipher;
    constexpr int repeated = 100000;
    for (int i = 0; i < repeated; ++i) {

        FillRandomBytes(key, 32);
        FillRandomBytes(iv, 12);

        int len = rand() % 1200 + 200;
        FillRandomBytes(plain, len);

        int cipher_len = cipher.AES_Encrypt(plain, len, key, iv, encrypted, tag);
        int cmp1 = memcmp(plain, encrypted, len);
        EXPECT_NE(cmp1, 0);
        EXPECT_EQ(cipher_len, len);

        int decrypt_res = cipher.AES_Decrypt(encrypted, len, key, iv, tag, tmp);
        int cmp2 = memcmp(plain, tmp, len);
        EXPECT_EQ(decrypt_res, len);
        EXPECT_EQ(cmp2, 0);
    }
}

TEST(Cipher, ChaCha20) {
    uint8_t plain[BUFSIZ];
    uint8_t encrypted[BUFSIZ];
    uint8_t tmp[BUFSIZ];

    uint8_t key[32];
    uint8_t iv[12];
    Cipher cipher;

    constexpr int repeated = 100000;
    for (int i = 0; i < repeated; ++i) {

        FillRandomBytes(key, 32);
        FillRandomBytes(iv, 12);

        int len = rand() % 1200 + 200;
        FillRandomBytes(plain, len);

        int cipher_len = cipher.ChaCha20_Encrypt(plain, len, key, iv, encrypted);
        int cmp1 = memcmp(plain, encrypted, len);
        EXPECT_NE(cmp1, 0);
        EXPECT_EQ(cipher_len, len);

        int decrypt_res = cipher.ChaCha20_Decrypt(encrypted, len, key, iv, tmp);
        int cmp2 = memcmp(plain, tmp, len);
        EXPECT_EQ(decrypt_res, len);
        EXPECT_EQ(cmp2, 0);
    }
}