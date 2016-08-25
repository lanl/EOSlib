#include <List.h>

_List::~_List() {}
VoidList::~VoidList() {}


void _List::Initialize()
{
//	Create null list
//
	list_size = save_depth = 0;
	first = link = save_link = 0;
}



void _List::Delete()
{
//	Delete all links
//
	_Link* temp = first;
	
	while(list_size--)
	{
		_Link *l = temp;
		temp = temp->next;
		PreDeLink(l); delete l;
	}
	
	temp = save_link;
	while(save_depth--)
	{
		_Link *l = temp;
		temp = temp->prev;
		delete l;	// PreDeLink(l) not needed
	}
}


void _List::Append(Entry obj, int perm)
{
//	Add new link at end of list
//	current link is unchanged or for empty list set to new link
//
	_Link* temp = new _Link(obj, perm);
	
	if(!first)
		link = first = temp;	
	else
		first->AddBefore(temp);

	list_size++;
}

void _List::Insert(Entry obj, int perm)
{
//	insert new link before current link
//	if current link is first or null,
//		then Prepend and reset first to new link
//	set current link to new link
//
	if( !link || link == first )
	{
		Append(obj, perm);
		link = first = first->prev;
	}
	else
	{
		_Link *temp = new _Link(obj, perm);
		link->AddBefore(temp);
		link = temp;
		list_size++;
	}
}

void _List::Add(Entry obj, int perm)
{
//	add after current link
//	if current link null then add at end
//	set current link to new link
//
	if( !link )
	{
		Append(obj, perm);
		link = first->prev;
	}
	else
	{
		_Link *temp = new _Link(obj, perm);
		link->AddAfter(temp);
		link = temp;
		list_size++;
	}
}

void _List::Replace(Entry obj, int perm)
{
//	replace current link
//	if current link null then add at end and set current link
//
	if( !link )
	{
		Append(obj, perm);
		return;
	}
	PreDeLink(link);
	link->Replace(obj, perm);
}

void _List::Remove(int dir)
{
//	remove current link
//	reset current link according to dir
//	if dir = 1, move current link forward   (null if current link = last)
//	   dir = 0, move current link backwards (null if current link = first)
//	also if current link on save link stack set entry in stack to null
//
	if(!link)
		return;
		
	_Link* temp;
	if( dir )
		temp = (link->next == first) ? 0 : link->next;
	else
		temp = (link == first) ? 0 : link->prev;
	
		
	if(link == first)
		first = link->next;
	
	_Link *save = save_link;
	for(int i = save_depth; i; i--, save = save->prev)
	{
		if( (_Link *)save->object == link )
		{
			save->object = 0;
			break;
		}
	}
	
		
	link->unlink();
	PreDeLink(link); delete link;
		
	if(!--list_size)
		link = first = 0;
	else
		link = temp;		
}


Entry _List::Get(int i, int set, int force)
{
//	return object of (current link + i)
//	if force = 0 then return null
//		if forward past last element
//              or if backwards before first element
//	current link is not changed if set = 0
//
	if( !link )
		return 0;
	
	_Link *temp = link;
	
	if(i < 0)
	{
		while(i++)
		{
			if(force || temp != first)
				temp = temp->prev;
			else
			{
				if( set )
					link = 0;
				return 0;
			}
		}
	}
	else
	{
		while(i--)
		{
			temp = temp->next;
			if(!force && temp == first)
			{
				if( set )
					link = 0;
				return 0;
			}
		}
	}
	
	if( set )
		link = temp;
	
	return temp->object;
}

void  _List::Transfer(_List &L)
{
//	Transfer elements of list L to *this after current link (last if link=0)
//	link is set to last link in List
//	i.e. fast "concatenation", L is emptied into *this
//
	if( !L.first )
		return ;	// L is <empty>
		
	_Link *Llast = L.first->prev;	// Last link of list L
	if( !first )
		first = L.first;		// *this is <empty>
	else
	{
		if(!link)
			link = first->prev;	// when link = 0 append
		_Link *last   =  link->next;
		L.first->prev =  link;
		Llast->next   =  last;
		link->next    = L.first;
		last->prev    = Llast;
	}
	list_size += L.list_size;
	link = Llast;		// last link on list L
	//	empty list L
	_Link *temp = L.save_link;
	while(L.save_depth--)
	{
		_Link *l = temp;
		temp = temp->prev;
		PreDeLink(l); delete l;
	}
	L.list_size = L.save_depth = 0;
	L.first = L.link = L.save_link = 0;
}

int _List::InList(Entry ptr)
{
//	return true if ptr is Entry in List else false
//
	if(!first)
		return 0;
	
	_Link* temp = first;
	while( temp->object != ptr )
	{
		temp = temp->next;
		if( temp == first )
			return 0;
	}
	return 1;
}

void _List::Save()
{
//	push current link on save_link stack
//
	_Link* temp = new _Link( (Entry)link, 1);
	if( save_link )
		save_link->AddAfter(temp);
	save_link = temp;
	
	save_depth++;
}

void _List::SaveAtEnd()
{
//	push current link on save_link stack
//
	_Link* temp = new _Link( (Entry)link, 1);
	if( save_link )
		save_link->AddBefore(temp);
	else
		save_link = temp;
			
	save_depth++;
}

void _List::Restore()
{
//	set current link to top of save_link stack
//	pop save_link stack
//	if save_link stack is empty, set current link to 0
//
	if( !save_link )
	{
		link = 0;
		return;
	}
		
	link = (_Link *) save_link->object;
	
	_Link* temp = save_link;
	save_link = temp->prev;
	temp->unlink();
	delete temp;		// PreDeLink(temp) not needed
	
	if( !(--save_depth) )
		save_link = 0;
}



void _List::PopSave()
{
//	pop save_link stack
//
	if( !save_link )
		return;
			
	_Link* temp = save_link;
	save_link = temp->prev;
	temp->unlink();
	delete temp;		// PreDeLink(temp) not needed
	
	if( !(--save_depth) )
		save_link = 0;
}
	
	
void _List::EmptySave()
{
// Empties save_link stack
//
	_Link* temp = save_link;
	while(save_depth--)
	{
		_Link *l = temp;
		temp = temp->prev;
		delete l;	// PreDeLink(l) not needed
	}
	save_link = 0;
	save_depth = 0;
}
