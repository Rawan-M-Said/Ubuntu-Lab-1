int main () {
    signal(SIGCHLD, on_child_exit);
    setup_environment();
    shell();
    return 0;
}
 
void on_child_exit() {
    reap_child_zombie();
    write_to_log_file("Child terminated");
}
 
void reap_child_zombie() {
    // TODO
}
 
void write_to_log_file(char* message) {
    // TODO
}
 
void setup_environment() {
    // TODO
}
 
void shell() {
    char **command = NULL;
    COMMANDS_t input_command;
    COMMANDS_t input_type;
    do
    {
        // todo
        if (command != NULL)
        {
            // todo
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
    }while(/* todo */);
    exit(0);
}
 
void execute_shell_bultin(COMMANDS_t input_command, char **command)
{
    switch (input_command)
    {
    case CD:
        // todo
        break;
    
    case ECHO:
        // todo
        break;
    
    case EXPORT:
        // todo
        break;
 
    default:
        // todo
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
