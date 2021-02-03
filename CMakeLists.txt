cmake_minimum_required(VERSION 3.10)

project(peppapeg C)

set(P4_LIB peppapeg)
set(P4_HEADERS peppapeg.h)
set(P4_SOURCES peppapeg.c)
add_library("${P4_LIB}" SHARED "${P4_HEADERS}" "${P4_SOURCES}")

add_subdirectory(tests)
