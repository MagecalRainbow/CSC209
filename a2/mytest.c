#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"


#define SIZE 4096 * 64


/* Simple test for smalloc and sfree. */

int main(void) {

    mem_init(100);
    
    char *ptrs_temp[2];

    /*Testing mem_init, allocation, free with initiating other than SIZE*/
    ptrs_temp[0] = smalloc(28);
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();

	/*Testing, successful free*/
    printf("freeing %p result = %d\n", ptrs_temp[0], sfree(ptrs_temp[0]));
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
	
    
	/*Testing unsuccessful memory allocation*/
    ptrs_temp[1] = smalloc(100);
    if (ptrs_temp[1] == NULL) {
		printf("Memory allocation unsuccessful. smalloc returned null.\n");	
		printf("Intended test successful.\n");
    } else {
		return 1;
	}

	/*Testing unsuccessful free*/
	int free_result_2 = sfree(ptrs_temp[1]);
	if (free_result_2 == -1) {
		printf("Memory free unsuccessful. sfree returned -1.\n"); 
		printf("Intended test successful.\n");
	} else {
		return 1;
	}
	
	mem_clean();
    
	mem_init(SIZE);
	
	/*Testing by adding more blocks than simpletest*/ 
	char *ptrs[20];
    int i;

    for(i = 0; i < 15; i++) {
        int num_bytes = (i+1) * 10;
    
        ptrs[i] = smalloc(num_bytes);
        write_to_mem(num_bytes, ptrs[i], i);
    }

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();
    
	printf("====Freeing the first block in the allocated list====\n");
	/*Freeing the first block in the allocated list*/
    printf("freeing %p result = %d\n", ptrs[0], sfree(ptrs[0]));
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

	printf("====Freeing the last block in the allocated list====\n");
	/*Freeing the last block in the allocated list*/
    printf("freeing %p result = %d\n", ptrs[14], sfree(ptrs[14]));
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

	printf("====Freeing one of the block in the allocated list====\n");
	/*Freeing one of the elements in the middle*/
    printf("freeing %p result = %d\n", ptrs[10], sfree(ptrs[10]));
    
    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();


	/*Reallocating one of the block*/
	printf("====Reallocating one of the freeblock====\n");
	ptrs[0] = smalloc(16);

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();

	/*Reallocating smaller mount than available block*/
	printf("====Reallocating one of the freeblock====\n");
	ptrs[10] = smalloc(48);

    printf("List of allocated blocks:\n");
    print_allocated();
    printf("List of free blocks:\n");
    print_free();
    printf("Contents of allocated memory:\n");
    print_mem();


    mem_clean();

    return 0;
}
