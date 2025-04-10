CC:=gcc
CXXFLAGS:=-g -Wall -Wextra
CPPFLAGS:=-DDEBUG
SRC:=$(wildcard *.c)
OBJDIR:=./build
OBJ:=$(SRC:%.c=$(OBJDIR)/%.o)
OUTPUT:=DNS
RM=rm -rf

$(OUTPUT): $(OBJ)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) $^ -o $@
	echo "Final Binary have been built..."

$(OBJDIR)/%.o:%.c
	mkdir -p $(@D)
	$(CC) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@
	echo "Object files created..."

clean:
	$(RM) $(OBJ) $(OUTPUT) $(OBJDIR) *.o
