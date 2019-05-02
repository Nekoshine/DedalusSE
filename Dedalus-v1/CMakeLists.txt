cmake_minimum_required(VERSION 3.0.0)
project(Dedalus VERSION 0.1.0)


#########################################################################
# General parameters
#########################################################################

# Require out-of-source builds
file(TO_CMAKE_PATH "${PROJECT_BINARY_DIR}/CMakeLists.txt" LOC_PATH)
if(EXISTS "${LOC_PATH}")
    message(FATAL_ERROR "You cannot build in a source directory (or any directory with a CMakeLists.txt file). Please make a build subdirectory. Feel free to remove CMakeCache.txt and CMakeFiles.")
endif()

# Potential CMAKE directory
set(CMAKE_MODULE_PATH "${PROJECT_SOURCE_DIR}/cmake" ${CMAKE_MODULE_PATH})

#########################################################################
# Functions
#########################################################################
function(set_compile_options target)

    #target_compile_features(${target} PUBLIC cxx_std_11)
    #set_target_properties(${target} PROPERTIES CXX_STANDARD 11)

    set_target_properties(${target} PROPERTIES C_STANDARD 99)
    target_compile_options(${target} PRIVATE -W)
    target_compile_options(${target} PRIVATE -Wall)
    target_compile_options(${target} PRIVATE -Wextra)
    target_compile_options(${target} PRIVATE -Wwrite-strings)
    target_compile_options(${target} PRIVATE -Wconversion)
    target_compile_options(${target} PRIVATE -Werror)

    #target_compile_options(${target} PRIVATE -pedantic-errors)

    target_compile_definitions(${target} PUBLIC _DEFAULT_SOURCE) # to get usleep
    #set_property(TARGET ${target} PROPERTY POSITION_INDEPENDENT_CODE ON)


endfunction(set_compile_options)

#########################################################################
# BUILD Dedalus BIN
#########################################################################

file(GLOB_RECURSE Dedalus_sources ${CMAKE_CURRENT_SOURCE_DIR}/src/*.c)
set(DedalusSRC  ${Dedalus_sources})
file(GLOB_RECURSE Dedalus_headers ${CMAKE_CURRENT_SOURCE_DIR}/src/*.h)
set(DedalusHEADERS  ${Dedalus_headers})

add_executable(Dedalus ${DedalusSRC} ${DedalusHEADERS})
set_compile_options(Dedalus)

# add lib math
target_link_libraries(Dedalus PUBLIC m) 

#########################################################################
# INSTALL
#########################################################################
install (TARGETS Dedalus DESTINATION ${PROJECT_SOURCE_DIR}/bin)

#########################################################################
# DOCUMENTATION
#########################################################################
find_package(Doxygen)
if(DOXYGEN_FOUND)
    add_custom_target(doc COMMAND ${DOXYGEN_EXECUTABLE} "${PROJECT_SOURCE_DIR}/src/dox/Doxyfile"
        WORKING_DIRECTORY ${CMAKE_HOME_DIRECTORY}
        COMMENT "Building user's documentation into doxyDoc build dir..."
    )
endif()
