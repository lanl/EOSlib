//**********************************************************************//
//                                                                      //
//  ReferenceList.h                                                     //
//  Date: March 1992                                                    //
//  Version: 1.0                                                        //
//  Latest Update: April 1992                                           //
//    Templates:     December 2001                                      //
//                                                                      //
//  Authors:                                                            //
//      Klaus S. Lackner                                                //
//      Ralph Menikoff                                                  //
//                                                                      //
//**********************************************************************//

#ifndef _REFLIST_
#define _REFLIST_

#include <List.h>


class _RefList;     // internal data structure


//  Usage:
//      elements in a class OBJECT are tagged by TAGforOBJECT-type
//      tags. Objects may generate multiple tags and different
//      objects may generate identical tags.
//      OBJECTREfList functions enter objects into the reference
//      list or delete them from the list.  They retrieve a list
//      of all objects which match a tag or at least one or all
//      tags in a given list of tags.
//
//  Initialization:
//
//  class OBJECT;
//  class TAGforOBJECT;
//
//  LIST<OBJECT>;
//  LIST<TAGforOBJECT>;
//  REFLIST<OBJECT,TAGforOBJECT>;
//
//  Must supply:
//  int Hash(TAGforOBJECT& tag) { /* ... */ }
//  int IsTag(TAGforOBJECT& tag, OBJECT& obj) { /* ... */ }
//  void GetTags(OBJECT& obj, TAGforOBJECTList& TL) {/*...*/}
//
//  Calls:
//
//  class OBJECTRefList X(size);        // size defines the size of
//                                      // internal hash tables.
//                                      // The number of objects
//                                      // is not limited.
//  class OBJECT a, b, c, d;
//  X.Enter(a); X.Enter(b); /* ... */   // Enter objects into list
//
//  class OBJECTList retrieved;
//  class TAGforOBJECT tag;
//  class TAGforOBJECTList TagList;
//
//  X.Get(tag, retrieved);              // Appends to list "retrieved"
//                                      // all OBJECTS which
//                                      // are referenced by tag.
//
//  X.GetAnd(TagList, retrieved)        // if "retrieved" is non-empty
//                                      //     Deletes from retrieved
//                                      //     OBJECT which do not match
//                                      //     all tags in TagList
//                                      // else sets retrieved
//                                      //     to OBJECTS which match
//                                      //     all tags in TagList
//
//  X.GetOr(TagList, retrieved)         // Appends to list "retrieved"
//                                      // objects match at least one
//                                      // Tag in TagList. 
//
//  X.Delete(a)                         // Removes a from the
//                                      // reference list.
//
//  Note:   Get(), GetAnd() & GetOr() set list "retrieved" to first OBJECT
//          and return number of elements on list "retrieved"
//      Delete() returns the number of references removed
//
class _RefList
{
private:
    int hash_table_size;
    int n_entries;
    int n_references;
    
protected:
    _RefList(int size);
    ~_RefList() {}
    virtual int hash(const void*)       { /* Not Used */ return 0;}
    void EnterRef(void* obj);
    void Enter(void* tag, void* obj);
    int Get(const void* tag, VoidList* Matches);
    int GetAnd(VoidList* taglist, VoidList* Matches);
    int GetOr (VoidList* taglist, VoidList* Matches);
    virtual int is_tag(const void*, void*)  = 0;
    virtual VoidList* get_tags(void*)   = 0;
    virtual VoidList& SubList(int n)    = 0;
    
public:
    int Delete(void*);
    int TableDimension() const      { return hash_table_size; }
    int N_entries() const           { return n_entries; }
    int N_references() const        { return n_references; }
};

template <class elem,class tag> extern int IsTag(const tag&, const elem&);
template <class elem,class tag> extern void GetTags(elem&, LIST<tag>&);
template <class tag>            extern int Hash(const tag&);
template <class elem,class elem_tag> class REFLIST : public _RefList
{
private:
    LIST<elem> *SubList1;
    int is_tag(const void* tp, void* ob)
        { return IsTag(*(elem_tag *)tp, *(elem *)ob); }
    VoidList* get_tags(void* obj)
        { return Get_Tags(*(elem *)obj); }
    int hash(const void* tp); //    { return Hash(*(elem_tag *)tp); }
    LIST<elem_tag>* Get_Tags(elem& el)
    {
        LIST<elem_tag> *TL = new LIST<elem_tag>;
        GetTags(el, *TL);
        return TL;
    }
    VoidList& SubList(int n) { return *(VoidList *)(SubList1+n);}
    void Enter(void* tag, void* obj) {_RefList::Enter(tag,obj);}
    int Get(const void* tag, VoidList* Matches)
        { return _RefList::Get(tag, Matches);}
    int GetAnd(VoidList* taglist, VoidList* Matches)
        { return _RefList::GetAnd(taglist, Matches);}
    int GetOr (VoidList* taglist, VoidList* Matches)
        { return _RefList::GetOr(taglist, Matches);}
public:
    REFLIST(int size) : _RefList(size)
        { SubList1 = new LIST<elem>[TableDimension()]; }
    ~REFLIST() { delete [] SubList1;}
    void Enter(elem& el)    { _RefList::EnterRef(&el); }
    int Get(const elem_tag& tag, LIST<elem>& Lst)
        { return _RefList::Get(&tag, &Lst); }
    int GetAnd(LIST<elem_tag>& tags, LIST<elem>& Matches)
        { return _RefList::GetAnd(&tags, &Matches); }
    int GetOr(LIST<elem_tag>& tags, LIST<elem>& Matches)
        { return _RefList::GetOr(&tags, &Matches); }
};

#endif /* _REFLIST_ */
