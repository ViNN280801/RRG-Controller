#include <stdarg.h>
#include <stdio.h>

#include "utils.h"

int _check_ptrs(void *first_ptr, const char *first_ptr_name, ...)
{
    if (!first_ptr)
    {
        fprintf(stderr, "RRG DEBUG: Argument '%s' is NULL\n", first_ptr_name);
        return -1;
    }

    va_list args;
    va_start(args, first_ptr_name);
    void *ptr;
    const char *ptr_name;

    while ((ptr = va_arg(args, void *)) != NULL)
    {
        ptr_name = va_arg(args, const char *);
        if (!ptr)
        {
            fprintf(stderr, "RRG DEBUG: Argument '%s' is NULL\n", ptr_name);
            va_end(args);
            return -1;
        }
    }

    va_end(args);
    return 0;
}
