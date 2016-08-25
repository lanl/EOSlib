//#include <StdInclude.h>
#include <iostream>
#include <ErrorHandler.h>

#include "Array.h"


_Array::_Array()
{
	offset = 0;
	n_alloc = 0;
	dimension = 0;
	off_ptr = 0;
	status = 0;
	
	for(std::size_t i = 0; i < (sizeof(int)*BitsPerChar); i++)
		block[i] = 0;
}

void _Array::ArrayInit(int n)
{
	if(n < 4)
		n = 4;
		
	offset = 0;
	dimension = 0;
	
	int n0 = n;
	int block_address;
	
	
	for(n=4, block_address=1; n < n0; )
	{
		n += n;
		block_address += 1;
	}
	n_alloc = n;
	
	off_ptr = block[0] = this->allocate(n_alloc);
	
	if(off_ptr == 0)
	{
		status = -1;
	        LocalError->Abort("_Array::ArrayInit",__FILE__, __LINE__,
		     "Insufficient Memory\n");
		return;
	}
	
	int i, k;
	
	for(i = 2, k = 1; k <= block_address; i *= 2, k++)
		block[k] = array_ptr(block[0], i);
		
	n_dynamic = k;
	status = 0;
}



_Array::~_Array()
{
  for(std::size_t k=0; k < (sizeof(int)*BitsPerChar); k++)
	{
		if(block[k])
		{
	            LocalError->Log("_Array::~_Array",
			"WARNING Cleanup from derived class is incomplete\n");
		    break;
		}
	}
}

void _Array::Cleanup()
{
  for(std::size_t k=0; k < (sizeof(int)*BitsPerChar) && block[k]; k++)
	{
	  if(k==0 || (int)k >= n_dynamic)
			deallocate(block[k]);
		
		block[k] = 0;
	}

}



int _Array::address(int i)
{
	int k;
	
	for(k = 0; i >>= 1; )
		k++;
		
	return k;
}


void *_Array::operator+=(int i)
{
	offset += i;
	if(offset >= n_alloc || offset < 0)
	{
		offset -= i;
		status = -1;
		return 0;
	}
	
	int temp = address(offset);
	off_ptr = (void *)(array_ptr(block[temp], sub_address(offset, temp)));
	status = 0;
	return get();
}



//	get() returns pointer to existing element

void* _Array::get(int i)		
{
	status = 0;
	
	i += offset;
	
	if(i >= dimension || i < 0)
		return 0;
		
	int block_address = address(i);
	
	return array_ptr(block[block_address], sub_address(i, block_address));
}


//	Xget() returns pointer to existing element and if necessary
//		adds one more element to the array

void* _Array::Xget(int i)
{
	status = 0;
	
	i += offset;
	
	if(i > dimension || i < 0)
		return 0;
	
	int block_address = address(i);
		
	if(!block[block_address])
	{
		int n = i ? 1 << block_address : 2;
		if( (block[block_address] = allocate(n)) == 0)
		{
			status = -1;
			return 0;
		}
		
		if(!n_alloc)
		{
			offset=0;
			off_ptr = block[0];
		}
		n_alloc += n;
	}
		
	void *ptr = array_ptr(block[block_address],
				sub_address(i, block_address));
	
	if(i == dimension)
		dimension++;
	
	return ptr;
}



