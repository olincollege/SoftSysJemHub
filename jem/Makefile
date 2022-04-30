# glib: sudo apt-get install libglib2.0-dev
# openssl: sudo apt install openssl libssl-dev
sources = reference.c storage.c commit.c index.c snaptree.c
headers = reference.h storage.h commit.h index.h snaptree.h
jem:  $(sources) $(headers)
	gcc `pkg-config --cflags glib-2.0` -Wall jem.c $(sources) -o jem `pkg-config --libs glib-2.0 openssl`
