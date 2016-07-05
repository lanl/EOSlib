if(${verbose_traversal})
  message(STATUS "Entered src subdirectory")
endif(${verbose_traversal})

include_directories(Local)
add_subdirectory(Local)

include_directories(LinkList)
add_subdirectory(LinkList)

include_directories(Calc)
add_subdirectory(Calc)

include_directories(DataBase)
add_subdirectory(DataBase)

include_directories(OneDFunction)
add_subdirectory(OneDFunction)

include_directories(ODE)
add_subdirectory(ODE)

include_directories(Spline)
add_subdirectory(Spline)

include_directories(Eos)
add_subdirectory(Eos)

include_directories(EOStypes)
include_directories(EOStypes/IdealGas)
include_directories(EOStypes/StiffenedGas)
include_directories(EOStypes/Hayes)
include_directories(EOStypes/HayesBM)
#include_directories(EOStypes/Sesame)
include_directories(EOStypes/JWL)
add_subdirectory(EOStypes)

if(${verbose_traversal})
  message(STATUS "Exiting src subdirectory")
endif(${verbose_traversal})
