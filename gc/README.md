Simple mark and sweep garbage collector for C
=============================================

Example:
```c
#include "gc.h"

typedef struct _MyType MyType;

struct _MyType
{
    int dummy1;
    int dummy2;

    MyType *ref1;
    MyType *ref2;
};

static MyType* my_type_new()
{
    return g_malloc(sizeof(MyType));
}

static void my_type_free(MyType *instance)
{
    g_free(instance);
}

GC_REGISTER(MyType, my_type_new, my_type_free, GC_REF(ref1), GC_REF(ref2))

static void my_function()
{
	GC_AUTO(MyType) *my_instance1 = GC_NEW(MyType);
    GC_AUTO(MyType) *my_instance2 = GC_NEW(MyType);

    my_instance1->ref1 = my_instance1;
    my_instance1->ref2 = my_instance2;
    my_instance2->ref1 = my_instance1;
    my_instance2->ref2 = NULL;
}

int main()
{
    my_function();
    gc_collect();
    return 0;
}
```
