#ifndef AIO_CORE_H
#define AIO_CORE_H

#include "../../lib/utils/string_utils/string_utils.h"
#include "../aio_context/aio_context.h"
#include "../../lib/utils/collections/sets/string_set.h"

#define NUMBER_OF_CORE_TYPES 5

typedef struct aio_core {
    aio_context_list *core_context_list;
    //유형 포함 (Keep types):
    string_set *aio_type_set;
} aio_core;

extern aio_core *core;

void inflate_aio_core();

const_aio_context * inflate_aio_context_and_put_in_core(const_string path);

boolean is_aio_type(const_string string);

boolean is_aio_type_hooked(const_str_hook *hook);

void free_aio_core();

#endif //AIO_CORE_H