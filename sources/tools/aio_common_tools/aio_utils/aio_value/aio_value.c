#include <malloc.h>
#include <mem.h>
#include "lang/aio_function/aio_value/aio_value.h"
#include "lib/utils/str_hook/str_hook.h"
#include "lib/utils/memory_utils/memory_utils.h"
#include "lib/utils/string_utils/string_utils.h"
#include "lib/utils/int_utils/int_utils.h"
#include "lib/utils/double_utils/double_utils.h"
#include "lib/utils/boolean_utils/boolean_utils.h"
#include "lib/utils/error_utils/error_utils.h"
#include "lang/aio_type/aio_type.h"
#include "lib/utils/str_hook/str_hook_utils/str_hook_utils.h"
#include "lang/aio_reserved_names/aio_reserved_names_container.h"
#include "lib/utils/log_utils/log_utils.h"

#define AIO_VALUE_TAG "AIO_VALUE"

//#define AIO_VALUE_DEBUG

aio_value *new_aio_value(str_hook *type)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->type = type;
    return value;
}

aio_value *new_aio_value_by_string(string undefined_value)
{
    if (matches_int(undefined_value)) {
        const int int_value = string_to_int(undefined_value);
        return new_aio_int_value(int_value);
    }
    if (matches_double(undefined_value)) {
        const double double_value = string_to_double(undefined_value);
        return new_aio_double_value(double_value);
    }
    if (matches_string(undefined_value)) {
        string string_value = remove_prefix_suffix(undefined_value, "\'", "\'");
        return new_aio_string_value(string_value);
    }
    if (matches_boolean(undefined_value)) {
        const_boolean boolean_value = string_to_boolean(undefined_value);
        return new_aio_boolean_value(boolean_value);
    } else {
        throw_error_with_tag(AIO_VALUE_TAG, "Classes are not yet developed in AIO");
    }
}

aio_value *new_aio_int_value(const int int_acc)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->get.int_acc = int_acc;
    value->type = new_str_hook_by_string(INTEGER);
    return value;
}

aio_value *new_aio_double_value(const double double_acc)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->get.double_acc = double_acc;
    value->type = new_str_hook_by_string(DOUBLE);
    return value;
}

aio_value *new_aio_double_value_by_ref(const double *double_acc)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->get.double_acc = *double_acc;
    value->type = new_str_hook_by_string(DOUBLE);
    return value;
}

aio_value *new_aio_string_value(const_string string_acc)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->get.string_acc = new_string(string_acc);
    value->type = new_str_hook_by_string(STRING);
    return value;
}

aio_value *new_aio_boolean_value(const_boolean boolean_acc)
{
    aio_value *value = new_object(sizeof(aio_value *));
    value->get.boolean_acc = boolean_acc;
    value->type = new_str_hook_by_string(BOOLEAN);
    return value;
}

aio_value *new_aio_reference_value(void *reference)
{
    aio_value *value = new_object(sizeof(aio_value));
    value->get.reference = reference;
    value->type = new_str_hook_by_string(VOID);
    return value;
}

aio_value *new_aio_type_value(void *reference, const_str_hook *type)
{
    aio_value *value = new_object(sizeof(aio_value));
    value->get.reference = reference;
    value->type = new_str_hook_by_other(type);
    return value;
}

void free_aio_value(aio_value *value)
{
#ifdef AIO_VALUE_DEBUG
    log_info_boolean(AIO_VALUE_TAG, "Free not null value:", value != NULL);
#endif
    if (value != NULL) {
        str_hook *type = value->type;
#ifdef AIO_VALUE_DEBUG
        log_info_str_hook(AIO_VALUE_TAG, "Value type:", type);
#endif
        const_boolean is_string_type = is_hook_equals_str(type, STRING);
        if (is_string_type) {
            string string_value = value->get.string_acc;
            value->get.string_acc = NULL;
            free(string_value);
        }
        value->get.reference = NULL;
        free_const_str_hook(type);
        free(value);
#ifdef AIO_VALUE_DEBUG
        log_info(AIO_VALUE_TAG, "Value is deleted!");
#endif
    }
}

/**
 * List.
 */

aio_value_list *new_aio_value_list()
{
    aio_value_list *list = new_object(sizeof(aio_value_list));
    list->capacity = 2;
    list->size = 0;
    list->values = new_object_array(2, sizeof(string));
    return list;
}

static void update_memory_in_aio_value_list(aio_value_list *list)
{
    if (list->size == list->capacity) {
        list->capacity = list->capacity * 2;
        list->values = realloc(list->values, list->capacity * sizeof(aio_value *));
    }
}

void add_aio_value_in_list(aio_value_list *string_list, aio_value *value)
{
    update_memory_in_aio_value_list(string_list);
    string_list->values[string_list->size] = value;
    string_list->size++;
}

void free_aio_value_list(aio_value_list *list)
{
    aio_value **values = list->values;
    for (int i = 0; i < list->size; ++i) {
        aio_value *value = values[i];
        if (value != NULL) {
            values[i] = NULL;
            free_aio_value(value);
        }
    }
    free(values);
    free(list);
}

/**
 * Comparing utils.
 */

boolean are_equal_aio_values(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        if (!value_2) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        if (!value_2) {
            return FALSE;
        }
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc == new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc == new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return are_equal_strings(value_1->get.string_acc, new_value_2->get.string_acc);
    }
    if (is_hook_equals_str(type_1, BOOLEAN)) {
        aio_value *new_value_2 = cast_to_boolean(value_2);
        return value_1->get.boolean_acc == new_value_2->get.boolean_acc;
    } else {
        return value_1->get.reference == value_2->get.reference;
    }
}

boolean are_not_equal_aio_values(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        if (value_2) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        if (!value_2) {
            return TRUE;
        }
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc != new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc != new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return !are_equal_strings(value_1->get.string_acc, new_value_2->get.string_acc);
    }
    if (is_hook_equals_str(type_1, BOOLEAN)) {
        aio_value *new_value_2 = cast_to_boolean(value_2);
        return value_1->get.boolean_acc != new_value_2->get.boolean_acc;
    } else {
        return value_1->get.reference != value_2->get.reference;
    }
}

boolean is_more_aio_value_then_other(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        throw_error_with_tag(AIO_VALUE_TAG, "Left value is null!");
    }
    if (!value_2) {
        throw_error_with_tag(AIO_VALUE_TAG, "Right value is null!");
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc > new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc > new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return strcmp(value_1->get.string_acc, new_value_2->get.string_acc) > 0;
    } else {
        throw_error_with_hook(AIO_VALUE_TAG, "Can not compare values by type:", type_1);
    }
}

boolean is_more_or_equals_aio_value_then_other(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        if (!value_2) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        if (!value_2) {
            return FALSE;
        }
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc >= new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc >= new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return strcmp(value_1->get.string_acc, new_value_2->get.string_acc) >= 0;
    } else {
        throw_error_with_hook(AIO_VALUE_TAG, "Can not compare values by type:", type_1);
    }
}

boolean is_less_aio_value_then_other(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        throw_error_with_tag(AIO_VALUE_TAG, "Left value is null!");
    }
    if (!value_2) {
        throw_error_with_tag(AIO_VALUE_TAG, "Right value is null!");
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc < new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc < new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return strcmp(value_1->get.string_acc, new_value_2->get.string_acc) < 0;
    } else {
        throw_error_with_hook(AIO_VALUE_TAG, "Can not compare values by type:", type_1);
    }
}

boolean is_less_or_equals_aio_value_then_other(aio_value *value_1, aio_value *value_2)
{
    if (!value_1) {
        if (!value_2) {
            return TRUE;
        } else {
            return FALSE;
        }
    } else {
        if (!value_2) {
            return FALSE;
        }
    }
    const_str_hook *type_1 = value_1->type;
    if (is_hook_equals_str(type_1, INTEGER)) {
        aio_value *new_value_2 = cast_to_int(value_2);
        return value_1->get.int_acc <= new_value_2->get.int_acc;
    }
    if (is_hook_equals_str(type_1, DOUBLE)) {
        aio_value *new_value_2 = cast_to_double(value_2);
        return value_1->get.double_acc <= new_value_2->get.double_acc;
    }
    if (is_hook_equals_str(type_1, STRING)) {
        aio_value *new_value_2 = cast_to_string(value_2);
        return strcmp(value_1->get.string_acc, new_value_2->get.string_acc) <= 0;
    } else {
        throw_error_with_hook(AIO_VALUE_TAG, "Cannot compare values by type:", type_1);
    }
}

/**
 * Casts.
 */

aio_value *cast_to_int(aio_value *value)
{
    if (!value) {
        return NULL;
    }
    const_str_hook *type = value->type;
    if (is_hook_equals_str(type, INTEGER)) {
        return new_aio_int_value(value->get.int_acc);
    }
    if (is_hook_equals_str(type, DOUBLE)) {
        const double input_value = value->get.double_acc;
        int result = (int) input_value;
        return new_aio_int_value(result);
    }
    if (is_hook_equals_str(type, STRING)) {
        return new_aio_int_value(string_to_int(value->get.string_acc));
    }
    if (is_hook_equals_str(type, BOOLEAN)) {
        return new_aio_int_value(value->get.boolean_acc);
    } else {
        return new_aio_int_value((int) value->get.reference);
    }
}

aio_value *cast_to_double(aio_value *value)
{
    if (!value) {
        return NULL;
    }
    const_str_hook *type = value->type;
    if (is_hook_equals_str(type, INTEGER)) {
        return new_aio_double_value(value->get.int_acc);
    }
    if (is_hook_equals_str(type, DOUBLE)) {
        return new_aio_double_value(value->get.double_acc);
    }
    if (is_hook_equals_str(type, STRING)) {
        const_string string = value->get.string_acc;
        if (matches_int(string)) {
            return new_aio_double_value(string_to_int(string));
        } else if (matches_double(string)) {
            return new_aio_double_value(string_to_double(string));
        } else {
            throw_error_with_details(AIO_VALUE_TAG, "String doesn't matches double!", string);
        }
    }
    if (is_hook_equals_str(type, BOOLEAN)) {
        return new_aio_double_value(value->get.boolean_acc);
    } else {
        return new_aio_double_value_by_ref(value->get.reference);
    }
}

aio_value *cast_to_string(aio_value *value)
{
    if (!value) {
        return NULL;
    }
    const_str_hook *type = value->type;
    if (is_hook_equals_str(type, INTEGER)) {
        return new_aio_string_value(int_to_string(value->get.int_acc));
    }
    if (is_hook_equals_str(type, DOUBLE)) {
        string double_string = double_to_string(value->get.double_acc);
        return new_aio_string_value(double_string);
    }
    if (is_hook_equals_str(type, STRING)) {
        return new_aio_string_value(value->get.string_acc);
    }
    if (is_hook_equals_str(type, BOOLEAN)) {
        return new_aio_string_value(boolean_to_string(value->get.boolean_acc));
    } else {
        return new_aio_string_value(value->get.reference);
    }
}

aio_value *cast_to_boolean(aio_value *value)
{
    if (!value) {
        return NULL;
    }
    const_str_hook *type = value->type;
    if (is_hook_equals_str(type, INTEGER)) {
        const int int_value = value->get.int_acc;
        if (int_value == 1) {
            return new_aio_boolean_value(TRUE);
        }
        if (int_value == 0) {
            return new_aio_boolean_value(FALSE);
        } else {
            throw_error_with_tag(AIO_VALUE_TAG, "Can not cast int to boolean");
        }
    }
    if (is_hook_equals_str(type, DOUBLE)) {
        const double double_value = value->get.double_acc;
        if (double_value == 1.0) {
            return new_aio_boolean_value(TRUE);
        }
        if (double_value == 0.0) {
            return new_aio_boolean_value(FALSE);
        } else {
            throw_error_with_tag(AIO_VALUE_TAG, "Can not cast double to boolean");
        }
    }
    if (is_hook_equals_str(type, STRING)) {
        const_string string_value = value->get.string_acc;
        if (are_equal_strings(string_value, AIO_TRUE_VALUE)
            || are_equal_strings(string_value, "1")
            || are_equal_strings(string_value, "1.0")) {
            return new_aio_boolean_value(TRUE);
        } else if (are_equal_strings(string_value, AIO_FALSE_VALUE)
                   || are_equal_strings(string_value, "0")
                   || are_equal_strings(string_value, "0.0")) {
            return new_aio_boolean_value(FALSE);
        } else {
            throw_error_with_tag(AIO_VALUE_TAG, "Can not cast string to boolean");
        }
    }
    if (is_hook_equals_str(type, BOOLEAN)) {
        return new_aio_boolean_value(value->get.boolean_acc);
    } else {
        return new_aio_boolean_value((const_boolean) value->get.reference);
    }
}

aio_value *cast_to_void(aio_value *value)
{
    if (!value) {
        return NULL;
    }
    const_str_hook *type = value->type;
    if (is_hook_equals_str(type, INTEGER)) {
        return new_aio_int_value(value->get.int_acc);
    }
    if (is_hook_equals_str(type, DOUBLE)) {
        return new_aio_double_value(value->get.double_acc);
    }
    if (is_hook_equals_str(type, STRING)) {
        return new_aio_string_value(value->get.string_acc);
    }
    if (is_hook_equals_str(type, BOOLEAN)) {
        return new_aio_boolean_value(value->get.boolean_acc);
    } else {
        return new_aio_reference_value(value->get.reference);
    }
}

aio_value *cast_to_type(aio_value *value, const_str_hook *type)
{
    if (!value) {
        return NULL;
    }
    if (is_hook_equals_str(type, VOID)) {
        return cast_to_void(value);
    } else if (is_hook_equals_str(type, INTEGER)) {
        return cast_to_int(value);
    } else if (is_hook_equals_str(type, DOUBLE)) {
        return cast_to_double(value);
    } else if (is_hook_equals_str(type, STRING)) {
        return cast_to_string(value);
    } else if (is_hook_equals_str(type, BOOLEAN)) {
        return cast_to_boolean(value);
    } else {
        throw_error_with_hook(AIO_VALUE_TAG, "Sorry, AIO Core hasn't supported type:", type);
    }
}

/**
 * Log utils.
 */

void log_info_aio_value(const_string tag, string message, const aio_value *value)
{
    if (value) {
        const_str_hook *type = value->type;
        if (is_hook_equals_str(type, INTEGER)) {
            log_info_int(tag, message, value->get.int_acc);
        } else if (is_hook_equals_str(type, DOUBLE)) {
            log_info_double(tag, message, value->get.double_acc);
        } else if (is_hook_equals_str(type, STRING)) {
            log_info_string(tag, message, value->get.string_acc);
        } else if (is_hook_equals_str(type, BOOLEAN)) {
            log_info_boolean(tag, message, value->get.boolean_acc);
        } else {
            log_info_string(tag, message, "Can not log void reference!");
        }
    } else {
        log_info_string(tag, message, "NULL");
    }
}