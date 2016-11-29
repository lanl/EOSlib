if(${verbose_traversal})
  message(STATUS "Entered doc subdirectory")
endif(${verbose_traversal})

file(GLOB foo LIST_DIRECTORIES false *.tex *.bib)
list(APPEND doc_files README.txt makefile Ref.tex EOS.bib)

foreach(doc_file ${doc_files})
  configure_file(${doc_file} ${CMAKE_CURRENT_BINARY_DIR}/${doc_file} COPYONLY)
endforeach(doc_file)

if(${verbose_traversal})
  message(STATUS "Exiting doc subdirectory")
endif(${verbose_traversal})
