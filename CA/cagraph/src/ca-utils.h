#ifndef __UTILS_H__
#define __UTILS_H__

#include <stdio.h>
#include <stdlib.h>

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
