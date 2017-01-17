if(${verbose_traversal})
  message(STATUS "Entered GenHayes subdirectory")
endif(${verbose_traversal})
#
# headers
#
file(GLOB GenHayes_h "*.h")
list( APPEND HEADER_FILES ${GenHayes_h} )
set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

file(GLOB GenHayes_h  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.h")
foreach( file_h ${GenHayes_h})
    execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}/${file_h}
                                ${HEADER_DIR}/${file_h})
endforeach( file_h)
#
# SOURCE
#
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                                ${CMAKE_CURRENT_SOURCE_DIR}
                                ${CMAKE_CURRENT_BINARY_DIR}/SOURCE)
#
# library
#
file(GLOB GenHayes_src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Subs*/*.C")

add_library( GenHayes ${${projectName}_policy} ${GenHayes_src} )
set_property(TARGET GenHayes PROPERTY CXX_STANDARD 11)
set_property(TARGET GenHayes PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET GenHayes PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)
<<<<<<< HEAD
target_link_libraries(GenHayes Eos)
#
# Tests
#
add_subdirectory(Tests)
=======

target_link_libraries( GenHayes Eos DataBase  )

# list( APPEND tests Test1  MinP)
# list( APPEND testsWithArgs HotSpot Isobar Test2 Test2K Test2VT Test3)
# 
# foreach(test ${tests})
#   add_executable(${test}GenHayes ${test}.C)
#   target_link_libraries(${test}GenHayes GenHayes)
#  # add_test(${test}IdealGas ${test}IdealGas)
#  # set(args "file ${EOSLIB_DATA_PATH}/test_data/IdealGasTest.data")
#   add_test(NAME ${test}GenHayes
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes)
#   set_tests_properties(${test}GenHayes PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)
# 
# #### use this if test(s) require file specification####
# 
# foreach(test ${testsWithArgs})
#   add_executable(${test}GenHayes ${test}.C)
#   target_link_libraries(${test}GenHayes GenHayes)
#  # add_test(${test}IdealGas ${test}IdealGas)
#   set(args "file ${EOSLIB_DATA_PATH}/test_data/GenHayesTest.data")
#   add_test(NAME ${test}GenHayes
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_GENHAYES_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}GenHayes)
#   set_tests_properties(${test}GenHayes PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)#

install(TARGETS GenHayes
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS GenHayes)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)
>>>>>>> c30ac671e07def170a16c86a2343d793370d798c

if(${verbose_traversal})
  message(STATUS "Exiting GenHayes subdirectory")
endif(${verbose_traversal})
