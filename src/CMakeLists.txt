# Initialize ######################################################################################

Include                     ( cotire OPTIONAL )
Include                     ( pedantic OPTIONAL )
Include                     ( strip_symbols OPTIONAL )

Include                     ( ${CMAKE_CURRENT_SOURCE_DIR}/../cmake/options.cmake )

Set                         ( BUILD_SHARED_LIBS ${CPPUTILS_BUILD_SHARED} )
Set                         ( CMAKE_CXX_STANDARD 17 )
Set                         ( CMAKE_C_FLAGS   "${CMAKE_C_FLAGS}   ${PEDANTIC_C_FLAGS}" )
Set                         ( CMAKE_CXX_FLAGS "${CMAKE_CXX_FLAGS} ${PEDANTIC_CXX_FLAGS}" )

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
If                          ( __STRIP_SYMBOLS_INCLUDED AND BUILD_SHARED_LIBS )
    Strip_Symbols               ( cpputils DBG_FILE )
EndIf                       ()
