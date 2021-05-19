#include <stddef.h>
#include <stdint.h>

void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size);

void * virtual_malloc(void * heapstart, uint32_t size);

int virtual_free(void * heapstart, void * ptr);

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size);

void virtual_info(void * heapstart);


enum{
    FREE,RED,BLUE,FREE2 /*Use 4 color to distinguish the free blocks and allocated block*//*this is accroding to "Four color theorem"*/
};

typedef unsigned char status;

struct heap{/*heap structure*/
    int32_t block_min_size;/*record how small can a block be*/
    int32_t heap_size;/*record heap size*/
    int32_t current_block_num;/*record current block num*/
    void* data_section;/*heapspace*/
    status* messages; /*message block reflects status of the heap space*/

};
