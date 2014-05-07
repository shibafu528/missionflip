CC = gcc
TARGET = missionflip
PKG_CONFIG = pkg-config
GTKFLAG = `$(PKG_CONFIG) --cflags --libs gtk+-2.0`
CFLAGS = -O2
LDFLAGS = 

$(TARGET): missionflip.c dir_choose.o
	$(CC) -o $@ $^ $(GTKFLAG) $(CFLAGS) $(LDFLAGS)

dir_choose.o: dir_choose.c
	$(CC) -c -o $@ $< $(GTKFLAG) $(CFLAGS)
