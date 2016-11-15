if(${verbose_traversal})
  message(STATUS "Entered StiffenedGas subdirectory")
endif(${verbose_traversal})

set(EOSLIB_STIFFENEDGAS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/StiffenedGas)
set(StiffenedGas_src)

add_subdirectory(SubsVT)
add_subdirectory(SubsVe)

add_library( StiffenedGas ${${projectName}_policy} ${StiffenedGas_src} )
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD 11)
set_property(TARGET StiffenedGas PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET StiffenedGas PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( StiffenedGas Eos DataBase ODE)

# list( APPEND tests Test1 Test2 Test2VT )
# 
# #add_executable(Test1StiffenedGas Test1.C)
# #target_link_libraries(Test1StiffenedGas StiffenedGas)
# #add_test(Test1StiffenedGas Test1StiffenedGas)
# 
# foreach(test Test1)
#   add_executable(${test}StiffenedGas ${test}.C)
#   target_link_libraries(${test}StiffenedGas StiffenedGas)
#   add_test(NAME ${test}StiffenedGas #COMMAND ${test}StiffenedGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}StiffenedGas
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_STIFFENEDGAS_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_STIFFENEDGAS_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   set_tests_properties(${test}StiffenedGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)
# 
# foreach(test Test2 Test2VT)
#   add_executable(${test}StiffenedGas ${test}.C)
#   target_link_libraries(${test}StiffenedGas StiffenedGas)
#   set (args "file ${EOSLIB_DATA_PATH}/test_data/StiffenedGasTest.data")
#   add_test(NAME ${test}StiffenedGas #COMMAND ${test}StiffenedGas
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}StiffenedGas
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_STIFFENEDGAS_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_STIFFENEDGAS_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   set_tests_properties(${test}StiffenedGas PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

install(TARGETS StiffenedGas
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS StiffenedGas)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting StiffenedGas subdirectory")
endif(${verbose_traversal})
