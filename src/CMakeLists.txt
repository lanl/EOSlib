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



include_directories(Eos)
add_subdirectory(Eos)

include_directories(EOStypes)
include_directories(EOStypes/Davis)
include_directories(EOStypes/IdealGas)
include_directories(EOStypes/GenHayes)
include_directories(EOStypes/Hayes)
include_directories(EOStypes/HayesBM)
#include_directories(EOStypes/Sesame)
include_directories(EOStypes/JWL)
include_directories(EOStypes/MieGruneisen)
include_directories(EOStypes/Porous)
include_directories(EOStypes/PTequilibrium)
include_directories(EOStypes/StiffenedGas)
#include_directories(Uniaxial)
add_subdirectory(EOStypes)

include_directories(Manager)
add_subdirectory(Manager)

include_directories(ExtEOS)
add_subdirectory(ExtEOS)

include_directories(ExtEOStypes)
include_directories(ExtEOStypes/ArrheniusHE)
include_directories(ExtEOStypes/Elastic1D)
include_directories(ExtEOStypes/ElasticPlastic)
include_directories(ExtEOStypes/HEburn)
add_subdirectory(ExtEOStypes)

add_subdirectory(Applications)

include_directories(DnKappa)
add_subdirectory(DnKappa)


set( HEADER_FILES ${HEADER_FILES} PARENT_SCOPE )


if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
