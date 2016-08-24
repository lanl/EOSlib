if(${verbose_traversal})
  message(STATUS "Entered HayesBM subdirectory")
endif(${verbose_traversal})

set(EOSLIB_HAYESBM_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/HayesBM)
set(HayesBM_src)

add_subdirectory(SubsVe)
add_subdirectory(SubsVT)

add_library( HayesBM ${${projectName}_policy} ${HayesBM_src} )
set_property(TARGET HayesBM PROPERTY CXX_STANDARD 11)
set_property(TARGET HayesBM PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET HayesBM PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( HayesBM Eos DataBase ODE)

foreach(test Test1)
  add_executable(${test}HayesBM ${test}.C)
  target_link_libraries(${test}HayesBM HayesBM)
  add_test(NAME ${test}HayesBM
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}HayesBM
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_HAYESBM_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_HAYESBM_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}HayesBM)
  set_tests_properties(${test}HayesBM PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

#### use this if test(s) require file specification####
#
foreach(test Test2 Test2VT)
  add_executable(${test}HayesBM ${test}.C)
  target_link_libraries(${test}HayesBM HayesBM)
  set(args "file ${EOSLIB_DATA_PATH}/test_data/HayesBMTest.data")
  add_test(NAME ${test}HayesBM
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}HayesBM
    -D test_args:string=${args}
    -D out_blessed=${EOSLIB_HAYESBM_BLESSED_OUTPUT}/${test}Out.blessed
    -D err_blessed=${EOSLIB_HAYESBM_BLESSED_OUTPUT}/${test}Err.blessed 
    -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
    )
  message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}HayesBM)
  set_tests_properties(${test}HayesBM PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
endforeach(test)

install(TARGETS HayesBM
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)


list(APPEND INSTALL_TARGETS HayesBM)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting HayesBM subdirectory")
endif(${verbose_traversal})

