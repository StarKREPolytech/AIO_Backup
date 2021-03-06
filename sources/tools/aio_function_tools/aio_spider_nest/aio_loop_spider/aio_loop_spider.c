#include <malloc.h>
#include <mem.h>
#include "../../../../../headers/lib/utils/boolean_utils/boolean_utils.h"
#include "../../../../../headers/lib/utils/error_utils/error_utils.h"
#include "../../../../../headers/lib/utils/char_utils/char_utils.h"
#include "../../../../../headers/tools/aio_common_tools/aio_spider_nest/aio_spider.h"
#include "../../../../../headers/tools/aio_function_tools/aio_instruction_spider_nest/aio_loop_spider/aio_loop_spider.h"
#include "../../../../../headers/tools/aio_common_tools/aio_block_body_explorer/aio_block_body_explorer.h"
#include "../../../../../headers/lib/utils/memory_utils/memory_utils.h"
#include "../../../../../headers/tools/aio_function_tools/aio_instructions/aio_tasks/aio_assign_task.h"
#include "../../../../../headers/lib/utils/str_hook/str_hook_utils/str_hook_utils.h"
#include "../../../../../headers/tools/aio_function_tools/aio_instructions/aio_tasks/aio_loop_task.h"
#include "../../../../../headers/lib/utils/point_watcher/point_watcher.h"
#include "../../../../../headers/lib/utils/string_utils/string_utils.h"
#include "../../../../../headers/tools/aio_common_tools/aio_spider_nest/aio_function_instruction_spider_nest.h"
#include "../../../../../headers/tools/aio_function_tools/aio_instructions/aio_function_instruction_holder.h"
#include "../../../../../headers/tools/aio_function_tools/aio_instructions/aio_function_instruction.h"
#include "../../../../../headers/lang/aio_function/aio_variable/aio_definition/aio_variable_definition.h"
#include "../../../../../headers/lib/utils/str_hook/str_hook.h"

#define AIO_NUMBER_OF_SPIDERS 1 //3

/**
 * 주 논리 (Business logic).
 */

#define AIO_LOOP_SPIDER_DEBUG

#define AIO_LOOP_SPIDER_TAG "AIO_LOOP_SPIDER"

#ifdef AIO_LOOP_SPIDER_DEBUG

#include "../../../../../headers/lib/utils/log_utils/log_utils.h"

#endif

void refresh_loop_spider(aio_spider *spider, point_watcher *ripper_watcher)
{
    //Refresh main materials:
    aio_loop_materials *materials = spider->materials;
    materials->scope_type = AIO_LOOP_MODIFIER_SCOPE;
    materials->main_watcher->start = ripper_watcher->pointer;
    materials->main_watcher->end = ripper_watcher->pointer;
    materials->main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
    reset_point_watcher(materials->header_watcher);
    reset_point_watcher(materials->body_watcher);
    spider->message = AIO_SPIDER_NOT_FOUND_MATERIALS;
//#ifdef AIO_LOOP_SPIDER_DEBUG
//    log_info(AIO_LOOP_SPIDER_TAG, "Refresh is complete!");
//#endif
}

/**
 * 거미를 비우다 (Free spider).
 */

void free_loop_spider(aio_spider *spider)
{
    aio_loop_materials *materials = spider->materials;
    //Free applied header materials:
    const aio_loop_header_material_type header_material_type = materials->applied_header_material_type;
    if (header_material_type == AIO_LOOP_MATERIALS_DEFAULT_HEADER) {
        aio_default_loop_header_materials *default_header_materials = materials->get
                .default_loop_header;
        free(default_header_materials);
    }
    if (header_material_type == AIO_LOOP_MATERIALS_IN_HEADER) {

    }
    if (header_material_type == AIO_LOOP_MATERIALS_TINY_HEADER) {

    }
    //Free main materials:
    free_point_watcher(materials->main_watcher);
    free_point_watcher(materials->header_watcher);
    free_point_watcher(materials->body_watcher);
    free(materials);
    free(materials);
    free(spider);
}

/**
 * 건설자 (Constructor).
 */

aio_spider *new_aio_loop_spider(point_watcher *ripper_watcher)
{
    aio_spider *spider = new_object(sizeof(aio_spider));
    //함수들을 놓다 (Put functions):
    spider->refresh = refresh_loop_spider;
    spider->is_found_context = is_found_loop_instruction;
    spider->weave_context_for = weave_loop_instruction_for;
    spider->free = free_loop_spider;
    //재료들을 만들다 (Create materials):
    aio_loop_materials *materials = new_object(sizeof(aio_loop_materials));
    materials->scope_type = AIO_LOOP_MODIFIER_SCOPE;
    materials->main_watcher = new_point_watcher();
    materials->main_watcher->start = ripper_watcher->start;
    materials->main_watcher->end = ripper_watcher->pointer;
    materials->header_watcher = new_point_watcher();
    materials->body_watcher = new_point_watcher();;
    //재료들을 놓다 (Set materials):
    spider->materials = materials;
    spider->message = AIO_SPIDER_NOT_FOUND_MATERIALS;
    return spider;
}

const aio_spider_message is_found_loop_instruction(
        const_string source_code,
        point_watcher *ripper_watcher,
        aio_spider *spider
)
{
    //재료들을 추출하다 (Extract materials):
    const aio_loop_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    main_watcher->end = ripper_watcher->pointer;
    const char current_symbol = source_code[main_watcher->end];
#ifdef AIO_LOOP_SPIDER_DEBUG
    //log_info_char(AIO_LOOP_SPIDER_TAG, "Current symbol:", symbol);
#endif
    //TODO: 코드 복제 (Code duplication)!
    if (main_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
        if (is_space_or_line_break(current_symbol)) {
            main_watcher->start++;
        } else {
            main_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
        }
    }
    if (main_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
        if (materials->scope_type == AIO_LOOP_MODIFIER_SCOPE) {
            handle_loop_modifier_scope(source_code, spider);
        }
        if (materials->scope_type == AIO_LOOP_HEADER_SCOPE) {
            handle_loop_header_scope(source_code, spider);
        }
        if (materials->scope_type == AIO_LOOP_BODY_SCOPE) {
            handle_loop_body_scope(source_code, spider);
        }
    }
    return spider->message;
}

void handle_loop_modifier_scope(const_string source_code, aio_spider *spider)
{
    aio_loop_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    const int current_position = main_watcher->end;
    const char current_symbol = source_code[current_position];
    //Check current symbol:
    const_boolean is_opening_parenthesis_cond = is_opening_parenthesis(current_symbol);
    const_boolean is_whitespace_cond = is_space_or_line_break(current_symbol);
    if (is_whitespace_cond || is_opening_parenthesis_cond) {
        const int start_index = main_watcher->start;
        const int end_index = main_watcher->end;
        const int hold_positions = end_index - start_index;
        if (hold_positions == 3) {
            const_boolean is_loop_modifier =
                    source_code[start_index] == 'l'
                    && source_code[start_index + 1] == 'o'
                    && source_code[start_index + 2] == 'o';
            if (is_loop_modifier) {
                //Shift main_watcher:
                main_watcher->start = end_index;
                main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
                //Set scope:
                materials->scope_type = AIO_LOOP_HEADER_SCOPE;
                //Set message:
                spider->message = AIO_SPIDER_FOUND_MATERIALS;
#ifdef AIO_LOOP_SPIDER_DEBUG
                log_info(AIO_LOOP_SPIDER_TAG, "Found loop modifier!");
#endif
            }
        }
    }
}

void handle_loop_header_scope(const_string source_code, aio_spider *spider)
{
    aio_loop_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    point_watcher *header_watcher = materials->header_watcher;
    //Define last position:
    const int current_position = main_watcher->end;
    const char current_symbol = source_code[current_position];
    //Check symbol:
    const_boolean is_whitespace_cond = is_space_or_line_break(current_symbol);
    const_boolean is_opening_parenthesis_cond = is_opening_parenthesis(current_symbol);
    const_boolean is_closing_parenthesis_cond = is_closing_parenthesis(current_symbol);
    //Meet opening parenthesis:
    if (is_opening_parenthesis_cond) {
        //Start of loop header:
        if (header_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
            //Start with opening parenthesis:
            header_watcher->start = main_watcher->end;
            header_watcher->mode = POINT_WATCHER_ACTIVE_MODE;
        }
        //Parenthesis inside loop header:
        if (header_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
            //Count parentheses:
            header_watcher->pointer++;
        }
    }
    //Meet closing parenthesis:
    if (is_closing_parenthesis_cond) {
        //Loop header doesn't start with closing parenthesis:
        if (header_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
            throw_error_with_tag(AIO_LOOP_SPIDER_TAG, "Loop condition can not start with open parenthesis!");
        }
        //Inside loop header:
        if (header_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
            header_watcher->pointer--;
            //Parenthesis closes header:
            if (header_watcher->pointer == 0) {
                //End of condition:
                header_watcher->end = main_watcher->end + 1;
                //Shift main main_watcher:
                main_watcher->start = main_watcher->end + 1;
                main_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
                //Set scope:
                materials->scope_type = AIO_LOOP_BODY_SCOPE;
#ifdef AIO_LOOP_SPIDER_DEBUG
                const_string header_string = substring_by_point_watcher(source_code, header_watcher);
                log_info_string(AIO_LOOP_SPIDER_TAG, "Captured header:", header_string);
                free((void *) header_string);
#endif
                //Dig header materials:
                dig_header_materials(source_code, spider);
            }
        }
        return;
    }
    //Skip whitespaces before condition:
    if (!is_whitespace_cond && header_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
        throw_error_with_tag(AIO_LOOP_SPIDER_TAG, "Invalid context_ref after 'loo' modifier!");
    }
}

//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
//HEADER ANALYSING START:

static void handle_weaving(
        aio_spider *child_spider,
        aio_spider *parent_spider,
        const_string source_code,
        point_watcher *header_watcher,
        aio_spider_nest *child_spider_nest
)
{
#ifdef AIO_LOOP_SPIDER_DEBUG
    log_info(AIO_LOOP_SPIDER_TAG, "START WE-WE-WE");
#endif
    //Spider takes current holder and weave for holder instruction:
    child_spider->weave_context_for(parent_spider, source_code, header_watcher, child_spider);
    //Reset spiders:
    refresh_aio_spider_nest(child_spider_nest, header_watcher);
    header_watcher->mode = POINT_WATCHER_PASSIVE_MODE;
    //거미 무리 리셋 (Spider nest reset):
    child_spider_nest->mode = AIO_ALL_SPIDERS_WORK;
    child_spider_nest->active_spider = NULL;
#ifdef AIO_LOOP_SPIDER_DEBUG
    log_info(AIO_LOOP_SPIDER_TAG, "All loop header spiders work:");
#endif
}

void dig_header_materials(const_string source_code, aio_spider *parent_spider)
{
#ifdef AIO_LOOP_SPIDER_DEBUG
    log_info(AIO_LOOP_SPIDER_TAG, "Dig header materials...");
#endif
    aio_loop_materials *materials = parent_spider->materials;
    point_watcher *header_watcher = materials->header_watcher;
    //Header main_watcher pointer is already useless. Thus we can use pointer again!
    header_watcher->pointer = header_watcher->start + 1;
    const_boolean is_not_empty_header = header_watcher->end - header_watcher->start > 0;
    if (is_not_empty_header) {
        //Create spider swarm for searching instructions:
        aio_spider_nest *child_spider_nest = breed_aio_loop_header_spider_nest(header_watcher);
        //After weaving instruction need from check function body string rest:
        while (header_watcher->pointer < header_watcher->end) {
            if (header_watcher->mode == POINT_WATCHER_PASSIVE_MODE) {
                if (has_context_rest(source_code, header_watcher)) {
                    break;
                } else {
                    refresh_aio_spider_nest(child_spider_nest, header_watcher);
                }
            }
            //Active mode for spider nest:
            if (header_watcher->mode == POINT_WATCHER_ACTIVE_MODE) {
                const aio_spider_nest_mode swarm_mode = child_spider_nest->mode;
                if (swarm_mode == AIO_ALL_SPIDERS_WORK) {
                    for (int j = 0; j < AIO_NUMBER_OF_SPIDERS; ++j) {
                        aio_spider *child_spider = child_spider_nest->spiders[j];
                        //Spider try from match "string web" with it task:
                        const aio_spider_message message
                                = child_spider->is_found_context(source_code, header_watcher, child_spider);
                        if (message == AIO_SPIDER_FOUND_MATERIALS) {
#ifdef AIO_LOOP_SPIDER_DEBUG
                            log_info(AIO_LOOP_SPIDER_TAG, "One loop header spider works:");
#endif
                            child_spider_nest->active_spider = child_spider;
                            child_spider_nest->mode = AIO_ONE_SPIDER_WORKS;
                            break;
                        }
                        if (message == AIO_SPIDER_IS_READY_FOR_WEAVING) {
                            handle_weaving(child_spider, parent_spider, source_code, header_watcher, child_spider_nest);
                        }
                    }
                }
                if (swarm_mode == AIO_ONE_SPIDER_WORKS) {
                    aio_spider *child_spider = child_spider_nest->active_spider;
                    const aio_spider_message message
                            = child_spider->is_found_context(source_code, header_watcher, child_spider);
                    if (message == AIO_SPIDER_IS_READY_FOR_WEAVING) {
                        handle_weaving(child_spider, parent_spider, source_code, header_watcher, child_spider_nest);
                    }
                }
            }
            header_watcher->pointer++;
        }
        //--------------------------------------------------------------------------------------------------------------
        //찌꺼기 수집기 (Garbage collector):
        free_aio_spider_nest(child_spider_nest);
    } else {
        throw_error_with_tag(AIO_LOOP_SPIDER_TAG, "Loop header is empty!");
    }
}

struct aio_spider_nest *breed_aio_loop_header_spider_nest(point_watcher *header_watcher)
{
    //Create spiders:
    aio_spider **spiders = new_object_array(AIO_NUMBER_OF_SPIDERS, sizeof(struct aio_spider *));
    spiders[0] = new_aio_default_loop_header_spider(header_watcher);
    //spiders[1] = new_aio_in_loop_header_spider();
    //spiders[2] = new_aio_tiny_loop_header_spider();
    aio_spider_nest *nest = new_object(sizeof(aio_spider_nest));
    nest->number_of_spiders = AIO_NUMBER_OF_SPIDERS;
    nest->spiders = spiders;
    nest->active_spider = NULL;
    nest->mode = AIO_ALL_SPIDERS_WORK;
    return nest;
}

//HEADER ANALYSING END:
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------

void handle_loop_body_scope(const_string source_code, aio_spider *spider)
{
#ifdef AIO_LOOP_SPIDER_DEBUG
    log_info(AIO_LOOP_SPIDER_TAG, "Searching for loop block body...");
#endif
    aio_loop_materials *materials = spider->materials;
    point_watcher *main_watcher = materials->main_watcher;
    point_watcher *body_watcher = materials->body_watcher;
    body_watcher->start = main_watcher->start;
    explore_aio_block_bounds(source_code, &body_watcher->start, &body_watcher->end);
#ifdef AIO_LOOP_SPIDER_DEBUG
    const_string explored_body = substring_by_point_watcher(source_code, body_watcher);
    log_info_string(AIO_LOOP_SPIDER_TAG, "EXPLORED BODY:", explored_body);
    free((void *) explored_body);
#endif
    //Ready for weaving:
    materials->scope_type = AIO_LOOP_WEAVING_SCOPE;
    spider->message = AIO_SPIDER_IS_READY_FOR_WEAVING;
}

void weave_loop_instruction_for(
        void *parent,
        const_string source_code,
        point_watcher *ripper_watcher,
        aio_spider *spider
)
{
    aio_function_instruction_holder *holder = parent;
#ifdef AIO_LOOP_SPIDER_DEBUG
    log_info(AIO_LOOP_SPIDER_TAG, "Start weaving...");
#endif
    //재료들을 추출하다 (Extract materials):
    const aio_loop_materials *main_loop_materials = spider->materials;
    const_boolean is_ready_for_weaving = main_loop_materials->scope_type == AIO_LOOP_WEAVING_SCOPE;
    if (is_ready_for_weaving) {
        const point_watcher *body_watcher = main_loop_materials->body_watcher;
        //Shift ripper watcher:
        ripper_watcher->pointer = body_watcher->end - 1;
        ripper_watcher->start = body_watcher->end - 1;
        //Create init holder:
        aio_function_instruction_holder *init_holder = new_aio_function_instruction_holder(parent);
        aio_function_instruction_list *init_instruction_list = init_holder->instruction_list;
        aio_variable_definition_list *init_variable_definition_list = init_holder->variable_definition_list;
        //Define cycle holder:
        aio_function_instruction_holder *cycle_holder = NULL;
        //Define loop condition:
        string loop_condition = NULL;
        //Fill holders:
        const aio_loop_header_material_type header_material_type = main_loop_materials->applied_header_material_type;
        if (header_material_type == AIO_LOOP_MATERIALS_DEFAULT_HEADER) {
            //Extract applied materials:
            const aio_default_loop_header_materials *default_loop_header_materials
                    = main_loop_materials->get.default_loop_header;
            if (!default_loop_header_materials) {
                throw_error_with_tag(AIO_LOOP_SPIDER_TAG, "Default loop header spider can not weave materials!");
            }
            const aio_default_loop_header_pointer_declaration_type declaration_type
                    = default_loop_header_materials->declaration_type;
#ifdef AIO_LOOP_SPIDER_DEBUG
            log_info(AIO_LOOP_SPIDER_TAG, "DEFAULT LOOP HEADER MATERIALS...");
            log_info_string(AIO_LOOP_SPIDER_TAG, "CONDITION:", default_loop_header_materials->loop_condition);
#endif
            //Set condition:
            loop_condition = new_string(default_loop_header_materials->loop_condition);
            //Check pointer in header:
            const_str_hook_list *pointer_data_list = default_loop_header_materials->pointer_data_list;
            const_boolean has_pointer_in_header = pointer_data_list->size > 0;
            const_str_hook *pointer_name = NULL;
            if (has_pointer_in_header) {
                const_str_hook_array pointer_data_array = pointer_data_list->hooks;
                const_string init_value = default_loop_header_materials->init_value;
                //Create pointer definition:
                pointer_name = get_default_loop_pointer_name_by_materials(
                        default_loop_header_materials
                );
                const_aio_variable_definition *definition = get_aio_variable_definition_in_function_tree(
                        pointer_name,
                        parent
                );
                if (definition == NULL) {
                    definition = create_pointer_variable_definition_by_default_loop_header_spider(
                            declaration_type,
                            pointer_data_array
                    );
                    aio_function_instruction *init_instruction = new_aio_assign_instruction(
                            init_holder,
                            init_value,
                            new_str_hook_by_other(pointer_name)
                    );
                    //Put definition in init map:
                    add_aio_variable_definition_in_list(init_variable_definition_list, definition);
#ifdef AIO_LOOP_SPIDER_DEBUG
                    log_info_str_hook(AIO_LOOP_SPIDER_TAG, "ADDED POINTER DEFINITION IN LIST:", definition->name);
#endif
                    //Put init instruction in init holder:
                    add_aio_instruction_in_list(init_instruction_list, init_instruction);
                } else {
                    throw_error_with_tag(
                            AIO_LOOP_SPIDER_TAG,
                            "Invalid loop pointer name! Variable name already exists!"
                    );
                }
            }
            //Create cycle holder:
            cycle_holder = inflate_local_aio_instruction_holder(
                    source_code,
                    init_holder,
                    body_watcher->start,
                    body_watcher->end
            );
            aio_function_instruction_list *cycle_instruction_list = cycle_holder->instruction_list;
            if (has_pointer_in_header) {
                const_string step_value = default_loop_header_materials->step_value;
#ifdef AIO_LOOP_SPIDER_DEBUG
                log_info_string(AIO_LOOP_SPIDER_TAG, "CHECK STEP:", step_value);
#endif
                //If has step value then add step instruction in the bottom of cycle instruction:
                if (step_value) {
                    aio_function_instruction *step_instruction = new_aio_assign_instruction(
                            cycle_holder,
                            step_value,
                            new_str_hook_by_other(pointer_name)
                    );
                    //Put in the bottom step instruction in cycle holder:
                    add_aio_instruction_in_list(cycle_instruction_list, step_instruction);
                }
            }
        }
        if (header_material_type == AIO_LOOP_MATERIALS_IN_HEADER) {

        }
        if (header_material_type == AIO_LOOP_MATERIALS_TINY_HEADER) {

        }
        //Create loop instruction:
        aio_function_instruction *loop_instruction = new_aio_loop_instruction(
                parent,
                loop_condition,
                init_holder,
                cycle_holder
        );
        aio_function_instruction_list *parent_instruction_list = holder->instruction_list;
        //Weave 'Loop' instruction:
        add_aio_instruction_in_list(parent_instruction_list, loop_instruction);
        //위빙이 완료되었습니다 (Weaving complete)!
#ifdef AIO_LOOP_SPIDER_DEBUG
        log_info(AIO_LOOP_SPIDER_TAG, "WEAVED INSTRUCTION:");
        log_info_string(AIO_LOOP_SPIDER_TAG, "CONDITION:", loop_instruction->get.loop_task->loop_condition);
#endif
    } else {
        throw_error_with_tag(AIO_LOOP_SPIDER_TAG, "Not ready for weaving!");
    }
}