#include<stdio.h>
#include <string.h>
#include <stdlib.h>

int main()
{
	char *str;
	str = (char *)malloc(sizeof(char)*5);
	strcpy(str, "Hello World\n");

	return 0;
}
