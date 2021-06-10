/*
 * Copyright (C) 2012 The Android Open Source Project
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

// based on  adb / adb_auth_host.c in AOSP

#include <stdio.h>

#ifdef _WIN32
#  define WIN32_LEAN_AND_MEAN
#  include "windows.h"
#  include "shlobj.h"
#else
#  include <sys/types.h>
#  include <sys/stat.h>
#  include <unistd.h>
#endif
#include <string.h>

/* HACK: we need the RSAPublicKey struct
 * but RSA_verify conflits with openssl */
#define RSA_verify RSA_verify_mincrypt
#include "rsa.h"
#undef RSA_verify

#include <openssl/objects.h>
#include <openssl/opensslv.h>
#include <openssl/pem.h>
#include <openssl/rsa.h>

#define D printf

#define PATH_MAX 4096


/* Convert OpenSSL RSA private key to android pre-computed RSAPublicKey format */
static int RSA_to_RSAPublicKey(RSA *rsa, RSAPublicKey *pkey)
{
    int ret = 1;
    unsigned int i;

    BN_CTX* ctx = BN_CTX_new();
    BIGNUM* r32 = BN_new();
    BIGNUM* rr = BN_new();
    BIGNUM* r = BN_new();
    BIGNUM* rem = BN_new();
    BIGNUM* n = BN_new();
    BIGNUM* n0inv = BN_new();

    if (RSA_size(rsa) != RSANUMBYTES) {
        ret = 0;
        goto out;
    }

    BN_set_bit(r32, 32);
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    BN_copy(n, RSA_get0_n(rsa));
#else
    BN_copy(n, rsa->n);
#endif
    BN_set_bit(r, RSANUMWORDS * 32);
    BN_mod_sqr(rr, r, n, ctx);
    BN_div(NULL, rem, n, r32, ctx);
    BN_mod_inverse(n0inv, rem, r32, ctx);

    pkey->len = RSANUMWORDS;
    pkey->n0inv = 0 - BN_get_word(n0inv);
    for (i = 0; i < RSANUMWORDS; i++) {
        BN_div(rr, rem, rr, r32, ctx);
        pkey->rr[i] = BN_get_word(rem);
        BN_div(n, rem, n, r32, ctx);
        pkey->n[i] = BN_get_word(rem);
    }
#if OPENSSL_VERSION_NUMBER >= 0x10100000L
    pkey->exponent = BN_get_word(RSA_get0_e(rsa));
#else
    pkey->exponent = BN_get_word(rsa->e);
#endif

out:
    BN_free(n0inv);
    BN_free(n);
    BN_free(rem);
    BN_free(r);
    BN_free(rr);
    BN_free(r32);
    BN_CTX_free(ctx);

    return ret;
}

static int read_key(const char *file, RSA** rsa)
{
    D("read_key '%s'\n", file);

    FILE* f = fopen(file, "r");
    if (!f) {
        D("Failed to open '%s'\n", file);

        return 0;
    }

    RSA* read = PEM_read_RSAPrivateKey(f, NULL, NULL, NULL);
    if (!read){
        D("Failed to read key\n");
        fclose(f);

        return 0;
    }

    fclose(f);
    *rsa = read; 

    return 1;
}

int main(int argc, char** argv) {
    if (argc < 3) {
        fprintf(stderr, "Usage: %s <PEM key> <mincrypt key>\n", argv[0]);
        exit(1);
    }

    OpenSSL_add_all_algorithms();
    
    const char* pem_key = argv[1];
    const char* mincrypt_pubkey = argv[2];
    D("Converting PEM key '%s' to mincrypt publickey '%s'...\n", 
            pem_key, mincrypt_pubkey);

    RSA* rsa;
    int rc = read_key(argv[1], &rsa);
    if (!rc) {
        D("Error reading key: %d\n", rc);
        exit(1);
    }

    RSAPublicKey pubKey;
    rc = RSA_to_RSAPublicKey(rsa, &pubKey);
    if (!rc) {
        D("Error converting key: %d\n", rc);
        exit(1);
    }
    D("RSAPublicKey.len: %d\n", pubKey.len);
    D("RSAPublicKey.n0inv: %u\n", pubKey.n0inv);
    D("RSAPublicKey.exponent: %u\n", pubKey.exponent);

    FILE* fm = fopen(mincrypt_pubkey, "wb");
    if (!fm) {
        D("Error reading %s\n", mincrypt_pubkey);
        exit(1);
    }

    size_t items = fwrite(&pubKey, sizeof(RSAPublicKey), 1, fm);
    if (items != 1) {
        D("Could not write mincrypt key.\n");
        fclose(fm);
        unlink(mincrypt_pubkey);
        exit(1);
    }

    fflush(fm);
    fclose(fm);

    return 0;
}
