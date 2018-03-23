# Project: test_cpputils ##########################################################################

Project                     ( test_cpputils )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Executable              ( test_cpputils EXCLUDE_FROM_ALL ${SOURCE_FILES} )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( test_cpputils )
EndIf                       ( )
Target_Link_Libraries       (
                                test_cpputils
                                cpputils
                                gtest
                                gmock
                                gmock_main
                                pthread
                            )

# Build Tests #####################################################################################

If                          ( NOT TARGET build_tests)
    Add_Custom_Target           ( build_tests )
EndIf                       ( )
Add_Dependencies            ( build_tests test_cpputils )

# Run Tests #######################################################################################

If                          ( NOT TARGET tests)
    Add_Custom_Target           ( tests )
EndIf                       ( )
Add_Custom_Target           ( run_test_cpputils DEPENDS test_cpputils COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_cpputils )
Add_Dependencies            ( tests run_test_cpputils )

# CTest ###########################################################################################

Add_Test                    ( NAME cpputils COMMAND ${CMAKE_CURRENT_BINARY_DIR}/test_cpputils )
