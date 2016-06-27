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

#define G_LOG_DOMAIN "GC"

#include "gc.h"

typedef struct _GCObject GCObject;

struct _GCObject
{
    gpointer instance;
    GDestroyNotify notify;
    GSList *references; // <gpointer*>

    GList link;
    gboolean mark;
};

GHashTable *gc_table; // <gpointer, GCObject>
GList *gc_list; // <GCObject>

__attribute__((constructor))
static void gc_init()
{
    gc_table = g_hash_table_new(g_direct_hash, g_direct_equal);
    gc_list = NULL;
}

__attribute__((destructor))
static void gc_deinit()
{
    g_hash_table_destroy(gc_table);
}

static void gc_sweep(GCObject *gco)
{
    g_debug("mark %p", gco->instance);
    gco->mark = TRUE;

    GSList *link = gco->references;
    while(link)
    {
        gco = g_hash_table_lookup(gc_table, *(gpointer*)link->data);
        if(gco && !gco->mark) gc_sweep(gco);

        link = link->next;
    }
}

void gc_register(gpointer instance, GDestroyNotify notify, ...)
{
    g_debug("register %p", instance);
    GCObject *gco = g_slice_new(GCObject);
    gco->instance = instance;
    gco->notify = notify;
    gco->references = NULL;

    va_list refs;
    va_start(refs, notify);

    gpointer ref;
    while((ref = va_arg(refs, gpointer)))
        gco->references = g_slist_prepend(gco->references, ref);

    va_end(refs);

    gco->link.data = gco;
    gco->link.prev = NULL;
    gco->link.next = NULL;
    gc_list = g_list_concat(&gco->link, gc_list);

    gco->mark = FALSE;
    g_hash_table_insert(gc_table, instance, gco);
}

void gc_cleanup(gpointer instance)
{
    g_debug("cleanup %p", instance);
    GCObject *gco = g_hash_table_lookup(gc_table, instance);
    gc_list = g_list_remove_link(gc_list, &gco->link);
}

void gc_collect()
{
    GList *link = gc_list;
    while(link)
    {
        gc_sweep(link->data);
        link = link->next;
    }

    GHashTableIter iter;
    g_hash_table_iter_init(&iter, gc_table);

    gpointer value;
    while(g_hash_table_iter_next(&iter, NULL, &value))
    {
        GCObject *gco = value;
        if(!gco->mark)
        {
            g_debug("collect %p", gco->instance);
            gco->notify(gco->instance);
            g_slist_free(gco->references);
            g_slice_free(GCObject, gco);
            g_hash_table_iter_remove(&iter);
        }
        else gco->mark = FALSE;
    }
}

