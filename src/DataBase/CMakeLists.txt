if(${verbose_traversal})
  message(STATUS "Entered DataBase subdirectory")
endif(${verbose_traversal})

set(DataBase_src)

add_subdirectory(Subs)

add_library( DataBase ${${projectName}_policy} ${DataBase_src} )
set_property(TARGET DataBase PROPERTY CXX_STANDARD 11)
set_property(TARGET DataBase PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET DataBase PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( DataBase Calc)
target_link_libraries( DataBase ${CMAKE_DL_LIBS})

list( APPEND tests Data Convert TestDB Testdl)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} DataBase)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting DataBase subdirectory")
endif(${verbose_traversal})