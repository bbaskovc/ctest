# Minimum required version of CMake
cmake_minimum_required(VERSION 3.16)

# Print a status message indicating the project being built
message(STATUS "Configuring CMake project: ctest")

# Define the list of source files
set(SRC_FILES
    src/ctest.c
)

# Define a list of include directories
set(INC_DIRS
    inc/ 
)

# Define a list of required libraries for linking
set(REQ_LIBS 
)

# Check if the build is targeting ESP-IDF.
if(IDF_TARGET)
    message(STATUS "Building as ESP-IDF project")
    # Register component to ESP-IDF
    idf_component_register(SRCS ${SRC_FILES} INCLUDE_DIRS ${INC_DIRS} REQUIRES ${REQ_LIBS})
else()
    message(STATUS "Building as CMake project")
    # Define the project name.
    project(ctest)
    # Define a static library from the specified source files.
    add_library(${PROJECT_NAME} ${SRC_FILES})
    # Specify the include directories for the target library.
    target_include_directories(${PROJECT_NAME} PUBLIC ${INC_DIRS})
    # Link required libraries (empty in this case).
    target_link_libraries(${PROJECT_NAME} PRIVATE ${REQ_LIBS})
endif()