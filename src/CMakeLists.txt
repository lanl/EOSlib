if(${verbose_traversal})
  message(STATUS "Entered src subdirectory")
endif(${verbose_traversal})

add_subdirectory(Local)
add_subdirectory(LinkList)
add_subdirectory(OneDFunction)
add_subdirectory(ODE)
add_subdirectory(Spline)
add_subdirectory(Calc)

add_library( CClib ${${projectName}_policy}
                   $<TARGET_OBJECTS:Io-obj> 
                   $<TARGET_OBJECTS:Math-obj> 
                   $<TARGET_OBJECTS:String-obj> 
                   $<TARGET_OBJECTS:LinkList-obj> 
                   $<TARGET_OBJECTS:OneDfunction-obj> 
                   $<TARGET_OBJECTS:Spline-obj> 
                   $<TARGET_OBJECTS:ODE-obj> 
                   $<TARGET_OBJECTS:Calc-obj> )
set_property(TARGET CClib PROPERTY CXX_STANDARD 11)
set_property(TARGET CClib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET CClib PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)


add_subdirectory(DataBase)

add_subdirectory(EOS)
add_subdirectory(EOStypes)

add_subdirectory(ExtEOS)
add_subdirectory(ExtEOStypes)

add_subdirectory(Applications)

add_subdirectory(Manager)

add_subdirectory(DnKappa)


set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
