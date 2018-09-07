#include "HashList3.h"

int main(int argc, char const *argv[])
{
	HashTable *ht = new HashTable();
	const char* key[]={"a","b"};
	const char* value[]={"value1","value2"};
	for (int i = 0; i < 2; ++i)
	{
		ht->install(key[i],value[i]);
	}
	ht->display();
	return 0;
}