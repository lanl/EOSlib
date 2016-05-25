if(${verbose_traversal})
  message(STATUS "Entered DataBase subdirectory")
endif(${verbose_traversal})

set(DataBase_src)

add_subdirectory(Subs)

add_library( DataBase ${${projectName}_policy} ${DataBase_src} )
set_property(TARGET DataBase PROPERTY CXX_STANDARD 11)
set_property(TARGET DataBase PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DataBase PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( shared_obj ${${projectName}_policy} shared_obj.C)
target_link_libraries( shared_obj Math )

target_link_libraries( DataBase Calc)
target_link_libraries( DataBase ${CMAKE_DL_LIBS})

list( APPEND tests Data Convert TestDB Testdl )

file(COPY Test.data Units.data DESTINATION ${CMAKE_CURRENT_BINARY_DIR})

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} DataBase)
#  add_test(${test} ${test})
endforeach(test)

add_test(NAME TestDB COMMAND TestDB file ${CMAKE_CURRENT_SOURCE_DIR}/Units.data type hydro name std)

#add_test(NAME Data0 COMMAND Data file ${CMAKE_CURRENT_SOURCE_DIR}/Units.data base Units type hydro name std use)



target_link_libraries(Testdl shared_obj)

if(${verbose_traversal})
  message(STATUS "Exiting DataBase subdirectory")
endif(${verbose_traversal})
