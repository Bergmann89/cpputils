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

# Build
Project                     ( cpputils VERSION 1.0.0.0 LANGUAGES CXX )
Set                         ( CPPUTILS_INCLUDE_DIR ${CMAKE_CURRENT_SOURCE_DIR}/../include )
File                        ( GLOB_RECURSE SOURCE_FILES ${CMAKE_CURRENT_SOURCE_DIR}/*.cpp )
Add_Library                 ( cpputils ${SOURCE_FILES} )
Target_Include_Directories  ( cpputils
                              PUBLIC ${CPPUTILS_INCLUDE_DIR} )

# Install
If                          ( BUILD_SHARED_LIBS OR CPPUTILS_INSTALL_DEV_FILES )
    Install                     ( TARGETS cpputils DESTINATION lib )
EndIf                       ( )

If                          ( CPPUTILS_INSTALL_DEV_FILES )
    Install                     ( DIRECTORY ${CPPUTILS_INCLUDE_DIR}/cpputils DESTINATION include )
    Install                     ( DIRECTORY ${CPPUTILS_INCLUDE_DIR}/nlohmann DESTINATION include )
EndIf                       ( )

# Optimize
If                          ( __COTIRE_INCLUDED )
    Cotire                      ( cpputils )
EndIf                       ( )
If                          ( __STRIP_SYMBOLS_INCLUDED AND BUILD_SHARED_LIBS )
    Strip_Symbols               ( cpputils DBG_FILE )
EndIf                       ()
