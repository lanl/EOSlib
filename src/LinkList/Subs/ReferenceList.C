#include <ReferenceList.h>


_RefList::_RefList(int size)
{
	hash_table_size = size <= 0 ? 1 : size;
	n_entries = n_references = 0;
}


void _RefList::EnterRef(void* obj)
{
	void* tg;
	VoidList *tags = get_tags(obj);
	
	for(tags->Start(); (tg = tags->Get()); tags->Next())
		Enter(tg, obj);

	n_entries++;
	
	delete tags;
}

void _RefList::Enter(void* tg, void* obj)
{
	VoidList& SL = SubList(hash(tg) % hash_table_size);
	
	void* e;
	for(SL.Start(); (e = SL.Get()); SL.Next())
	{
		if(obj <  e)
		{
			SL.Insert(obj);
			break;
		}
		if(obj == e)
			break;
	}
	
	if(!e)
		SL.Append(obj);
	n_references++;
}



int _RefList::Get(const void* tg, VoidList* Matches)
{
	void* ob;
	
	VoidList& SL = SubList(hash(tg) % hash_table_size);
	
	for(SL.Start(); (ob = SL.Get()); SL.Next())
	{
		if(is_tag(tg, ob))
		{
			if(!Matches->InList(ob))
				Matches->Append(ob);
		}
	}
	
	Matches->Start();
	return Matches->Dimension();
	
}

int _RefList::GetAnd( VoidList* tags, VoidList* Matches)
{
	tags->Start();
	void* tg;
	
	int n = Matches->Dimension();
	if( !n )
	{
		n = Get(tags->Get(), Matches);
		tags->Next();
	}
	
	for( ; n && (tg = tags->Get()); tags->Next() )
	{
		void* ob;
		for(Matches->Start(); (ob = Matches->Get());  )
		{
			if(!is_tag(tg, ob))
			{
				Matches->Remove();
				n--;
			}
			else
			{
				Matches->Next();
			}
		}
	}

	Matches->Start();
	return n;
}


int _RefList::GetOr( VoidList* tags, VoidList* Matches)
{
	void* tg;
	
	for(tags->Start(); (tg = tags->Get()); tags->Next())
		Get(tg, Matches);
		
	Matches->Start();	
	return Matches->Dimension();
}


int _RefList::Delete(void* obj)
{
	void *tg;
	
	VoidList* tags = get_tags(obj);
	
	int n_tags = tags->Dimension();

	for(tags->Start(); (tg = tags->Get()); tags->Next())
	{
		VoidList& SL = SubList(hash(tg) % hash_table_size);
		
		if(SL.InList(obj))
			SL.Remove();
	}
	
	n_references -= n_tags;
	n_entries--;
	
	delete tags;
	
	return n_tags;
}
