Obtaining the source code:

Clone the repository from git:
- git clone git@github.com:lanl/EOSlib.git
- source directory <eoslib>

Building the library:

mkdir <build_dir>
cd <build_dir>
cmake <eoslib>
make
make test

Usage:
  Add to path <build_dir>/bin
      or run: source <build_dir>/SetEnv
      or add to shell initialization file alias
         for runEOS  -> <build_dir>/bin/runEOS
         and listEOS -> <build_dir>/bin/listEOS
  runEOS help   # for examples
                # runEOS can run any utility in build/bin
  listEOS       # for all EOS in default database (<build_dir>/DATA)

Environment variables:
- EOSLIB_DATA_PATH
  - Directory where EOSlib data files are located
- EOSLIB_SHARED_LIBRARY_PATH
  - Directory where EOSlib shared libraries are located

Accessing library examples:
- Examples/README.txt

Required packages:
- cmake 3.0+
- C++11 compiler (tested on gcc)
- boost C++ library (optional, required for some tests)
- perl (optional, required for GUI)
- Tk (optional, required for GUI)
- XWindows (optional, required for GUI)
- Xmgrace (optional, required for plots in GUI)



