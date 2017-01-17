if(${verbose_traversal})
  message(STATUS "Entered DnKappa subdirectory")
endif(${verbose_traversal})

message(STATUS "DnKappa\n")
<<<<<<< HEAD
#
# headers
#
file(GLOB DnKappa_h "*.h")
list( APPEND HEADER_FILES ${DnKappa_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB DnKappa_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${DnKappa_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
#
# Source
#
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB DnKappa_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")
=======

set(EOSLIB_DNKAPPA_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/DnKappa)

set(DnKappa_src dummy.cpp)
add_subdirectory(Subs)


>>>>>>> c30ac671e07def170a16c86a2343d793370d798c
add_library( DnKappa ${${projectName}_policy} ${DnKappa_src} )
set_property(TARGET DnKappa PROPERTY CXX_STANDARD 11)
set_property(TARGET DnKappa PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DnKappa PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
set_property(TARGET DnKappa PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( DnKappa Eos)
#
# test
#
add_subdirectory(Tests)

<<<<<<< HEAD
=======
target_link_libraries( DnKappa Io Math Eos Materials ${CMAKE_DL_LIBS} )

# list( APPEND tests DnKappa TestDnKappa )
# 
# foreach(test ${tests})
#   add_executable(${test}DnKappa ${test}.C)
#   target_link_libraries(${test}DnKappa DnKappa)
#   add_test(NAME ${test}DnKappa
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}DnKappa
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
#   set_tests_properties(${test}DnKappa PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)



>>>>>>> c30ac671e07def170a16c86a2343d793370d798c
if(${verbose_traversal})
  message(STATUS "Exiting DnKappa subdirectory")
endif(${verbose_traversal})
