PROJECT  := space-impact-ii
SRC      := shotlist.c scenery.c saves.c main.c audio.c graphics.c font.c enemies.c
OBJDIR   := obj
BINDIR   := bin
TARGET   := $(BINDIR)/$(PROJECT)

CC       := gcc
CFLAGS   := -Wall -Wextra -std=c11
DEBUG    := -g -O0
RELEASE  := -O2 -s

SDL_CFLAGS := $(shell pkg-config --cflags sdl)
SDL_LIBS   := $(shell pkg-config --libs sdl SDL_image SDL_ttf SDL_mixer SDL_gfx)

CFLAGS  += $(SDL_CFLAGS)
LDFLAGS := $(SDL_LIBS) -lm

OBJ := $(SRC:%.c=$(OBJDIR)/%.o)
DEP := $(OBJ:.o=.d)

all: release

debug: CFLAGS += $(DEBUG)
debug: prepare $(TARGET)

release: CFLAGS += $(RELEASE)
release: prepare $(TARGET)

prepare:
	mkdir -p $(OBJDIR) $(BINDIR)

$(TARGET): $(OBJ)
	$(CC) $^ -o $@ $(LDFLAGS)

$(OBJDIR)/%.o: %.c
	$(CC) $(CFLAGS) -MMD -MP -c $< -o $@

clean:
	rm -rf $(OBJDIR) $(BINDIR)

-include $(DEP)

.PHONY: all debug release clean prepare
