Obtaining the source code:

Clone the repository from git:
- git clone git@github.com:lanl/EOSlib.git

Building the library:

Create a build directory and cd to it
cmake $PATH_TO_SOURCE_DIRECTORY
make
ctest

Environment variables:
- EOSLIB_DATA_PATH
  - Directory where EOSlib data files are located
- EOSLIB_SHARED_LIBRARY_PATH
  - Directory where EOSlib shared libraries are located

Accessing library examples:
- Coming soon

Required packages:
- cmake 3.0+
- C++11 compiler (tested on gcc)
- boost C++ library (optional, required for some tests)
- perl (optional, required for GUI)
- Tk (optional, required for GUI)
- XWindows (optional, required for GUI)
- Xmgrace (optional, required for plots in GUI)



