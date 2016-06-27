/*
 * Copyright (c) 2016, Martin Jaros <xjaros32@stud.feec.vutbr.cz>
 *
 * Permission to use, copy, modify, and/or distribute this software for any
 * purpose with or without fee is hereby granted, provided that the above
 * copyright notice and this permission notice appear in all copies.
 *
 * THE SOFTWARE IS PROVIDED "AS IS" AND THE AUTHOR DISCLAIMS ALL WARRANTIES
 * WITH REGARD TO THIS SOFTWARE INCLUDING ALL IMPLIED WARRANTIES OF
 * MERCHANTABILITY AND FITNESS. IN NO EVENT SHALL THE AUTHOR BE LIABLE FOR
 * ANY SPECIAL, DIRECT, INDIRECT, OR CONSEQUENTIAL DAMAGES OR ANY DAMAGES
 * WHATSOEVER RESULTING FROM LOSS OF USE, DATA OR PROFITS, WHETHER IN AN
 * ACTION OF CONTRACT, NEGLIGENCE OR OTHER TORTIOUS ACTION, ARISING OUT OF
 * OR IN CONNECTION WITH THE USE OR PERFORMANCE OF THIS SOFTWARE.
 */

#include "gc.h"

typedef struct _Test Test;

struct _Test
{
    int dummy1;
    int dummy2;

    Test *ptr;
};

static Test* test_new()
{
    g_print("test_new()\n");
    return g_new0(Test, 1);
}

static void test_free(Test *test)
{
    g_print("test_free()\n");
    g_free(test);
}

GC_REGISTER(Test, test_new, test_free, GC_REF(ptr))

static void test_append(Test *test1)
{
    GC_AUTO(Test) *test2 = GC_NEW(Test);
    GC_AUTO(Test) *test3 = GC_NEW(Test);

    test1->ptr = test2;
    test2->ptr = test1;
}

static void test_run()
{
    GC_AUTO(Test) *test1 = GC_NEW(Test);
    test_append(test1);

    g_print("gc_collect()\n");
    gc_collect(); // should collect test3
}

int main()
{
    test_run();

    g_print("gc_collect()\n");
    gc_collect(); // should collect test1 and test2

    return 0;
}

