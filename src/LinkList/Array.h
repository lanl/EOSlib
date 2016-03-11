//**********************************************************************//
//							    		                                //
//	List.h	   						                                    //
//	Date: July 1992							                            //
//	Version: 1.0							                            //
//	Latest Update: July 1992					                        //
//    Templates:     December 2001                                      //
//									                                    //
//	Authors:							                                //
//		Klaus S. Lackner 					                            //
//		Ralph Menikoff   					                            //
//									                                    //
//**********************************************************************//


#ifndef _ARRAY_
#define _ARRAY_

typedef void *Entry;
#define BitsPerChar 8


// 	BASE CLASS FOR VECTORS
//
class _Array
{
private:
	int offset;		    //	Current index of "pointer"
	void* off_ptr;		//	Current pointer
	int n_alloc;		//	Size of assigned element buffer
	void* block[sizeof(int)*BitsPerChar];	// Addresses of buffer blocks
	int dimension;		// maximum index referenced + 1 (<= n_alloc)
	int n_dynamic;		// First block that is not allocated in
				        // the initialization.
	
	
	virtual void* allocate(int) = 0;	    // Dummy Allocation routine
	virtual void  deallocate(void *)  = 0;	// Dummy DeAllocation Routine
	virtual void* array_ptr(void *p, int off) = 0;
						// Dummy Routine to return
						// address offset by off from p
					
	int address(int);	// Compute block address from element num.
	int sub_address(int i, int k) {return (i & ~(k ? (1 << k)  : 0) );}
	int sub_address(int i) {return sub_address(i, address(i));}
					    // Subaddresses within blocks
protected:
	int status;		    // Error status;
	_Array();
	~_Array();
public:
	void ArrayInit(int);
protected:
	void Cleanup();
	
// 	get() : returns current pointer, without increasing dimension
	void* get() {return off_ptr;}
	void* get(int);
	
//	Xget() : returns current pointer, if array dim = 0, Xget() creates it
	void* Xget() {if(!dimension) return Xget(0); else return off_ptr;}

//	Xget(int + n): set pointer to (off_ptr + n) it will exceed the current
//	last element by one.
	void* Xget(int);

//	Xget_next() : adds one more element to the array and points to it
	void* Xget_next() {return Xget(dimension-offset);}
	
//	Set offset (element pointed to) to the beginning of array	
	void* reset() {offset = 0; off_ptr=n_alloc ? block[0] : 0; return off_ptr;}

//	Add to offset
	void *operator+=(int);
public:
	int Nalloc() {return n_alloc;}
	int Offset() {return offset;}
	int Dimension() {return dimension;}
	int Status() { return status; }
};

//	derived class from base class _Array
//
template <class type> class ARRAY : private _Array
{
private:
	void* allocate(int n)      { return (void *) new type[n]; }
	void deallocate(void *ptr) { delete[] (type *)ptr; }
	void* array_ptr(void *ptr, int offs)
		{ return (void *)((type *)ptr + offs); }
	type error_val;
	void operator=(const ARRAY&);   // Disallowed
	ARRAY(const ARRAY&);            // Disallowed
public:
	ARRAY()                 {}
	ARRAY(int n_min)        { ArrayInit(n_min); }
	~ARRAY()                { Cleanup(); }
    
	type* get()		        { return (type *)_Array::get(); }
	type* get(int n)	    { return (type *)_Array::get(n); }
	type* Xget()		    { return (type *)_Array::Xget(); }
	type* Xget(int n)	    { return (type *)_Array::Xget(n); }
	type& Xget_next()	    { return *(type *)_Array::Xget_next();}
	type* reset()		    { return (type *)_Array::reset(); }
	type* operator+=(int n)	{ return (type *)_Array::operator+=(n);}
	type& operator[](int i)
	{
		type *temp = (type *)Xget(i);
		return temp ? *temp : (status = -1, error_val);
	}
	using _Array::ArrayInit;
	using _Array::Nalloc;
	using _Array::Offset;
	using _Array::Dimension;
	using _Array::Status;
};

#endif /* _ARRAY_ */
	
