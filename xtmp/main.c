#include <stdio.h>
#include <stdlib.h>
int main(int argc, char *argv[])
{
	int a = 0;
	fprintf(stdout, "0x%p.\n", &a);
	return 0;
}
