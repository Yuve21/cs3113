# Source and target
SRCS = main.cpp
TARGET = raylib_app

# Add the CS3113 library if it exists
ifeq ($(wildcard CS3113/cs3113.cpp),CS3113/cs3113.cpp)
    SRCS += CS3113/cs3113.cpp
endif

# Default values
CXX = g++
CXXFLAGS = -std=c++11

# Raylib configuration using pkg-config (macOS / Linux only)
RAYLIB_CFLAGS = $(shell pkg-config --cflags raylib)
RAYLIB_LIBS = $(shell pkg-config --libs raylib)

# Which shell will make use for recipes? `echo ~` expands to a path under a POSIX
# shell (Git Bash) and prints a literal ~ under cmd.exe. Neither writes to stderr,
# so this is silent either way. Used only to pick the right delete command below.
SHELLPROBE := $(shell echo ~)
ifeq ($(SHELLPROBE),~)
    CLEANCMD = if exist $(TARGET) del /Q $(TARGET)
else
    CLEANCMD = rm -f $(TARGET)
endif

# OS detection. Windows sets $(OS) itself, so this branch is chosen from PowerShell,
# cmd AND Git Bash. uname is only called off-Windows, where it actually exists.
ifeq ($(OS),Windows_NT)
    # Windows configuration (assumes raylib in C:/raylib)
    CXXFLAGS += -IC:/raylib/include
    # -static is REQUIRED on this machine: other apps (Android SDK, PostgreSQL) put an
    # older libwinpthread-1.dll earlier on PATH, so a dynamically-linked build dies at
    # launch with 0xC0000139 as soon as the code uses std::string / iostream.
    LIBS = -LC:/raylib/lib -lraylib -lopengl32 -lgdi32 -lwinmm -static
    TARGET := $(TARGET).exe
    EXEC = ./$(TARGET)
else
    UNAME_S := $(shell uname -s)
    ifeq ($(UNAME_S), Darwin)
        # macOS configuration
        CXXFLAGS += -arch arm64 $(RAYLIB_CFLAGS)
        LIBS = $(RAYLIB_LIBS) -framework OpenGL -framework Cocoa -framework IOKit -framework CoreVideo
        EXEC = ./$(TARGET)
    else
        # Linux/WSL fallback
        LIBS = -lraylib -lGL -lm -lpthread -ldl -lrt -lX11
        EXEC = ./$(TARGET)
    endif
endif

# Build rule
$(TARGET): $(SRCS)
	$(CXX) $(CXXFLAGS) -o $(TARGET) $(SRCS) $(LIBS)

# Clean rule
clean:
	@$(CLEANCMD)

# Run rule
run: $(TARGET)
	$(EXEC)

.PHONY: clean run
