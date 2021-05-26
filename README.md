# malloc_implement
assignment3
## how you are maintaining the state of the allocation and your memory layout？
The information of the heap is represented as a dynamic array which is called the message in the end of the heap. This array is dynamic array like tree which can be modified by the sbrk function.
And the array is filled with unsigned char which are to distinguish between the allocated and unallocated block and also between each allocated block and free block.

## how virtual_malloc and/or virtual_free and/or virtual_realloc change your state and memory layout
when malloc if the heap is empty, it will get a closest size array for the size you want and perform body allocation algorithm within it to allocate a suitable place for it. if the
heap is not empty, we will first look at the existing message array and find a suitable place for it. if the current size message array can not fit the size you want, we interatively double the
size by sbrk until you find it is enough to store. The allocate is to filled the message array with a certain color enum and return the address by counting offset of smallest block.
when free happends we first set the area to free and then perform merge allocation with its adjacent free block iteratively. 
The reallocation is just free first and then malloc.

## the overhead and space efficiency of how you are maintaining state
This method is basicly an array tree like structure for storing heap status. The difference is the tree is just partly allocated. We acctually start with a subtree in the whole tree and it will
keep grow double if the space is not enough. As a result, the amortized space usage will be half less than the static whole tree allocation. And since evenry min block takes only 1 byte as
in the message array, the worst case space usage is low. By the way, using byte operation can be 4 times smaller than the current design since we only have 4 kinds of enum you can use 2 bits to 
represent 1 kind of enum (Well, you can try if you think making your heap shrink is cool.
