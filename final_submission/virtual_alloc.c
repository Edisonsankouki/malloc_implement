#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <string.h>

int my_pow(int size);
/*a function to calculate pow(2,size)*/
unsigned char color_decision(int index,status* messages);
/*a functuion to decide wahat color to choose when encounter free block*/
int get_block_num(uint32_t size,uint32_t min_size);
/*a function to calculate how many blocks a size need to be allocated*/
int is_all_free(void* heap);
/*a function to check if the heap is empty*/
int is_all_allocted(void* heap);
/* a function to check if the heao is full*/
int find_space_in_message(void* heapstart,uint32_t len, uint32_t min_size,uint32_t size);
/*a fucntion to see if our current status array has any suitable space*/
int print(status* message,int cursor,int len,int block_size);
/*a fucntion to print the status message*/
void merge_free_block(void* heapstart);
/*a fucntion to merge all the free block after free operation*/
void free_color_strategy(int blocks_away,void* heapstart,unsigned char my_color);
/*a function to decide what color should we put on a free block*/
void malloc_color_start(uint32_t size,void* heapstart);
/*a fucntion to start color if the status message is empty*/
void malloc_color_free_space(uint32_t size, void* heapstart,int find_num);
/*color the free space in the existing length of status array*/


void init_allocator(void * heapstart, uint8_t initial_size, uint8_t min_size){
    /*First clear all the exsiting length of brk*/
    void* start = virtual_sbrk(0);
    if(start!=heapstart){
        int distance = (unsigned char*)start - (unsigned char*)heapstart;
        virtual_sbrk(-distance);
    }

    if(initial_size<=min_size){
        printf("initial_size must greater than min_size\n");
        return;
    }

    /*initalize heap structure*/
    virtual_sbrk(sizeof(struct heap));
    ((struct heap*)heapstart)->block_min_size = my_pow(min_size);
    ((struct heap*)heapstart)->heap_size = my_pow(initial_size);
    ((struct heap*)heapstart)->current_block_num= 0;
    ((struct heap*)heapstart)->data_section = virtual_sbrk(((struct heap*)heapstart)->heap_size);
    ((struct heap*)heapstart)->messages = virtual_sbrk(0);

  

                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                                
}

void * virtual_malloc(void * heapstart, uint32_t size){


    if(((struct heap*)heapstart)->current_block_num==0){//if we did not even start yet, just do a first step of starting coloring.
 
        int max_block_num = (((struct heap*)heapstart)->heap_size)/(((struct heap*)heapstart)->block_min_size);
        if(max_block_num<get_block_num(size,((struct heap*)heapstart)->block_min_size)){
            return NULL;
        }
        malloc_color_start(size,heapstart);
        return (unsigned char*)((struct heap*)heapstart)->data_section+0*((struct heap*)heapstart)->block_min_size;

    }
    int find_num = find_space_in_message(heapstart,((struct heap*)heapstart)->current_block_num,((struct heap*)heapstart)->block_min_size,size);
    /*this will decide if we want to expand the current array or coloring inside*/
    if(find_num!=-1){/*Suitable place exists, do color*/
        malloc_color_free_space(size,heapstart,find_num);
        return (unsigned char*)((struct heap*)heapstart)->data_section+find_num*((struct heap*)heapstart)->block_min_size;
    }
    
    else{//if the previous messages block have no place for us, just expand the message to give further blocks for it.
        int the_block_needed = get_block_num(size,((struct heap*)heapstart)->block_min_size);
        int current_len = ((struct heap*)heapstart)->current_block_num;
        
        int max_block_num = (((struct heap*)heapstart)->heap_size)/(((struct heap*)heapstart)->block_min_size);
        if(current_len+the_block_needed>max_block_num){
            return NULL;
        }
        int previous_start = 0;
        previous_start = ((struct heap*)heapstart)->current_block_num;
        virtual_sbrk(((struct heap*)heapstart)->current_block_num);
        ((struct heap*)heapstart)->current_block_num = ((struct heap*)heapstart)->current_block_num*2;

        for(int i = previous_start; i < ((struct heap*)heapstart)->current_block_num;i++){
            ((struct heap*)heapstart)->messages[i] = FREE;
        }

        while(((struct heap*)heapstart)->current_block_num/2 < the_block_needed){
        
            previous_start = ((struct heap*)heapstart)->current_block_num;
            virtual_sbrk(((struct heap*)heapstart)->current_block_num);
            ((struct heap*)heapstart)->current_block_num = ((struct heap*)heapstart)->current_block_num*2;
            int the_previous_free_color = ((struct heap*)heapstart)->messages[previous_start-1];
            if(the_previous_free_color == FREE){

                for(int i = previous_start; i < ((struct heap*)heapstart)->current_block_num;i++){
                    ((struct heap*)heapstart)->messages[i] = FREE2;
                 }

            }else{
                for(int i = previous_start; i < ((struct heap*)heapstart)->current_block_num;i++){
                    ((struct heap*)heapstart)->messages[i] = FREE;
                 }
            }

        }

        if(is_all_free(heapstart)==0 && ((struct heap*)heapstart)->current_block_num/2 == the_block_needed){
            for(int i = 0; i< ((struct heap*)heapstart)->current_block_num/2;i++){
                ((struct heap*)heapstart)->messages[i] = RED;
            }
            return (unsigned char*)((struct heap*)heapstart)->data_section + 0;
        } 

        if(((struct heap*)heapstart)->messages[previous_start-1]==RED){
            int k = previous_start;
            int j = ((struct heap*)heapstart)->current_block_num;
            int start_color = ((struct heap*)heapstart)->messages[previous_start];
           
            while(j-k>the_block_needed){
                
                int old_j = j;
                j = (j+k)/2;

                if(old_j==((struct heap*)heapstart)->current_block_num){
                    continue;
                }else{
                    if(start_color==FREE){
                        
                        for(int count = j;count < old_j;count++){
                            ((struct heap*)heapstart)->messages[count]=FREE2;
                        }
                        start_color = FREE2;
                    }else{
                        if(start_color==FREE2){
                            for(int count = j;count < old_j;count++){
                                ((struct heap*)heapstart)->messages[count]=FREE;
                            }
                        start_color=FREE;

                    }
                }

            }
            
           
        }

       

            for(int i = previous_start; i < previous_start+the_block_needed;i++){
                    
                ((struct heap*)heapstart)->messages[i] = BLUE;
                }
            }
        else{
            int k = previous_start;
            int j = ((struct heap*)heapstart)->current_block_num;
            int start_color = ((struct heap*)heapstart)->messages[previous_start];
            while(j-k>the_block_needed){
                int old_j = j;
                j = (j+k)/2;

                if(old_j==((struct heap*)heapstart)->current_block_num){
                    continue;
                }else{
                    if(start_color==FREE){
                        for(int count = j;count < old_j;count++){
                            ((struct heap*)heapstart)->messages[count]=FREE2;
                        }
                        start_color = FREE2;
                    }else{
                        if(start_color==FREE2){
                            for(int count = j;count < old_j;count++){
                                ((struct heap*)heapstart)->messages[count]=FREE;
                            }
                        start_color=FREE;

                    }
                }

            }
            }

        
            int the_col;

            the_col = color_decision(previous_start, ((struct heap*)heapstart)->messages);

            if(((struct heap*)heapstart)->messages[previous_start-1]==BLUE){
                the_col=RED;
            }
            
            for(int i = previous_start; i < previous_start+the_block_needed;i++){
                    ((struct heap*)heapstart)->messages[i] = the_col;
                 }
                 
        }


        return (unsigned char*)((struct heap*)heapstart)->data_section+previous_start*((struct heap*)heapstart)->block_min_size;
    
    }
   
}

int virtual_free(void * heapstart, void * ptr) {

    int distance = (unsigned char*)ptr - (unsigned char*)((struct heap*)heapstart)->data_section;
    int blocks_away = distance/((struct heap*)heapstart)->block_min_size;// count where is the ptr in our status message.
    float x = (float)blocks_away;
    /*error check*/
    if(blocks_away-x !=0){
        return 1;
    }
    if(blocks_away>=((struct heap*)heapstart)->current_block_num){
        return 1;
    }
    if(blocks_away<0){
        return 1;
    }
    unsigned char my_color = ((struct heap*)heapstart)->messages[blocks_away];//current color if it is already free, do nothing and return 1.

    if(my_color==FREE || my_color == FREE2){
        return 1;
    }
    free_color_strategy(blocks_away,heapstart,my_color);
    merge_free_block(heapstart);


   
    return 0;
}

void * virtual_realloc(void * heapstart, void * ptr, uint32_t size) {


    int distance = (unsigned char*)ptr - (unsigned char*)((struct heap*)heapstart)->data_section;
    int blocks_away = distance/((struct heap*)heapstart)->block_min_size;
    int i = blocks_away;// locate pointer in the status 

    int the_block_current = 0;

    unsigned char my_color = ((struct heap*)heapstart)->messages[blocks_away];// record what the current color is 

    while(((struct heap*)heapstart)->messages[i]==my_color){
        the_block_current+=1;
        i+=1;
    }// store the size of the blcoks need to be reallocated.

    int original_size = the_block_current*((struct heap*)heapstart)->block_min_size;

    int check = virtual_free(heapstart, ptr);// free the block.

    if(check == -1){//if free can succeed
        return NULL;
    }
    void* x = virtual_malloc(heapstart,size);

    if(x==NULL){// if malloc fail restore the block
        for(int i = blocks_away;i < blocks_away+the_block_current;i++){
            
            ((struct heap*)heapstart)->messages[i] = my_color;
        }
        return NULL;
    }

    memcpy(x,ptr,original_size);//move data

    
    return x;
}

void virtual_info(void * heapstart) {

    status* m = ((struct heap*)heapstart)->messages;
    int cursor = 0;
    
    int free_num = is_all_free(heapstart);

    if(free_num==0){
        printf("free %d\n",((struct heap*)heapstart)->heap_size);
        return;
    }
    while(cursor!=-1){
      
        int x = print(m,cursor,((struct heap*)heapstart)->current_block_num,((struct heap*)heapstart)->block_min_size);

        cursor = x;
    }

    int my_free_half = ((struct heap*)heapstart)->current_block_num*((struct heap*)heapstart)->block_min_size;
    if(((struct heap*)heapstart)->current_block_num==0){
        printf("free %d\n",((struct heap*)heapstart)->heap_size);
        return;
    }
    while(my_free_half<((struct heap*)heapstart)->heap_size){
        printf("free %d\n",my_free_half);
        my_free_half = my_free_half*2;
    }

    
    
}





/*tool like  functions*/
int my_pow(int size){
    int start = 2;
    if(size ==0){
        return 1;
    }
    while(size!=1){
        start = start << 1;
        size-=1;

    }
    return start;
}
unsigned char color_decision(int index,status* messages){
    while(messages[index]== FREE || messages[index]==FREE2){
        index--;
    }
    return messages[index];
}
int get_block_num(uint32_t size,uint32_t min_size){
    int block_num = 1;
    while(block_num*min_size<size){
        block_num = block_num*2;
    }
    return block_num;
}
int is_all_free(void* heap){
    for(int i = 0; i < ((struct heap*)heap)->current_block_num;i++){
        if(((struct heap*)heap)->messages[i]==RED || ((struct heap*)heap)->messages[i]==BLUE){
            return 1;
            
        }
    }

    return 0;
}

int is_all_allocted(void* heap){
    for(int i = 0; i < ((struct heap*)heap)->current_block_num;i++){
        if(((struct heap*)heap)->messages[i]==FREE || ((struct heap*)heap)->messages[i]==FREE2){

            return 1;
            
        }
    }
    return 0;

}

int find_space_in_message(void* heapstart,uint32_t len, uint32_t min_size,uint32_t size){
    int required_block_num = get_block_num(size,min_size);
    for(int i = 0; i < len;i++){
        if(((struct heap*)heapstart)->messages[i]==FREE || ((struct heap*)heapstart)->messages[i]==FREE2){
            unsigned char current_color = ((struct heap*)heapstart)->messages[i];
            int k = i;
            while(((struct heap*)heapstart)->messages[k]==current_color && k < ((struct heap*)heapstart)->current_block_num ){
                k+=1;
            }
            

            if((k-i) == required_block_num){
                return i;
            }
            i = k-1;

        }
    }

    
    for(int i = 0; i < len;i++){
      
        if(((struct heap*)heapstart)->messages[i]==FREE || ((struct heap*)heapstart)->messages[i]==FREE2){
            unsigned char current_color = ((struct heap*)heapstart)->messages[i];
            int k = i;
            while(((struct heap*)heapstart)->messages[k]==current_color&& k < ((struct heap*)heapstart)->current_block_num ){
                k+=1;
            }
            int mark = k-i;

            while((k-i) > required_block_num){
                int old_k = k;
                k = (k+i)/2;

                int right_color;
                if(old_k==((struct heap*)heapstart)->current_block_num){
                    right_color = ((struct heap*)heapstart)->messages[old_k-1];
                }else{
                    right_color = ((struct heap*)heapstart)->messages[old_k];
                }   
                
                if(old_k-i == mark){
                    continue;
                }else{
                    if(right_color==FREE){
                        
                        for(int count = k;count < old_k;count++){
                            ((struct heap*)heapstart)->messages[count] = FREE2;
                        }
                        right_color = FREE;
                    }else{
                       
                        for(int count = k;count < old_k;count++){
                            ((struct heap*)heapstart)->messages[count] = FREE;
                        }

                        right_color = FREE2;
                    }
                      
                } 
              
            }
            if(mark != k-i){
                return i;
            }
            i = k;
            
        }
    }
    return -1;

}


int print(status* message,int cursor,int len,int block_size){
    if(cursor>=len){
        return -1;
    }
    unsigned char current_color = message[cursor];
    int old_cursor = cursor;
    
    

    while(message[cursor]==current_color&&cursor<len){
        cursor+=1;
    }
    
    if(current_color==FREE || current_color == FREE2){
        printf("free %d\n",block_size*(cursor-old_cursor));
    }else{
        int total_space = (block_size*(cursor-old_cursor));
        printf("allocated %d\n",total_space);
    }

    return cursor;

}

void merge_free_block(void* heapstart){
    for(int i = 0; i <((struct heap*)heapstart)->current_block_num;i++){
        int current_size = 0;
        int previous_size = 0;
        int k = i;
        while((((struct heap*)heapstart)->messages[k]==FREE || ((struct heap*)heapstart)->messages[k]==FREE2)&&(k < ((struct heap*)heapstart)->current_block_num)){
            current_size+=1;

         
        
            if(((struct heap*)heapstart)->messages[k]==((struct heap*)heapstart)->messages[k+1] && (k < (((struct heap*)heapstart)->current_block_num)-1)){
               k++;
               continue;
            }else{
                if(previous_size==current_size){
                   
                    
                    int start = k+1-2*(current_size);
                    unsigned char col=((struct heap*)heapstart)->messages[k];
        
                    for(int a = start; a < k; a++){
                        ((struct heap*)heapstart)->messages[a] = col;
                    }
                    if(start>0){
                        if(((struct heap*)heapstart)->messages[start-1]==((struct heap*)heapstart)->messages[start]){
                            for(int a = i; a < start; a++){
                                if(((struct heap*)heapstart)->messages[start]==FREE){
                                    ((struct heap*)heapstart)->messages[a] = FREE2;
                                }
                                else if(((struct heap*)heapstart)->messages[start]==FREE2){
                                    ((struct heap*)heapstart)->messages[a] = FREE;
                                }
                            }
                        }
                    }
                    k = i;
                    current_size=0;
                    previous_size=0;
                    continue;
                
                }
                previous_size=current_size;
                current_size=0;
                k++;
            }
        }
        i=k;
    }

}

void free_color_strategy(int blocks_away,void* heapstart,unsigned char my_color){
    int i = blocks_away;
    unsigned char previous_col;
    unsigned char next_col;
    if(i == 0){
        previous_col = RED;
    }else{
        previous_col = ((struct heap*)heapstart)->messages[blocks_away-1];
    }

    int k = i;
    while(((struct heap*)heapstart)->messages[k]==my_color && k < ((struct heap*)heapstart)->current_block_num){
        k+=1;
    }
    next_col = ((struct heap*)heapstart)->messages[k];
    unsigned char the_color;
    if(my_color==RED){
        the_color=FREE2;
    }
    if(my_color==BLUE){
        the_color=FREE;
    }
    
    if((next_col==FREE || previous_col==FREE)){
        
        the_color = FREE2;
    }
    if((next_col==FREE2 || previous_col==FREE2)){
        the_color = FREE;
    }

    while(((struct heap*)heapstart)->messages[i]==my_color && i <((struct heap*)heapstart)->current_block_num){
        if(my_color==RED){
            ((struct heap*)heapstart)->messages[i++]=the_color;
        }else{
            ((struct heap*)heapstart)->messages[i++]=the_color;
            
        }
        
    }
}

void malloc_color_start(uint32_t size,void* heapstart){
      int add_block = get_block_num(size,((struct heap*)heapstart)->block_min_size);

        if((add_block*((struct heap*)heapstart)->block_min_size)==(((struct heap*)heapstart)->heap_size)){
            ((struct heap*)heapstart)->current_block_num = add_block*2;
        }else{
            ((struct heap*)heapstart)->current_block_num = add_block*2;

        }

        virtual_sbrk(((struct heap*)heapstart)->current_block_num);
        int a = 0; 
        
        for(int i= 0;i <((struct heap*)heapstart)->current_block_num/2;i++){
         
            ((struct heap*)heapstart)->messages[i] = RED;
            a++;
        }

        for(int k = a; k < ((struct heap*)heapstart)->current_block_num;k++){
            ((struct heap*)heapstart)->messages[k] = FREE;
        }

        if((add_block*((struct heap*)heapstart)->block_min_size)==(((struct heap*)heapstart)->heap_size)){
            virtual_sbrk(-((struct heap*)heapstart)->current_block_num/2);
            ((struct heap*)heapstart)->current_block_num-=((struct heap*)heapstart)->current_block_num/2;
        }

}

void malloc_color_free_space(uint32_t size, void* heapstart,int find_num){
        int block_num = get_block_num(size,((struct heap*)heapstart)->block_min_size);
        unsigned char previous_color = ((struct heap*)heapstart)->messages[find_num-1];
        unsigned char after_color = ((struct heap*)heapstart)->messages[find_num+block_num];


        if((previous_color==RED&&after_color==BLUE)||(previous_color==BLUE&&after_color==RED)){
                for(int i = find_num-1;i >=0;i--){
                    if(((struct heap*)heapstart)->messages[i]==RED){
                        ((struct heap*)heapstart)->messages[i]=BLUE;
                    }
                    if(((struct heap*)heapstart)->messages[i]==BLUE){
                        ((struct heap*)heapstart)->messages[i]=RED;
                    }
                }
                previous_color = ((struct heap*)heapstart)->messages[find_num-1];
        }
        

        if(find_num==0){
            previous_color = BLUE;
        }
        
        unsigned char color;

        if(previous_color==RED){
            color=BLUE;
        }else{
            if(previous_color==BLUE){
                color = RED;
            }else{
                unsigned char col = color_decision(previous_color, ((struct heap*)heapstart)->messages);
                color = col;
            }
                
        }
        for(int i = find_num; i < find_num+block_num;i++){
            ((struct heap*)heapstart)->messages[i]=color;
        }
}
