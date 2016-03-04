if(${verbose_traversal})
  message(STATUS "Entered Spline subdirectory")
endif(${verbose_traversal})

set(Spline_src)

add_subdirectory(Subs)

add_library( Spline ${${projectName}_policy} ${Spline_src} )
set_property(TARGET Spline PROPERTY CXX_STANDARD 11)
set_property(TARGET Spline PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET Spline PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

target_link_libraries( Spline Io Math )

list( APPEND tests TestSpline )

foreach(test ${tests})
  add_executable(${test} ${test}.C)
  target_link_libraries(${test} Spline)
endforeach(test)

if(${verbose_traversal})
  message(STATUS "Exiting Spline subdirectory")
endif(${verbose_traversal})
