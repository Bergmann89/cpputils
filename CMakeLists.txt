# Initialize CMake ################################################################################

If                          ( NOT CMAKE_BUILD_TYPE )
    Set                         ( CMAKE_BUILD_TYPE "Release" CACHE STRING "Choose the type of build!" FORCE )
EndIf                       ( NOT CMAKE_BUILD_TYPE )

CMake_Minimum_Required      ( VERSION 3.5.1 FATAL_ERROR )
Include                     ( cotire OPTIONAL )
CMake_Policy                ( SET CMP0065 NEW )
CMake_Policy                ( SET CMP0065 NEW )

# Projects ########################################################################################

Add_SubDirectory            ( ${CMAKE_CURRENT_SOURCE_DIR}/src )
Add_SubDirectory            ( ${CMAKE_CURRENT_SOURCE_DIR}/test )
