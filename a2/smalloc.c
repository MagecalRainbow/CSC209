#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <sys/types.h>
#include <sys/mman.h>
#include "smalloc.h"



void *mem;
struct block *freelist;
struct block *allocated_list;

void *smalloc_loop_helper(struct block *prev_b, struct block *cur_b, unsigned int nbytes) {
    /*Has to loop through freelist and see if there is a matching size or divide the blocks*/
    while (cur_b != NULL) {
        /*The requested block size available in freelist*/
        if (cur_b->size == nbytes) { 
            prev_b->next = cur_b->next;
            cur_b->next = allocated_list;
            allocated_list = cur_b;
            return allocated_list->addr;
        }

        /*Larger sized block available*/
        else if (cur_b->size > nbytes) {
            //Separate the large block into two.
            struct block *new_b = malloc(sizeof(struct block));
            new_b->addr = prev_b->addr;
            new_b->size = nbytes;
            new_b->next = allocated_list; 
            cur_b->addr += nbytes*sizeof(char);
            cur_b->size -= nbytes;
            allocated_list = new_b;
            return allocated_list->addr;
        } 

        /*Going into the next block, if the next of prev_b is null, the cur_b not updated*/
        prev_b = cur_b;
        if (prev_b != NULL) {
            cur_b = prev_b->next;
        }
    }
    return NULL;
}

void *smalloc(unsigned int nbytes) {
    /* Modify nbytes so that it is divisible by 8  */
    nbytes += (8 - (nbytes % 8)) % 8;

    /*Initializing the required variables to loop through*/
    struct block *prev_b = freelist; 
    struct block *cur_b = prev_b->next;

    /*The size of the first block in freelist matches the nbytes*/
    if (prev_b->size == nbytes) {
        prev_b->next = allocated_list;   
        allocated_list = prev_b; 
        freelist = cur_b;
        return allocated_list->addr;
    } 

    /*The size of the first block in freelist is greater than nbytes*/
    else if (prev_b->size > nbytes) {
        struct block *new_b = malloc(sizeof(struct block));
        new_b->addr = prev_b->addr;
        new_b->size = nbytes;
        new_b->next = allocated_list;
        allocated_list = new_b;
        prev_b->addr += nbytes * sizeof(char); 
        prev_b->size -= nbytes;
        return allocated_list->addr;
    } 

    /*The other cases, the size of the block is larger or equal to 
     * nbytes elsewhere, or not at all*/
    else {
        smalloc_loop_helper(prev_b, cur_b, nbytes);
    }
    return NULL;
}

/*Inserting as the first element in the freelist*/
int first_insert(struct block *to_be_free) {
    to_be_free->next = freelist;
    freelist = to_be_free;
    return 0;
}

/*Inserting as the last element in the freelist*/
int last_insert(struct block *to_be_free, struct block *search_block) {
    to_be_free->next = NULL;
    search_block->next = to_be_free;
    return 0;
} 

/*Inserting into the freelist at the right location.
 * If there is no block that have address lower 
 * than the block trying to insert, insert as first.*/
int freelist_insert(struct block *to_be_free) {
    struct block *search_block = freelist;

    if (search_block == NULL) {
        freelist = to_be_free;
        return 0;
    }

    /*The first block having the higher address than
     * the block trying to insert into freelist
     * the block we try to insert become the one which freelist
     * pointer points to*/
    if (to_be_free->addr < freelist->addr) {
        return first_insert(to_be_free);
    }

    while (search_block->next != NULL) {
        if ((search_block->next)->addr > to_be_free->addr) {
            to_be_free->next = search_block->next;
            search_block->next = to_be_free;
            return 0;
        }
        search_block = search_block->next;
    }

    return last_insert(to_be_free, search_block);
}

/*Uses the helper functions above. Frees the block*/
int sfree(void *addr) {
    struct block *cur_b = allocated_list;

    /* If we find the address we were looking for
     * free the block, else return negative one*/ 
    if (cur_b != NULL) {
        if (cur_b->addr == addr) {
            allocated_list = cur_b->next;
            return freelist_insert(cur_b);
        }
        while (cur_b->next != NULL) {
            if ((cur_b->next)->addr == addr) {
                struct block *to_be_free = cur_b->next;
                cur_b->next = (cur_b->next)->next;
                return freelist_insert(to_be_free);
            }   
            cur_b = cur_b->next;
        }
    }

    return -1;
}


/* Initialize the memory space used by smalloc,
 * freelist, and allocated_list
 * Note:  mmap is a system call that has a wide variety of uses.  In our
 * case we are using it to allocate a large region of memory. 
 * - mmap returns a pointer to the allocated memory
 * Arguments:
 * - NULL: a suggestion for where to place the memory. We will let the 
 *         system decide where to place the memory.
 * - PROT_READ | PROT_WRITE: we will use the memory for both reading
 *         and writing.
 * - MAP_PRIVATE | MAP_ANON: the memory is just for this process, and 
 *         is not associated with a file.
 * - -1: because this memory is not associated with a file, the file 
 *         descriptor argument is set to -1
 * - 0: only used if the address space is associated with a file.
 */
void mem_init(int size) {
    mem = mmap(NULL, size,  PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANON, -1, 0);
    if(mem == MAP_FAILED) {
         perror("mmap");
         exit(1);
    }

    /*Initiates the freelist in the beginning*/
    freelist = malloc(sizeof(struct block));
    freelist -> addr = mem;
    freelist -> size = size;
    freelist -> next = NULL;
    allocated_list = NULL;
}

/*Helper function for mem_clean().
 * Checks if it is the last element in the list and removes else recursion and remove.*/
void recursive_cleaner(struct block *block_ptr) {
    if (block_ptr->next == NULL) {
        free(block_ptr);
    } else {
        recursive_cleaner(block_ptr->next);
        free(block_ptr);
    } 
}

/*Cleans all possible memory used in both freelist and allocated_list through recursion.*/
void mem_clean(){
    if (freelist != NULL) { 
        recursive_cleaner(freelist);
    }

    if (allocated_list != NULL) {
        recursive_cleaner(allocated_list);
    }
}

