if(${verbose_traversal})
  message(STATUS "Entered test_data subdirectory")
endif(${verbose_traversal})

list(APPEND data_files ArrheniusTest ApplicationsHE ApplicationsEOS ApplicationsNitromethane ApplicationsPBX9501 ApplicationsPBX9502 ApplicationsPolymer ApplicationsTest ApplicationsValidationHE DataBaseTest DataBaseUnits DavisTest DnKappaTest Elastic1DTest ElasticPlasticTest EOSTest ExtEOStypesTest GenHayesTest
  HayesTest HayesBMTest HEburnTest IdealGasTest JWLTest MieGruneisenTest PorousTest PTequilibriumTest StiffenedGasTest )

foreach(data_file ${data_files})
  configure_file(${data_file}.data ${CMAKE_CURRENT_BINARY_DIR}/${data_file}.data COPYONLY)
endforeach(data_file)

if(${verbose_traversal})
  message(STATUS "Exiting test_data subdirectory")
endif(${verbose_traversal})
