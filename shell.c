#include <sys/types.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>
#include <string.h>
#include <stdlib.h>
#include <ctype.h>

#define MAX_ARGS 6

// Structs
typedef struct aliasNode {
    char *alias;
    char *command;
    struct aliasNode *next;
} aliasNode;

typedef struct aliasList {
    aliasNode *head;
} aliasList;

aliasNode* createAliasNode(char *alias, char *command);
void printAliasList(aliasList *list);
aliasNode* findAlias(aliasList *list, char *alias);
void removeAlias(aliasList *list, char *alias);
void insertAndCheckAlias(aliasList *list, char *alias, char *command);
void processAliasesCases(char **args, aliasList *alias_list, int *numOfAliasas, int *numOfCmd);
char** handle_regular_command(char* words[], const char* input, int* word_count);
void handle_alias_command(char* args[], const char* input, int* word_count);
void handle_unalias_command(char* args[], const char* input, int* word_count);
void printPrompt(int *numOfCmd, int *numOfAliasas, int *numOfScriptLines);
void captureString(char *command);
void createProcess(char *args[], int *numOfCmd, aliasList *alias_list);
void handleSourceCommand(char **args, aliasList *alias_list, int *numOfAliasas, int *numOfCmd, int *numOfScriptLines);
void handleScript(const char *filename, aliasList *alias_list, int *numOfAliasas, int *numOfCmd, int *numOfScriptLines);
void checkMalloc(void *ptr);
void freeAliasList(aliasList *list);
void freeCounters(int **numOfCmd, int **numOfAliasas, int **numOfScriptLines, int **numOfParantass);
void ifExit(char *args[], int *numOfParantass, aliasList *alias_list, int *numOfAliasas, int *numOfScriptLines, int *numOfCmd);
void ifSource(char *args[], int *numOfScriptLines);

// Main
int main() {
    // Mallocs for the counters && the Alias list
    int *numOfCmd = (int *) malloc(sizeof(int));
    int *numOfAliasas = (int *) malloc(sizeof(int));
    int *numOfScriptLines = (int *) malloc(sizeof(int));
    int *numOfParantass = (int *) malloc(sizeof(int));

    aliasList *alias_list = (aliasList *) malloc(sizeof(aliasList));

    // Checks if malloc failed
    checkMalloc(numOfCmd);
    checkMalloc(numOfAliasas);
    checkMalloc(numOfScriptLines);
    checkMalloc(numOfParantass);
    checkMalloc(alias_list);

    // Initialize the counters
    *numOfCmd = 0;
    *numOfAliasas = 0;
    *numOfScriptLines = 0;
    *numOfParantass = 0;

    // Initialize the alias list
    alias_list->head = NULL;

    // Command input string
    char command [1024];

    // Command arguments
    char *args[MAX_ARGS];
    int wordCount = 0;

    // Linux Shell loop
    while (1) {
        for (int i = 0; i < MAX_ARGS; i++) {
            args[i] = NULL;
        }
        printPrompt(numOfCmd, numOfAliasas, numOfScriptLines);
        captureString(command);

        // Check if the input string contains alias or unalias
        if (strstr(command, "unalias")) { // Found substring in input of unalias
            handle_unalias_command(args, command, &wordCount);
            processAliasesCases(args, alias_list, numOfAliasas, numOfCmd);
        } else
        if (strstr(command, "alias")) { // Found substring in input of alias
            handle_alias_command(args, command, &wordCount);
            processAliasesCases(args, alias_list, numOfAliasas, numOfCmd);
        } else { // Handle a regular command
            handle_regular_command(args, command, &wordCount);
            if (strcmp(args[0], "source") == 0) {
                handleSourceCommand(args, alias_list, numOfAliasas, numOfCmd, numOfScriptLines);
            } else {
                createProcess(args, numOfCmd, alias_list);
            }
        }
        ifExit(args, numOfParantass, alias_list, numOfAliasas, numOfScriptLines, numOfCmd); // Check for exit command
    }
    return 0;
}

// Alias Functions
aliasNode* createAliasNode(char *alias, char *command){
    aliasNode* newNode = (aliasNode*)malloc(sizeof(aliasNode));
    newNode->alias = strdup(alias);
    newNode->command = strdup(command);
    newNode->next = NULL;
    return newNode;
}
void printAliasList(aliasList *list){
    aliasNode* current = list->head;
    while (current != NULL){
        printf("%s: %s\n", current->alias, current->command);
        current = current->next;
    }
}
aliasNode* findAlias(aliasList *list, char *alias){
    aliasNode* current = list->head;
    while (current != NULL){ // Traverse the list
        if (strcmp(current->alias, alias) == 0){ // If alias is found
            return current; // Return the node
        }
        current = current->next;
    }
    return NULL;
}
void removeAlias(aliasList *list, char *alias){
    aliasNode* current = list->head;
    aliasNode* prev = NULL;
    while (current != NULL){
        if (strcmp(current->alias, alias) == 0){
            if (prev == NULL){
                list->head = current->next;
            } else {
                prev->next = current->next;
            }
            free(current->alias);
            free(current->command);
            free(current);
            return;
        }
        prev = current;
        current = current->next;
    }
}
void insertAndCheckAlias(aliasList *list, char *alias, char *command){
    // Check if the alias already exists
    aliasNode* existingAlias = findAlias(list, alias);
    if (existingAlias != NULL){
        // If the alias exists, update the command
        free(existingAlias->command);
        existingAlias->command = strdup(command);
    } else {
        // If the alias does not exist, add a new alias
        aliasNode* newNode = createAliasNode(alias, command);
        newNode->next = list->head;
        list->head = newNode;
    }
}
void processAliasesCases(char **args, aliasList *alias_list, int *numOfAliasas, int *numOfCmd) {
    if (strcmp(args[0], "alias") == 0 && args[1] == NULL) { // Print list
        printAliasList(alias_list);
    } else
    if (strcmp(args[0], "alias") == 0 && args[1] != NULL) { // Add alias
        // We check in main if the prompt is alias; if it is, add the alias
        insertAndCheckAlias(alias_list, args[1], args[2]);
        *numOfAliasas += 1;
        *numOfCmd += 1; // Increment command counter for successful alias addition
    } else
    if (strcmp(args[0], "unalias") == 0) { // Remove alias
        if (args[1] != NULL) { // Check if alias name is provided
            aliasNode* aliasToRemove = findAlias(alias_list, args[1]);
            if (aliasToRemove != NULL) {
                removeAlias(alias_list, args[1]);
                *numOfAliasas -= 1;
                *numOfCmd += 1; // Increment command counter for successful unalias
            } else {
                printf("unalias: %s: not found\n", args[1]);
            }
        } else {
            printf("ERR: 'unalias name'\n");
        }
    }
}


// Scan and set command
char** handle_regular_command(char* words[], const char* input, int* word_count) {
    int len = strlen(input);
    int count = 0;

    char* i = input;
    while (*i) {
        while (isspace(*i)) i++;
        if (*i == 0) break;

        char quote_char = '\0';
        if (*i == '\'' || *i == '"') {
            quote_char = *i;
            i++;
        }

        char* j = i;
        if (quote_char) {
            while (*j && *j != quote_char) j++;
            if (*j == quote_char) {
                int word_length= j - i;
                words[count] = (char*)malloc((word_length + 1) * sizeof(char));
                if (!words[count]) {
                    perror("malloc");
                    exit(1);
                }
                strncpy(words[count], i, word_length);
                words[count][word_length + 1] = '\0';
                count++;
                i = j + 1;
            }
            else
                printf("ERR\n");
        } else {
            while (*j && !isspace(*j)) j++;
            int word_length = j - i;
            words[count] = (char*)malloc((word_length + 1) * sizeof(char));
            if (!words[count]) {
                perror("malloc");
                exit(1);
            }
            strncpy(words[count],i, word_length);
            words[count][word_length + 1] = '\0';
            count++;
            i = j + 1;
        }
    }

    *word_count = count;
    return words;
}
void handle_alias_command(char* args[], const char* input, int* word_count){
    // Initialize all elements of args to NULL
    for (int i = 0; i < MAX_ARGS; i++) {
        args[i] = NULL;
    }
    // copy the first word in input to args[0]
    args[0] = "alias";
    char* s = input + 6;
    while(isspace( *s)) s++;
    char* e = s;
    while(*e && *e != '=' && *e != ' ') e++;
    if(*e == 0)
        return;
    int len = e - s;
    args[1] = (char*)malloc(len + 1);
    if (!args[1]) {
        perror("malloc");
        exit(1);
    }
    strncpy(args[1], s, len);
    args[1][len] = '\0';
    s = strchr(input, '=');
    if (s == NULL)
        return;// If token contains an equals sign
    s = strchr(input,'\''); // Skip the equals sign
    s++;
    e = strchr(s,'\'');
    len = e - s;
    args[2] = (char*)malloc(len + 1);
    if (!args[2]) {
        perror("malloc");
        exit(1);
    }
    strncpy(args[2], s, len);
    args[2][len] = '\0';
}
void handle_unalias_command(char* args[], const char* input, int* word_count) {
    for (int i = 0; i < MAX_ARGS; i++) {
        args[i] = NULL;
    }
    args[0] = strdup("unalias");

    // Skip the 'unalias' command in the input
    const char *s = input + strlen("unalias");
    while (isspace(*s)) s++;

    // Copy the alias name to args[1]
    if (*s != '\0') {
        args[1] = strdup(s);
        *word_count = 2;
    } else {
        *word_count = 1;
    }
}

// Print prompt
void printPrompt(int *numOfCmd, int *numOfAliasas, int *numOfScriptLines){ // Print the prompt
    printf("#cmd:%d|#alias:%d|#script lines:%d> ",
           *numOfCmd, *numOfAliasas,*numOfScriptLines);
}

// Capture string
void captureString(char *command){
    if((fgets(command, 1024, stdin)) == NULL){
        perror("fgets failed");
        exit(1);
    }
    // Remove the newline character at the end of the string
    command[strlen(command) - 1] = '\0';
}

// Fork child process to execute the command, and count the succeeded commands
void createProcess(char *args[], int *numOfCmd, aliasList *alias_list){
    pid_t pid;
    int status;

    // Look up the alias in the alias list
    aliasNode* alias = findAlias(alias_list, args[0]);
    if (alias != NULL) {
        // The alias exists, so we replace the alias name with the actual command
        char *aliasCommand = strdup(alias->command);
        char *tok = strtok(aliasCommand, " ");
        int i = 0;
        while(tok) {
            args[i] = tok;
            tok = strtok(NULL, " ");
            i++;
        }
    }
    pid = fork();
    if (pid < 0) {
        perror("fork failed");
        exit(1);
    } else if (pid == 0) {
        execvp(args[0], args);
        perror("exec failed");
        exit(1);
    } else {
        wait(&status);
        *numOfCmd += 1;
    }
}
// Function to handle the 'source' command
void handleSourceCommand(char **args, aliasList *alias_list, int *numOfAliasas, int *numOfCmd, int *numOfScriptLines) {
    // Check if args[0] equals "source" and args[1] is not NULL
    if (strcmp(args[0], "source") == 0 && args[1] != NULL) {
        // Call the handleScript function to execute the commands in the script file
        handleScript(args[1], alias_list, numOfAliasas, numOfCmd, numOfScriptLines);
    } else {
        printf("ERR\n");
    }
}

void handleScript(const char *filename, aliasList *alias_list, int *numOfAliasas, int *numOfCmd, int *numOfScriptLines) {
    // Check if the file has a .sh extension
    if (strstr(filename, ".sh") == NULL) {
        printf("ERR\n");
        return;
    }
    FILE *f = fopen(filename, "r");
    // Check if file open succeeded
    if (f == NULL) {
        printf("ERR\n");
        return;
    }
    // Check if first line in the file is #!/bin/bash
    char line[1025]; // Buffer for reading the first line
    if (fgets(line, 1025, f) == NULL) {
        printf("ERR\n");
        fclose(f);
        return;
    }
    if (strcmp(line, "#!/bin/bash\n") != 0) {
        printf("ERR\n");
        fclose(f);
        return;
    }

    // Read the rest of the file and execute each line
    while (fgets(line, 1024, f) != NULL) {
        (*numOfScriptLines)++; // Increment the script lines counter for each line
        line[strcspn(line, "\n")] = 0; // Remove the newline character at the end of the line

        // Command arguments
        char *args[MAX_ARGS];
        int wordCount = 0;

        // Check if the line contains alias or unalias
        if (strstr(line, "unalias")) {
            handle_unalias_command(args, line, &wordCount);
            processAliasesCases(args, alias_list, numOfAliasas, numOfCmd);
        } else
        if (strstr(line, "alias")) {
            handle_alias_command(args, line, &wordCount);
            processAliasesCases(args, alias_list, numOfAliasas, numOfCmd);
        } else {
            // Handle a regular command
            handle_regular_command(args, line, &wordCount);
            createProcess(args, numOfCmd, alias_list);
        }
    }
    fclose(f);
}


// Helpers ----------------------------------------------------------------------------

// Memory functions -------------------------------------------------------------------

// Checks if malloc failed
void checkMalloc(void *ptr){
    if (ptr == NULL){
        perror("malloc failed");
        exit(1);
    }
}

// Free the alias list
void freeAliasList(aliasList *list){
    aliasNode* current = list->head;
    aliasNode* next;
    while (current != NULL){
        next = current->next;
        free(current->alias);
        free(current->command);
        free(current);
        current = next;
    }
    free(list);
}

// Free the counters
void freeCounters(int **numOfCmd, int **numOfAliasas, int **numOfScriptLines, int **numOfParantass){
    free(*numOfCmd),free(*numOfAliasas),free(*numOfScriptLines),free(*numOfParantass);
    *numOfCmd = NULL,*numOfAliasas = NULL,*numOfScriptLines = NULL,*numOfParantass = NULL;
}
//----------------------------------------------------------------------------------------------------

// Checks if command  is "exit_shell" -> free the memory and exit the program
void ifExit(char *args[], int *numOfParantass, aliasList *alias_list, int *numOfAliasas, int *numOfScriptLines, int *numOfCmd){
    if (strcmp(args[0], "exit_shell") == 0) {
        printf("%d\n", *numOfParantass); // print the total number of lines of code executed
        freeCounters(&numOfCmd, &numOfAliasas, &numOfScriptLines, &numOfParantass);
        freeAliasList(alias_list);
        exit(1);
    }
}

// check if it's a source command
void ifSource(char *args[], int *numOfScriptLines){
    if (strcmp(args[0], "source") == 0) {
        //handleScript(args[1]);
        *numOfScriptLines += 1;
    }
}
