#include "swap.h"
#include "stdlib.h"

void Swap(char *left, char *right)
{
	// ваш код здесь
	char *temp = malloc(sizeof(char));
	
	*temp = *left;
    *left = *right;
    *right = *temp;

    free(temp);
}
