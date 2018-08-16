#include <malloc.h>
#include <ctype.h>
#include "../../../../../headers/lib/utils/boolean_utils/boolean_utils.h"
#include "../../../../../headers/lang/aio_function/aio_function_definition/aio_instructions/aio_instructions.h"
#include "../../../../../headers/tools/aio_parsers/aio_spiders/aio_spider.h"
#include "../../../../../headers/lib/utils/memory_utils/memory_utils.h"
#include "../../../../../headers/lib/utils/char_utils/char_utils.h"

#define AIO_PROCEDURE_SPIDER_DEBUG

#define AIO_PROCEDURE_SPIDER_TAG "AIO_PROCEDURE_SPIDER"

#ifdef AIO_PROCEDURE_SPIDER_DEBUG

#include "../../../../../headers/lib/utils/log_utils/log_utils.h"

#endif

/**
 * Refresh.
 */

void refresh_procedure_spider(aio_spider *spider, point_watcher *ripper_watcher) {
    point_watcher *main_watcher = spider->get.procedure_materials->watcher;
    main_watcher->start_index = ripper_watcher->pointer;
    main_watcher->end_index = ripper_watcher->pointer;
    main_watcher->mode = POINT_PASSIVE_MODE;
}

/**
 * Free spider.
 */

void free_procedure_function(aio_spider *spider) {
    aio_procedure_materials *materials = spider->get.procedure_materials;
    point_watcher *watcher = materials->watcher;
    free_point_watcher(watcher);
    free(materials);
    free(spider);
}

struct aio_spider *new_aio_procedure_spider(point_watcher *ripper_watcher) {
    aio_spider *spider = new_object(sizeof(aio_spider));
    //Bind main spider's functions:
    spider->refresh = refresh_procedure_spider;
    spider->is_found_instruction = is_found_procedure_instruction;
    spider->weave_instruction_for = weave_procedure_instruction_for;
    spider->free = free_procedure_function;
    //Create materials:
    aio_procedure_materials *materials = new_object(sizeof(aio_procedure_materials));
    materials->watcher = new_point_watcher();
    materials->watcher->start_index = ripper_watcher->start_index;
    materials->watcher->end_index = ripper_watcher->pointer;
    spider->get.procedure_materials = materials;
    //Init start message:
    spider->message = AIO_SPIDER_NOT_FOUND_MATERIALS;
    return spider;
}

const enum aio_spider_message is_found_procedure_instruction(const_string source_code, point_watcher *ripper_watcher,
                                                             struct aio_spider *spider) {
    if (spider->message == AIO_SPIDER_IS_READY_FOR_WEAVING) {
        ripper_watcher->pointer--;
        return spider->message;
    }
    const aio_procedure_materials *materials = spider->get.procedure_materials;
    point_watcher *watcher = materials->watcher;
    watcher->end_index = ripper_watcher->pointer;
    const char current_symbol = source_code[watcher->end_index];
    if (watcher->mode == POINT_PASSIVE_MODE) {
        if (is_space_or_line_break(current_symbol)) {
            watcher->start_index++;
        } else {
            watcher->mode = POINT_ACTIVE_MODE;
        }
    }
    if (watcher->mode == POINT_ACTIVE_MODE) {
        handle_procedure_scope(source_code, spider);
    }
    return spider->message;
}

void handle_procedure_scope(const_string source_code, struct aio_spider *spider) {
    //재료들을 추출하다 (Extract materials):
    aio_procedure_materials *materials = spider->get.procedure_materials;
    point_watcher *watcher = materials->watcher;
    //Define current symbol:
    const int current_position = watcher->end_index;
    const char current_symbol = source_code[current_position];
    //Check current symbol:
    const_boolean is_whitespace_cond = is_space_or_line_break(current_symbol);
    const_boolean is_close_parenthesis_cond = is_closing_parenthesis(current_symbol);
    const_boolean is_letter_cond = isalpha(current_symbol);
    const_boolean is_letter_or_number_or_close_parenthesis_cond = isalnum(current_symbol) || is_close_parenthesis_cond;
    const_boolean is_close_brace_cond = is_closing_brace(current_symbol);
    if (is_whitespace_cond && watcher->mode == POINT_ACTIVE_MODE) {
        watcher->pointer++;
        return;
    }
    if (((is_letter_cond && watcher->pointer > 0) || is_close_brace_cond)
        && watcher->mode == POINT_ACTIVE_MODE) {
        watcher->end_index = watcher->end_index - watcher->pointer;
        spider->message = AIO_SPIDER_IS_READY_FOR_WEAVING;
    } else {
        watcher->mode = POINT_PASSIVE_MODE;
        watcher->pointer = 0;
        if (is_letter_or_number_or_close_parenthesis_cond) {
            watcher->mode = POINT_ACTIVE_MODE;
            return;
        }
    }
}

void weave_procedure_instruction_for(aio_instruction_holder *holder, const_string source_code,
                                     point_watcher *ripper_watcher, struct aio_spider *spider) {
#ifdef AIO_PROCEDURE_SPIDER_DEBUG
    log_info(AIO_PROCEDURE_SPIDER_TAG, "Start weaving...");
#endif
    //재료들을 추출하다 (Extract materials):
    const aio_procedure_materials *materials = spider->get.procedure_materials;
    const point_watcher *watcher = materials->watcher;
    //Change ripper_watcher:
    const int new_point = watcher->end_index;
    ripper_watcher->pointer = new_point;
    ripper_watcher->start_index = new_point;
    string dirty_expression = substring_by_point_watcher(source_code, watcher);
    string clean_expression = squeeze_string(dirty_expression);
#ifdef AIO_PROCEDURE_SPIDER_DEBUG
    log_info_string(AIO_PROCEDURE_SPIDER_TAG, "CAPTURED EXPRESSION:", clean_expression);
#endif
    aio_instruction *procedure_instruction = new_aio_procedure_instruction(holder, clean_expression);
    //명부에게 지침을 추가하다 (Add 'Procedure' instruction in holder's instructions):
    aio_instruction_list *instruction_list = holder->instruction_list;
    add_aio_instruction_in_list(instruction_list, procedure_instruction);
    //위빙이 완료되었습니다 (Weaving complete)!
#ifdef AIO_PROCEDURE_SPIDER_DEBUG
    log_info(AIO_PROCEDURE_SPIDER_TAG, "WEAVED INSTRUCTION:");
    log_info_string(AIO_PROCEDURE_SPIDER_TAG, "EXPRESSION:", procedure_instruction->get.procedure_task->expression);
#endif
    //------------------------------------------------------------------------------------------------------------------
    //찌거기 수집기 (Garbage collector):
    free(dirty_expression);
}