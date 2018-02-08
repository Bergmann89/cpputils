# Project: tsoutils ###############################################################################

Project                     ( tsoutils )
Set                         ( CMAKE_CXX_STANDARD 14 )
Include                     ( GlobalCompilerFlags OPTIONAL )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( tsoutils STATIC ${SOURCE_FILES} )
Target_Include_Directories  (
                                tsoutils
                                INTERFACE ${CMAKE_CURRENT_SOURCE_DIR}
                            )
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( tsoutils )
EndIf                       ( )