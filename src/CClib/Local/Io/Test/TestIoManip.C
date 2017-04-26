//#define BOOST_TEST_DYN_LINK
//#define BOOST_TEST_MODULE TestLocalIoManip
#include <LocalIo.h>
#include <boost/test/unit_test.hpp>
#include <boost/test/output_test_stream.hpp>
#include <iostream>

void test_str_ofunc (std::ostream& os, const char* str)
{
  os << str;
}

BOOST_AUTO_TEST_CASE(TestStrIoManip0){
  char* teststr = "Hi!";
  std::ostringstream testostream;
  str_iomanip test1 = str_iomanip(test_str_ofunc);

  testostream << test1(teststr);
  BOOST_CHECK(testostream.str() == teststr);
}

void test_str_ifunc (std::istream& is, const char* str)
{
// discards 5 characters from is
  char* a = new char[6];
  is.read(a, 5);
  delete[] a;
}


BOOST_AUTO_TEST_CASE(TestStrIoManip1){
  std::string teststr("Lorem Ipsum.");
  std::istringstream testistream(teststr);
  str_iomanip test2(test_str_ifunc);
  std::string foo;

  testistream >> test2;
  std::getline(testistream, foo);
  BOOST_CHECK(foo == " Ipsum.");
}

BOOST_AUTO_TEST_CASE(TestStrIoManip2){
  std::string teststr1("Lorem Ipsum.");
  std::string teststr2("Hi!");
  char* teststr3("Hello World!");

  std::istringstream testistream(teststr1);
  std::ostringstream testostream(teststr2);

  str_iomanip testiomanip(test_str_ifunc, test_str_ofunc, 0);
  
  std::string foo;
  testistream >> testiomanip;
  std::getline(testistream, foo);
  BOOST_CHECK(foo == " Ipsum.");

  testostream << testiomanip(teststr3);
  BOOST_CHECK(testostream.str() == "Hello World!");

  testostream << testiomanip(teststr1.c_str());
  BOOST_CHECK(testostream.str() == std::string(teststr3)+teststr1);
}

void test_int_ifunc(std::istream& inp, int n){
// discards n characters from is
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

  int_iomanip test_iomanip(test_int_ifunc, test_int_ofunc, 0);

  testistream >> test_iomanip(5);
  std::string outstring;
  std::getline(testistream, outstring);
  BOOST_CHECK(outstring == std::string(" Ipsum."));

  testostream << test_iomanip(3);
  BOOST_CHECK(testostream.str() == std::to_string(3));

  testostream << test_iomanip(6);
  BOOST_CHECK(testostream.str() == std::to_string(36));
}


void test_str_istream_func(std::istream& inp, const char* str, std::ostream& stream){
  // read string from inp, prefix with str and write to stream
  stream << str;
  std::string tmp;
  inp >> tmp;
  stream << tmp;
}

BOOST_AUTO_TEST_CASE(TestStrOstreamIoManip){
  std::ostringstream testostream;
  char* str = "Hello World!";
  str_ostream_iomanip test_str_ostream_iomanip(test_str_istream_func, 0, std::cerr);
  std::istringstream testistream("Lorem Ipsum");

  testistream >> test_str_ostream_iomanip(str,testostream);
  BOOST_CHECK(testostream.str() == std::string("Hello World!Lorem"));
}
