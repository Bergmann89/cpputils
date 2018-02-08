# Project: test_cpputils ##########################################################################

Project                     ( test_cpputils )
Set                         ( CMAKE_CXX_STANDARD 14 )
Include                     ( GlobalCompilerFlags OPTIONAL )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Executable              ( test_cpputils EXCLUDE_FROM_ALL ${SOURCE_FILES} )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_cpputils )
EndIf                       ( )
Target_Link_Libraries       (
                                test_cpputils
                                gtest
                                gmock
                                gmock_main
                                pthread
                            )

Add_Custom_Target           ( run_test_cpputils DEPENDS test_cpputils COMMAND ./test_cpputils )

If                          ( NOT TARGET tests)
    Add_Custom_Target           ( tests )
EndIf                       ( )
Add_Dependencies            ( tests run_test_cpputils )