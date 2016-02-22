CClib=_Local _LinkList _OneDFunction _ODE _Spline _Calc
EOS=_DataBase _Eos _EOStypes _Manager
EXTEOS=_ExtEOS _ExtEOStypes
APPS=_Applications
CWD=_CWD

###

all: _MakeMake Cleanup libs eos

libs: $(CClib)

eos: $(EOS) $(EXTEOS) $(APPS)

exteos: $(EXTEOS) $(APPS)

Applications: $(APPS)


EOS: CleanEOS eos Applications

.PHONY: tags
tags:
	ctags --recurse --exclude=include/\* \
                --exclude=lib/\*         \
                --exclude=bin/\*         \
                --exclude=pl/\*          \
                --exclude=out/\*         \
                --exclude=Debug/\*       \
                --exclude=InProgress/\*  \
                --exclude=Test/\*        \
                --exclude=\*~

CleanEOS:
	cd DataBase;        ../Compile CWD=${CWD} CLEANUP
	@echo   
	cd Eos;		        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd EOStypes;        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Applications;    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Manager;         ../Compile CWD=${CWD} CLEANUP
	@echo
	cd ExtEOS;		    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd ExtEOStypes;		../Compile CWD=${CWD} CLEANUP
	@echo


###

_Calc:
	@echo; echo "##### Compile Calc         #####"; echo
	cd Calc;           ../Compile NewMake CWD=${CWD} install

_DataBase:
	@echo; echo "##### Compile DataBase     #####"; echo
	cd DataBase;       ../Compile NewMake CWD=${CWD} install

_Eos:
	@echo; echo "##### Compile Eos          #####"; echo
	cd Eos;            ../Compile NewMake CWD=${CWD} install InstallEOS

_EOStypes:
	@echo; echo "##### Compile MaterialLib  #####"; echo
	cd EOStypes;       ../Compile CWD=${CWD} CLEANUP; \
	                   ../Compile CWD=${CWD} install

_ExtEOS:
	@echo; echo "##### Compile ExtEOS       #####"; echo
	cd ExtEOS;         ../Compile NewMake CWD=${CWD} install InstallExtEOS

_ExtEOStypes:
	@echo; echo "##### Compile ExtEOStypes  #####"; echo
	cd ExtEOStypes;    ../Compile CWD=${CWD} CLEANUP; \
	                   ../Compile CWD=${CWD} install
###

_Applications:
	@echo; echo "##### Compile Applications #####"; echo
	cd Applications;   ../Compile NewMake CWD=${CWD} install

_Manager:
	@echo; echo "##### Compile Manager      #####"; echo
	cd Manager;        ../Compile NewMake CWD=${CWD} install InstallEOSmanager

###


_MakeMake:
	@echo; echo "##### Installing MakeMake  #####"; echo
	cd MakeMake;    ./Install

_Local:
	@echo; echo "##### Compile Local        #####"; echo
	cd Local;           ../Compile NewMake CWD=${CWD} install

_LinkList:
	@echo; echo "##### Compile LinkList     #####"; echo
	cd LinkList;        ../Compile NewMake CWD=${CWD} install

_OneDFunction:
	@echo; echo "##### Compile OneDFunction #####"; echo
	cd OneDFunction;    ../Compile NewMake CWD=${CWD} install

_ODE:
	@echo; echo "##### Compile ODE          #####"; echo
	cd ODE;             ../Compile NewMake CWD=${CWD} install

_Spline:
	@echo; echo "##### Compile Spline       #####"; echo
	cd Spline;          ../Compile NewMake CWD=${CWD} install
###

ListTypes:
	Run -test Data base \* type \*

ListUnits:
	Run -test Data base Units type hydro name \*

ListEOS:
	Run -test Data base EOS type \* name \* plain

ListHMX:
	Run -test Data base EOS type Hayes name HMX parameters

PrintVonMises:
	Run -test EOSdata material VonMises::HMX

###

cleanup:
	rm -f *% *~ core junk* tags
	@echo
	cd DataBase;        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Eos;		        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd EOStypes;        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Manager;         ../Compile CWD=${CWD} CLEANUP
	@echo
	cd ExtEOS;	        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd ExtEOStypes;	    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Applications;    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd DnKappa;		    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Table;		    ../Compile CWD=${CWD} CLEANUP
	@echo


Cleanup: cleanup
	@echo
	cd Local;           ../Compile CWD=${CWD} CLEANUP
	@echo
	cd LinkList;        ../Compile CWD=${CWD} CLEANUP
	@echo
	cd OneDFunction;    ../Compile CWD=${CWD} CLEANUP
	@echo
	cd ODE;             ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Spline;          ../Compile CWD=${CWD} CLEANUP
	@echo
	cd Calc;            ../Compile CWD=${CWD} CLEANUP
	@echo

RealClean: Cleanup
	cd MakeMake;      make Cleanup
	rm -rf bin lib include out Plots
	find . -name ".kdbg*" -print -exec rm -f {} \;

###

tar:	RealClean
	cd .. ; tar cvf - src | gzip -c > EOSsrc.tgz
	wc -l `find . -name "*.[hC]"`
	du -ks
	find . -name "*.h" | wc -l
	find . -name "*.C" | wc -l
	ls -l ../EOSsrc.tgz

untar:	../EOSsrc.tgz
	cd ..; rm -rf src.0; mv src src.0; \
               gunzip -c src.tgz | tar xvf -
