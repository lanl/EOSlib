//#define BOOST_TEST_DYN_LINK
#define BOOST_TEST_MODULE TestLocalIoManip
#include <LocalIo.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>

void test_str_ofunc (std::ostream& os, const char* str)
{
  os << str;
}

void test_str_ifunc (std::istream& is, const char* str)
{
  char* a = new char[6];
  is.read(a, 5);
  delete[] a;
}

BOOST_AUTO_TEST_CASE(TestStrIoManip0){
  char* teststr = "Hi!";
  std::ostringstream testostream;
  str_iomanip test1 = str_iomanip(test_str_ofunc);

  test1(teststr);
  testostream<<test1;
  BOOST_CHECK(testostream.str() == teststr);
}

BOOST_AUTO_TEST_CASE(TestStrIoManip1){
  std::string teststr2("Lorem Ipsum.");
  std::istringstream testistream(teststr2);
  str_iomanip test2(test_str_ifunc);
  std::string foo;

  testistream>>test2;
  std::getline(testistream, foo);
  BOOST_CHECK(foo == " Ipsum.");
}

BOOST_AUTO_TEST_CASE(TestStrIoManip2){
  std::string teststr("Lorem Ipsum.");
  std::string teststr2("Hi!");
  char* teststr3("Hello World!");
  std::istringstream testistream(teststr);
  std::ostringstream testostream(teststr2);
  str_iomanip testiomanip(test_str_ifunc, test_str_ofunc, teststr3);
  std::string foo;
  
  testistream>>testiomanip;
  std::getline(testistream, foo);
  BOOST_CHECK(foo == " Ipsum.");
  testostream<<testiomanip;
  BOOST_CHECK(testostream.str() == "Hello World!");
  testiomanip(teststr.c_str());
  testostream<<testiomanip;
  BOOST_CHECK(testostream.str() == std::string(teststr3)+teststr);
}

void test_int_ifunc(std::istream& inp, int n){
  char* a = new char[n + 1];
  inp.read(a, n);
  delete[] a;
}

void test_int_ofunc(std::ostream& out, int n){
  out << std::to_string(n);
}

BOOST_AUTO_TEST_CASE(TestIntIoManip){
  std::ostringstream testostream;
  std::istringstream testistream("Lorem Ipsum.");
  int_iomanip test_iomanip(test_int_ifunc, test_int_ofunc, 5);
  std::string outstring;
  testistream >> test_iomanip;
  std::getline(testistream, outstring);
  BOOST_CHECK(outstring == std::string(" Ipsum."));
  testostream << test_iomanip;
  BOOST_CHECK(testostream.str() == std::to_string(5));
  test_iomanip(6);
  testostream << test_iomanip;
  BOOST_CHECK(testostream.str() == std::to_string(56));
}

void test_str_istream_func(std::istream& inp, const char* str, std::ostream& stream){
  char* intermediate = new char[6];
  inp.read(intermediate, 5);
  stream << std::string(intermediate);
  stream << std::string(str);
  delete[] intermediate;
}

BOOST_AUTO_TEST_CASE(TestStrOstreamIoManip){
  std::ostringstream testostream;
  char* str = "Hello World!";
  str_ostream_iomanip test_str_ostream_iomanip(test_str_istream_func, str, testostream);
  std::istringstream testistream("Lorem Ipsum.");

  testistream >> test_str_ostream_iomanip;
  BOOST_CHECK(testostream.str() == std::string("LoremHello World!"));
  test_str_ostream_iomanip(str, testostream);
  testistream >> test_str_ostream_iomanip;
  BOOST_CHECK(testostream.str() == std::string("LoremHello World! IpsuHello World!"));
}

