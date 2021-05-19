#include <stdarg.h>
#include <stddef.h>
#include <setjmp.h>
#include <string.h>
#include "cmocka.h"

#include "virtual_alloc.h"
#include "virtual_sbrk.h"
#include <stdio.h>
#include <stdlib.h>
void * virtual_heap = NULL;
void * endp = NULL;
void * brkp = NULL;

#define MAX_HEAP 10000000
void init(){

    virtual_heap = malloc(MAX_HEAP);
    brkp = virtual_heap;
    endp = (unsigned char*)brkp + MAX_HEAP;
}

int icmp(const char *a,const char *b)
{
    int i;
    for (i = 0; a[i]&&b[i]; i++) {
        if (a[i]!=b[i]) {
        break;
        }
    }
    return i+1;
}
int compare_file(char* file1,char* file2)
{
    FILE *fp1,*fp2;
    int n=0,fg=0;
    char c1[255],c2[255];
    fp1=fopen(file1,"r"); 
    fp2=fopen(file2,"r"); 
    while (!feof(fp1)&&!feof(fp2))
    {
        fgets(c1,255,fp1);
        fgets(c2,255,fp2);
        ++n;
        if (strcmp(c1,c2)) {
            fg=1;
        }
        fflush(fp1);
        fflush(fp2);
    }
    fclose(fp1);
    fclose(fp2);

    if (!fg) {
        return 0;
    }
    else if (!feof(fp1)||!feof(fp2)) {
        return 1;
    }
    else{
        return 1;
    }

}






void * virtual_sbrk(int32_t increment) {
    //Self built sbrk
    if(increment == 0){
        return (void*)brkp;
    }
    void* free = brkp;
    brkp= (unsigned char*)brkp+increment;
    if(brkp>=endp){
        return (void *)(-1);
    }
    return free;
}





static void initialize_test_0(void **state){
    //tests initallocator
    init();
    init_allocator(virtual_heap, 16, 11);
    int heapsize = ((struct heap*)virtual_heap)->heap_size;
    assert_non_null(heapsize);
    assert_int_equal(heapsize,65536);
    int blockminsize = ((struct heap*)virtual_heap)->block_min_size;
    assert_int_equal(blockminsize,2048);
    int current_block_num = ((struct heap*)virtual_heap)->current_block_num;
    assert_int_equal(current_block_num,0);
    free(virtual_heap);
    
}

static void initialize_test_1(void **state){
    //tests initallocator
    init();
    init_allocator(virtual_heap, 8, 1);
    int heapsize = ((struct heap*)virtual_heap)->heap_size;
    assert_non_null(heapsize);
    assert_int_equal(heapsize,256);
    int blockminsize = ((struct heap*)virtual_heap)->block_min_size;
    assert_int_equal(blockminsize,2);
    int current_block_num = ((struct heap*)virtual_heap)->current_block_num;
    assert_int_equal(current_block_num,0);
    free(virtual_heap);
 
}

static void malloc_tests_0(void **state){
    //tests basic function of malloc
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,1000);
    virtual_malloc(virtual_heap,1000);
    virtual_malloc(virtual_heap,1000);
    virtual_malloc(virtual_heap,1000);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/malloc_0_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}



static void malloc_tests_1(void **state){
    //tests functionality of malloc(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,3000);
    virtual_malloc(virtual_heap,2000);
    virtual_malloc(virtual_heap,3000);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    int n = compare_file("tests_temp.txt","test_file/malloc_1_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    assert_int_equal(n,0);
    free(virtual_heap);
    
}

static void malloc_test_2(void **state){
    //tests functionality of malloc(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,20000);
    virtual_malloc(virtual_heap,10000);
    virtual_malloc(virtual_heap,10000);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    int n = compare_file("tests_temp.txt","test_file/malloc_2_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    assert_int_equal(n,0);
    free(virtual_heap);
}

static void malloc_test_3(void **state){
    //error tests(negative)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,700000);

    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/malloc_3_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}

static void free_test_0(void **state){
    //tests functionality of free(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    void* a = virtual_malloc(virtual_heap,60000);
    virtual_free(virtual_heap,a);

    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/free_0_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void free_test_1(void **state){
    //tests functionality of free(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,1000);
    void* b = virtual_malloc(virtual_heap,2000);
    void* d = virtual_malloc(virtual_heap,2000);
    virtual_malloc(virtual_heap,2000);

    virtual_free(virtual_heap,b);
    virtual_free(virtual_heap,d);

    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/free_1_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void free_test_2(void **state){
    //tests functionality of free(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    void* d = virtual_malloc(virtual_heap,15000);
    void* c = virtual_malloc(virtual_heap,15000);

    virtual_free(virtual_heap,d);
    virtual_free(virtual_heap,c);

    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/free_2_correct.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);



}
static void free_test_3(void **state){
    //error case(negative)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,1000);
    void* b = malloc(1000);
    int x = virtual_free(virtual_heap,b);
    free(b);

    assert_int_equal(x,1);
 
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("test_file/free_3_correct.txt","tests_temp.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);


}
static void realloc_test_0(void **state){
    //tests functionality of realloc(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    void* a = virtual_malloc(virtual_heap,1000);
    virtual_realloc(virtual_heap,a,3000);

    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/realloc_correct_0.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void realloc_test_1(void **state){
    //tests functionality of realloc(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    void* a = virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    virtual_realloc(virtual_heap,a,20000);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/realloc_correct_1.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void realloc_test_2(void **state){
    //tests functionality of realloc(positive)
    init();
    init_allocator(virtual_heap, 16, 11);
    void* a = virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    virtual_realloc(virtual_heap,a,2000);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/realloc_correct_2.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void realloc_test_3(void **state){
    //error test(negative)
    init();
    init_allocator(virtual_heap, 16, 11);
    virtual_malloc(virtual_heap,15000);
    virtual_malloc(virtual_heap,15000);
    void* c = virtual_malloc(virtual_heap,15000);
    void* e = virtual_realloc(virtual_heap,c,200000);
    assert_null(e);
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    virtual_info(virtual_heap);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/realloc_correct_3.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);

}
static void data_test(void **state){
    //tests if we can actually store data
    init();
    init_allocator(virtual_heap, 16, 11);
    char* a = virtual_malloc(virtual_heap,15000);
    
    a = "hello";

    assert_string_equal(a,"hello");

    virtual_free(virtual_heap,a);

    char* b = virtual_malloc(virtual_heap,2000);

    b = "hello";

    assert_string_equal(b,"hello");
   
    free(virtual_heap);
    
    

}

static void edge_1(void **state){
    //edge case test
    init();
    FILE* f1 = freopen("tests_temp.txt","w",stdout);
    init_allocator(virtual_heap, 1, 11);
    fclose(f1);
    free(virtual_heap);
    int n = compare_file("tests_temp.txt","test_file/edge_1.txt");
    freopen ("/dev/tty", "a", stdout);
    
    assert_int_equal(n,0);


}
static void edge_2(void **state){
    //edge case test
    init();
    init_allocator(virtual_heap, 17, 0);

    void* a  = virtual_malloc(virtual_heap,1);
    
    assert_non_null(a);

    free(virtual_heap);


}
static void edge_3(void **state){
    //edge case test
    init();
    init_allocator(virtual_heap, 20, 0);

    void* a  = virtual_malloc(virtual_heap,1);
    assert_non_null(a);

    int x = virtual_free(virtual_heap,a);

    assert_int_equal(x,0);
    free(virtual_heap);

}


int main(void) {

      
    const struct CMUnitTest tests[] = {

        cmocka_unit_test(initialize_test_0),
        cmocka_unit_test(initialize_test_1),
        cmocka_unit_test(malloc_tests_0),
        cmocka_unit_test(malloc_tests_1),
        cmocka_unit_test(malloc_test_2),
        cmocka_unit_test(malloc_test_3),
        cmocka_unit_test(free_test_0),
        cmocka_unit_test(free_test_1),
        cmocka_unit_test(free_test_2),
        cmocka_unit_test(free_test_3),
        cmocka_unit_test(realloc_test_0),
        cmocka_unit_test(realloc_test_1),
        cmocka_unit_test(realloc_test_2),
        cmocka_unit_test(realloc_test_3),
        cmocka_unit_test(data_test),
        cmocka_unit_test(edge_1),
        cmocka_unit_test(edge_2),
        cmocka_unit_test(edge_3),

    };

// cmocka_run_group_tests 函数用于启动测试并展示测试结果，可以为测试集指定全局的 startup 和 teardown（示例中都是NULL）。
    return cmocka_run_group_tests(tests, NULL, NULL);
}
