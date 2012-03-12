#ifndef __CA__UTIL__H__
#define __CA__UTIL__H__

#include <unistd.h>
#include <stdio.h>

#define   MALLOC(var,size)      \
            (var) = malloc(size);    \
       	    if(!(var))               \
	            fprintf(stderr, "%s:%d: out of memory\n", __FILE__, __LINE__), \
	            exit(EXIT_FAILURE); \

#define   ASSERT(x) \
            if(!(x)) \
              fprintf(stderr, "%s:%d: ASSERT failed\n", __FILE__, __LINE__), \
	          exit(EXIT_FAILURE);

#endif 
