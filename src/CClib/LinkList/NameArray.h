//**********************************************************************//
//                                                                      //
//  NameArray.h                                                         //
//  Date: March 1992                                                    //
//  Version: 1.0                                                        //
//  Latest Update: March 1992                                           //
//    Templates:     December 2001                                      //
//                                                                      //
//  Authors:                                                            //
//      Klaus S. Lackner                                                //
//      Ralph Menikoff                                                  //
//                                                                      //
//**********************************************************************//

#ifndef _NAMEARRAY_
#define _NAMEARRAY_

#include <List.h>

class _NameEntry;           // internal data structure
class _NameEntryList;       // internal data structure
class _NameArray;           // internal data structure

//  class NameArray<type> VAR(int HashTableSize, int (*HashFunction)(char*));
//      HashTableSize is optional: Default = 101
//      HashFunction is optional:  Default = &_DefaultNameArrayHash
//
//  Notes: 
//      Number of entries in NameArray is unlimited
//      Can reset both HashTableSize and  HashFunction
//          but only if number of entries is 0
//          Needed for
//              NameArray<type> *VAR = new NameArray<type>;
//              VAR->SetHashTableDimension(HashTableSize);


class _NameEntry
{
    friend class _NameArray;
    friend class _NameEntryList;
private:
    Entry object;
    char *name;
    
    _NameEntry(const char *s, Entry obj);
    ~_NameEntry();
    
    Entry Object()     { return object; }
    const char *Name() { return name; }
};

class _NameArray;

class _NameEntryList : private _List
{
    friend class _NameArray;
public:
    _NameArray *parent;
    ~_NameEntryList() { Empty(); }
private:
    _NameEntryList() { parent = 0; }
    
    void DeAlloc(Entry obj);
    
    void Append(_NameEntry *obj) { _List::Append((Entry)obj, 0); }
    void Insert(_NameEntry *obj) { _List::Insert((Entry)obj, 0); }
    void Remove()                { _List::Remove(); }
    void Remove(int dir)         { _List::Remove(dir); }
    
    _NameEntry* Start() { _List::Start(); return (_NameEntry *) _List::Get(); }
    _NameEntry* Next()  { return (_NameEntry *) _List::Next();}
};

int _DefaultNameArrayHash(const char *name);
    
class _NameArray
{
    friend class _NameEntryList;
protected:
    int hash_table_size;
    _NameEntryList* SubList;
private:
    int n_entries;

    int (*Hash)(const char* name);
    virtual void DeAllocate(Entry obj) = 0;
    virtual Entry Allocate() = 0;
    const char *last_name;
    Entry last_entry;
protected:
    _NameArray(int size, int (*func)(const char*)=&_DefaultNameArrayHash);
    ~_NameArray() { }   // SubList must be deleted by derived class
    
    void SetTable(int size);
    void SetHashFunction(int (*func)(const char*));
        
    Entry Get(const char *name);
    Entry Replace(const char *name, Entry val);
    void Delete(const char *name);
    int Defined(const char *name);
    int Dimension()        const {return n_entries;}
    int TableDimension()   const {return hash_table_size;}
    const char *LastName() const { return last_name; }
    Entry LastEntry()      const { return last_entry; }
    const char **Names()   const;
};


template <class type, int size=101> class NAMEARRAY : private _NameArray
{
private:
    Entry Allocate()           { return ((Entry) new type);}
    void DeAllocate(Entry obj) { delete (type *)obj; }
    Entry Replace(const char *name, Entry val)
          {return _NameArray::Replace(name,val);}
public:
    NAMEARRAY ()      : _NameArray(size) { }
    NAMEARRAY(int sz) : _NameArray(sz)  { }
    NAMEARRAY(int sz, int (*func)(const char*)) : _NameArray(sz, func)  { }
    ~NAMEARRAY() { if(SubList) delete [] SubList;}
    type* Replace(const char *name, type *val)
          { return (type*) _NameArray::Replace(name, (Entry) val); }
    type& operator[](const char *name) { return *(type*) _NameArray::Get(name);}
    void Delete(const char *name){ _NameArray::Delete(name); }
    int Defined(const char *name){ return _NameArray::Defined(name);}
    int Dimension()        const { return _NameArray::Dimension(); }
    void SetHashTableDimension(int sz) { _NameArray::SetTable(sz);}
    void SetHashFunction(int (*func)(const char*))
                                 { _NameArray::SetHashFunction(func); }
    int TableDimension()   const { return _NameArray::TableDimension();}
    const char* LastName() const { return _NameArray::LastName(); }
    type& LastElement()    const { return *(type*)_NameArray::LastEntry();}
    const char** Names()   const { return _NameArray::Names(); }
};

#endif  /* _NAMEARRAY_ */
