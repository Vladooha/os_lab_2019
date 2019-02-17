#include <stdlib.h>
#include <string.h>
#include <stdio.h>

#include "revert_string.h"

void RevertString(char *str)
{
    int length = strlen(str);
    
	char *temp_str = malloc(sizeof(char) * (length + 1));
	
	for (int i = 0; i < length; ++i) {
	   temp_str[i] = str[length - i - 1];    
	}
	temp_str[length] = '\0';
	
	strcpy(str, temp_str);
	free(temp_str);
}

