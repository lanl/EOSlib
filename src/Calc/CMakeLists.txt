if(${verbose_traversal})
  message(STATUS "Entered Calc subdirectory")
endif(${verbose_traversal})

set(Calc_src)

add_subdirectory(Subs)

add_library( Calc ${${projectName}_policy} ${Calc_src} )
set_property(TARGET Calc PROPERTY CXX_STANDARD 11)
set_property(TARGET Calc PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Calc PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Calc LinkList Io Math String)

list( APPEND tests TestCalc)

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Calc)
  add_test(${test} ${test})
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Calc subdirectory")
endif(${verbose_traversal})
