#ifndef EOSLIB_POINTER_ARRAY_H
#define EOSLIB_POINTER_ARRAY_H

class _PointerArray
{
private:
	void operator=(const _PointerArray&);	// Disallowed
	_PointerArray(const _PointerArray&);	// Disallowed	
protected:
	int dim;
	int n;
	char **names;
	void **pointers;
	void ReAlloc();
	void DeAlloc();
	virtual void Delete(void *pointer) = 0;
public:
	_PointerArray(int Dim = 32);
	~_PointerArray() {}
	int Add(const char *name, void *ptr);
	int Replace(const char *name, void *ptr);
	int Find(const char *name);
	void     *Pointer(int i) const {return pointers[i];}
	const char *Names(int i) const {return names[i]; }
	int N() const { return n; }
};

template<class C>class PointerArray : protected _PointerArray
{
private:
	void operator=(const PointerArray&);	// Disallowed
	PointerArray(  const PointerArray&);  	// Disallowed	
protected:
    void Delete(void *pointer) { delete (C *)pointer; }
public:
	PointerArray(int Dim = 32) : _PointerArray(Dim) {}
	~PointerArray() {DeAlloc();}

	int Add(const char *name, C *&ptr)
            { void *vptr=(void*)ptr; ptr=0;
              return _PointerArray::Add(name,vptr); }
	int Replace(const char *name, C *&ptr)
	    { void *vptr=(void*)ptr; ptr=0;
              return _PointerArray::Replace(name,vptr); }
	int Find(const char *name)
	    { return _PointerArray::Find(name); }
	C *operator[](int i)
	    { return (C *)_PointerArray::Pointer(i); }
	const char *Names(int i) { return _PointerArray::Names(i); }
	int N()                  { return _PointerArray::N();      }	
};

#endif //  EOSLIB_POINTER_ARRAY_H
