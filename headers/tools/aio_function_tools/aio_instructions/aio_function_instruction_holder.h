#ifndef AIO_INSTRUCTION_HOLDER_H
#define AIO_INSTRUCTION_HOLDER_H

#include "aio_function_instruction.h"
#include "../../../lang/aio_function/aio_variable/aio_definition/aio_variable_definition.h"

typedef struct aio_function_instruction_holder {
    const struct aio_function_instruction_holder *parent;
    struct aio_variable_definition_list *variable_definition_list;
    struct aio_function_instruction_list *instruction_list;
} aio_function_instruction_holder;

typedef const aio_function_instruction_holder const_aio_function_instruction_holder;

aio_function_instruction_holder *new_aio_function_instruction_holder(
        const_aio_function_instruction_holder *parent_holder);

aio_function_instruction_holder *inflate_root_aio_instruction_holder(const_string source_code,
                                                                     int *start_code_pointer_ref,
                                                                     struct aio_variable_definition_list
                                                                     *arg_definitions);

aio_function_instruction_holder *inflate_local_aio_instruction_holder(const_string source_code,
                                                                      const_aio_function_instruction_holder
                                                                      *parent_holder,
                                                                      const int start_index, const int end_index);

void free_aio_function_instruction_holder(aio_function_instruction_holder *holder);

#endif //AIO_INSTRUCTION_HOLDER_H