# Quelldateien
SRCS             = main.c io.c logic.c scene.c texture.c stringOutput.c #debugGL.c

# ausfuehrbares Ziel
TARGET           = ueb01

# Objektdateien
OBJS             = $(SRCS:.c=.o)

# Compiler
CC               = gcc

# Linker Flags
CFLAGS  = -Wall -Wextra -O3 -Wno-unused-parameter -Werror -O3 #-D DEBUG

# Linker
LD               = gcc

# Linker libraries
LDLIBS    	 = -lm  -lglut -lGLU -lGL

.SUFFIXES: .o .c
.PHONY: all clean

# TARGETS
all: $(TARGET)

# Linken des ausfuehrbaren Programms
$(TARGET): $(OBJS)
	$(LD) $(OBJS) $(LDLIBS) -o $(TARGET)

# Kompilieren der Objektdateien
%.o: %.c
	$(CC) $(CPPFLAGS) $(CFLAGS) -c -o $*.o $*.c

# einfaches Aufraeumen
clean:
	rm -f $(TARGET)
	rm -f $(OBJS)
	rm -f *~

