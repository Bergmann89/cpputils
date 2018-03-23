# Initialize CMake ################################################################################

If                          ( NOT CMAKE_BUILD_TYPE )
    Set                         ( CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build!" FORCE )
EndIf                       ( NOT CMAKE_BUILD_TYPE )

CMake_Minimum_Required      ( VERSION 3.5.1 FATAL_ERROR )
Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_MODULE_PATH ${CMAKE_MODULE_PATH} "${CMAKE_CURRENT_SOURCE_DIR}/../../../inc/cmake/")
Include                     ( GlobalCompilerFlags OPTIONAL )
Include                     ( cotire OPTIONAL )

# Tests ###########################################################################################

Include                     ( CTest )
Enable_Testing              ( )

# Projects ########################################################################################

Add_SubDirectory            ( ${CMAKE_CURRENT_SOURCE_DIR}/src )
Add_SubDirectory            ( ${CMAKE_CURRENT_SOURCE_DIR}/test )
