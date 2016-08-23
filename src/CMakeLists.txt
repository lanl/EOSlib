if(${verbose_traversal})
  message(STATUS "Entered src subdirectory")
endif(${verbose_traversal})

include_directories(Applications)
add_subdirectory(Applications)

include_directories(Local)
include_directories(Local/Io)
include_directories(Local/String)
include_directories(Local/Math)
add_subdirectory(Local)

include_directories(LinkList)
add_subdirectory(LinkList)

include_directories(Calc)
add_subdirectory(Calc)

include_directories(DataBase)
add_subdirectory(DataBase)

include_directories(DnKappa)
add_subdirectory(DnKappa)

include_directories(OneDFunction)
add_subdirectory(OneDFunction)

include_directories(ODE)
add_subdirectory(ODE)

include_directories(Spline)
add_subdirectory(Spline)

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

set(INSTALL_TARGETS PARENT_SCOPE)

if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
