if(${verbose_traversal})
  message(STATUS "Entered CClib subdirectory")
endif(${verbose_traversal})

#
add_subdirectory(Local)
add_subdirectory(LinkList)
add_subdirectory(OneDFunction)
add_subdirectory(ODE)
add_subdirectory(Spline)
add_subdirectory(Calc)

list( APPEND Local_subs Local/Io Local/Math Local/String )
foreach(sub ${Local_subs})
  file(GLOB src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${sub}/*.C")
  list( APPEND CClib_src ${src} )
endforeach(sub)

list( APPEND CClib_subs LinkList
                        OneDFunction
                        ODE
                        Spline
                        Calc )
foreach(sub ${CClib_subs})
  file(GLOB src  RELATIVE "${CMAKE_CURRENT_SOURCE_DIR}" "${sub}/Subs/*.C")
  list( APPEND CClib_src ${src} )
endforeach(sub)

add_library( CClib-obj OBJECT ${CClib_src} )
  set_property(TARGET CClib-obj PROPERTY POSITION_INDEPENDENT_CODE ON )
  set_property(TARGET CClib-obj PROPERTY CXX_STANDARD 11)
  set_property(TARGET CClib-obj PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET CClib-obj PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( CClib ${${projectName}_policy}
                   $<TARGET_OBJECTS:CClib-obj> )
  set_property(TARGET CClib PROPERTY CXX_STANDARD 11)
  set_property(TARGET CClib PROPERTY CXX_STANDARD_REQUIRED ON)
  set_property(TARGET CClib PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

if(${verbose_traversal})
  message(STATUS "Exiting CClib subdirectory")
endif(${verbose_traversal})
