# Output binary
BIN := remote.exe

COMPILE_WITH_NO_BUTTONS := -DNO_BUTTONS
#BUTTONS_LIBRARY := -lwiringPi
#BUTTONS_SRC_FILE := RemoteKeyboardInputReceiver.cpp

# Source files
SRCS := \
    RemoteController.cpp \
    sender.c \
    $(BUTTONS_SRC_FILE)

# Intermediate directory for generated object files
OBJDIR := .o
# Intermediate directory for generated dependency files
DEPDIR := .d

# Object files, auto generated from source files
OBJS := $(patsubst %,$(OBJDIR)/%.o,$(basename $(SRCS)))
# Dependency files, auto generated from source files
DEPS := $(patsubst %,$(DEPDIR)/%.d,$(basename $(SRCS)))

# Compilers (at least gcc and clang) don't create the subdirectories automatically
$(shell mkdir -p $(dir $(OBJS)) >/dev/null)
$(shell mkdir -p $(dir $(DEPS)) >/dev/null)

# C compiler
CC := gcc
# C++ compiler
CXX := g++
# linker
LD := g++

# C flags
CFLAGS := -std=c17
# C++ flags
CXXFLAGS := -std=c++14
# C/C++ shared flags
CPPFLAGS := -I../include -pipe -Os -Wall -Wextra -Wpedantic -Wconversion -Wformat-security -Wstack-protector -fstack-clash-protection -ftrapv -D_FORTIFY_SOURCE=1
# Linker flags
LDFLAGS := -z relro -z now
# Linker flags: libraries to link (e.g. -lfoo)
LDLIBS := -lncurses -lpthread $(BUTTONS_LIBRARY)
# Flags required for dependency generation; passed to compilers
DEPFLAGS = -MT $@ -MD -MP -MF $(DEPDIR)/$*.Td

# Compile C source files
COMPILE.c = $(CC) $(DEPFLAGS) $(CFLAGS) $(CPPFLAGS) -c -o $@
# Compile C++ source files
COMPILE.cpp = $(CXX) $(DEPFLAGS) $(CXXFLAGS) $(CPPFLAGS) $(COMPILE_WITH_NO_BUTTONS) -c -o $@
# Link object files to binary
LINK.o = $(LD) $(LDFLAGS) $(LDLIBS) -o $@
# Precompile step
PRECOMPILE =
# Postcompile step
POSTCOMPILE = mv -f $(DEPDIR)/$*.Td $(DEPDIR)/$*.d

all: $(BIN)

.PHONY: clean
clean:
	$(RM) -r $(OBJDIR) $(DEPDIR) $(BIN)

.PHONY: check
check:
	@echo No tests configured

.PHONY: help
help:
	@echo Available targets: all check clean

$(BIN): $(OBJS)
	$(LINK.o) $^

$(OBJDIR)/%.o: %.c
$(OBJDIR)/%.o: %.c $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.c) $<
	$(POSTCOMPILE)

$(OBJDIR)/%.o: %.cpp
$(OBJDIR)/%.o: %.cpp $(DEPDIR)/%.d
	$(PRECOMPILE)
	$(COMPILE.cpp) $<
	$(POSTCOMPILE)

.PRECIOUS: $(DEPDIR)/%.d
$(DEPDIR)/%.d: ;

-include $(DEPS)
