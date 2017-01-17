if(${verbose_traversal})
  message(STATUS "Entered src subdirectory")
endif(${verbose_traversal})

<<<<<<< HEAD
=======

>>>>>>> c30ac671e07def170a16c86a2343d793370d798c
add_subdirectory(Local)
add_subdirectory(LinkList)
add_subdirectory(OneDFunction)
add_subdirectory(ODE)
add_subdirectory(Spline)
add_subdirectory(Calc)
<<<<<<< HEAD
=======

#add_library( CClib ${${projectName}_policy}
#                   $<TARGET_OBJECTS:Io-obj> 
#                   $<TARGET_OBJECTS:Math-obj> 
#                   $<TARGET_OBJECTS:String-obj> 
#                   $<TARGET_OBJECTS:LinkList-obj> 
#                   $<TARGET_OBJECTS:OneDfunction-obj> 
#                   $<TARGET_OBJECTS:Spline-obj> 
#                   $<TARGET_OBJECTS:ODE-obj> 
#                   $<TARGET_OBJECTS:Calc-obj> )
#set_property(TARGET CClib PROPERTY CXX_STANDARD 11)
#set_property(TARGET CClib PROPERTY CXX_STANDARD_REQUIRED ON)
#set_property(TARGET CClib PROPERTY INTERPROCEDURAL_OPTIMIZATION ON)

add_library( CClib ${${projectName}_policy} "")
set_property(TARGET CClib PROPERTY CXX_STANDARD 11)
set_property(TARGET CClib PROPERTY CXX_STANDARD_REQUIRED ON)
set_property(TARGET CClib PROPERTY LINKER_LANGUAGE CXX)
target_link_libraries( CClib Io Math String Calc Spline
                             ODE OneDfunction LinkList  )


add_subdirectory(DataBase)


>>>>>>> c30ac671e07def170a16c86a2343d793370d798c

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

add_subdirectory(Eos)
add_subdirectory(EOStypes)

include_directories(ExtEOStypes/ArrheniusHE)
include_directories(ExtEOStypes/Elastic1D)
include_directories(ExtEOStypes/ElasticPlastic)
include_directories(ExtEOStypes/HEburn)
add_subdirectory(ExtEOS)
add_subdirectory(ExtEOStypes)

add_subdirectory(Manager)

add_subdirectory(Applications)

<<<<<<< HEAD
add_subdirectory(DnKappa)


set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )
=======
include_directories(DnKappa)
add_subdirectory(DnKappa)


set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )

>>>>>>> c30ac671e07def170a16c86a2343d793370d798c

if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
