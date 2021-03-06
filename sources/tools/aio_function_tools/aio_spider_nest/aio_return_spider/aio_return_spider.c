#include <mem.h>
#include <ctype.h>
#include <malloc.h>
#include <lib/utils/str_hook/str_hook.h>
#include <lib/utils/str_hook/str_hook_utils/str_hook_utils.h>
#include <tools/aio_common_tools/aio_spider_nest/aio_spider.h>
#include <lib/utils/point_watcher/point_watcher.h>
#include <tools/aio_function_tools/aio_instruction_spider_nest/aio_return_spider/aio_return_spider.h>
#include <lib/utils/collections/lists/string_list.h>
#include <lib/utils/memory_utils/memory_utils.h>
#include <lib/utils/string_utils/string_utils.h>
#include <lib/utils/char_utils/char_utils.h>
#include <tools/aio_function_tools/aio_instructions/aio_function_instruction.h>
#include <tools/aio_function_tools/aio_instructions/aio_tasks/aio_return_task.h>
#include <lib/utils/error_utils/error_utils.h>

#define AIO_RETURN_SPIDER_DEBUG

#define AIO_RETURN_SPIDER_TAG "AIO_RETURN_SPIDER"

#ifdef AIO_RETURN_SPIDER_DEBUG

#include <lib/utils/log_utils/log_utils.h>

#endif

/**
 * Refresh.
 */

void refresh_return_spider(aio_spider *spider, point_watcher *ripper_watcher)
{
    spider->message = AIO_SPIDER_NOT_FOUND_MATERIALS;
    //재료 리셋 (Reset materials):
    aio_return_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    main_watcher->start = ripper_watcher->pointer;
    main_watcher->end = ripper_watcher->pointer;
    main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
    reset_point_watcher(materials->value_watcher);
    materials->scope_type = AIO_RETURN_MODIFIER_SCOPE;
    //------------------------------------------------------------------------------------------------------------------
    //찌꺼기 수집기 (Garbage collector):
    string_list *old_return_values = materials->value_list;
    materials->value_list = new_string_list();
    free_strings_in_list(old_return_values);
    free_string_list(old_return_values);
    //------------------------------------------------------------------------------------------------------------------
}

/**
 * 거미를 비우다 (Free spider).
 */

void free_return_spider(aio_spider *spider)
{
    aio_return_materials *materials = spider->materials;
    free_point_watcher(materials->main_watcher);
    free_point_watcher(materials->value_watcher);
    free_strings_in_list(materials->value_list);
    free_string_list(materials->value_list);
    free(materials);
    free(spider);
}

/**
 * 건설자 (Constructor).
 */

struct aio_spider *new_aio_return_spider(point_watcher *ripper_watcher)
{
    aio_spider *spider = new_object(sizeof(aio_spider));
    //함수들을 놓다 (Put functions):
    spider->refresh = refresh_return_spider;
    spider->is_found_context = is_found_return_instruction;
    spider->weave_context_for = weave_return_instruction_for;
    spider->free = free_return_spider;
    //재료들을 만들다 (Create materials):
    aio_return_materials *materials = new_object(sizeof(aio_return_materials));
    materials->scope_type = AIO_RETURN_MODIFIER_SCOPE;
    materials->main_watcher = new_point_watcher();
    materials->main_watcher->start = ripper_watcher->start;
    materials->main_watcher->end = ripper_watcher->pointer;
    materials->value_watcher = new_point_watcher();
    materials->value_list = new_string_list();
    //재료들을 놀다 (Set materials):
    spider->materials = materials;
    //시작 메시지 초기화하다 (Init start message):
    spider->message = AIO_SPIDER_NOT_FOUND_MATERIALS;
    return spider;
}

const aio_spider_message is_found_return_instruction(
        const_string source_code,
        point_watcher *ripper_watcher,
        aio_spider *spider
)
{
    //재료들을 추출하다 (Extract materials):
    const aio_return_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    main_watcher->end = ripper_watcher->pointer;
    //Define current symbol:
    const char current_symbol = source_code[main_watcher->end];
    //TODO: 코드 복제 (Code duplication)!
    if (main_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
        if (is_space_or_line_break(current_symbol)) {
            main_watcher->start++;
        } else {
            main_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
        }
    }
    if (main_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
        if (materials->scope_type == AIO_RETURN_MODIFIER_SCOPE) {
            handle_return_modifier_scope(source_code, spider);
        }
        if (materials->scope_type == AIO_RETURN_VALUE_SCOPE) {
            handle_return_value_scope(source_code, spider);
        }
    }
    return spider->message;
}

void handle_return_modifier_scope(const_string source_code, aio_spider *spider)
{
    aio_return_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    const int current_position = main_watcher->end;
    const char current_symbol = source_code[current_position];
    //Check current symbol:
    const_boolean is_end_of_modifier = !isalpha(current_symbol);
    if (is_end_of_modifier) {
        const int start_index = main_watcher->start;
        const int hold_positions = current_position - start_index;
        if (hold_positions == 3) {
            const_boolean is_return_modifier =
                    source_code[start_index] == 'o'
                    && source_code[start_index + 1] == 'u'
                    && source_code[start_index + 2] == 't';
            if (is_return_modifier) {
                //Shift main_watcher:
                main_watcher->start = current_position;
                main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
                //Set scope:
                materials->scope_type = AIO_RETURN_VALUE_SCOPE;
                //Set message:
                spider->message = AIO_SPIDER_FOUND_MATERIALS;
#ifdef AIO_RETURN_SPIDER_DEBUG
                log_info(AIO_RETURN_SPIDER_TAG, "Found return modifier!");
#endif
            }
        }
    }
}

static str_hook_list *make_return_chunks(const_string chunk)
{
    str_hook_list *list = new_str_hook_list();
    const size_t length = strlen(chunk);
    point_watcher *main_watcher = new_point_watcher();
    main_watcher->start = 0;
    main_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
    for (int i = 0; i < length; ++i) {
        const char symbol = chunk[i];
        const_boolean is_opening_parenthesis_cond = is_opening_parenthesis(symbol);
        const_boolean is_closing_parenthesis_cond = is_closing_parenthesis(symbol);
        const_boolean is_quote_cond = is_single_quote(symbol);
        const_boolean is_comma_cond = is_comma(symbol);
        if (main_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
            if (is_opening_parenthesis_cond) {
                main_watcher->pointer++;
            }
            if (is_closing_parenthesis_cond) {
                main_watcher->pointer--;
            }
            if (is_comma_cond && main_watcher->pointer == 0) {
                main_watcher->end = i;
                add_str_hook_in_list(list, new_str_hook_by_point_watcher(chunk, main_watcher));
                main_watcher->start = i;
            }
        }
        if (is_quote_cond) {
            if (main_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
                main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
            } else {
                main_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
            }
        }
    }
    if (list->size == 0) {
        add_str_hook_in_list(list, new_str_hook_by_string(chunk));
    }
    log_info_str_hook_list(AIO_RETURN_SPIDER_TAG, "CAPTURED RETURN CHUNKS:", list);
    return list;
}

void handle_return_value_scope(const_string source_code, struct aio_spider *spider)
{
    //재료들을 추출하다 (Extract materials):
    aio_return_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    point_watcher *value_watcher = materials->value_watcher;
    //Define current symbol:
    const int current_position = main_watcher->end;
    const char current_symbol = source_code[current_position];
    //Check current symbol:
    const_boolean is_whitespace_cond = is_space_or_line_break(current_symbol);
    const_boolean is_closing_parenthesis_cond = is_closing_parenthesis(current_symbol);
    const_boolean is_letter_cond = isalpha(current_symbol);
    const_boolean is_quote_cond = is_single_quote(current_symbol);
    const_boolean is_closing_brace_cond = is_closing_brace(current_symbol);
    const_boolean is_valid_bound = isalnum(current_symbol)
                                   || is_closing_parenthesis_cond
                                   || is_quote_cond;
    if (is_whitespace_cond && value_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
        value_watcher->pointer++;
        return;
    }
    if (((is_letter_cond && value_watcher->pointer > 0) || is_closing_brace_cond)
        && value_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
        value_watcher->start = main_watcher->start;
        value_watcher->end = main_watcher->end - value_watcher->pointer;
        //값을 놓다 (Set value):
        string dirty_chunk = substring_by_point_watcher(source_code, value_watcher);
        string dirty_squeezed_chunk = squeeze_string_for_expression(dirty_chunk);
        str_hook_list *clean_return_values = make_return_chunks(dirty_squeezed_chunk);
        const int number_of_return_values = clean_return_values->size;
        for (int i = 0; i < number_of_return_values; ++i) {
            add_string_in_list(materials->value_list, substring_by_str_hook(clean_return_values->hooks[i]));
        }
        //위빙 준비 (Prepare for weaving):
        materials->scope_type = AIO_RETURN_WEAVING_SCOPE;
        main_watcher->start = main_watcher->end;
        spider->message = AIO_SPIDER_IS_READY_FOR_WEAVING;
        //--------------------------------------------------------------------------------------------------------------
        //찌거기 수집기 (Garbage collector):
        free(dirty_chunk);
        free(dirty_squeezed_chunk);
        free_str_hooks_in_list(clean_return_values);
        free_str_hook_list(clean_return_values);
        return;
    }
    if (is_quote_cond) {
        if (value_watcher->mode != POINT_WATCHER_UNDEFINED_MODE) {
            value_watcher->mode = POINT_WATCHER_UNDEFINED_MODE;
            return;
        } else {
            value_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
        }
    }
    if (value_watcher->mode != POINT_WATCHER_UNDEFINED_MODE) {
        value_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
        value_watcher->pointer = 0;
        if (is_valid_bound) {
            value_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
            return;
        }
    }
}

void weave_return_instruction_for(
        void *parent, const_string _,
        point_watcher *ripper_watcher,
        aio_spider *spider
)
{
    aio_function_instruction_holder *holder = parent;
#ifdef AIO_RETURN_SPIDER_TAG
    log_info(AIO_RETURN_SPIDER_TAG, "Start weaving...");
#endif
    //재료들을 추출하다 (Extract materials):
    const aio_return_materials *materials = spider->materials;
    const string_list *return_values = materials->value_list;
    const boolean is_ready_for_weaving = materials->scope_type == AIO_RETURN_WEAVING_SCOPE;
    if (is_ready_for_weaving) {
        ripper_watcher->pointer = materials->value_watcher->end - 1;
        ripper_watcher->start = materials->value_watcher->end - 1;
        //'Assign' 지침을  짜다 (Weave 'Return' instruction):
        aio_function_instruction *instruction = new_aio_return_instruction(parent, return_values);
        //명부에게 지침을 추가하다 (Add 'Return' instruction in holder's instructions):
        aio_function_instruction_list *instruction_list = holder->instruction_list;
        add_aio_instruction_in_list(instruction_list, instruction);
        //위빙이 완료되었습니다 (Weaving complete)!
#ifdef AIO_RETURN_SPIDER_DEBUG
        log_info(AIO_RETURN_SPIDER_TAG, "WEAVED INSTRUCTION:");
        const string_list *values = instruction->get.return_task->return_values;
        for (int i = 0; i < values->size; ++i) {
            log_info_string(AIO_RETURN_SPIDER_TAG, "Return value:", values->strings[i]);
        }
#endif
    } else {
        throw_error_with_tag(AIO_RETURN_SPIDER_TAG, "Not ready for weaving!");
    }
}