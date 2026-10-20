libldkeylog.so: libldkeylog.c
	cc -shared -fPIC -O2 -o libldkeylog.so libldkeylog.c -ldl -lssl
