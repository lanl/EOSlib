COMPL=_CC

CFLAGS=
IFLAGS=-I..
 
############################

lib.a : lib.a(Depend.o) lib.a(List.o) lib.a(Template.o) lib.a(subs.o)
	-ranlib lib.a


############################

lib.a(Depend.o) : Depend.C
	$(COMPL) -c $(CFLAGS) $(IFLAGS) Depend.C
	ar ru lib.a Depend.o;
	rm Depend.o
	
lib.a(List.o) : List.C
	$(COMPL) -c $(CFLAGS) $(IFLAGS) List.C
	ar ru lib.a List.o;
	rm List.o
	
lib.a(Template.o) : Template.C
	$(COMPL) -c $(CFLAGS) $(IFLAGS) Template.C
	ar ru lib.a Template.o;
	rm Template.o
	
lib.a(subs.o) : subs.C
	$(COMPL) -c $(CFLAGS) $(IFLAGS) subs.C
	ar ru lib.a subs.o;
	rm subs.o
	
############################

Depend.C : ../Depend.h ../subs.h 
	touch Depend.C

List.C : ../List.h 
	touch List.C

Template.C : ../Depend.h ../subs.h 
	touch Template.C

subs.C : ../subs.h 
	touch subs.C

############################

cleanup:
	-rm -f *.o *% *~ core junk* lib.a

Cleanup: cleanup
	-rm -f MakeFile
	


