COMPL=_COMPL
OS=_OS
ECHO=bin/$(OS)/Echo


IFLAGS=-I. -I../backward
IFLAGS_SUB=-I.. -I../../backward

CFLAGS=

PWD=MakeMake

###### Suffix Rules ###### 

.SUFFIXES:
.SUFFIXES: .o .C

.C.o:
	$(COMPL) -c $(CFLAGS) $(IFLAGS) $<;

####################################################

EXECS=bin/$(OS)/Echo bin/$(OS)/MakeMake

all: $(EXECS)

############################

bin/$(OS)/MakeMake: MakeMake.o MakeLib.a
	$(COMPL) -o MakeMake $(CFLAGS) MakeMake.o  MakeLib.a
	mv MakeMake bin/$(OS)
	rm MakeMake.o MakeLib.a
	

bin/$(OS)/Echo:  Echo.o
	$(COMPL) -o Echo $(CFLAGS) Echo.o
	mv Echo bin/$(OS)
	rm Echo.o
	
bin/$(OS)/dso_handle.o:  dso_handle.o
	mv dso_handle.o bin/$(OS)
	
############################

MakeLib.a : subs/*.C
	@rm -f MakeLib.a
	cd subs; make "COMPL=$(COMPL)" "CFLAGS=$(CFLAGS)" "IFLAGS=$(IFLAGS_SUB)"
	mv subs/lib.a MakeLib.a
	@$(ECHO)

############################

Depend.h : List.h 
	touch Depend.h

MakeMake.c : Depend.h 
	touch MakeMake.c


############################

cleanup:
	rm -f MakeLib.a a.out core *.o *% *~ junk*
	rm -f MakeFile Echo MakeMake subs/MakeFile MakeLib.a
	os=`uname`; rm -f bin/$$os/Echo bin/$$os/MakeMake bin/$$os/dso_handle.o
	rm -f MAKEDIR/*%   MAKEDIR/*~ MAKEDIR/*/*% MAKEDIR/*/*~
	rm -f     bin/*%       bin/*~     bin/*/*%     bin/*/*~
	cd subs; make cleanup

Cleanup: cleanup
	rm -f bin/*/Echo bin/*/MakeMake
	cd subs; make Cleanup
