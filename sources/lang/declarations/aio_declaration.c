#include <malloc.h>
#include <mem.h>
#include <stdio.h>
#include <process.h>
#include "../../../headers/lang/declarations/aio_declaration.h"

//Passed JUnitTest!
aio_declaration* new_aio_declaration(char *method_name, string_list *args) {
    //Create declaration:
    aio_declaration* declaration = calloc(1, sizeof(aio_declaration));
    if (declaration == NULL){
        perror("cannot allocate memory for aio_declaration");
        exit(1);
    }
    //Set method name:
    declaration->methodName = method_name;
    //Set args:
    declaration->argList = args;
    return declaration;
}