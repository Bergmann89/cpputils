# Project: cpputils ###############################################################################

Project                     ( cpputils VERSION 1.0.0.0 LANGUAGES CXX )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cpputils ${SOURCE_FILES} )
Target_Include_Directories  (
                                cpputils
                                PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/../include
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cpputils )
EndIf                       ( )
