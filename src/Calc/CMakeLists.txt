if(${verbose_traversal})
  message(STATUS "Entered Calc subdirectory")
endif(${verbose_traversal})

file(GLOB Calc_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
file(COPY ${Calc_h} DESTINATION ${HEADER_DIR})
#list(APPEND HEADER_FILES ${Calc_h})
#set(HEADER_FILES "${HEADER_FILES}" PARENT_SCOPE)
#
# To Do: src/CMakeList.txt
#    remove include_directories(Calc)
#    add include_directories(${HEADER_DIR})
#
# To Do: eoslib/CMakeList.txt
#    delete list(APPEND HEADER_FILES . . .
#    delete install(FILES ${HEADER_FILES} DESTINATION include)
#    add install for all header files in ${CMAKE_CURRENT_BINARY_DIR}/include
#        install for all executables  in ${CMAKE_CURRENT_BINARY_DIR}/bin
#        install for all libs         in ${CMAKE_CURRENT_BINARY_DIR}/lib
#        install for all DATA files   in ${CMAKE_CURRENT_BINARY_DIR}/DATA
#    Note:
#      runEOS and listEOS scripts assume directories eoslib/{bin,lib,DATA}
#      typically headers     in /usr/local/include
#                executables in /usr/local/bin
#                libs        in /usr/local/lib64
#                data        in /usr/share
#      would need to modify runEOS and listEOS to find libs and data directories
#      Alternatively install in /opt/eoslib with subdirs {bin,lib,DATA,include}
#      then user would need to add /opt/eoslib/bin to PATH
#      or skip install and add to PATH .../eoslib/build/bin


#file(GLOB Calc_src  "Subs/*.C")
file(GLOB Calc_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs/*.C")
#message( STATUS "Subs ${Calc_src}")

add_library( Calc ${${projectName}_policy} ${Calc_src} )
set_property(TARGET Calc PROPERTY CXX_STANDARD 11)
set_property(TARGET Calc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries(Calc LinkList Io Math String)
target_include_directories(Calc BEFORE PUBLIC ${CMAKE_CURRENT_SOURCE_DIR})

add_subdirectory(Tests)

execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)

if(${verbose_traversal})
  message(STATUS "Exiting Calc subdirectory")
endif(${verbose_traversal})
