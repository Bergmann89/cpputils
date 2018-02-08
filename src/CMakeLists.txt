# Project: cpputils ###############################################################################

Project                     ( cpputils )
Set                         ( CMAKE_CXX_STANDARD 14 )
Include                     ( GlobalCompilerFlags OPTIONAL )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cpputils STATIC ${SOURCE_FILES} )
Target_Include_Directories  (
    cpputils
                                INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cpputils )
EndIf                       ( )