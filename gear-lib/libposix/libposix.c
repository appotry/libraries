/******************************************************************************
 * Copyright (C) 2014-2020 Zhifeng Gong <gozfree@163.com>
 *
 * Permission is hereby granted, free of charge, to any person obtaining a copy
 * of this software and associated documentation files (the "Software"), to deal
 * in the Software without restriction, including without limitation the rights
 * to use, copy, modify, merge, publish, distribute, sublicense, and/or sell
 * copies of the Software, and to permit persons to whom the Software is
 * furnished to do so, subject to the following conditions:
 *
 * The above copyright notice and this permission notice shall be included in all
 * copies or substantial portions of the Software.
 *
 * THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
 * IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
 * FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
 * AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
 * LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
 * OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
 * SOFTWARE.
 ******************************************************************************/
#include "libposix.h"
#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/param.h>

#ifndef PATH_SPLIT
#define PATH_SPLIT       '/'
#endif

GEAR_API void *memdup(const void *src, size_t len)
{
    void *dst = NULL;
    if (len == 0) {
        return NULL;
    }
    dst = calloc(1, len);
    if (LIKELY(dst != NULL)) {
        memcpy(dst, src, len);
    }
    return dst;
}

GEAR_API struct iovec *iovec_create(size_t len)
{
    struct iovec *vec = calloc(1, sizeof(struct iovec));
    if (LIKELY(vec != NULL)) {
        vec->iov_len = len;
        vec->iov_base = calloc(1, len);
        if (UNLIKELY(vec->iov_base == NULL)) {
            free(vec);
            vec = NULL;
        }
    }
    return vec;
}

GEAR_API void iovec_destroy(struct iovec *vec)
{
    if (LIKELY(vec != NULL)) {
        /* free(NULL) do nop */
        free(vec->iov_base);
        free(vec);
    }
}


/**
 * Fast little endian check
 * NOTE: not applicable for PDP endian
 */
GEAR_API bool is_little_endian(void)
{
    uint16_t x = 0x01;
    return *((uint8_t *) &x);
}

#if !defined(OS_RTTHREAD)
GEAR_API int get_proc_name(char *name, size_t len)
{
    int i, ret;
    char proc_name[PATH_MAX];
    char *ptr = NULL;
    memset(proc_name, 0, sizeof(proc_name));
    if (-1 == readlink("/proc/self/exe", proc_name, sizeof(proc_name))) {
        fprintf(stderr, "readlink failed!\n");
        return -1;
    }
    ret = strlen(proc_name);
    for (i = ret, ptr = proc_name; i > 0; i--) {
        if (ptr[i] == PATH_SPLIT) {
            ptr+= i+1;
            break;
        }
    }
    if (i == 0) {
        fprintf(stderr, "proc path %s is invalid\n", proc_name);
        return -1;
    }
    if (ret-i > (int)len) {
        fprintf(stderr, "proc name length %d is larger than %d\n", ret-i, (int)len);
        return -1;
    }
    strncpy(name, ptr, ret - i);
    return 0;
}
#endif
