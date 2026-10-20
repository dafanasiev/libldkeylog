#define _GNU_SOURCE
#include <dlfcn.h>
#include <openssl/ssl.h>
#include <stdio.h>
#include <stdlib.h>

// OpenSSL v3 uses SSL_CTX_new_ex to instantiate contexts with custom libraries/properties
typedef SSL_CTX *(*fn_SSL_CTX_new_ex)(OSSL_LIB_CTX *, const char *, const SSL_METHOD *);
typedef SSL_CTX *(*fn_SSL_CTX_new)(const SSL_METHOD *);

static FILE *keylog_file = NULL;

static void keylog_callback(const SSL *ssl, const char *line) {
    if (keylog_file) {
        fprintf(keylog_file, "%s\n", line);
        fflush(keylog_file); // Ensure immediate write for Wireshark
    }
}

static void init_keylog(SSL_CTX *ctx) {
    if (!ctx) return;
    
    // Initialize the file once if it isn't already open
    if (!keylog_file) {
        char *path = getenv("SSLKEYLOGFILE");
        if (path) {
            keylog_file = fopen(path, "a");
        }
    }
    
    if (keylog_file) {
        SSL_CTX_set_keylog_callback(ctx, keylog_callback);
    }
}

// Hook for OpenSSL v3's modern constructor
SSL_CTX *SSL_CTX_new_ex(OSSL_LIB_CTX *libctx, const char *propq, const SSL_METHOD *method) {
    fn_SSL_CTX_new_ex orig_SSL_CTX_new_ex = (fn_SSL_CTX_new_ex)dlsym(RTLD_NEXT, "SSL_CTX_new_ex");
    SSL_CTX *ctx = orig_SSL_CTX_new_ex(libctx, propq, method);
    init_keylog(ctx);
    return ctx;
}

// Fallback hook for legacy OpenSSL constructor still used by some applications
SSL_CTX *SSL_CTX_new(const SSL_METHOD *method) {
    fn_SSL_CTX_new orig_SSL_CTX_new = (fn_SSL_CTX_new)dlsym(RTLD_NEXT, "SSL_CTX_new");
    SSL_CTX *ctx = orig_SSL_CTX_new(method);
    init_keylog(ctx);
    return ctx;
}
