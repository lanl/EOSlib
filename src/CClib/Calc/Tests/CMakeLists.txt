if(${verbose_traversal})
  message(STATUS "Entered Calc/Tests subdirectory")
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
file(GLOB Test_c  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "Test*.C")

foreach( test_c ${Test_c} )
  file(COPY ${test_c} DESTINATION ${CMAKE_CURRENT_BINARY_DIR}) 
  get_filename_component( test ${test_c} NAME_WE )

  add_executable(${test} ${CMAKE_CURRENT_BINARY_DIR}/${test_c})
  target_link_libraries( ${test} Calc Io String Math LinkList)
endforeach( test_c )
#
# args for test
#
list( APPEND tlist TestCalc )

set(TestCalc_args  "rho=rho^2 rho+1 m(1,2)+m(1,3) pi=4*atan(1)")
#
# run tests
#
file(GLOB DIR  RELATIVE "${CMAKE_SOURCE_DIR}" "${CMAKE_CURRENT_SOURCE_DIR}")
set( bin "${CMAKE_CURRENT_BINARY_DIR}" )


foreach( test ${tlist} )
  add_test( NAME "${DIR}/${test}"
    COMMAND ${CMAKE_COMMAND}
      -D test_name=${test}
      -D test_cmd=${bin}/${test}
      -D test_args:string=${${test}_args}
      -D blessed_dir=${BLESSED_OUTPUT_DIR}/${test}
      -D run_dir=${CMAKE_CURRENT_BINARY_DIR}/Testing
      -P ${CMAKE_BINARY_DIR}/TEST/NEWcompare_outputs.cmake
    )
    set_tests_properties("${DIR}/${test}" PROPERTIES ENVIRONMENT
                         "${CTEST_ENVIRONMENT}")
endforeach( test )


if(${verbose_traversal})
  message(STATUS "Exiting Calc/Tests subdirectory")
endif(${verbose_traversal})
