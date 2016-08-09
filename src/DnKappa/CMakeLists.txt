if(${verbose_traversal})
  message(STATUS "Entered DnKappa subdirectory")
endif(${verbose_traversal})

message(STATUS "blessed output in DnKappa: ${EOSLIB_BLESSED_OUTPUT}\n")
set(EOSLIB_DNKAPPA_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/DnKappa)

set(DnKappa_src dummy.cpp)

add_subdirectory(Subs)


add_library( DnKappa ${${projectName}_policy} ${DnKappa_src} )
set_property(TARGET DnKappa PROPERTY CXX_STANDARD 11)
set_property(TARGET DnKappa PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DnKappa PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( DnKappa Io Math Eos Materials ${CMAKE_DL_LIBS} )

list( APPEND tests DnKappa TestDnKappa )

foreach(test ${tests})
  add_executable(${test}DnKappa ${test}.C)
  target_link_libraries(${test}DnKappa DnKappa)
  add_test(NAME ${test}DnKappa
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}DnKappa
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_DNKAPPA_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test})
  set_tests_properties(${test}DnKappa PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting DnKappa subdirectory")
endif(${verbose_traversal})
