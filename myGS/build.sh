#!/bin/bash

# Colors for output
GREEN='\033[0;32m'
RED='\033[0;31m'
BLUE='\033[0;34m'
NC='\033[0m' # No Color

# Project paths
PROJECT_ROOT="/home/leona/jpleona_c/raylibGS/myGS"
EXECUTABLE_PATH="$PROJECT_ROOT/build/example"

# Navigate to project root
echo -e "${BLUE}Navigating to project directory...${NC}"
cd "$PROJECT_ROOT" || {
    echo -e "${RED}Failed to navigate to project directory${NC}"
    exit 1
}

# Create build directory if it doesn't exist
if [ ! -d "build" ]; then
    echo "Creating build directory..."
    mkdir build
fi

# Run CMake configure step
echo -e "${GREEN}Running CMake configure...${NC}"
if ! cmake -B build; then
    echo -e "${RED}CMake configure failed${NC}"
    exit 1
fi

# Run CMake build step
echo -e "${GREEN}Building project...${NC}"
if ! cmake --build build; then
    echo -e "${RED}Build failed${NC}"
    exit 1
fi

# Run the executable
if [ -f "$EXECUTABLE_PATH" ]; then
    echo -e "${GREEN}Build successful! Running the game...${NC}"
    "$EXECUTABLE_PATH"
else
    echo -e "${RED}Executable not found at: $EXECUTABLE_PATH${NC}"
    exit 1
fi