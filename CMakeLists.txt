#####################################################################################################################

# Jorge Agramunt Ros    @ IFIC(Valencia,Spain)  jorge.agramunt@ific.uv.es
# Alvaro Tolosa Delgado @ IFIC(Valencia,Spain)  alvaro.tolosa@ific.uv.es
# Oscar Hall            @  UoE(Edinburgh, UK )  oscar.hall@ed.ac.uk
# Copyright (c) 2016 Jorge Agramunt, Alvaro Tolosa & Oscar Hall. All rights reserved.

#####################################################################################################################

# Example adapted from https://root.cern.ch/faq/can-i-integrate-root-my-cmake-build

#####################################################################################################################



# CMakeLists.txt for event package. It creates a library with dictionary and a main program
cmake_minimum_required(VERSION 3.0 FATAL_ERROR)

   SET(CMAKE_BUILD_TYPE Debug)
   include(CheckCCompilerFlag)
   include(CheckCXXCompilerFlag)
   

project(aidasort)

# You need to tell CMake where to find the ROOT installation. This can be done in a number of ways:
#   - ROOT built with classic configure/make use the provided $ROOTSYS/etc/cmake/FindROOT.cmake
#   - ROOT built with CMake. Add in CMAKE_PREFIX_PATH the installation prefix for ROOT
list(APPEND CMAKE_PREFIX_PATH $ENV{ROOTSYS})

#---Locate the ROOT package and defines a number of variables (e.g. ROOT_INCLUDE_DIRS)
find_package(ROOT REQUIRED COMPONENTS MathCore RIO Hist Tree Net Graf3d Graf Gpad Thread)

#---Define useful ROOT functions and macros (e.g. ROOT_GENERATE_DICTIONARY)
include(${ROOT_USE_FILE})

include_directories(${CMAKE_SOURCE_DIR} ${ROOT_INCLUDE_DIRS})
add_definitions(${ROOT_CXX_FLAGS} -lTreePlayer )
#ROOT_GENERATE_DICTIONARY(G__data2Tree data2Tree.hpp BRIKENTreeData.h LINKDEF data2TreeLinkDef.hpp)


#---Create  a main program using the library
add_executable(AIDASort main.cpp)
target_link_libraries(AIDASort ${ROOT_LIBRARIES})

# Library threads
find_package (Threads)
# C++11
set_property(TARGET AIDASort PROPERTY CXX_STANDARD 11)
set_property(TARGET AIDASort PROPERTY CXX_STANDARD_REQUIRED ON)
# PIC
set_property(TARGET AIDASort PROPERTY POSITION_INDEPENDENT_CODE ON)
# 64-bit
check_c_compiler_flag(-m64 M64TestFlagC)
check_cxx_compiler_flag(-m64 M64TestFlagCXX)