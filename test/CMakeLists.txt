# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( cmake_tests OPTIONAL )

Include                     ( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/options.cmake )

Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

# Project: test_cpputils ##########################################################################

Project                     ( test_cpputils )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Executable              ( test_cpputils EXCLUDE_FROM_ALL ${SOURCE_FILES} )
Target_Link_Libraries       ( test_cpputils
                              cpputils
                              gmock_main
                              gmock
                              gtest
                              pthread )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_cpputils )
EndIf                       ( )
If                          ( __CMAKE_TESTS_INCLUDED )
    Add_CMake_Test             ( NAME cpputils
                                 TARGET test_cpputils )
EndIf                       ( )
