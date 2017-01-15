if(${verbose_traversal})
  message(STATUS "Entered Manager subdirectory")
endif(${verbose_traversal})

#message(STATUS "blessed output in Manager: ${EOSLIB_BLESSED_OUTPUT}\n")
#set(EOSLIB_MANAGER_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/Manager)
#set(Manager_src)

add_subdirectory(Subs)

add_library( Manager ${${projectName}_policy} ${Manager_src} )
set_property(TARGET Manager PROPERTY CXX_STANDARD 11)
set_property(TARGET Manager PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Manager PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Manager ExtEOS )

#list (APPEND testsWithArgs Test2JWL_VT TestBKW TestDeflagration TestDetonation TestHEfit TestHEprods TestJWL TestJWLPT )
#list (APPEND tests Test1JWL_VT  )

# foreach(test ${tests} )
#   add_executable(${test} ${test}.C)
#   target_link_libraries(${test} JWL)
#   #set(args "file ${EOSLIB_DATA_PATH}/test_data/JWLTest.data")
#   add_test(NAME ${test}
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Out.blessed
#   -D err_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Err.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
#   set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

# foreach(test ${testsWithArgs} )
#   add_executable(${test} ${test}.C)
#   target_link_libraries(${test} JWL)
#   set(args "file ${EOSLIB_DATA_PATH}/test_data/JWLTest.data")
#   add_test(NAME ${test}
#   COMMAND ${CMAKE_COMMAND}
#   -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}
#   -D test_args:string=${args}
#   -D out_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Out.blessed
#   -D err_blessed=${EOSLIB_JWL_BLESSED_OUTPUT}/${test}Err.blessed 
#   -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
#   )
#   set_tests_properties(${test} PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

install(TARGETS Manager
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS Manager)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Manager subdirectory")
endif(${verbose_traversal})



