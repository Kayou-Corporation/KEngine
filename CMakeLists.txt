# --------------------------------------- #
#               CMAKE CONFIG              #
# --------------------------------------- #

# Define minimum CMake version required
cmake_minimum_required(VERSION 4.0)

# Define Project Name
project(Kayou VERSION 0.0.1 LANGUAGES CXX)

# Define output build folder
set(OUTPUT_DIR_BASE "${CMAKE_SOURCE_DIR}/Builds/${CMAKE_CXX_COMPILER_ID}_$<CONFIG>")

# Define Output Build path for libraries & executable
set(CMAKE_ARCHIVE_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}/Libs")
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}/Libs")
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${OUTPUT_DIR_BASE}")

# Build position independent code.
set(CMAKE_POSITION_INDEPENDENT_CODE ON)

# Disable C and C++ compiler extensions.
set(CMAKE_C_EXTENSIONS OFF)
set(CMAKE_CXX_EXTENSIONS OFF)



# --------------------------------------- #
#              SUB DIRECTORIES            #
# --------------------------------------- #
add_subdirectory(Engine)
add_subdirectory(Editor)



# --------------------------------------- #
#              ENABLE TESTING             #
# --------------------------------------- #
# Enable CTests
enable_testing()