//**********************************************************************//
//                                                                      //
//    List.h                                                            //
//    Date: March 1992                                                  //
//    Version: 1.0                                                      //
//    Latest Update: February 1992                                      //
//    Templates:     December 2001                                      //
//                                                                      //
//    Authors:                                                          //
//        Klaus S. Lackner                                              //
//        Ralph Menikoff                                                //
//                                                                      //
//**********************************************************************//

#ifndef _LIST_
#define _LIST_

#include <iostream>
typedef void *Entry;

class _Link;           // internal data structure
class _List;           // internal data structure
class _ListAccess;     // internal data structure

class VoidList;        // List Class with unspecified TYPE

//        class LIST<type> Variable;
//        class FIFO<type> Variable;
//        class FILO<type> Variable;

//    Circular LINKED LIST with marked first link
//        Array (variable dimensions)
//        Circular List
//        FILO (fist in last out or push down stack)
//        FIFO (first in first out or pipe)
//
//  Examples of Usage
//
//        class Point
//        {
//            double x;
//            double y;
//            double z;
//        };
//
//  Ex 1: Variable Dimension Array
//
//        LIST<Point> Array;
//
//        Point p1, p2, p3, p4;
//
//        Array.Insert(p1);
//        Array.Add(p2);
//        Array.Append(p3);    // does not change current link
//        p2 = Array.Get();
//        Array.Replace(p4);
//        Array.Remove();       // remove and move current link forward
//
//    Loop over Array
//        const Point *p;    // note, only *p is constant but not p
//        Array.Save();
//        for( Array.Start(); p = Array; Array++)           // forward loop
//        // for( Array.End();   p = Array; Array--)     // backwards loop
//        {
//            . . .
//
//            Point *p_next = Array.Get(1);    // preview next link
//                            // does not effect loop
//
//            Point *p_prev = Array.Get(-1);    // previous link
//                            // does not effect loop
//
//            // To remove element and not upset loop
//            // delete Array++ or Array-- from for( ) and add at end
//                if( )
//                    Array.Remove();
//                    // Array.Remove(0)     // backwards loop
//                else
//                    Array++; // or Array + 1
//                     // Array--; or Array-1 // backwards loop
//
//            Array.Reset();        // break at end of loop
//
//            . . .
//        }
//        Array.Restore();
//                // current link is same as at beginning of loop
//
//
// Ex 2:  Use array as Circular List
//        LIST<Point> Clist;
//
//        Point p1, p2, p3, p4;
//
//        Clist.Add(p1);
//        Clist.Add(p2);
//        Clist.Add(p3);
//        p3 = Clist.Get();
//        p1 = Clist.Next(1,1);        // force past marker
//        p3 = Clist.Prev(1,1);        // force past marker
//
//    Loop backwards from current link over List
//        const Point *p;    // note, only *p is constant but not p
//    for(p = Array, int n = Array.Dimension(); n; n--, p = Array.Prev(1,1))
//    {
//            . . .
//
//    }
//    // current link is same as at beginning of loop
//
//
//  Ex 3: FILO or Push Down Stack
//
//      FIFO<Point> stack;
//      Point p1, p2, p3;
//      Push stack
//      stack << p1 << p2 << p3;
//    Peek at stack
//      p3 = stack.Peek();
//      p2 = stack.Peek(1);
//      p1 = stack.Peek(2);
//    Pop stack
//      stack >> p3 >> p2 >> p1;
//    or to check on stack
//      if( stack >> p1 )
//          { error empty stack, p1 unchanged }
//    or
//      if( stack.Length() > 0 )
//          stack >> p1;
//
//
//  Ex 4: FIFO or pipe
//
//      <FIFO>PointFifo pipe;
//
//      Point p1, p2, p3;
//        
//    input to pipe
//      pipe << p1 << p2 << p3;
//    peek at pipe
//      p1 = pipe.Peek();
//      p2 = pipe.Peek(1);
//      p3 = pipe.Peek(2);
//    output from pipe
//      pipe >> p1 >> p2;
//    or to check on pipe
//      if( pipe >> p1 )
//          { error empty pipe, p1 unchanged }
//    or
//      if( pipe.BufSize() > 0 )
//          pipe >> p1;
//    Unget from pipe
//      pipe.Unget(p2);
//
//
class _Link    // For use ONLY by base class _List
{
    friend class _List;
private:
    Entry object;         // Pointer to object in list
    _Link *prev;          // previous link
    _Link *next;          // next link
    int permanent;        // Link destructor does not delete object

    _Link()    { object = 0; next = prev = this; permanent = 0;}
    _Link(Entry obj, int perm);
    
    _Link(const _Link&);             // DISALLOWED, force loader error
    void operator=(const _Link&);    // DISALLOWED
    
    ~_Link();
    
    void unlink();
    void AddBefore(_Link *p);
    void AddAfter (_Link *p);
    void Replace(Entry obj, int perm);
};


class _List            // Use ONLY as base class for derived classes
{
    friend class _ListAccess;
private:
    void Initialize();
    void Delete();
    _List(const _List& ) { Initialize(); }             // disallowed
    void operator=(const _List&) {/* exit(-1); */}     // disallowed
protected:
    _Link *first;              // first link
    _Link *link;               // current link
    _Link *save_link;          // stack of saved links
    int list_size;             // number of links in List
    int save_depth;            // depth of saved link stack

    _List() { Initialize(); }
    virtual void DeAlloc(Entry obj) = 0;    // { delete (type *) obj; }
    virtual ~_List() = 0;
        
    void PreDeLink(_Link *p)
    {
        if( !p->permanent )
            DeAlloc(p->object);
        p->object = 0;
    }
public:
    void Append(Entry, int perm = 1);     // Add new link at end of list
    void Prepend(Entry obj, int perm = 1) // Add new link at beginning of list
         { Append(obj, perm); first = first->prev; }
    void Insert(Entry, int perm = 1);     // insert before current link
    void Add(Entry, int perm = 1);        // add after current link
    void Replace(Entry, int perm = 1);    // replace current link
    void Remove(int dir = 1);             // delete current link
    inline Entry Get();                   // return object of current link
    inline Entry Next();                  // set current link to next
          // return object of new current link
    inline Entry Prev();                  // set current link to previous
                                          // return object of new current link
    Entry Get(int i, int set, int force); // general case
          // return object of (current link + i)
          // option to set current link
          // option to force past marked link
    void Transfer(_List &L);              // transfer links from L into *this
          // (concatenation), L is emptied in
          // the process.
    int InList(Entry);                    // return true if entry is in list
    void Start() { link = first;}         // set current link to first
    void End() { link = first ? first->prev : 0;} // set current link to last
    void SetFirst() { if(link) first = link;} // set first to current link
    void Reset() { link = 0;}   // set current link to null
    void Save();                // save current link on stack
    void SaveAtEnd();           // save current link on stack
    void PopSave();             // pop save stack
    void EmptySave();           // empties save stack
    void Restore();             // restore current link
    int SaveDepth() const { return save_depth;}    // depth of save stack
    void NotPermanent()   { if(link) link->permanent = 0;}
    void Permanent()      { if(link) link->permanent = 1;}
    int Dimension() const {return list_size;}   // number of links
    int IsCurrent() const {return (link != 0);} // true if current link is not 0
    int IsCurrentSaved()        // true if current link on top of stack
    {
        if(!link || !save_link)
            return 0;
        else
            return ( link == (_Link *) save_link->object ); 
    }
    int IsFirst() const            // true if current link is first
            { return ( first ? (link == first) : 0);}
    int IsLast() const            // true if current link is last
            { return ( first ? (link == first->prev) : 0);}
    void EmptyList()    { Delete(); Initialize(); }
    void Empty()    { Delete(); Initialize(); }
};

class VoidList : private _List
{
//    Basic List Class with unspecified objects
private:
    VoidList(const VoidList&) {}                         // Disallowed
    void operator=(const VoidList&) {/* exit(-1); */}    // Disallowed

//    /* Avoids warnings: hides the function _List . . . */
//    void Transfer(_List &L) { _List::Transfer(L); }
protected:
    Entry Get(int i, int set, int force) {return _List::Get(i, set, force);}
    // Protected Member Functions are only used to access _List
    // in the derived classes
    
    void Append(void *obj, int perm)    { _List::Append((Entry)obj, perm); }
    void Prepend(void *obj, int perm)   { _List::Prepend((Entry)obj, perm);}
    void Insert(void *obj, int perm)    { _List::Insert((Entry)obj, perm); }
    void Add(void *obj, int perm)       { _List::Add((Entry)obj, perm);    }
    VoidList() {}
//    virtual void DeAlloc(Entry obj) = 0;    // { delete (type *) obj; }
public:
    virtual ~VoidList() = 0; // derived function must call EmptyList()
    
    void Append(void *obj)              { _List::Append((Entry)obj); }
    void Prepend(void *obj)             { _List::Prepend((Entry)obj);}
    void Insert(void *obj)              { _List::Insert((Entry)obj); }
    void Add(void *obj)                 { _List::Add((Entry)obj);    }
    void Replace(void *obj, int perm=1) { _List::Replace((Entry)obj, perm);}
    void Transfer(VoidList &L)          { _List::Transfer(L);}
    void Remove(int dir = 1)            { _List::Remove(dir);}
    void* Get()                         { return (void*)_List::Get();}
    void* Get(int i, int force = 0)     { return (void*)_List::Get(i,0,force);}
    void* Next()                        { return (void*)_List::Next();}
    void* Next(int i, int force = 0)    { return (void*)_List::Get(i,1,force);}
    void* Prev()                        { return (void*)_List::Prev();}
    void* Prev(int i, int force = 0)    { return (void*)_List::Get(-i,1,force);}
    void* operator+(int i)              { return ((void*)_List::Get( i, 1, 0));}
    void* operator-(int i)              { return ((void*)_List::Get(-i, 1, 0));}
    void* operator++()                  { return ((void*)_List::Next());}
    void* operator--()                  { return ((void*)_List::Prev());}
    //operator void* ()                 { return ((void*)_List::Get());}
    _List::Start;
    _List::End;
    _List::SetFirst;
    _List::Save;
    _List::SaveAtEnd;
    _List::PopSave;
    _List::EmptySave;
    _List::Restore;
    _List::SaveDepth;
    _List::Dimension;
    _List::Permanent;
    _List::NotPermanent;
    _List::InList;
    _List::IsCurrent;
    _List::IsCurrentSaved;
    _List::IsFirst;
    _List::IsLast;
    _List::EmptyList;
};



//    derived class from base class List
//
template <class type> class LIST : public VoidList
{
private:
    void DeAlloc(Entry obj)              { delete (type*) obj; }
    LIST(const LIST&);                   /* Disallowed */
    void operator=(const LIST&);         /* Disallowed */
private:
    void Append(void *obj, int perm)     { VoidList::Append(obj, perm);}
    void Prepend(void *obj, int perm)    { VoidList::Prepend(obj, perm);}
    void Insert(void *obj, int perm)     { VoidList::Insert(obj, perm);}
    void Add(void *obj, int perm)        { VoidList::Add(obj, perm);}
    void Append(void *obj)               { VoidList::Append(obj); }
    void Prepend(void *obj)              { VoidList::Prepend(obj);}
    void Insert(void *obj)               { VoidList::Insert(obj); }
    void Add(void *obj)                  { VoidList::Add(obj);    }
    void Replace(void *obj, int perm=1)  { VoidList::Replace(obj, perm);}
    Entry Get(int i, int set, int force) { return VoidList::Get(i,set,force);}
    int InList(void* obj)                { return VoidList::InList(obj);}
    void* operator++()                   { return VoidList::Next();}
    void* operator--()                   { return VoidList::Prev();}
public:
    LIST() {}
    ~LIST(){ EmptyList();}
    void Append(type* obj, int perm = 1)
                { VoidList::Append((void*)obj, perm);}
    void Append(const type& obj)
                { type* t = new type; *t = obj; VoidList::Append((void*)t, 0);}
    void Prepend(type* obj, int perm = 1)
                { VoidList::Prepend((void*)obj, perm);}
    void Prepend(const type& obj)
                { type* t = new type; *t = obj; VoidList::Prepend((void*)t, 0);}
    void Insert(type* obj, int perm = 1)
                { VoidList::Insert((void*)obj, perm);}
    void Insert(const type& obj)
                { type* t = new type; *t = obj; VoidList::Insert((void*)t, 0);}
    void Add(type* obj, int perm=1)
                { VoidList::Add((void*)obj, perm);}
    void Add(const type& obj)
                { type* t = new type; *t = obj; VoidList::Add((void*)t, 0);}
    LIST& operator<<(LIST& L) { VoidList::Transfer(L); return *this; }
    void Replace(type* obj, int perm)
                { VoidList::Replace((void*)obj, perm);}
    void Replace(const type& obj)
                { type* t = new type; *t = obj; VoidList::Replace((void*)t);}
    type* Get()                      {return ((type*)VoidList::Get());}
    type* Get(int i, int force = 0)  {return ((type*)VoidList::Get(i, force));}
    type* Next()                     {return ((type*)VoidList::Next());}
    type* Next(int i, int force = 0) {return ((type*)VoidList::Next(i, force));}
    type* Prev()                     {return ((type*)VoidList::Prev());}
    type* Prev(int i, int force = 0) {return ((type*)VoidList::Prev(i, force));}
    type* operator+(int i)           {return (type*)VoidList::operator+(i);}
    type* operator-(int i)           {return (type*)VoidList::operator-(i);}
    type* operator++(int)            {return (type*)VoidList::operator++();}
    type* operator--(int)            {return (type*)VoidList::operator--();}
    operator type* ()                {return ((type*)VoidList::Get());}
    int InList(type* obj)            {return VoidList::InList((void*)obj);}
};

//    FILO (first in last out or: push down stack)
//    derived class from base class List
//
template <class type> class FILO : protected _List
{
private:
    int error;
    FILO (const FILO&);             /* Disallowed */
    void operator=(const FILO&);    /* Disallowed */
    void DeAlloc(Entry obj) { delete (type*) obj; }
public:
    FILO()  { error = 0; }
    ~FILO() { EmptyList(); }
    FILO &operator<<(type* obj) { _List::Add((Entry)obj); return *this;}
    FILO &operator<<(const type& obj)
        { type *t = new type; *t = obj; _List::Add((Entry)t, 0); return *this; }
    void Push(type* obj)        { *this << obj; }
    void Push(const type& obj)  { *this << obj; }
    FILO &operator>>(type &obj)
    {
        if( _List::IsCurrent() )
        {
            obj = *(type *)_List::Get();
            _List::Remove(0);
        }
        else
            error++;
        return *this;
    }
    FILO &operator>>(type *&obj)
    {
        if( _List::IsCurrent() )
        {
            obj = (type *)_List::Get();
            _List::Permanent();
            _List::Remove(0);
        }
        else
            error++;
        return *this;
    }
    type Pop()            { type temp; *this>>temp; return temp;}
    type* Peek()          { return (type*) _List::Get();}
    type* Peek(int i)     { return (type*) _List::Get(-i,0,0);}
    int Length()          { return _List::Dimension();}
    void Permanent()      { _List::Permanent();}
    void NotPermanent()   { _List::NotPermanent();}
    int OnStack(type* obj){ return (_List::InList((Entry)obj));}
    void Reset()          { error = 0; }
    int Status()          { return error; }
    operator int()        { if(error) { error = 0; return 1;} else return 0; }
    void EmptyStack()     { _List::EmptyList(); error = 0;}
};



//    FIFO (first in first out or pipe)
//    derived class from base class List
//
template <class type> class FIFO : private _List
{
private:
    int error;
    void DeAlloc(Entry obj)      { delete (type*) obj; }
    FIFO(const FIFO&);           /* Disallowed */
    void operator=(const FIFO&); /* Disallowed */
public:
    FIFO () { error = 0; }
    ~FIFO() { EmptyList(); }
    FIFO &operator<<(type*obj) { _List::Prepend((Entry)obj); return *this;}
    FIFO &operator<<(const type& obj)
        {type *t=new type; *t = obj; _List::Prepend((Entry)t,0); return *this;}
    FIFO &operator>>(type& obj)
    {
        if( _List::IsCurrent() )
        {
            obj = *(type *)_List::Get();
            _List::Remove(0);
        }
        else
            error++;
        return *this;
    }
    void Unget(type* obj)       { _List::Add((Entry)obj); }
    void Unget(const type& obj) { type* t=new type; *t=obj; _List::Add(t,0);}
    type* Peek()                { return (type*)_List::Get();}
    type* Peek(int i)           { return (type*) _List::Get(-i,0,0);}
    int BufSize()               { return _List::Dimension();}
    void Permanent()            { _List::Permanent();}
    void NotPermanent()         { _List::NotPermanent();}
    int InPipe(type* obj)       { return (_List::InList((Entry)obj));}
    void Reset()                { error = 0; }
    int Status()                { return error; }
    operator int()              { if(error){error=0; return 1;} else return 0;}
    void EmptyPipe()            { _List::EmptyList(); error = 0;}
};

/////////////////////////////////////////////////////////////////////////
//
//    class _Link inline member functions:
//
//////////////////////////////////////////////////////////////////////////


inline _Link::_Link(Entry obj, int perm)
{
    object = obj;
    permanent = perm;
    next = prev = this;
}
    
inline _Link::~_Link()
{
    if(!permanent && object)
        std::cerr << "Warning ~_Link(): " <<
            "object has not yet been deleted\n"<<
            "\t(too late to call proper destructor!)\n";
}
    
inline void _Link::unlink()
{
    prev->next = next;
    next->prev = prev;
}

inline void _Link::AddBefore(_Link *p)
{
    p->next = this;
    p->prev = prev;
    prev->next = p;
    prev = p;
}


inline void _Link::AddAfter(_Link *p)
{
    p->prev = this;
    p->next = next;
    next->prev = p;
    next = p;
}

inline void _Link::Replace(Entry obj, int perm)
{
    object = obj;
    permanent = perm;
}




/////////////////////////////////////////////////////////////////
//
//    class _List inline member functions:
//
/////////////////////////////////////////////////////////////////



inline Entry _List::Get()
{
//    return object of current link
//
    return link ? link->object : 0;
}

inline Entry _List::Next()
{
//    set current link to next link
//        or null if current link = last
//    return object of new current link
//
    if(link)
    {
        link = link->next;
        if( link == first )
            link = 0;
    }
    return link ? link->object : 0;
}

inline Entry _List::Prev()
{
//    set current link to previous link
//        or null if current link = first
//    return object of new current link
//
    if(link)
    {
        if( link == first)
            link = 0;
        else
            link = link->prev;
    }
    return link ? link->object : 0;
}

#endif /* _LIST_ */
