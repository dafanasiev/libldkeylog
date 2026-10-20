# libldkeylog

A small C utility that dumps TLS session keys from applications dynamically linking against OpenSSL on Unix-like system.

## Usage

```sh
SSLKEYLOGFILE=/path/to/logfile.txt LD_PRELOAD=./libldkeylog.so /path/to/target/app
```


## How It Works

* Mechanism: It uses `LD_PRELOAD` to inject a shared library (`libldkeylog.so`) into an application's process.
* Interception: It hooks `SSL_CTX_new` and `SSL_CTX_new_ex` via `dlsym(RTLD_NEXT)`.
* Logging: It reads the `SSLKEYLOGFILE` environment variable and registers a keylog callback (`SSL_CTX_set_keylog_callback`) to write TLS secret lines compatible with tools like Wireshark for decryption.
