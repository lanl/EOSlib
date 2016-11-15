if(${verbose_traversal})
  message(STATUS "Entered Porous subdirectory")
endif(${verbose_traversal})

set(EOSLIB_POROUS_BLESSED_OUTPUT ${EOSLIB_BLESSED_OUTPUT}/EOStypes/Porous)
set(Porous_src)

add_subdirectory(SubsEOS)
add_subdirectory(SubsPhi)

add_library( Porous ${${projectName}_policy} ${Porous_src} )
set_property(TARGET Porous PROPERTY CXX_STANDARD 11)
set_property(TARGET Porous PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Porous PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Porous Eos DataBase ODE Hayes ${CMAKE_DL_LIBS} )

# list( APPEND  tests TestFrozenPorous TestPhi TestPorousEOS )
# 
# foreach(test ${tests})
#   add_executable(${test}Porous ${test}.C)
#   target_link_libraries(${test}Porous Porous)
#   add_test(NAME ${test}Porous
#     COMMAND ${CMAKE_COMMAND}
#     -D test_cmd=${CMAKE_CURRENT_BINARY_DIR}/${test}Porous
#     -D test_args:string=${args}
#     -D out_blessed=${EOSLIB_POROUS_BLESSED_OUTPUT}/${test}Out.blessed
#     -D err_blessed=${EOSLIB_POROUS_BLESSED_OUTPUT}/${test}Err.blessed 
#     -P ${CMAKE_SOURCE_DIR}/compare_outputs.cmake
#     )
#   message(STATUS ${CMAKE_CURRENT_BINARY_DIR}/${test}Porous)
#   set_tests_properties(${test}Porous PROPERTIES ENVIRONMENT "${CTEST_ENVIRONMENT}")
# endforeach(test)

install(TARGETS Porous
  RUNTIME DESTINATION ./bin
  LIBRARY DESTINATION ./lib
  ARCHIVE DESTINATION ./lib/static)

list(APPEND INSTALL_TARGETS Porous)
set(INSTALL_TARGETS ${INSTALL_TARGETS} PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting Porous subdirectory")
endif(${verbose_traversal})

