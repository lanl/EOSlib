//#define BOOST_TEST_MODULE TestErrorHandler
#include <ErrorHandler.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>

BOOST_AUTO_TEST_CASE(TestLocalErrorHandlerLog){
  std::ostringstream testostream;
  LocalErrorHandler testerrorhandler(testostream);
  char* func = "Hello World!";
  testerrorhandler.Log(func, "%s %s %s", "Hi there!", "I'm a function!", "Hello?");
  BOOST_CHECK(testostream.str() == std::string("LocalErrorHandler called from Hello World!, Hi there! I'm a function! Hello?\n"));
}

BOOST_AUTO_TEST_CASE(TestLocalErrorHandlerAbort){
  std::ostringstream testostream;
  LocalErrorHandler testerrorhandler(testostream);
  char* func = "I'm a function! ";
  char* file = "I'm a file! ";
  int line = 101;
  BOOST_CHECK_THROW(testerrorhandler.Abort(func, file, line, "%s %s",
					   "Lorem Ipsum.", "Hello?");,
		    LocalErrorHandler);
  BOOST_CHECK(std::string("Error in I'm a function! , file `I'm a file! 'line 101\n") +
	      std::string("Lorem Ipsum. Hello?\n") == testostream.str());
}
