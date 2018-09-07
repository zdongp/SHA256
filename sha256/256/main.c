#include "sha256.h"
#include <stdio.h>


int main(int argc, char const *argv[])
{
	uint32 H[8];
	int i;

	HashFile(argv[1], H);

	for(i=0; i<8; i++)
	{
		printf("%08x ", H[i]);
	}
	
	printf("\n");
	return 0;
}



