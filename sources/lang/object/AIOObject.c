#include <mem.h>
#include <malloc.h>
#include <stdio.h>
#include <process.h>
#include "../../../headers/lang/object/objectManager/AIOObjectManager.h"
#include "../../../headers/lang/methods/methodDefinition/aio_method_definition_builder.h"
#include "../../../headers/lang/methods/AIOMethod.h"
#include "../../../headers/lib/utils/fileUtils/FileUtils.h"

AIOObjectManager *aio_object_manager;

//Passed JUnitTest!
void createAIOMethodManager(AIOMethodManager **methodManager, AIOMethodDefinitionMap *methodDefinitionMap) {
    //Create the same method manager:
    *methodManager = calloc(1, sizeof(AIOMethodManager));
    if (*methodManager == NULL) {
        perror("cannot allocate memory for aio method manager!");
        exit(1);
    }
    //Set method definition map:
    (*methodManager)->methodDefinitionMap = methodDefinitionMap;
    //Set boolean "has main": (default false)
    (*methodManager)->hasMain = calloc(1, sizeof(int));
    if ((*methodManager)->hasMain == NULL) {
        perror("cannot allocate memory for hasMain property in aio method manager!");
        exit(1);
    }
    *(*methodManager)->hasMain = -1;
}

//Passed JUnitTest!
void findMethodsInManager(aio_object *aioObject) {
    for (int i = 0; i < *aioObject->sourceCode->size; ++i) {
        char *line = aioObject->sourceCode->strings[i];
        int length = strlen(line);
        if (length > 1) {
            //starts with @
            if (line[0] == '@') {
                unsigned nameSize = 1;
                char pointer;
                for (int j = 1; j < length; ++j) {
                    if (line[j] == ' ') {
                        if (j == 1) {
                            perror("incorrect method name @");
                        }
                        break;
                    }
                    nameSize = nameSize + 1;
                }
                char *methodName = calloc(nameSize + 1, sizeof(char));
                for (int k = 0; k < nameSize; ++k) {
                    methodName[k] = line[k];
                }
                if (strcmp(methodName, "@main") == 0) {
                    *aioObject->methodManager->hasMain = 0;
                    printf("HAS MAIN: %d\n", *aioObject->methodManager->hasMain);
                }
                aio_method_definition *methodDefinition = build_aio_method_definition(methodName, aioObject->sourceCode,
                                                                                      i);
                putAIOMethodDefinitionInMap(aioObject->methodManager->methodDefinitionMap, methodDefinition);
            }
        }
    }
}

#define CHUNK 1024

//Path example:
//"../aioPrograms/test.txt", "r"
//Passed JUnitTest!
void loadSourceCodeInAIOObject(aio_object *object, char *path) {
    printf("Loading source code...\n");
    //Create source code mutable list:
    string_list *sourceCode;
    new_string_list(&sourceCode);
    //Create file:
    FILE *file;
    //Create line buffer:
    char buffer[CHUNK];
    if ((file = fopen(path, "r")) == NULL) {
        perror("cannot open source-file");
    }
    while (fgets(buffer, sizeof(buffer), file) != NULL) {
        buffer[strlen(buffer) - 1] = '\0';
        //Create string:
        char *line = calloc(1, CHUNK);
        strcpy(line, buffer);
        //put string in list:
        add_in_string_list(sourceCode, line);
    }
    fclose(file);
    //Set source code:
    object->sourceCode = sourceCode;
}

//Passed JUnitTest!
void createAIOObject(aio_object **object, AIOMethodManager *methodManager, char *path) {
    //Create the same object:
    *object = malloc(sizeof(aio_object));
    //Set method manager:
    (*object)->methodManager = methodManager;
    //Set name from path:
    string_pair *nameVsFolder = extract_name_and_folder_path_from_path(path);
    (*object)->name = nameVsFolder->first;
    //Set folder path from path:
    (*object)->folderPath = nameVsFolder->second;
    //Loading code:
    loadSourceCodeInAIOObject(*object, path);
    findMethodsInManager(*object);
}

void invokeMethodInManager(aio_object *object, char *methodName, aio_bundle *bundle) {
    aio_method_definition *methodDefinition = getAIOMethodDefinitionInMapByName(
            object->methodManager->methodDefinitionMap, methodName);
    if (methodDefinition->declaration != NULL) {
        if (*methodDefinition->declaration->argList->size != *bundle->input_values->size) {
            perror("number of args not matches with arg size of declaration!");
        }
    }
    AIOMethod *method;
    createAIOMethodAndInvoke(object, &method, methodDefinition, bundle);
}