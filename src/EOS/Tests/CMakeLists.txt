if(${verbose_traversal})
  message(STATUS "Entered EOS/Tests subdirectory")
endif(${verbose_traversal})

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY "${CMAKE_CURRENT_BINARY_DIR}")
#
# sym link to blessed_output directory
set(BLESSED_OUTPUT_DIR ${CMAKE_CURRENT_SOURCE_DIR}/blessed_output)
execute_process(COMMAND "${CMAKE_COMMAND}" -E create_symlink
                         ${BLESSED_OUTPUT_DIR}
                         ${CMAKE_CURRENT_BINARY_DIR}/blessed_output)
#  Executables
#
file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "*.C")
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
 
foreach( test_c ${Test_c} )
  file(COPY ${test_c} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
  get_filename_component( test ${test_c} NAME_WE )

  add_executable(${test} ${CMAKE_CURRENT_BINARY_DIR}/${test_c})
  target_link_libraries( ${test} EOS )
endforeach( test_c )
#
# args for test
#
list( APPEND tlist Test1 Test2 Test3 )
# no args for tests

# data file for Test3
file(COPY Test.data DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
#
# run tests
#
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
set( bin "${CMAKE_CURRENT_BINARY_DIR}" )

set(CTEST_ENVIRONMENT
  "EOSLIB_DATA_PATH=${CMAKE_CURRENT_BINARY_DIR}"
  "EOSLIB_BLESSED_OUTPUT_PATH=${CMAKE_BINARY_DIR}/TEST/blessed_output"
  "CTEST_OUTPUT_ON_FAILURE=1"
  "BOOST_TEST_LOG_LEVEL='message'"
  "EOSLIB_SHARED_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")

foreach( test ${tlist} )
  add_test( NAME "${DIR}/${test}"
    COMMAND ${CMAKE_COMMAND}
      -D test_name=${test}
      -D test_cmd=${bin}/${test}
      #-D test_args:string=${${test}_args}
      -D blessed_dir=${BLESSED_OUTPUT_DIR}/${test}
      -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
      -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )
    set_tests_properties("${DIR}/${test}" PROPERTIES ENVIRONMENT
                         "${CTEST_ENVIRONMENT}")
endforeach( test )




if(${verbose_traversal})
  message(STATUS "Exiting EOS/Tests subdirectory")
endif(${verbose_traversal})
