CC = gcc
TARGET = missionflip
PKG_CONFIG = pkg-config
GTKFLAG = `$(PKG_CONFIG) --cflags --libs gtk+-2.0`
LDFLAGS = 

$(TARGET): missionflip.c
	$(CC) -o $@ $^ $(GTKFLAG) $(LDFLAGS)
