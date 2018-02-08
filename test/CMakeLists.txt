# Project: test_tsoutils ##########################################################################

Project                     ( test_tsoutils )
Set                         ( CMAKE_CXX_STANDARD 14 )
Include                     ( GlobalCompilerFlags OPTIONAL )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Executable              ( test_tsoutils EXCLUDE_FROM_ALL ${SOURCE_FILES} )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_tsoutils )
EndIf                       ( )
Target_Link_Libraries       (
                                test_tsoutils
                                gtest
                                gmock
                                gmock_main
                                pthread
                            )

Add_Custom_Target           ( run_test_tsoutils DEPENDS test_tsoutils COMMAND ./test_tsoutils )

If                          ( NOT TARGET tests)
    Add_Custom_Target           ( tests )
EndIf                       ( )
Add_Dependencies            ( tests run_test_tsoutils )