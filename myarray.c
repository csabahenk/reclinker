#include <string.h>

#include "reclinker.h"

#define movetonew(arr,newp) { 					\
	arr->str = newp + (arr->str - arr->array);		\
	arr->strmid = newp + (arr->strmid - arr->array);	\
	arr->strend = newp + (arr->strend - arr->array);	\
	arr->array = newp;					\
}

int
appendtomyarray(struct myarray *arr, char *string)
{
	size_t len = strlen(string);

	if (arr->array + arr->length <  arr->strend + len + 1) {
		char *newp;
		arr->length = 2 * arr->length + len + 1;
	       	newp = (char *)REALLOC(arr->array,arr->length);
		movetonew(arr,newp);
	}
			
	arr->strend = (char *)memcpy(arr->strend,string,len) + len;
	*(arr->strend) = '\0';
	return len;
}

int
prependtomyarray(struct myarray *arr, char *string)
{
	size_t len = strlen(string);
	if (arr->str < arr->array + len + 1) {
		char *p, *q, *newp;
		newp = (char *)REALLOC(arr->array,arr->length * 2 + len + 1);
		movetonew(arr,newp);
		p = arr->strend;	
		q = arr->strend + arr->length + len + 1;
		arr->strend = q;
		while (1) {
			*q = *p;
			if(p == arr->str)
				break;
			*q--;
			*p--;
		}
		arr->strmid = q + (arr->strmid - arr->str);
		arr->str = q;
		arr->length = arr->length * 2 + len + 1;
	}		
				
	arr->str -= len;
	arr->str = memcpy(arr->str,string,len);
	return len;
}
