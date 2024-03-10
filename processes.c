#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <signal.h>
#include <string.h>
#include <sys/wait.h>
#include <limits.h>
#include <ctype.h>

//I did my best in this lab but I still can't get the things right.
//There are random things that are happenning and I don't know where is the problem.

typedef enum 
{
   CD = 0,
   ECHO,
   EXPORT,
   SHELL_BUILT_IN,
   EXECUTABLE_OR_ERROR,
   EXIT,
}COMMANDS_t;

char variables[100][30];
char values[100][50];
char final_replacement[100][30];
char final_output[100];
int var_counter = 0;

//Takes the message and saves it to the log file.
void write_to_log_file(char* message) {
    FILE *file;
    file = fopen("./logfile.txt", "a");
    if (file == NULL){
        printf("Failed to create / open the log file.\n");
        exit(1);
    }
    fputs(message, file);
    fclose(file);
    return;
}

//Empties the log file to start writing in it.
//This function is executed once at the start.
void clear_log_file(){
    FILE *file;
    file = fopen("./logfile.txt", "w");
    if (file == NULL){
        printf("Failed to create / open the log file.\n");
        exit(1);
    }
    fclose(file);
}

//I got the function from the complete code.
//function that return a char** containing the input from user.
int counter_args;
char** parse_input(){
    char input[100];
    memset(input, '\0', 100);
    fgets(input, 100, stdin);
    char *temp = strpbrk(input, "\n");
    temp[0] = '\0';
    fflush(stdin);

    char *garbage_buffer = NULL;
    char **final_input = NULL;
    int flag = 1;
    counter_args = 0;

    char *temp_input = NULL;
    temp_input = strtok(input, " ");
    if(temp_input != NULL)
    {
        counter_args = 1;
        final_input = (char **)calloc(counter_args, sizeof(char*));
        while (temp_input != NULL)
        {
            final_input = (char **)realloc(final_input, counter_args * sizeof(char *));
            *(final_input + counter_args - 1) = temp_input;
            
            if (flag)
            {
                temp_input = strtok(NULL, " ");
            }
            else
            {
                garbage_buffer = strtok(NULL, "\"");
                flag = 1;
                continue;
            }
            if ((temp_input != NULL) && (strchr(temp_input, '\"') != NULL) && flag)
            {
                temp_input[strlen(temp_input)] = ' ';
                flag = 0;
            }
            counter_args++;
        }
        counter_args--;
    }
    final_input = (char **)realloc(final_input, (counter_args + 1) * sizeof(char *));
    final_input[counter_args] = NULL;
    return final_input;
}

//function to replace a variable in the input.
void replace_string(char* input, int index, char* var, int num, int flag){
    for (int i=0; i<100; i++){
        final_output[i] = '\0';
    }
    int i = index;
    int j = 0;
    int k = 0;
    for (k; k<index; k++){
        final_output[k] = input[k];
    }
    //if (input[index] == '$'){
        //printf("%d\n",index);
        //i++;
        printf("%s\n", input);
        printf("%c\n", var[j]);
        while (input[i] == var[j] && j != strlen(var))
        {
            i++;
            j++;
        }
        printf("%d\n", i);
        if(j == strlen(var)){
            if (flag == 0){
                j = 0;
                while (j != strlen(values[num]))
                {
                    final_output[k] = values[num][j];
                    k++;
                    j++;
                    printf("^");
                }
            }
            while (i != strlen(input))
            {
                final_output[k] = input[i];
                i++;
                k++;
            }
            final_output[k] = '\0';
        }
    //}
    return;
}

void replace_substing(char *str, char *substr, char *replace, char *final)
{
    char output[30];
    int i = 0, j = 0, flag = 0, start = 0;
    int dolar_flag = 0;
    str[strlen(str)] = '\0';
    substr[strlen(substr)] = '\0';
    replace[strlen(replace)] =  '\0';
    // check whether the substring to be replaced is present 
    while (str[i] != '\0')
    {
        if(str[i] == '$')
        {
            dolar_flag = 1;
        }
        if (dolar_flag)
        {
            if (str[i] == substr[j]) 
            {
                if (!flag)
                    start = i;
                j++;
                if (substr[j] == '\0')
                {
                    flag = 1;
                    break;
                }
            } 
            else 
            {
                flag = start = j = 0;
            }
        }
        i++;
    }
    if (substr[j] == '\0' && flag)
    {
        j = 0;
        for (i = 0; i < start; i++)
        {
            if(str[i] != '$')
            {
                output[j] = str[i];
                j++;
            }
            else
            {
                break;
            }
        }
        i = j;
        // replace substring with another string 
        for (j = 0; j < strlen(replace); j++) 
        {
            output[i] = replace[j];
            i++;
        }
        // copy remaining portion of the input string "str" 
        for (j = start+1 ; j < strlen(str); j++)
        {
            output[i] = str[j];
            i++;
        }
        // print the final string 
        output[i] = '\0';
        strcpy(final, output);
    } 
    else 
    {
    }
}

void replaceWord(char* str, char* oldWord, char* newWord)
{
    char *pos, temp[1000];
    int index = 0;
    int owlen;
 
    owlen = strlen(oldWord);
 
    // Repeat This loop until all occurrences are replaced.
 
    while ((pos = strstr(str, oldWord)) != NULL) {
        // Bakup current line
        strcpy(temp, str);
 
        // Index of current found word
        index = pos - str;
 
        // Terminate str after word found index
        str[index] = '\0';
 
        // Concatenate str with new word
        strcat(str, newWord);
 
        // Concatenate str with remaining words after
        // oldword found index.
        strcat(str, temp + index + owlen);
    }
}

//detects the prescence of $ and checks the variable after it and replace it with its true value.
void evaluate_expression(char **input)
{
    int counter = 1, i = 0, j = 0;
    int replace_counter = 0;
    char flag = 1;
    char to_be_replaced[30];
    for (int i = 0; i < 100; i++)
    {
        for (int j = 0; j < 30; j++)
        {
            final_replacement[i][j] = '\0';
        }
        
    }
    for(counter = 1; counter < counter_args; counter++)
    {
        i = 0;
        while ((input[counter][i] != '$') && (input[counter][i] != '\0'))
            i++;
        if(input[counter][i] == '$')
        {
            while (strchr(input[counter], '$') != NULL)
            {
                j = 0;
                i = 0;
                flag = 1;
                for (int iteration = 0; iteration < 30; iteration++)
                {
                    to_be_replaced[iteration] = '\0';
                }
                while ((input[counter][i]!='$') && (input[counter][i]!='\0'))
                {
                    i++;
                }
                i++;
                while ((input[counter][i]!='\0') && (input[counter][i]!='\"') && (input[counter][i]!='$'))
                {
                    to_be_replaced[j] = input[counter][i];
                    i++;
                    j++;
                }
                to_be_replaced[j] = '\0';
                
                for(replace_counter = 0; replace_counter < var_counter; replace_counter++)
                {
                    if(strcmp(to_be_replaced, variables[replace_counter]) == 0)
                    {
                        replace_substing(input[counter], to_be_replaced, values[replace_counter], final_replacement[replace_counter]);
                        input[counter] = final_replacement[replace_counter];
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    replace_substing(input[counter], to_be_replaced, values[99], final_replacement[replace_counter]);
                    input[counter] = final_replacement[replace_counter];
                }
            }
            
        }
        
    }
}

COMMANDS_t command_detect(char **input){
    COMMANDS_t type;
    if (input[0] != NULL)
    {
        if(strcmp(input[0], "cd") == 0)
        {
            type = CD;
        }
        else if(strcmp(input[0], "echo") == 0)
        {
            type = ECHO;
        }
        else if(strcmp(input[0], "export") == 0)
        {
            type = EXPORT;
        }
        else if (strcmp(input[0], "exit") == 0)
        {
            type = EXIT;
        }
        else
        {
            type = EXECUTABLE_OR_ERROR;
        }
    }
    return type;
}

void cd_command(char **input){
    int length = counter_args;
    if (length>2){
        printf("Too many arguments");
    }
    else if (length == 2){
        if (chdir(input[1]) != 0){
            perror(input[1]);
        }
    }
}

//function that prints what is after echo.
void echo_command(char **input){
    int counter = 1;
    int i = 0;
    for (counter = 1; counter < counter_args; counter++)
    {
        i = 0;
        if((input[counter] != NULL) && strcmp(input[counter], "")!=0)
        {
            while (input[counter][i] != '\0')
            {   
                if (input[counter][i] == '\"')
                {
                    i++;
                    continue;
                }
                printf("%c", input[counter][i]);
                i++;
            }
            
            printf(" ");
        }
    }
    printf("\n");
}

void export_command(char **input){
    int counter = 1;
    const char delim[2] = "=";
    char *temp_str = NULL;
    int flag = 1, i = 0;
    for(counter = 1; counter < counter_args; counter++)
    {
        if(isalpha(input[counter][0]) == 0)
        {
            printf("Export: \'%s\' is not a valid identifier\n", input[counter]);
        }
        else
        {
            temp_str = strtok(input[counter], delim);
            for (i = 0; i < var_counter; i++)
            {
                if (strcmp(temp_str, variables[i]) == 0)
                {
                    flag = 0;
                    break;
                }
            }
            if (flag)
            {
                if(temp_str != NULL)
                    strcpy(variables[var_counter], temp_str);
                temp_str = strtok(NULL, delim);
                if(temp_str != NULL)
                {
                    if (temp_str[0] == '\"')
                    {
                        char *temp_delete_qoutes = strchr(temp_str + 1, '\"');
                        if (temp_delete_qoutes != NULL)
                            *temp_delete_qoutes = '\0';
                        strcpy(values[var_counter], temp_str + 1);
                    }
                    else
                    {
                        strcpy(values[var_counter], temp_str);
                    }
                }
                var_counter++;
            }
            else
            {
                temp_str = strtok(NULL, delim);
                if(temp_str != NULL)
                {
                    memset(values[i], '\0', 30);
                    if (temp_str[0] == '\"')
                    {
                        char *temp_delete_qoutes = strchr(temp_str + 1, '\"');
                        if (temp_delete_qoutes != NULL)
                            *temp_delete_qoutes = '\0';
                        strcpy(values[i], temp_str + 1);
                    }
                    else
                    {
                        strcpy(values[i], temp_str + 1);
                    }
                } 
            }
            
        }
    }
}

void execute_shell_bultin(COMMANDS_t input_command, char **input)
{
    switch (input_command)
    {
    case CD:
        cd_command(input);
        break;
    
    case ECHO:
        echo_command(input);
        break;
    
    case EXPORT:
        export_command(input);
        break;
 
    default:
        if (strcmp(input[0], "exit") != 0)
        {
            printf("Error : No command %s\n", input[0]);
        }
        else
        {
            exit(0); 
        }
        break;
    }
}
 
void execute_command(char **command) {
    int foreground_flag;
    // todo
    __pid_t child_id = fork();
    if (child_id == 0)
    {
        // todo
    }
    else
    {
        if (foreground_flag)
        {
            // todo
        }
    }
}

void reap_child_zombie() {
    int status;
    pid_t pid;
    while ((pid = waitpid(-1,&status, WNOHANG)) > 0)
    {
        char string[50] = "Child process ";
        char num[10];
        sprintf(num, "%d", pid);
        strcat(string, num);
        strcat(string, " terminated (Zombie).");
        write_to_log_file(string);
    }
    return;
}

void on_child_exit() {
    reap_child_zombie();
    write_to_log_file("Child terminated (Zombie)");
}

void setup_environment() {
    clear_log_file();
    char cwd[100];
    memset(cwd, '\0', 100);
    if (getcwd(cwd, sizeof(cwd)) != NULL){
        chdir(cwd);
    }
    char *clear_command[] = {"clear", NULL};
    execute_command(clear_command);
}

void shell() {
    char **input = NULL;
    COMMANDS_t input_command;
    COMMANDS_t input_type;
    do
    {
        char cwd[100];
        memset(cwd, '\0', 100);
        getcwd(cwd, sizeof(cwd));
        printf("%s..$ ", cwd);
        input = parse_input();
        if (input[0] == NULL){
            continue;
        }
        evaluate_expression(input);
        printf("%s  %s\n", input[0], input[1]);

        if (input[0] != NULL)
        {
            input_command = command_detect(input);
            if ((input_command==CD) || (input_command==ECHO) || (input_command==EXPORT)){
                input_type = SHELL_BUILT_IN;
            }
            else{
                input_type = EXECUTABLE_OR_ERROR;
            }

            switch (input_type)
            {
            case SHELL_BUILT_IN:
                execute_shell_bultin(input_command, input);
                break;
 
            case EXECUTABLE_OR_ERROR:
                execute_command(input);
                break;
 
            default:
                break;
            }
        }
    }while(strcmp(input[0], "exit") != 0);
    exit(0);
}

int main () {
    signal(SIGCHLD, on_child_exit);
    setup_environment();
    shell();
    return 0;
}
