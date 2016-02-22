#include <iostream>
#include <Array.h>

int main(int, char**)
{
	ARRAY<int> Nums(12);		// Twelve is the wrong guess but
					            // it should work anyhow
	int i;
	
	for(i=0; i < 257; i++)
		Nums[i] = i;
		
	Nums+=30;
	
	for(i=-5; i <= 5; i++)
		std::cout << "Nums("<<i<<") = "<<Nums[i] << "\n";
}
