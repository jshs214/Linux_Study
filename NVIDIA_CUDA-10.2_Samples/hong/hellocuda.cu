#include <stdio.h>

__global__ void kernel()
{
	printf("Hi GPU\n");
}

int main(int argc, char** argv)
{
	printf("---Hello CPU---\n");
	kernel<<<3,1>>>();
	cudaDeviceReset();
	printf("---BYE CPU---\n");

	return 0;
}
