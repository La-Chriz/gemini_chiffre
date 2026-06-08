# This Makefile builds an executable and
# copies the executable into the $(INSTALL_DIR) directory

NAME := gemini_chiffre
EXE := $(NAME)

SRC_DIR := .
INC_DIR := $(HOME)/include
BIN_DIR := ./bin
BUILD_DIR := ./build
INSTALL_DIR := $(HOME)/bin

# Find all source files recursively
SRCS := $(shell find $(SRC_DIR) -type f \( -name "*.cpp" -o -name "*.c" -o -name "*.s" \))

# Find all unique header directories
INC_DIRS := $(sort $(dir $(shell find $(INC_DIR) -type f -name "*.h")))

# KORREKTUR: Sichere Generierung der Objektdateien ohne Pfad-Vervielfachung
OBJS := $(patsubst $(SRC_DIR)/%,$(BUILD_DIR)/%,$(basename $(SRCS)))
OBJS := $(addsuffix .o,$(OBJS))
DEPS := $(OBJS:.o=.d)

# Include flags for the compiler
INC_FLAGS := $(addprefix -I,$(INC_DIRS))

# Compiler flags
CFLAGS := $(INC_FLAGS) -Wall -pedantic -Wextra -std=c23
CPPFLAGS := $(INC_FLAGS) -MMD -MP
LDFLAGS :=

# Target rules
.PHONY: all
all: $(BIN_DIR)/$(EXE)

# KORREKTUR: Verwende $(CC) statt $(CXX) zum Linken, da es ein reines C-Projekt ist
$(BIN_DIR)/$(EXE): $(OBJS) | $(BIN_DIR)
	$(CC) $(LDFLAGS) $(CFLAGS) $(CPPFLAGS) $^ -o $@

# Rules for creating object files
$(BUILD_DIR)/%.o: $(SRC_DIR)/%.cpp
	mkdir -p $(dir $@)
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c
	mkdir -p $(dir $@)
	$(CC) $(CPPFLAGS) $(CFLAGS) -c $< -o $@

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.s
	mkdir -p $(dir $@)
	$(AS) $(ASFLAGS) -c $< -o $@

# Rule to create the build and bin directories
$(BUILD_DIR):
	mkdir -p $@

$(BIN_DIR):
	mkdir -p $@

$(INSTALL_DIR):
	mkdir -p $@

## Installation Regeln
.PHONY: install
install: install-exe

# Install the exe
install-exe: $(INSTALL_DIR)
	install -m 751 $(BIN_DIR)/$(EXE) $(INSTALL_DIR)

-include $(DEPS)

.PHONY: clean
clean:
	$(RM) -r $(BUILD_DIR) $(BIN_DIR)

