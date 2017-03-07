//	************************************************************************
//	**************************   WARNING   *********************************
//	************************************************************************
//	***	You are opening a Pandora's Box.			                     ***
//	***	These classes circumvent all protections of List.h	             ***
//	***	This is the last resort of the incompetent.		                 ***
//	***	There must be a better way to do what is needed!	             ***
//	************************************************************************
//	**************************   WARNING   *********************************
//	************************************************************************

#ifndef _LIST_ACCESS_
#define _LIST_ACCESS_

#include <List.h>

class _ListAccess
{
protected:
	void Initialize(_List* lst)	{ lst->Initialize();}
	void Delete(_List* lst)		{ lst->Delete(); }
	int ListSize(_List* lst)	{ return lst->list_size; }
	int SaveDepth(_List* lst)	{ return lst->save_depth; }
	
	void Append(_List* lst, Entry o, int perm=1)  { lst->Append(o, perm); }
	void Prepend(_List* lst, Entry o, int perm=1) { lst->Prepend(o, perm); }
	void Insert(_List* lst, Entry o, int perm=1)  { lst->Insert(o, perm); }
	void Add(_List* lst, Entry o, int perm=1)     { lst->Add(o, perm); }
	void Replace(_List* lst, Entry o, int perm=1) { lst->Replace(o, perm); }
	void Remove(_List* lst, int dir=1)            { lst->Remove(dir); }
					
	Entry Get(_List* lst)		{ return lst->Get(); }
	Entry Get(_List* lst, int i, int set, int force)
					{ return lst->Get(i, set, force); }
	int InList(_List* lst, Entry o)	{ return lst->InList(o); }
	void Start(_List* lst)		{ lst->Start(); }
	void End(_List* lst)		{ lst->End(); }
	void SetFirst(_List* lst)	{ lst->SetFirst(); }
	void Reset(_List* lst)		{ lst->Reset(); }
	void Save(_List* lst)		{ lst->Save(); }
	void Restore(_List* lst)	{ lst->Restore(); }
	void NotPermanent(_List* lst)	{ lst->NotPermanent(); }
	void Permanent(_List* lst)	{ lst->Permanent(); }
	int Dimension(_List* lst)	{ return lst->Dimension(); }
	int IsCurrent(_List* lst)	{ return lst->IsCurrent(); }
	int IsFirst(_List* lst)		{ return lst->IsFirst(); }
	void Empty(_List* lst)		{ lst->Empty(); }
};

template <class type> class LISTMANIP : private _ListAccess
{
private:
	_List* Lptr;
public:
	LISTMANIP(LIST<type>& L)	     { Lptr = (_List*)&L; }

	void Initialize()	             { _ListAccess::Initialize(Lptr);}
	void Delete()	  	             { _ListAccess::Delete(Lptr); }
	int ListSize()		             { return _ListAccess::ListSize(Lptr); }
	int SaveDepth()		             { return _ListAccess::SaveDepth(Lptr);}

	void Append(Entry o, int perm=1) { _ListAccess::Append(Lptr, o, perm); }
	void Prepend(Entry o, int perm=1){ _ListAccess::Prepend(Lptr, o, perm); }
	void Insert(Entry o, int perm=1) { _ListAccess::Insert(Lptr,o, perm); }
	void Add(Entry o, int perm=1)    { _ListAccess::Add(Lptr, o, perm); }
	void Replace(Entry o, int perm=1){ _ListAccess::Replace(Lptr, o, perm); }
	void Remove(int dir=1)           { _ListAccess::Remove(Lptr, dir); }

	type* Get()	                     { return (type*)_ListAccess::Get(Lptr); }
	type* Get(int i, int set, int force)
	    { return (type *)_ListAccess::Get(Lptr, i, set, force); }
	int InList(Entry o)              { return _ListAccess::InList(Lptr, o); }
	void Start()		             { _ListAccess::Start(Lptr); }
	void End()		                 { _ListAccess::End(Lptr); }
	void SetFirst()	                 { _ListAccess::SetFirst(Lptr); }
	void Reset()		             { _ListAccess::Reset(Lptr); }
	void Save()		                 { _ListAccess::Save(Lptr); }
	void Restore()	                 { _ListAccess::Restore(Lptr); }
	void NotPermanent()	             { _ListAccess::NotPermanent(Lptr); }
	void Permanent()	             { _ListAccess::Permanent(Lptr); }
	int Dimension()	                 { return _ListAccess::Dimension(Lptr); }
	int IsCurrent()	                 { return _ListAccess::IsCurrent(Lptr); }
	int IsFirst()		             { return _ListAccess::IsFirst(Lptr); }
	void Empty()		             { _ListAccess::Empty(Lptr); }
};


template <class type> class FILOMANIP : private _ListAccess
{
private:
	_List* Lptr;
public:
	FILOMANIP(LIST<type>& L)	      { Lptr = (_List*)&L; }

	void Initialize()	              { _ListAccess::Initialize(Lptr);}
	void Delete()		              { _ListAccess::Delete(Lptr); }
	int ListSize()		              { return _ListAccess::ListSize(Lptr); }
	int SaveDepth()		              { return _ListAccess::SaveDepth(Lptr);}

	void Append(Entry o, int perm=1)  { _ListAccess::Append(Lptr, o, perm); }
	void Prepend(Entry o, int perm=1) { _ListAccess::Prepend(Lptr, o, perm); }
	void Insert(Entry o, int perm=1)  { _ListAccess::Insert(Lptr,o, perm); }
	void Add(Entry o, int perm=1)     { _ListAccess::Add(Lptr, o, perm); }
	void Replace(Entry o, int perm=1) { _ListAccess::Replace(Lptr, o, perm); }
	void Remove(int dir=1)            { _ListAccess::Remove(Lptr, dir); }

	type* Get()	                      { return (type*)_ListAccess::Get(Lptr); }
	type* Get(int i, int set, int force)
	    { return (type *)_ListAccess::Get(Lptr, i, set, force); }
	int InList(Entry o)	              { return _ListAccess::InList(Lptr, o); }
	void Start()		              { _ListAccess::Start(Lptr); }
	void End()		                  { _ListAccess::End(Lptr); }
	void SetFirst()	                  { _ListAccess::SetFirst(Lptr); }
	void Reset()		              { _ListAccess::Reset(Lptr); }
	void Save()		                  { _ListAccess::Save(Lptr); }
	void Restore()	                  { _ListAccess::Restore(Lptr); }
	void NotPermanent()               { _ListAccess::NotPermanent(Lptr); }
	void Permanent()	              { _ListAccess::Permanent(Lptr); }
	int Dimension()		              { return _ListAccess::Dimension(Lptr); }
	int IsCurrent()	                  { return _ListAccess::IsCurrent(Lptr); }
	int IsFirst()		              { return _ListAccess::IsFirst(Lptr); }
	void Empty()		              { _ListAccess::Empty(Lptr); }
};

template <class type> class FIFOMANIP : private _ListAccess
{
private:
	_List* Lptr;
public:
	FIFOMANIP(LIST<type>& L)	      { Lptr = (_List*)&L; }

	void Initialize()	              { _ListAccess::Initialize(Lptr);}
	void Delete()		              { _ListAccess::Delete(Lptr); }
	int ListSize()		              { return _ListAccess::ListSize(Lptr); }
	int SaveDepth()		              { return _ListAccess::SaveDepth(Lptr);}

	void Append(Entry o, int perm=1)  { _ListAccess::Append(Lptr, o, perm); }
	void Prepend(Entry o, int perm=1) { _ListAccess::Prepend(Lptr, o, perm); }
	void Insert(Entry o, int perm=1)  { _ListAccess::Insert(Lptr,o, perm); }
	void Add(Entry o, int perm=1)     { _ListAccess::Add(Lptr, o, perm); }
	void Replace(Entry o, int perm=1) { _ListAccess::Replace(Lptr, o, perm); }
	void Remove(int dir=1)            { _ListAccess::Remove(Lptr, dir); }

	type* Get()	                      { return (type*)_ListAccess::Get(Lptr); }
	type* Get(int i, int set, int force)
	    { return (type *)_ListAccess::Get(Lptr, i, set, force); }
	int InList(Entry o)	              { return _ListAccess::InList(Lptr, o); }
	void Start()		              { _ListAccess::Start(Lptr); }
	void End()		                  { _ListAccess::End(Lptr); }
	void SetFirst()	                  { _ListAccess::SetFirst(Lptr); }
	void Reset()		              { _ListAccess::Reset(Lptr); }
	void Save()		                  { _ListAccess::Save(Lptr); }
	void Restore()	                  { _ListAccess::Restore(Lptr); }
	void NotPermanent()	              { _ListAccess::NotPermanent(Lptr); }
	void Permanent()	              { _ListAccess::Permanent(Lptr); }
	int Dimension()	                  { return _ListAccess::Dimension(Lptr); }
	int IsCurrent()	                  { return _ListAccess::IsCurrent(Lptr); }
	int IsFirst()		              { return _ListAccess::IsFirst(Lptr); }
	void Empty()		              { _ListAccess::Empty(Lptr); }
};

#endif /* _LIST_ACCESS_ */
