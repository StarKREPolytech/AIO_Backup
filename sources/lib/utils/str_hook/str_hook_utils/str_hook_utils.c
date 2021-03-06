#include <ctype.h>
#include <mem.h>
#include <stdio.h>
#include <process.h>
#include <lib/utils/str_hook/str_hook.h>
#include <lib/utils/string_utils/string_utils.h>
#include <lib/utils/memory_utils/memory_utils.h>
#include <lib/utils/point_watcher/point_watcher.h>
#include <lib/utils/boolean_utils/boolean_utils.h>
#include <lib/utils/collections/sets/string_set.h>
#include <aio_tools/aio_common_tools/aio_lexer/aio_lexer.h>
#include <lib/utils/char_utils/char_utils.h>
#include <lib/utils/error_utils/error_utils.h>
#include <lib/utils/type_utils/type.utils.h>

#define STRING_HOOK_DEBUG

#ifdef STRING_HOOK_DEBUG

#endif

#define STRING_HOOK_TAG "STRING_HOOK"

/**
 * Extra constructors.
 */

str_hook *new_str_hook_with_start(string source_string, const int start_index)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = start_index;
    hook->end = 0;
    return hook;
}

str_hook *new_str_hook_with_end(string source_string, const int end_index)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = 0;
    hook->end = end_index;
    return hook;
}

str_hook *new_str_hook_with_start_and_end(string source_string, const int start_index, const int end_index)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = start_index;
    hook->end = end_index;
    return hook;
}

str_hook *new_str_hook_by_point_watcher(string source_string, const point_watcher *watcher)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = watcher->start;
    hook->end = watcher->end;
    return hook;
}

str_hook *new_str_hook_by_other(const str_hook *other_hook)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = other_hook->source_string;
    hook->start = other_hook->start;
    hook->end = other_hook->end;
    return hook;
}

str_hook *new_str_hook_by_other_hook_string(const str_hook *other_hook)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = other_hook->source_string;
    hook->start = 0;
    hook->end = 0;
    return hook;
}

str_hook *new_str_hook_by_string(string source_string)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = 0;
    hook->end = strlen(source_string);
    return hook;
}

str_hook *new_str_hook_by_offset(string source_string, const int offset, const int length)
{
    str_hook *hook = new_object(sizeof(str_hook));
    hook->source_string = source_string;
    hook->start = offset;
    hook->end = offset + length;
    return hook;
}

/**
 * Getters.
 */

int get_str_hook_size(const str_hook *hook)
{
    return hook->end - hook->start;
}

char get_str_hook_char(const str_hook *hook, const int index)
{
    return hook->source_string[0];
}

/**
 * Common utils.
 */

boolean is_word_hooked(const str_hook *hook)
{
    int length = get_str_hook_size(hook);
    if (length < 1) {
        return FALSE;
    }
    const char first_symbol = get_str_hook_char(hook, 0);
    if (isalpha(first_symbol)) {
        if (length > 1) {
            for (int i = 1; i < length; ++i) {
                const char symbol = get_str_hook_char(hook, i);
                if (!isalnum(symbol)) {
                    return FALSE;
                }
            }
        }
    }
    return TRUE;
}

string substring_by_str_hook(const str_hook *hook)
{
    return substring(hook->source_string, hook->start, hook->end);
}

boolean are_equal_hooked_str(const str_hook *hook_1, const str_hook *hook_2)
{
    const int size_1 = get_str_hook_size(hook_1);
    const int size_2 = get_str_hook_size(hook_2);
    if (size_1 != size_2) {
        return FALSE;
    }
    const_string str_1 = hook_1->source_string;
    const_string str_2 = hook_2->source_string;
    const int start_1 = hook_1->start;
    const int start_2 = hook_2->start;
    for (int i = 0; i < size_1; ++i) {
        if (str_1[start_1 + i] != str_2[start_2 + i]) {
            return FALSE;
        }
    }
    return TRUE;
}

boolean is_hook_equals_str(const str_hook *hook, const_string str)
{
    const int hook_size = get_str_hook_size(hook);
    const int str_length = strlen(str);
    if (hook_size != str_length) {
        return FALSE;
    }
    const_string hooked_str = hook->source_string;
    const int start = hook->start;
    for (int i = 0; i < hook_size; ++i) {
        if (hooked_str[start + i] != str[i]) {
            return FALSE;
        }
    }
    return TRUE;
}

boolean contains_string_in_set_by_hook(string_set *set, const str_hook *hook)
{
    for (int i = 0; i < set->size; ++i) {
        if (is_hook_equals_str(hook, set->strings[i])) {
            return TRUE;
        }
    }
    return FALSE;
}

boolean is_empty_hooked_str(const str_hook *hook)
{
    return hook->end - hook->start <= 0;
}

boolean is_not_empty_hooked_str(const str_hook *hook)
{
    return hook->end - hook->start > 0;
}

str_hook_list *filter_str_hook_list(const str_hook_list *list, boolean (*filter_condition)(const str_hook *))
{
    val src_list_size = list->size;
    val src_hooks = list->hooks;
    var new_hook_list = new_str_hook_list();
    for (int k = 0; k < src_list_size; ++k) {
        val src_hook = src_hooks[k];
        if (filter_condition(src_hook)) {
            var *new_hook = new_str_hook_by_other(src_hook);
            add_str_hook_in_list(new_hook_list, new_hook);
        }
    }
    return new_hook_list;
}

/**
 * Log utils.
 */

void log_info_str_hook(const_string tag, const_string message, const str_hook *hook)
{
    printf("\n%s: %s -", tag, message);
    for (int i = hook->start; i < hook->end; ++i) {
        printf("%c", hook->source_string[i]);
    }
    printf("-\n");
}

void log_info_str_hook_list(const_string tag, const_string message, const str_hook_list *list)
{
    val size = list->size;
    val hooks = list->hooks;
    for (int i = 0; i < size; ++i) {
        val *hook = hooks[i];
        val src = hook->source_string;
        printf("\n%s: %s -", tag, message);
        for (int j = hook->start; j < hook->end; ++j) {
            printf("%c", src[j]);
        }
        printf("-\n");
    }
}

/**
 * Primitive type matchers.
 */

boolean is_int_hooked(const str_hook *hook)
{
    const_string string = hook->source_string;
    const int length = get_str_hook_size(hook);
    int start_position = hook->start;
    if (length <= 0) {
        throw_error_with_tag(STRING_HOOK_TAG, "Empty string doesn't matches int!");
    }
    if (string[start_position] == '-') {
        if (length == 1) {
            throw_error_with_tag(STRING_HOOK_TAG, "Minus doesn't matches int!");
        }
        start_position++;
    }
    for (int i = start_position; i < hook->end; ++i) {
        const int digit = string[i] - '0';
        if (digit < 0 || digit > 9) {
            return FALSE;
        }
    }
    return TRUE;
}


boolean is_double_hooked(const str_hook *hook)
{
    const_string string = hook->source_string;
    const int length = get_str_hook_size(hook);
    boolean was_dot = FALSE;
    boolean was_fraction = FALSE;
    int start_position = hook->start;
    if (length <= 0) {
        throw_error_with_tag(STRING_HOOK_TAG, "Empty string doesn't matches double!");
    }
    if (string[start_position] == '-') {
        if (length == 1) {
            throw_error_with_tag(STRING_HOOK_TAG, "Minus doesn't matches double!");
        }
        start_position++;
    }
    for (int i = start_position; i < hook->end; ++i) {
        const int digit = string[i] - '0';
        if ((digit < 0 || digit > 9)) {
            if (string[i] == '.' && was_dot == FALSE) {
                was_dot = TRUE;
            } else {
                return FALSE;
            }
        }
        if (digit >= 0 && digit < 10 && was_dot == TRUE) {
            was_fraction = TRUE;
        }
    }
    if (was_dot == TRUE && was_fraction == TRUE) {
        return TRUE;
    }
    return FALSE;
}

boolean is_string_hooked(const str_hook *hook)
{
    const int length = get_str_hook_size(hook);
    const_string hooked_string = hook->source_string;
    return length > 1 && hooked_string[hook->start] == '\'' && hooked_string[hook->end - 1] == '\'';
}

boolean is_boolean_hooked(const str_hook *hook)
{
    return is_hook_equals_str(hook, AIO_TRUE_VALUE) || is_hook_equals_str(hook, AIO_FALSE_VALUE);
}

boolean is_null_hooked(const str_hook *hook)
{
    return is_hook_equals_str(hook, AIO_NULL_VALUE);
}

void throw_error_with_hook(const_string tag, const_string message, const str_hook *hook)
{
    printf("\n%s %s: %s -", ERROR_TAG, tag, message);
    for (int i = hook->start; i < hook->end; ++i) {
        printf("%c", hook->source_string[i]);
    }
    printf("-\n");
    exit(1);
}

/**
 * Casts.
 */

int str_hook_to_int(const str_hook *hook)
{
    int result = 0;
    const_string string = hook->source_string;
    for (int i = hook->start; i < hook->end; i++) {
        result = result * 10 + (string[i] - '0');
    }
    return result;
}

double str_hook_to_double(const struct str_hook *hook)
{
    const static char CHAR_SHIFT = '0';
    const static int DIGIT_SHIFT = 10;
    const_string string = hook->source_string;
    int integer_part = 0;
    int i = hook->start;
    while (!is_dot(string[i])) {
        integer_part = integer_part * DIGIT_SHIFT + (string[i++] - CHAR_SHIFT);
    }
    double fraction_part = 0.0;
    double fraction_counter = DIGIT_SHIFT;
    for (int j = i + 1; j < hook->end; ++j) {
        const double digit = ((double) (string[j] - CHAR_SHIFT) / fraction_counter);
        fraction_part += digit;
        fraction_counter *= DIGIT_SHIFT;
    }
    return ((double) integer_part) + fraction_part;
}

str_hook *lower_str_hook_quotes(const str_hook *hook)
{
    return new_str_hook_with_start_and_end(hook->source_string, hook->start + 1, hook->end - 1);
}

boolean str_hook_to_boolean(const str_hook *hook)
{
    if (is_hook_equals_str(hook, AIO_TRUE_VALUE)) {
        return TRUE;
    }
    if (is_hook_equals_str(hook, AIO_FALSE_VALUE)) {
        return FALSE;
    }
    throw_error_with_hook(STRING_HOOK_TAG, "Can not cast to boolean!", hook);
}