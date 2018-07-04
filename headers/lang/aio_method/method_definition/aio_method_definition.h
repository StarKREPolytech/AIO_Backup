#include <malloc.h>
#include "../../annotations/aio_annotation.h"
#include "aio_method_size_type.h"
#include "../../../lib/collections/lists/aio_annotation_list.h"
#include "../../../lib/collections/lists/string_list.h"

#ifndef AIO_METHOD_DEFINITION_H
#define AIO_METHOD_DEFINITION_H

typedef struct aio_method_definition {
    const_string name;
    aio_annotation_list *annotations;
    string_list *source_code;
    enum aio_method_size_type method_size_type;
} aio_method_definition;

aio_method_definition *new_aio_method_definition(const_string name, aio_annotation_list *annotations,
                                                 string_list *source_code, enum aio_method_size_type method_size_type);

void delete_aio_method_definition(aio_method_definition* method_definition);

#endif //AIO_METHOD_DEFINITION_H