PROJECT(EOSlib)

cmake_minimum_required( VERSION 3.0 )

set(projectName "EOSlib")
set(projectVersion "2.0.0")

# Enable unit testing with the Boost framework.
find_package(Boost COMPONENTS unit_test_framework REQUIRED)
#find_package(Boost REQUIRED)
include_directories(${Boost_INCLUDE_DIRS})
#add_definitions(-DBOOST_ALL_NO_LIB)
# verbose traversal is disabled by default. in general, a trace of the
# source tree traversal just adds noise to the configuration output and
# may obscure other more important messages indiciating errors or warnings
# however, a trace of the traversal can be very helpful when the
# configuration itself is broken.
#
# this option can be enable when invoking cmake by passing
# '-D verbose_traversal=TRUE'
if(NOT DEFINED verbose_traversal)
  set(verbose_traversal FALSE)
endif(NOT DEFINED verbose_traversal)

# by default, projects should be built for debugging. this is useful
# in a development environment where you might swearing at your terminal
# from time-to-time.
#
# the build type default can be overwritten when invoking cmake
# by passing a definition for the build type.
# '-D build_type={option}'
# where option is
# + {debug}    - using this option will provide debug symbols
# + {coverage} - as debug, but also provide flags for use with profilers
# + {release}  - using this option will provide optimizations suitable
#                for the platform architecture
# + {native}   - as release, but also provide optimizations specific to
#                the processor
#
# in addition, the default build type (user-specified or otherwise) may
# be overriden for a specific projects by passing a definition as part
# of the cmake invocation
# '-D {projectName}_build_type={option}
if(NOT DEFINED build_type)
  set( build_type "debug")
endif(NOT DEFINED build_type)

if(NOT DEFINED ${projectName}_build_type)
  set( ${projectName}_build_type "${build_type}" )
endif(NOT DEFINED ${projectName}_build_type)

set(local_testing TRUE)
set(build_tree "${CMAKE_BINARY_DIR}")

# by default, libraries are generated as dynamic libraries which are
# linked at run time. This reduces the linking step overhead during
# recompilation following a small change to the source code, which is
# helpful during TDD iteration.
#
# The default libary type may be specified to be statically linked
# during cmake invocation by passing
# '-D static_libraries=TRUE'
#
# in addition, the default library type (user-specified or otherwise) may
# be overriden for a specific projects by passing a definition as part
# of the cmake invocation
# '-D {projectName}_static_libraries=TRUE/FALSE
if(NOT DEFINED static_libraries)
  set(static_libraries FALSE)
endif(NOT DEFINED static_libraries)

if(NOT DEFINED ${projectName}_static_libraries)
  set(${projectName}_static_libraries ${static_libraries})
endif(NOT DEFINED ${projectName}_static_libraries)

if(${${projectName}_static_libraries})
  set(${projectName}_policy STATIC)
else(${${projectName}_static_libraries})
  set(${projectName}_policy SHARED)
endif(${${projectName}_static_libraries})

# normally, it's best to allow the configuration script setup 
# compilation flags derived from other user-specified defintions.
#
# That said, means to augment or over-write the compiler flags are
# provided, either in general or for a specific project 
#
if(DEFINED compiler_flags)
  if (NOT DEFINED ${projectName}_compiler_flags)
    set(${projectName}_compiler_flags "${compiler_flags}")
  endif (NOT DEFINED ${projectName}_compiler_flags)
endif(DEFINED compiler_flags)

if (NOT DEFINED ${projectName}_appended_flags)
  set(${projectName}_appended_flags "${appended_flags}" )
endif (NOT DEFINED ${projectName}_appended_flags)

# if this is the first project directory traversed and the build
# tree is clean, it will be necessary to create the header directory
# shared amongst components 
if(NOT(EXISTS "${build_tree}/include"))
  file(MAKE_DIRECTORY "${build_tree}/include")
endif(NOT(EXISTS "${build_tree}/include"))

set(HEADER_DIR "${build_tree}/include")

# --------------------
# Begin project scopes
# --------------------
if(NOT DEFINED ${projectName}_header_source_directory)
  set(${projectName}_header_source_directory ${CMAKE_CURRENT_SOURCE_DIR})
endif(NOT DEFINED ${projectName}_header_source_directory)

if(NOT DEFINED ${projectName}_src_header)
  set(${projectName}_src_header ${projectName}.hpp.in)
endif(NOT DEFINED ${projectName}_src_header)

if(NOT DEFINED ${projectName}_bin_header)
  set(${projectName}_bin_header ${projectName}.hpp)
endif(NOT DEFINED ${projectName}_bin_header)

project (${projectName} VERSION ${projectVersion} LANGUAGES CXX)
enable_testing()

# message to indicate the order of traversal,
# which is often helpful for debugging
if(${verbose_traversal})
  message(STATUS "Entered ${projectName} directory")
endif(${verbose_traversal})

# Set up paths for the flattened header tree
if(NOT(EXISTS "${HEADER_DIR}/${projectName}"))
  file(MAKE_DIRECTORY "${HEADER_DIR}/${projectName}")

endif(NOT(EXISTS "${HEADER_DIR}/${projectName}"))

set(${projectName}_HEADER_DIR "${HEADER_DIR}/${projectName}")

# Initialize a variable for collecting utility implementation components
list(APPEND ${projectName}_src) # (empty list)
list(APPEND ${projectName}_src_headers) # (empty list)
list(APPEND ${projectName}_bin_headers) # (empty list)

# setup test labels
list(APPEND testLabels ${projectName})

# Gather and set revision information
execute_process(
  COMMAND git rev-parse --abbrev-ref HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE ${projectName}_GIT_BRANCH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )
execute_process(
  COMMAND git rev-parse HEAD
  WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}
  OUTPUT_VARIABLE ${projectName}_GIT_HASH
  OUTPUT_STRIP_TRAILING_WHITESPACE
  )

# setup library type
if(NOT ${projectName}_static_libraries)
  set(BUILD_SHARED_LIBS TRUE)
else(NOT ${projectName}_static_libraries)
  set(BUILD_SHARED_LIBS FALSE)
endif(NOT ${projectName}_static_libraries)

# setup compiler flags
if(DEFINED ${projectName}_compiler_flags)
  set( CMAKE_CXX_FLAGS
    "${${projectName}_compiler_flags} ${${projectName}_appended_flags}")
  set(${projectName}_flags "${CMAKE_CXX_FLAGS}")
  
else(DEFINED ${projectName}_compiler_flags)
  if( ${${projectName}_build_type} STREQUAL "debug" )
    set( CMAKE_BUILD_TYPE DEBUG )
  elseif( ${${projectName}_build_type} STREQUAL "coverage" )
    set( CMAKE_BUILD_TYPE DEBUG )
  elseif( ${${projectName}_build_type} STREQUAL "release" )
    set( CMAKE_BUILD_TYPE RELEASE )
  elseif( ${${projectName}_build_type} STREQUAL "native" )
    set( CMAKE_BUILD_TYPE RELEASE )
  endif( ${${projectName}_build_type} STREQUAL "debug" )

  # base flags used by every project
  set( ${projectName}_flags
  #  "-std=c++11 -Wall -Wextra ${CMAKE_CXX_FLAGS} " )
    "-std=c++11 -w ${CMAKE_CXX_FLAGS} " )
  #  "-w ${CMAKE_CXX_FLAGS} ")
  # debug variants
  if (${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
    set( ${projectName}_flags
      "-gdwarf-3 ${CMAKE_CXX_FLAGS_DEBUG} ${${projectName}_flags}" )
    if( ${${projectName}_build_type} STREQUAL "coverage" )
      set( ${projectName}_flags
        "-fprofile-arcs -ftest-coverage ${${projectName}_flags}" )
    endif( ${${projectName}_build_type} STREQUAL "coverage" )
  endif (${CMAKE_BUILD_TYPE} STREQUAL DEBUG)
  
  # optimized variants
  if (${CMAKE_BUILD_TYPE} STREQUAL RELEASE)
    set( ${projectName}_flags
      "-flto ${CMAKE_CXX_FLAGS_RELEASE} ${${projectName}_flags}")
    if( ${${projectName}_build_type} STREQUAL "native" )
      set( ${projectName}_flags
        "-march=native ${${projectName}_flags}")
    endif( ${${projectName}_build_type} STREQUAL "native" )
  endif (${CMAKE_BUILD_TYPE} STREQUAL RELEASE)
  
  set(${projectName}_flags
    "${${projectName}_flags} ${${projectName}_appended_flags}")
  
  set( CMAKE_CXX_FLAGS_${CMAKE_BUILD_TYPE}
    ${${projectName}_flags})
  
endif(DEFINED ${projectName}_compiler_flags)

# A short function for CTest setup used in subdirectories
function(setupTest TEST_NAME)
  if(${verbose_traversal})
    message(STATUS "Entered test subdirectory")
  endif(${verbose_traversal})
  set(EXE_NAME ${TEST_NAME}.test)
  file(GLOB SRC "${CMAKE_CURRENT_SOURCE_DIR}/*.C")
  add_executable(${EXE_NAME} ${SRC} )
  target_link_libraries( ${EXE_NAME} ${Boost_LIBRARIES} ${TEST_NAME})
  add_test( NAME ${TEST_NAME} COMMAND ${EXE_NAME} "--log_level=message")
  set_property( TEST ${TEST_NAME} PROPERTY PROCESSORS 1 )
  set_property( TEST ${TEST_NAME} APPEND PROPERTY LABELS ${testLabels} )
  if(${verbose_traversal})
    message(STATUS "Exiting test subdirectory")
  endif(${verbose_traversal})
endfunction(setupTest)

# setup include directory
include_directories( "${HEADER_DIR}" )
include_directories( "${CMAKE_CURRENT_BINARY_DIR}" )

message(STATUS "")
message(STATUS "-----------------------------------------------------------")
message(STATUS "")
message(STATUS "${projectName} Version: ${${projectName}_VERSION}")
message(STATUS "Git current branch: ${${projectName}_GIT_BRANCH}")
message(STATUS "Git commit hash: ${${projectName}_GIT_HASH}")
message(STATUS "")
message(STATUS "${projectName} path: ${PROJECT_SOURCE_DIR}")
message(STATUS "")
message(STATUS "${projectName} flags: ${${projectName}_flags}")
message(STATUS "")
message(STATUS "-----------------------------------------------------------")
message(STATUS "")

if(NOT DEFINED EOSLIB_DATA_TEST_PATH)
  set(EOSLIB_DATA_TEST_PATH "${CMAKE_CURRENT_BINARY_DIR}/DATA")
endif(NOT DEFINED EOSLIB_DATA_TEST_PATH)

if(NOT DEFINED EOSLIB_DATA_PATH)
  set(EOSLIB_DATA_PATH "${EOSLIB_DATA_TEST_PATH}")
endif(NOT DEFINED EOSLIB_DATA_PATH)

set(CTEST_ENVIRONMENT
  "EOSLIB_DATA_PATH=${EOSLIB_DATA_TEST_PATH}"
  "EOSLIB_BLESSED_OUTPUT_PATH=${CMAKE_BINARY_DIR}/test/blessed_output"
  "CTEST_OUTPUT_ON_FAILURE=1"
  "BOOST_TEST_LOG_LEVEL='message'"
  "EOSLIB_SHARED_LIBRARY_PATH=${CMAKE_BINARY_DIR}/lib")
set(LIBRARY_OUTPUT_PATH ${CMAKE_BINARY_DIR}/lib)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/bin)

#add_subdirectory(DATA)
#add_subdirectory(lib)
#add_subdirectory(bin)
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/DATA
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/lib
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
file(COPY ${CMAKE_CURRENT_SOURCE_DIR}/bin
          DESTINATION ${CMAKE_CURRENT_BINARY_DIR})
add_subdirectory(TEST)
add_subdirectory(src)
add_subdirectory(doc)

file(COPY SetEnv DESTINATION ${CMAKE_BINARY_DIR})

# Testing

find_program(BASH_PROGRAM bash)
set(runEOS ${CMAKE_BINARY_DIR}/bin/runEOS)
set(listEOS ${CMAKE_BINARY_DIR}/bin/listEOS)

function(TEST_BASH test_name test_args)
  add_test(NAME ${test_name} 
    COMMAND ${CMAKE_COMMAND}
    -D test_cmd=${BASH_PROGRAM}
    -D test_args:string=${test_args}
    -D out_blessed=${EOSLIB_BLESSED_OUTPUT}/${test_name}Out.blessed
    -D err_blessed=${EOSLIB_BLESSED_OUTPUT}/${test_name}Err.blessed
    -P ${CMAKE_BINARY_DIR}/TEST/compare_outputs.cmake
    )
endfunction(TEST_BASH)

if(BASH_PROGRAM)
  TEST_BASH(runEOShelp       "${runEOS} help")
  TEST_BASH(runEOSWavehelp   "${runEOS} Wave help")
  TEST_BASH(runEOSWave       "${runEOS} Wave       material HEburn::PBX9502.davis Ps 10")
  TEST_BASH(runEOSDetonation "${runEOS} Detonation material HEburn::PBX9502.davis P2 10")
  TEST_BASH(runEOSIsotherm   "${runEOS} Isotherm   material HEburn::PBX9502.davis Ps 10 P2 1")
  TEST_BASH(runEOSEOSdata    "${runEOS} EOSdata    material HEburn::PBX9502.davis all")
  TEST_BASH(runEOSData       "${runEOS} Data base EOS type HEburn name PBX9502.davis parameters")
  TEST_BASH(runEOSImpedanceMatch "${runEOS} ImpedanceMatch mat1 Hayes::Kel-F u1 2.7 mat2 HEburn::PBX9502.davis")
endif(BASH_PROGRAM)

# Install stuff

list(APPEND HEADER_FILES
  src/Calc/Token.h
  src/Calc/Calc.h
  src/Calc/Scanner.h
  src/Calc/CalcArrays.h
  src/Calc/CalcVars.h
  src/Calc/CalcArrayTypes.h
  src/Calc/CalcVarTypes.h
  src/DataBase/DataBaseError.h
  src/DataBase/DataFile.h
  src/DataBase/Handle.h
  src/DataBase/DataBaseIterators.h
  src/DataBase/Units.h
  src/DataBase/DataBase.h
  src/DataBase/DataBaseParameters.h
  src/Table/Table.h
  src/Table/MatToSesame.h
  src/Eos/Wave.h
  src/Eos/Riemann_gen.h
  src/Eos/ShockPolar_gen.h
  src/Eos/Isotherm_ODE.h
  src/Eos/Riemann.h
  src/Eos/Deflagration_gen.h
  src/Eos/ShockPolar.h
  src/Eos/Detonation_gen.h
  src/Eos/PrandtlMeyer_ODE.h
  src/Eos/Isentrope_ODE.h
  src/Eos/EOS.h
  src/Eos/EOSbase.h
  src/Eos/States.h
  src/Eos/IO.h
  src/Eos/Hugoniot_gen.h
  src/Eos/EOS_VT.h
  src/ODE/ODE.h
  src/LinkList/List.h
  src/LinkList/Array.h
  src/LinkList/ListAccess.h
  src/LinkList/NameArray.h
  src/LinkList/ReferenceList.h
  src/Local/String.h
  src/Local/LocalIo.h
  src/Local/LocalMath.h
  src/Local/ErrorHandler.h
  src/Local/Arg.h
  src/Manager/EOSmanager.h
  src/Manager/EOSstate.h
  src/Manager/PointerArray.h
  src/Manager/SplitWord.h
  src/ExtEOS/ExtEOSstate.h
  src/ExtEOS/ExtEOSmanager.h
  src/ExtEOS/Riemann_genX.h
  src/ExtEOS/Hydro.h
  src/ExtEOS/ExtEOS_VT.h
  src/ExtEOS/EqExtEOS.h
  src/ExtEOS/ExtEOS.h
  src/ExtEOS/IDOF.h
  src/DnKappa/ScaledDn.h
  src/DnKappa/States.h
  src/DnKappa/Wave.h
  src/DnKappa/DnKappa.h
  src/DnKappa/IO.h
  src/DnKappa/EOSbase.h
  src/DnKappa/EOS.h
  src/EOStypes/JWL/BKW_VT.h
  src/EOStypes/JWL/HEprods.h
  src/EOStypes/JWL/HEfit.h
  src/EOStypes/JWL/BKWwave.h
  src/EOStypes/JWL/BKW.h
  src/EOStypes/JWL/JWL_VT.h
  src/EOStypes/JWL/JWL.h
  src/EOStypes/Hayes/HayesParams.h
  src/EOStypes/Hayes/Hayes_VT.h
  src/EOStypes/Hayes/Hayes.h
  src/EOStypes/PTequilibrium/StiffIdeal.h
  src/EOStypes/PTequilibrium/PTequilibrium.h
  src/EOStypes/PTequilibrium/PTequilibrium_VT.h
  src/EOStypes/PTequilibrium/StiffIdeal_VT.h
  src/EOStypes/StiffenedGas/StiffGasParams.h
  src/EOStypes/StiffenedGas/StiffenedGas_VT.h
  src/EOStypes/StiffenedGas/StiffenedGas.h
  src/EOStypes/HayesBM/HayesBMparams.h
  src/EOStypes/HayesBM/HayesBM.h
  src/EOStypes/HayesBM/HayesBM_VT.h
  src/EOStypes/GenHayes/Keane_VT.h
  src/EOStypes/GenHayes/Keane.h
  src/EOStypes/GenHayes/KeaneParams.h
  src/EOStypes/GenHayes/BirchMurnaghan_VT.h
  src/EOStypes/GenHayes/GenHayes.h
  src/EOStypes/GenHayes/IntSCv_ODE.h
  src/EOStypes/GenHayes/BirchMurnaghanParams.h
  src/EOStypes/GenHayes/GenHayesFunc.h
  src/EOStypes/GenHayes/GenHayes_VT.h
  src/EOStypes/GenHayes/BirchMurnaghan.h
  src/EOStypes/IdealGas/IdealGas_Deflagration.h
  src/EOStypes/IdealGas/IdealGas_Riemann.h
  src/EOStypes/IdealGas/IdealGas_Isentrope.h
  src/EOStypes/IdealGas/IdealGas_VT.h
  src/EOStypes/IdealGas/IdealGas_Detonation.h
  src/EOStypes/IdealGas/IdealGas_Isotherm.h
  src/EOStypes/IdealGas/IdealGas_Hugoniot.h
  src/EOStypes/IdealGas/IdealGas.h
  src/EOStypes/Davis/DavisReactants.h
  src/EOStypes/Davis/DavisProducts.h
  src/EOStypes/Porous/Porous.h
  src/EOStypes/Porous/ECfit.h
  src/EOStypes/Porous/PhiTypes.h
  src/EOStypes/Porous/PhiEq.h
  src/EOStypes/Porous/PhiEqSpline.h
  src/EOStypes/Sesame/Sesame.h
  src/EOStypes/Sesame/Sesame_VT.h
  src/EOStypes/Sesame/SesSubs.h
  src/EOStypes/MieGruneisen/PeSplineParams.h
  src/EOStypes/MieGruneisen/UsUpSpline_VT.h
  src/EOStypes/MieGruneisen/UsUp.h
  src/EOStypes/MieGruneisen/PeSpline.h
  src/EOStypes/MieGruneisen/UsUpParams.h
  src/EOStypes/MieGruneisen/UsUpSplineParams.h
  src/EOStypes/MieGruneisen/UsUp_VT.h
  src/EOStypes/MieGruneisen/PeSpline_VT.h
  src/EOStypes/MieGruneisen/UsUpSpline.h
  src/OneDFunction/OneDFunction.h
  src/ExtEOStypes/HEburn/HEQ.h
  src/ExtEOStypes/HEburn/HEburn2.h
  src/ExtEOStypes/HEburn/ArrheniusRT.h
  src/ExtEOStypes/HEburn/HErate.h
  src/ExtEOStypes/HEburn/HEburn.h
  src/ExtEOStypes/HEburn/IgnitionGrowthRT.h
  src/ExtEOStypes/HEburn/ReactiveHugoniot.h
  src/ExtEOStypes/HEburn/HE1.h
  src/ExtEOStypes/HEburn/HErate2.h
  src/ExtEOStypes/HEburn/HotSpotRateCC.h
  src/ExtEOStypes/HEburn/HotSpotHE_CC.h
  src/ExtEOStypes/HEburn/HotSpotCC_Rate.h
  src/ExtEOStypes/HEburn/HotSpotRate2.h
  src/ExtEOStypes/HEburn/ViscousProfile.h
  src/ExtEOStypes/HEburn/FFQrate.h
  src/ExtEOStypes/HEburn/HotSpotRateExp.h
  src/ExtEOStypes/HEburn/FFrate.h
  src/ExtEOStypes/HEburn/HotSpotHE2.h
  src/ExtEOStypes/HEburn/IgnitionGrowth.h
  src/ExtEOStypes/HEburn/Arrhenius.h
  src/ExtEOStypes/HEburn/HotSpotRate.h
  src/ExtEOStypes/HEburn/PnRate.h
  src/ExtEOStypes/HEburn/HotSpotHE.h
  src/ExtEOStypes/HEburn/HotSpotCC_HE.h
  src/ExtEOStypes/ElasticPlastic/ElasticPlastic.h
  src/ExtEOStypes/ElasticPlastic/ElasticTypes.h
  src/ExtEOStypes/ElasticPlastic/Elastic.h
  src/ExtEOStypes/ElasticPlastic/ElasticPlasticTypes.h
  src/ExtEOStypes/Elastic1D/ElasticPolyG_VT.h
  src/ExtEOStypes/Elastic1D/ElasticPlastic1D.h
  src/ExtEOStypes/Elastic1D/ElasticPolyG.h
  src/ExtEOStypes/Elastic1D/Polynomial.h
  src/ExtEOStypes/Elastic1D/PlasticIDOF.h
  src/ExtEOStypes/Elastic1D/PlasticIDOF2.h
  src/ExtEOStypes/Elastic1D/PolyG.h
  src/ExtEOStypes/Elastic1D/VonMises1D.h
  src/ExtEOStypes/Elastic1D/VMrate.h
  src/ExtEOStypes/Elastic1D/VonMisesConstGV.h
  src/ExtEOStypes/Elastic1D/Elastic1D.h
  src/ExtEOStypes/Elastic1D/ElasticConstGV.h
  src/ExtEOStypes/Elastic1D/Polymer.h
  src/ExtEOStypes/Elastic1D/ConstGV.h
  src/ExtEOStypes/Elastic1D/Shear1D.h
  src/ExtEOStypes/Elastic1D/ElasticConstGV_VT.h
  src/ExtEOStypes/Elastic1D/Elastic1D_VT.h
  src/ExtEOStypes/EOSmanager.h
  src/ExtEOStypes/ArrheniusHE/AvArrheniusIDOF.h
  src/ExtEOStypes/ArrheniusHE/ArrheniusRate.h
  src/ExtEOStypes/ArrheniusHE/ArrheniusHE.h
  src/ExtEOStypes/ArrheniusHE/IdealHE_Isentrope.h
  src/ExtEOStypes/ArrheniusHE/StiffIdealHE.h
  src/ExtEOStypes/ArrheniusHE/IdealHE_Isotherm.h
  src/ExtEOStypes/ArrheniusHE/StiffIdealHEav.h
  src/ExtEOStypes/ArrheniusHE/IdealHE_Hugoniot.h
  src/ExtEOStypes/ArrheniusHE/IdealHE.h
  src/ExtEOStypes/ArrheniusHE/AvArrheniusRate.h
  src/ExtEOStypes/ArrheniusHE/ArrheniusIDOF.h
  src/Spline/Spline.h
  )
install(FILES ${HEADER_FILES} DESTINATION include)
install(DIRECTORY DATA DESTINATION .)
install(DIRECTORY TEST DESTINATION .)

#install(TARGETS 
#  RUNTIME DESTINATION ./bin
#  LIBRARY DESTINATION ./lib
#  ARCHIVE DESTINATION ./lib/static)
