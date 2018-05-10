
#include <stdio.h>
#include <stdlib.h>
#include "Maxfiles.h"
#include <MaxSLiCInterface.h>

int main()
{
	const int vectorSize = Vectors_vectorSize;
	//printf("%d\n",Vectors_vectorSize);
	const int streamSize = 4;
	size_t sizeBytes = vectorSize * streamSize * sizeof(int);
	float *inVector = malloc(sizeBytes);
	float *inVector2 = malloc(sizeBytes);
	float *out = malloc(streamSize*sizeof(int));

	for (int i = 0; i < vectorSize * streamSize; i++) {
		inVector[i] = i%100;
		inVector2[i] = 2*i+2;
	}

	printf("Running DFE.\n");
	Vectors(streamSize, inVector, inVector2, out);

    for (int i = 0; i < streamSize; i++)
    printf("r=%f\n",out[i]);
	

	return 0;
}
