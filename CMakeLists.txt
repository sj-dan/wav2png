cmake_minimum_required(VERSION 3.12)
project(wav2png C)

# set(CMAKE_SYSTEM_NAME Windows)
# set(CMAKE_C_COMPILER x86_64-w64-mingw32-gcc)
# set(CMAKE_RC_COMPILER x86_64-w64-mingw32-windres)

set(CMAKE_C_COMPILER gcc)

# Find libpng

set(PNG_INCLUDE_DIR "/usr/include/libpng16")
find_package(PNG REQUIRED)
include_directories(${PNG_INCLUDE_DIRS})

# Find libsndfile
find_package(SndFile REQUIRED)
include_directories(${SNDFILE_INCLUDE_DIRS})

# Define your executable
add_executable(wav2png wav2png.c)

# Link against libpng and libsndfile
target_link_libraries(wav2png ${PNG_LIBRARIES} ${SNDFILE_LIBRARIES} m)
 
