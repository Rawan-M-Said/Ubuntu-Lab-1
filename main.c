#include "main.h"


void on_child_exit()
{
    char pid_str[50];
    char temp_pid_str[10];
    pid_t pid;
    int status;
    //pid = waitpid(0, NULL, WUNTRACED);
    while ((pid = waitpid(-1, &status, WNOHANG)) > 0) {
        strcpy(pid_str, "Child process with PID ");
        int_to_string(pid, temp_pid_str);
        strcat(pid_str, temp_pid_str);
        strcat(pid_str, " terminated (Zombie).");
        write_to_log(pid_str);
    }
    /*
    int_to_string(pid, pid_str);
    strcat(pid_str, " : Child terminated");
    write_to_log(pid_str);
    */
}


int main ()
{
    // register child signal function to kill zombies processes
    // register_child_signal(on_child_exit())
    signal(SIGCHLD, on_child_exit);

    // go to default directory as defined
    setup_environment();

    // start the shell
    shell();
    return 0;
}

char directory[100];
void setup_environment()
{
    clear_log_file();
    memset(directory, '\0', 100);
    getcwd(directory, 100);
    chdir(directory);
    char *clear_command[] = {"clear", NULL};
    execute_command(clear_command);    
}

/**
 * @brief Contains main operation of shell
 */
void shell(void)
{
    char **command = NULL;
    COMMANDS_t input_command;
    COMMANDS_t input_type;
    do
    {
        memset(directory, '\0', 100);
        getcwd(directory, 100);
        printf("%s---$  ", directory);
        command = NULL; 
        command = parse_input();
        if (command[0] == NULL)
            continue;
        evaluate_expression(command);
        if (command != NULL)
        {
            input_command = command_detect(command);
            if ((input_command == CD) || (input_command == ECHO) || input_command == EXPORT)
            {
                input_type = SHELL_BUILT_IN;
            }
            else
            {
                input_type = input_command;
            }
            switch (input_type)
            {
            case SHELL_BUILT_IN:
                execute_shell_bultin(input_command, command);
                break;

            case EXECUTABLE_OR_ERROR:
                execute_command(command);
                break;

            default:
                break;
            }
        }
    }while((command == NULL) || (command[0] == NULL) || (strcmp(command[0], "exit") != STRING_MATCH));
    exit(0);
}

int counter_args = 0;
char **parse_input()
{
    static char input[100];
    char *temp_input = NULL;
    char *garbage_buffer = NULL;
    char **final_input = NULL;
    const char delim[2] = " ";
    int flag = 1;
    
    counter_args = 0;
    memset(input, '\0', 100);
    fgets(input, 100, stdin);
    char *temp = strpbrk(input, "\n");
    temp[0] = '\0';
    fflush(stdin);

    temp_input = strtok(input, delim);
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
                temp_input = strtok(NULL, delim);
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

COMMANDS_t command_detect(char **command)
{
    COMMANDS_t input_command;
    if (command[0] != NULL)
    {
        if(strcmp(command[0], "cd") == STRING_MATCH)
        {
            input_command = CD;
        }
        else if(strcmp(command[0], "echo") == STRING_MATCH)
        {
            input_command = ECHO;
        }
        else if(strcmp(command[0], "export") == STRING_MATCH)
        {
            input_command = EXPORT;
        }
        else if (strcmp(command[0], "exit") == STRING_MATCH)
        {
            input_command = EXIT;
        }
        else
        {
            input_command = EXECUTABLE_OR_ERROR;
        }
    }
    return input_command;
}

void execute_shell_bultin(COMMANDS_t input_command, char **command)
{
    switch (input_command)
    {
    case CD:
        cd_command_handle(command);
        break;
    
    case ECHO:
        echo_command_handle(command);
        break;
    
    case EXPORT:
        export_command_handle(command);
        break;

    default:
        if (strcmp(command[0], "exit") != STRING_MATCH)
        {
            printf("Error : No command %s\n", command[0]);
        }
        else
        {
           exit(0); 
        }
        break;
    }
}

void cd_command_handle(char **command)
{
    int length = counter_args;
    if (length > 2)
    {
        printf("bash: cd: too many arguments");
    }
    else if (length == 1)
    {
        // Nothing
    }
    else
    {
        if (strcmp(command[1], "~") == STRING_MATCH)
        {
            struct passwd *pw = getpwuid(getuid());
            const char *homedir = pw->pw_dir;
            chdir(homedir); 
        }
        else
        {
            if (command[1][0] == '~')
            {
                struct passwd *pw = getpwuid(getuid());
                const char *homedir = pw->pw_dir;
                chdir(homedir); 
                if(chdir(command[1] + 2) != 0)
                {
                    perror(command[1]);
                }
            }
            else if (chdir(command[1]) != 0)
            {
                perror(command[1]); 
            }
        }
    }
}

void echo_command_handle(char **command)
{
    int counter = 1;
    int i = 0;
    for (counter = 1; counter < counter_args; counter++)
    {
        i = 0;
        if(command[counter] != NULL)
        {
            if (strcmp(command[counter], "") != STRING_MATCH)
            {
                while (command[counter][i] != '\0')
                {   
                    if (command[counter][i] == '\"')
                    {
                        i++;
                        continue;
                    }
                    printf("%c", command[counter][i]);
                    i++;
                }
                
                //printf("%s", command[counter]);
                printf(" ");
            }
        }
    }
    printf("\n");
}

char replaced[100][30];
char replaced_by[100][30];
char final_replacement[100][30];
int export_counter = 0;
void export_command_handle(char **command)
{
    int counter = 1;
    const char delim[2] = "=";
    char *temp_str = NULL;
    int flag = 1, i = 0;
    for(counter = 1; counter < counter_args; counter++)
    {
        if(isalpha(command[counter][0]) == 0)
        {
            printf("bash: export: \'%s\': not a valid identifier\n", command[counter]);
        }
        else
        {
            temp_str = strtok(command[counter], delim);
            for (i = 0; i < export_counter; i++)
            {
                if (STRING_MATCH == strcmp(temp_str, replaced[i]))
                {
                    flag = 0;
                    break;
                }
            }
            if (flag)
            {
                if(temp_str != NULL)
                    strcpy(replaced[export_counter], temp_str);
                temp_str = strtok(NULL, delim);
                if(temp_str != NULL)
                {
                    if (temp_str[0] == '\"')
                    {
                        char *temp_delete_qoutes = strchr(temp_str + 1, '\"');
                        if (temp_delete_qoutes != NULL)
                            *temp_delete_qoutes = '\0';
                        strcpy(replaced_by[export_counter], temp_str + 1);
                    }
                    else
                    {
                        strcpy(replaced_by[export_counter], temp_str + 1);
                    }
                }
                export_counter++;
            }
            else
            {
                temp_str = strtok(NULL, delim);
                if(temp_str != NULL)
                {
                    memset(replaced_by[i], '\0', 30);
                    if (temp_str[0] == '\"')
                    {
                        char *temp_delete_qoutes = strchr(temp_str + 1, '\"');
                        if (temp_delete_qoutes != NULL)
                            *temp_delete_qoutes = '\0';
                        strcpy(replaced_by[i], temp_str + 1);
                    }
                    else
                    {
                        strcpy(replaced_by[i], temp_str + 1);
                    }
                } 
            }
            
        }
    }
}

void evaluate_expression(char **command)
{
    int counter = 1, i = 0, j = 0;
    int replace_counter = 0;
    char flag = 1;
    char temp_buffer[30];
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
        while ((command[counter][i] != '$') && (command[counter][i] != '\0'))
            i++;
        if(command[counter][i] == '$')
        {



            while (strchr(command[counter], '$') != NULL)
            {
                j = 0;
                i = 0;
                flag = 1;
                for (int iteration = 0; iteration < 30; iteration++)
                {
                    temp_buffer[iteration] = '\0';
                }
                while ((command[counter][i] != '$') && (command[counter][i] != '\0'))
                {i++;}
                i++;
                while ((command[counter][i] != '\0') && (command[counter][i] != '\"') && (command[counter][i] != '$'))
                {
                    temp_buffer[j] = command[counter][i];
                    i++;
                    j++;
                }
                temp_buffer[j] = '\0';
                
                for(replace_counter = 0; replace_counter < export_counter; replace_counter++)
                {
                    if(strcmp(temp_buffer, replaced[replace_counter]) == STRING_MATCH)
                    {
                        replace_substing(command[counter], temp_buffer, replaced_by[replace_counter], final_replacement[replace_counter]);
                        command[counter] = final_replacement[replace_counter];
                        flag = 0;
                        break;
                    }
                }
                if (flag)
                {
                    replace_substing(command[counter], temp_buffer, replaced_by[99], final_replacement[replace_counter]);
                    command[counter] = final_replacement[replace_counter];
                }
            }
            
        }
        
    }
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

void execute_command(char **command)
{
    int i = 0, foreground_flag = 1;
    int execvp_flag = 0;

    edit_command(command);

    while (command[i] != NULL)
    {
        if(strcmp(command[i], "&") == STRING_MATCH)
        {
            command[i] = NULL;
            foreground_flag = 0;
            break;
        }
        i++;
    }
    
    __pid_t child_id = fork();
    if (child_id == 0)
    {
        execvp(command[0], command);
        printf("Command \'%s\' not found\n", command[0]);
        exit(0);
    }
    else 
    {
        char pid_str[50];
        char temp_pid_str[10];
        strcpy(pid_str, "Child process with PID ");
        int_to_string(child_id, temp_pid_str);
        strcat(pid_str, temp_pid_str);
        strcat(pid_str, " created.");
        write_to_log(pid_str);

        if (foreground_flag)
        {
            if(execvp_flag != -1)
            {
                waitpid(child_id, NULL, WUNTRACED); 

                strcpy(pid_str, "Child process with PID ");
                int_to_string(child_id, temp_pid_str);
                strcat(pid_str, temp_pid_str);
                strcat(pid_str, " terminated normally.");
                write_to_log(pid_str);  
            }
        }
    }
}

void edit_command(char **command)
{
    char **temp_command = (char **)calloc(counter_args + 1, sizeof(char *));
    int temp_command_index = 0;
    char temp_buffer[30];
    int temp_command_flag = 1;
    char *temp_input;
    char delim[] = " ";
    int args_buffer = counter_args;
    if (command[0] != NULL)
    {
        for (int i = 0; i < counter_args; i++)
        {
            temp_command[i] = (char *)calloc(strlen(command[i]), sizeof(char));
            strcpy(temp_command[i], command[i]);
            if (i != 0)
            {
                command[i] = NULL;
            }
        }
        counter_args = 1;
        for (int i = 1; i < args_buffer; i++)
        {
            if(strchr(temp_command[i], ' ') != NULL)
            {
                strcpy(temp_buffer, temp_command[i]);
                temp_input = strtok(temp_buffer, delim);
                counter_args++;
                while (temp_input != NULL)
                {
                    command[counter_args - 1] = (char *)calloc(strlen(temp_input), sizeof(char));
                    strcpy(command[counter_args - 1], temp_input);
                    temp_input = strtok(NULL, delim);
                    counter_args++;
                }
            }
            else
            {
                command[counter_args ] = (char *)calloc(strlen(temp_command[i]), sizeof(char));
                strcpy(command[counter_args], temp_command[i]);
                counter_args++;
            }
        }
        for (int i = 0; i < args_buffer; i++)
        {
            free(temp_command[i]);
            temp_command[i] = NULL;
        }
    }
}

void write_to_log(const char *message) 
{

    FILE *file;
    file = fopen("/home/ahmed-hani/OS/Labs/1/logfile.txt", "a"); // "a" mode appends to the file if it exists, otherwise creates a new file
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }

    time_t rawtime;
    struct tm *timeinfo;
    time(&rawtime);
    timeinfo = localtime(&rawtime);

    fprintf(file, "[%04d-%02d-%02d %02d:%02d:%02d] %s\n", 
            timeinfo->tm_year + 1900, timeinfo->tm_mon + 1, timeinfo->tm_mday,
            timeinfo->tm_hour, timeinfo->tm_min, timeinfo->tm_sec, message);
    
    fclose(file);
}

void clear_log_file() 
{
    FILE *file;
    file = fopen("/home/ahmed-hani/OS/Labs/1/logfile.txt", "w+"); // "w+" mode truncates the file if it exists, otherwise creates a new file
    if (file == NULL) {
        printf("Error opening file!\n");
        exit(1);
    }
    fclose(file);
}

void int_to_string(int num, char *str) 
{
    sprintf(str, "%d", num);
}




