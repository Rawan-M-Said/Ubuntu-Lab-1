/**
 * @file main.h
 * @author Ahmed Hani Atef
 * @brief header file contains includes, new data types and function declaration.
 * @date 2024-02-27
*/

#ifndef MAIN_H
#define MAIN_H

/******************* Includes section ************************/
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <math.h>
#include <string.h>
#include <sys/types.h>
#include <pwd.h>
#include <ctype.h> 
#include <sys/wait.h>
#include <signal.h>
#include <sys/resource.h>
#include <time.h>
/*************************************************************/

#define STRING_MATCH (0)

/******************* Macro function section ******************/
/*************************************************************/

/******************** New types section **********************/
typedef enum 
{
   CD = 0,
   ECHO,
   EXPORT,
   SHELL_BUILT_IN,
   EXECUTABLE_OR_ERROR,
   EXIT,
}COMMANDS_t;
/*************************************************************/

/**************** function declaration section ***************/

/**
 * @brief Contains main operation of shell
*/
void shell();

/**
 * @brief take the input from user then parse it by spaces to array of strings
 * @return a pointer to array of strings 
 */
char **parse_input();

/**
 * @brief check if the command input is CD, ECHO, EXPORT or EXECUTABLE_OR_ERROR
 * @return returns the type of the command input
 */
COMMANDS_t command_detect(char **command);

/**
 * @brief handle execution of bulitin command like cd, echo and export
 * @param input_command type of command
 * @param command full command
 */
void execute_shell_bultin(COMMANDS_t input_command, char **command);

/**
 * @brief handle cd command
 * @param command full command
 */
void cd_command_handle(char **command);

/**
 * @brief handle echo command
 * @param command full command
 */
void echo_command_handle(char **command);

/**
 * @brief handle export command
 * @param command full command
 */
void export_command_handle(char **command);

/**
 * @brief handles replacing a variable with its value
 * @param command full command
 */
void evaluate_expression(char **command);

/**
 * @brief replace a substring by another string 
 * @param str main string 
 * @param substr string will be replaced  
 * @param replace new string eill replace substring
 * @param final final string after replacement
 */
void replace_substing(char *str, char *substr, char *replace, char *final);

/**
 * @brief used for handle non builtin shell commands
 * @param command full command
 */
void execute_command(char **command);

/**
 * @brief Set the up environment object
 */
void setup_environment();

/**
 * @brief edit commands for non builtin commands
 * @param command full command
 */
void edit_command(char **command);

/**
 * @brief write to log file 
 * @param message 
 */
void write_to_log(const char *message);

/**
 * @brief clear the content of log file
 */
void clear_log_file();

/**
 * @brief convert integer to string
 * @param num integer value
 * @param str string
 */
void int_to_string(int num, char *str);

/*************************************************************/

#endif

/**************************************************************
 **************************** Log *****************************
 **************************************************************
    Name                |Date       |brief
 **************************************************************
 *  Ahmed Hani Atef     |Feb-27     |File creation  
 *  Ahmed Hani Atef     |Feb-27     |(1) Add includes section
 *                      |           |(2) Create function declaration
 *                      |           |(3) Create New data types     
*/