#include <stdio.h>
extern int test_fun(void *param);
extern int var_from_asm;
int global_init = 10;
int global;
static int static_init = 1000;
static int static_var;
char *string = "test string";
int main() {
    int value = 666;
    test_fun(&value);
    printf("var from asm:%d\n", var_from_asm);
    printf("assign from asm:%d\n", global);
    printf("value passed by stack:%d\n", global_init);
    return 0;
}