#include <NameArray.h>
#include <LocalIo.h>
#include <ErrorHandler.h>
#include <cstring>

_NameEntry::_NameEntry(const char *s, Entry obj) : object(obj)
{
	name = strdup(s);
}

_NameEntry::~_NameEntry()
{
	delete [] name;
	if( object )
	    LocalError->Log("_NameEntry::~_NameEntry", 
	         "Warning: _NameEntry->object can not be deleted\n");
}


void _NameEntryList::DeAlloc(Entry obj)
{
	_NameEntry *name_entry = (_NameEntry *) obj;
	parent->DeAllocate(name_entry->object);
	name_entry->object = 0;
	delete name_entry;
}

_NameArray::_NameArray(int size, int (*func)(const char*))
{
	n_entries = 0;
	Hash = func;
	
	hash_table_size = size < 1 ? 1 : size;
	SubList = new _NameEntryList[hash_table_size];
	
	if(SubList == 0)
	    LocalError->Abort("_NameArray::~_NameArray",__FILE__, __LINE__,
	         "Insufficient Memory for SubList\n");
			
	for(int i=0; i < hash_table_size; i++)
		SubList[i].parent = this;
	
	last_name = 0;
	last_entry = 0;
}

void _NameArray::SetTable(int size)
{
	if( n_entries )
	{
	    LocalError->Log("_NameArray::SetTable", 
	         "Warning, size already set\n");
	    return;
	}
	
	if( hash_table_size == size )
		return;
	
	if( size > 0 )
	{
		delete[] SubList;
		hash_table_size = size;
		SubList = new _NameEntryList[hash_table_size];
		
		if(SubList == 0)
	    	    LocalError->Abort("_NameArray::SetTable",__FILE__, __LINE__,
	                 "Insufficient memory for SubList\n");
		
		for(int i=0; i < hash_table_size; i++)
			SubList[i].parent = this;
	}
	else
	    LocalError->Log("_NameArray::SetTable", 
	         "Warning, requested size < 0, ignored\n");
}
	
void _NameArray::SetHashFunction(int (*func)(const char*))
{
	if( n_entries )
	    LocalError->Log("_NameArray::SetHashFunction", 
	         "Warning, n_entries not 0, ignored\n");
	else
		Hash = func;
}


Entry _NameArray::Get(const char *name)
{
	_NameEntryList& list = SubList[(*Hash)(name) % hash_table_size];
	
	_NameEntry* el;
	for(el = list.Start(); el; el = list.Next())
	{
		int diff = strcmp(el->Name(), name);
		
		if(diff < 0)
			break;
		
		if(diff == 0)
		{
			last_name = el->Name();
			last_entry = el->Object();
			return last_entry;
		}
	}
	
	_NameEntry *temp = new _NameEntry(name, Allocate());
	if( el )
		list.Insert(temp);
	else
		list.Append(temp);
	n_entries++;
	last_name = temp->Name();
	last_entry = temp->Object();
	return last_entry;
}

Entry _NameArray::Replace(const char *name, Entry val)
{
	_NameEntryList& list = SubList[(*Hash)(name) % hash_table_size];
	
	_NameEntry* el;
	for(el = list.Start(); el; el = list.Next())
	{
		int diff = strcmp(el->Name(), name);
		
		if(diff < 0)
			break;
		
		if(diff == 0)
		{
			Entry old = el->object;
			last_name = el->Name();
			last_entry = el->object = val;
			return old;
		}
	}
	
	_NameEntry *temp = new _NameEntry(name, val);
	if( el )
		list.Insert(temp);
	else
		list.Append(temp);
	n_entries++;
	last_name = temp->Name();
	last_entry = temp->Object();
	return 0;
}

void _NameArray::Delete(const char *name)
{
	last_name = 0;
	last_entry = 0;
	_NameEntryList& list = SubList[(*Hash)(name) % hash_table_size];
	
	for(_NameEntry* el = list.Start(); el; el = list.Next())
	{
		int diff = strcmp(el->Name(), name);
		if(diff < 0)
			break;
			
		if(diff == 0)
		{
			list.Remove();
			n_entries--;
		}
	}	
}


int _NameArray::Defined(const char *name)
{
	last_name = 0;
	last_entry = 0;
    if( name == NULL )
        return 0;
	
	_NameEntryList& list = SubList[(*Hash)(name) % hash_table_size];
	
	for(_NameEntry* el = list.Start(); el; el = list.Next())
	{
		int diff = strcmp(el->Name(), name);
		
		if(diff < 0)
			return 0;
		if(diff == 0)
		{
			last_name = el->Name();
			last_entry = el->Object();
			return 1;
		}
	}
	return 0;	
}

static int compar(const void *s1, const void *s2)
{
	return strcmp(*(const char **)s1, *(const char **)s2);
}

const char **_NameArray::Names() const
{
	if( n_entries == 0 )
		return 0;
	const char **all = new const char*[n_entries];
	int i=0;
	int j;
	for( j=0; j<hash_table_size; j++ )
	{
		_NameEntry* el;
		_NameEntryList &list = SubList[j];
		for(el = list.Start(); el; el = list.Next())
			all[i++] = el->Name();	
	}
	qsort( (void *)all, n_entries, sizeof(char *), compar );
	return all;
}


int _DefaultNameArrayHash(const char *name)
{
	int sum;
	
	for(sum = 0; *name; name++)
		sum += *name;
		
	return sum;
}

