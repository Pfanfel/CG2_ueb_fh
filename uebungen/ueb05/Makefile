PROG = ueb05

SRCDIR = src/
BUILDDIR = build/

vpath %.c $(SRCDIR)
vpath %.h $(SRCDIR)
vpath %.o $(OBJDIR)

CC = gcc
CCFLAGS = -Wall -Wextra -Wno-unused-parameter -Werror -O3
SRCS = $(shell find $(SRCDIR) -type f -name '*.c')
HEDS = $(shell find $(SRCDIR) -type f -name '*.h')
OBJS = $(SRCS:$(SRCDIR)%.c=$(BUILDDIR)%.o)

GL   = -lglut -lGLU -lGL #-lGLEW
MATH = -lm
LIBS = $(MATH) $(GL)

INCLUDES = -I $(SRCDIR) # Kann spaeter wichtig werden

.PHONY: directories clean all doc debug

$(PROG): directories .depend $(OBJS)
	@echo "\e[1;34mBuilding" $@ "\e[0m"
	$(CC) $(CCFLAGS) -o $(PROG) $(OBJS) $(LIBS)
	@echo "\e[1;34mDONE Creating" $@ "\e[0m"
	

debug: CCFLAGS += -g -D DEBUG
debug: $(PROG)

all: $(PROG)

clean:
	rm -f  $(PROG)
	rm -f  $(OBJS)
	rm -f  .depend
	rm -rf $(BUILDDIR)

directories:
	mkdir -p $(BUILDDIR)

$(BUILDDIR)%.o : %.c
	$(CC) $(CCFLAGS) -c $< -o $@

.depend : $(SRCS)
	$(CC) $(CCFLAGS) -MM $^ > .depend

include .depend
