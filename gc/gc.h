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

#ifndef __GC_H__
#define __GC_H__

#include <glib.h>

// Use GC_REGISTER and GC_REF macros to register new types
void gc_register(gpointer instance, GDestroyNotify notify, ...);

// Use GC_AUTO to automatically cleanup local references
void gc_cleanup(gpointer instance);

// Collects unreachable objects by calling their destructors
void gc_collect();

// Declares automatically collected variable
// Usage: GC_AUTO(MyType) *my_instance = GC_NEW(MyType);
#define GC_AUTO(TypeName) __attribute__((cleanup(gc_cleanup_##TypeName))) TypeName

// Creates new instance of the specified type by calling its registered constructor
// The returned reference should be assigned to a variable declared with GC_AUTO
// or explicitelly cleaned by gc_cleanup() when leaving the scope
#define GC_NEW(TypeName) gc_new_##TypeName()

// Defines a reference member within the GC_REGISTER macro
#define GC_REF(struct_member) ((gpointer)instance + G_STRUCT_OFFSET(typeof(*instance), struct_member))

// Registers new type given the type name, constructor, destructor and a list of internal references
// Usage: GC_REGISTER(MyType, my_type_new, my_type_free, GC_REF(some_reference), GC_REF(another_reference))
// where the type itself is declared as a struct { SomeType *some_reference; AnotherType *another_reference; }
#define GC_REGISTER(TypeName, ctor, dtor, ...)                                              \
static inline void gc_cleanup_##TypeName(TypeName **ptr) { if(*ptr)gc_cleanup(*ptr); }      \
TypeName* gc_new_##TypeName(void)                                                           \
{                                                                                           \
    TypeName *instance = ctor();                                                            \
    if(instance) gc_register(instance, (GDestroyNotify)dtor, ##__VA_ARGS__, NULL);          \
    return instance;                                                                        \
}

#endif /* __GC_H__ */
