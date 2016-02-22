TESTS=TestShock      TestIsentrope        TestIsotherm  \
      TestImpedance1 TestImpedanceConvert TestImpedance2

install_src: makedirs
	@echo; echo "***** install_src *****" ; echo
	@os=`uname`; pwd=`pwd -P`; src/Install CWD=$$pwd 2>&1 | \
                        tee logs/$$os/src.log
	@os=`uname`; echo "log file: logs/$$os/src.log"
	@os=`uname`; status=`tail -1 logs/$$os/src.log`; \
            if [ $$status -ne 0 ] ; then                 \
		echo "install_src failed";               \
		exit 1;                                  \
            fi 
	@rm -f Run; ln -s src/Run Run
	@cd bin; rm -f Compile; ln -s ../src/Compile Compile

applications: 
	@echo; echo "***** src/applications *****" ; echo
	@os=`uname`; pwd=`pwd -P`; src/Install CWD=$$pwd Applications 2>&1 | \
                        tee logs/$$os/src.log
	@os=`uname`; echo "log file: logs/$$os/src.log"
	@os=`uname`; status=`tail -1 logs/$$os/src.log`; \
            if [ $$status -ne 0 ] ; then                 \
		echo "applications failed";               \
		exit 1;                                  \
            fi 

makedirs:
	if [ ! -d bin ] ; then mkdir bin; fi
	if [ ! -d lib ] ; then mkdir lib; fi
	if [ ! -d include ] ; then mkdir include; fi
	if [ ! -d logs ] ; then mkdir logs; fi
	if [ ! -d out ] ; then mkdir out; fi
	if [ ! -d include/backward ] ; then cp -r src/backward include; fi
	os=`uname`; if [ ! -d bin/$$os ] ; then mkdir bin/$$os; fi
	os=`uname`; if [ ! -d lib/$$os ] ; then mkdir lib/$$os; fi
	os=`uname`; if [ ! -d logs/$$os ] ; then mkdir logs/$$os; fi


all:	install_src 

TestLib: $(TESTS)



###

TestShock: Run
	@echo "Running TestShock"
	@./Run Wave > out/TestShock 2>&1 \
		material Hayes::HMX P2 10 shock 
	@-diff TestAnswers/TestShock out/TestShock \
		&& echo "	TestShock passes"
	
TestIsentrope: Run
	@echo "Running TestIsentrope"
	@./Run Wave > out/TestIsentrope 2>&1 \
		material Hayes::HMX P2 10 isentrope
	@-diff TestAnswers/TestIsentrope out/TestIsentrope \
		&& echo "	TestIsentrope passes"

TestIsotherm: Run
	@echo "Running TestIsotherm"
	@./Run Isotherm > out/TestIsotherm 2>&1 \
		material Hayes::HMX P2 10 PT 1 500
	@-diff TestAnswers/TestIsotherm out/TestIsotherm \
		&& echo "	TestIsotherm passes"

TestImpedance1: Run
	@echo "Running TestImpedance1"
	@./Run ImpedanceMatch > out/TestImpedance1 2>&1 \
		mat1 Hayes::HMX Ps 2 mat2 IdealGas::5/3 state2 100 1.5e-2
	@-diff TestAnswers/TestImpedance1 out/TestImpedance1 \
		&& echo "	TestImpedance1 passes"

TestImpedanceConvert: Run
	@echo "Running TestImpedanceConvert"
	@./Run ImpedanceMatch > out/TestImpedanceConvert 2>&1 \
		mat1 Hayes::HMX Ps 0.02 mat2 IdealGas::5/3 state2 100 1.5e-4 \
			units hydro::HiP
	@-diff TestAnswers/TestImpedanceConvert out/TestImpedanceConvert \
		&& echo "	TestImpedanceConvert passes"

TestImpedance2: Run
	@echo "Running TestImpedance2"
	@./Run ImpedanceMatch > out/TestImpedance2 2>&1 \
		mat1 Hayes::PMMA Ps 5 mat2 Hayes::HMX
	@-diff TestAnswers/TestImpedance2 out/TestImpedance2 \
		&& echo "	TestImpedance2 passes"
###

WipeOut:
	rm -f core junk* Run EOSlog psfile  *.eps *.gui
	#cd Tex;  make Cleanup
	cd src; make RealClean
	rm -rf bin lib include Plots out logs tmp
	cd Amrita; ./CLEANUP
	find . -name ".*.swp" -print -exec rm -f {} \;
	find . -name "*~" -exec rm -f {} \;
	@echo; echo "*** WipeOut Complete ***"; echo

tar:	WipeOut
	cd .. ; tar cvf - EOSlib | gzip -c > EOSlib.tgz

