######## Vienna RNA Package Configuration ##################
VIENNA=/usr/local/
############################################################

# Compiler settings - using latest GCC with reduced warnings for Vienna RNA compatibility
CXX = /usr/local/bin/g++-15
CXXFLAGS = -std=c++17 -O3 -Wall -Wextra -Wformat=2 -Wunused \
           -Wno-unused-parameter -Wno-pedantic -march=native -mtune=native
CPPFLAGS = -I$(VIENNA)/include/ViennaRNA/ -I$(VIENNA)/include/
LDFLAGS = -L$(VIENNA)/lib -fopenmp
LIBS = -lRNA

# Debug build option
DEBUG ?= 0
ifeq ($(DEBUG), 1)
    CXXFLAGS += -g -DDEBUG -fno-omit-frame-pointer -fsanitize=address,undefined
    LDFLAGS += -fsanitize=address,undefined
    CXXFLAGS := $(filter-out -O3,$(CXXFLAGS)) -O0
endif

# Source and object files
SRCDIR = src
SOURCES = $(SRCDIR)/CDSfold.cpp
OBJECTS = $(SOURCES:.cpp=.o)
TARGET = $(SRCDIR)/CDSfold

# Default target
all: $(TARGET)

# Compile object files
$(SRCDIR)/%.o: $(SRCDIR)/%.cpp
	$(CXX) $(CXXFLAGS) $(CPPFLAGS) -c $< -o $@

# Link executable
$(TARGET): $(OBJECTS)
	$(CXX) $(CXXFLAGS) $(LDFLAGS) -o $@ $^ $(LIBS)

# Legacy targets for compatibility
compile: $(OBJECTS)

link: $(TARGET)

# Debug build
debug:
	$(MAKE) DEBUG=1

# Clean build artifacts
clean:
	-rm -f $(OBJECTS) $(TARGET)

# Show compiler and system information
info:
	@echo "Compiler: $(CXX)"
	@$(CXX) --version
	@echo "CXXFLAGS: $(CXXFLAGS)"
	@echo "Vienna RNA path: $(VIENNA)"

# Check Vienna RNA installation
check-vienna:
	@echo "Checking Vienna RNA installation..."
	@test -d "$(VIENNA)" || (echo "Error: Vienna RNA directory not found: $(VIENNA)" && false)
	@test -f "$(VIENNA)/include/ViennaRNA/fold.h" || (echo "Error: Vienna RNA headers not found" && false)
	@test -f "$(VIENNA)/lib/libRNA.a" -o -f "$(VIENNA)/lib/libRNA.so" -o -f "$(VIENNA)/lib/libRNA.dylib" || (echo "Error: Vienna RNA library not found" && false)
	@echo "Vienna RNA installation verified"

# Test build (run basic test after building)
test: $(TARGET)
	@echo "Running basic functionality test..."
	@test -f example/test.faa || (echo "Error: Test file example/test.faa not found" && false)
	./$(TARGET) example/test.faa

# Install dependencies (macOS specific)
install-deps:
	@echo "Installing dependencies via Homebrew..."
	brew install gcc || echo "GCC already installed"

# Show help
help:
	@echo "CDSfold Makefile - Modern GCC Build System"
	@echo ""
	@echo "Targets:"
	@echo "  all          - Build the CDSfold executable (default)"
	@echo "  compile      - Compile source files to object files"
	@echo "  link         - Link object files to create executable"
	@echo "  debug        - Build with debug symbols and sanitizers"
	@echo "  clean        - Remove build artifacts"
	@echo "  test         - Build and run basic functionality test"
	@echo "  check-vienna - Verify Vienna RNA installation"
	@echo "  info         - Show compiler and build information"
	@echo "  install-deps - Install required dependencies (macOS)"
	@echo "  help         - Show this help message"
	@echo ""
	@echo "Variables:"
	@echo "  VIENNA       - Path to Vienna RNA installation (must be set)"
	@echo "  DEBUG        - Set to 1 for debug build (default: 0)"
	@echo "  CXX          - C++ compiler (default: $(CXX))"

.PHONY: all compile link debug clean info check-vienna test install-deps help
