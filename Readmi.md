# Minishell Project - School 42

## Description

This repository contains the implementation of a simple shell, known as
Minishell, as part of the curriculum at School 42. The goal of this project is
to create a minimal shell in C programming language, replicating some of the
behavior of well-known shells like bash, zsh, or fish.

## Features

The Minishell supports:

- Basic commands: echo, cd, setenv, unsetenv, env, exit.
- Redirections: standard input, standard output, and error (>, >>, <).
- Pipes (|).
- Environment variables ($ followed by characters).
- Command history.
- Signal management (Ctrl-C, Ctrl-D, Ctrl-\).
- Here documents (<<).

## Implementation Details

### Lexer

The first step in the implementation of our Minishell is the creation of a
Lexer. A Lexer, or lexical analyzer, is a program that transforms an input
sequence (in our case, the command line input) into a sequence of tokens.
These tokens are defined by a certain set of rules, which for our shell could
be command names, arguments, operators, etc.

We use an `enum` type in C to define these tokens. An `enum` type is a special
data type that enables for a variable to be a set of predefined constants.
The variable must be equal to one of the values that have been predefined for
it.

#### Lexer Implementation

The Lexer is responsible for breaking up the input into recognizable tokens.
Here's a simplified version of how you might implement a lexer for this
project:

1. **Struct Definitions:**

```c
typedef struct	s_token
{
	enum
	{
		TOKEN_PIPE,
		TOKEN_INFILE,
		TOKEN_OUTFILE,
		TOKEN_STRING,
		TOKEN_HERE_DOC,
		TOKEN_APPEND,
		TOKEN_EOF
	}	type;
	char	*content;
}	t_token;

typedef struct s_command
{
	t_token				*content;
	struct s_command	*next;
}	t_command;

These are the struct definitions for a token and a command. A token represents
a single, indivisible unit of meaning, like a word or an operator. A command
is a linked list of tokens that together form a complete command.

Add Command to End of List:

```c
void ft_lstadd_back(t_command **lst, t_command *new) 
{
    t_command *ptr;

    if (!lst) 
    {
        return;
    }
    if (!(*lst)) 
    {
        *lst = new;
        return;
    }
    ptr = *lst;
    while (ptr->next) 
    {
        ptr = ptr->next;
    }
    ptr->next = new;
}

This function adds a new command to the end of a linked list of commands. If
the list is empty, it sets the new command as the first item in the list.
Otherwise, it traverses the list to the last item and sets the next pointer of
the last item to the new command.

Create New Command:

```c
t_command *ft_lstnew(t_token *content)
{
    t_command *new;

    new = malloc(sizeof(t_command));
    if (!new)
        return (NULL);
    new->content = content;
    new->next = NULL;
    return (new);
}

This function creates a new command with the given token as its content. It
allocates memory for the new command, sets its content to the given token, and
sets its next pointer to NULL.

Add Character to String:

```c
char *add_char(char *str, char c)
{
    int i;
    char *new;

    i = 0;
    if (!str)
    {
        new = malloc(2);
        if (!new)
            return (NULL);
        new[0] = c;
        new[1] = '\0';
    }
    else
    {
        new = malloc(strlen(str) + 2);
        if (!new)
            return (NULL);
        while (str[i])
        {
            new[i] = str[i];
            i++;
        }
        new[i] = c;
        new[i + 1] = '\0';
        free(str);
    }
    return (new);
}

Function Description
This function adds a character to the end of a string. If the string is NULL,
it creates a new string with the character as its only content. Otherwise, it
creates a new string that is one character longer than the original string,
copies the original string into the new string, adds the character to the end
of the new string, and frees the original string.

Create Token:

```c
t_token *create_token(char character, char next_char)
{
    t_token *token;

    token = malloc(sizeof(t_token));
    if (!token)
        return (NULL);
    if (character == '<')
    {
        token->content = add_char(NULL, character);
        if (character == next_char)
        {
            token->content = add_char(token->content, character);
            token->type = TOKEN_HERE_DOC;
        }
        else
            token->type = TOKEN_INFILE;
    }
    else if (character == '>')
    {
        token->content = add_char(NULL, character);
        if (character == next_char)
        {
            token->content = add_char(token->content, character);
            token->type = TOKEN_APPEND;
        }
        else
            token->type = TOKEN_OUTFILE;
    }
    else if (character == '|')
    {
        token->content = add_char(NULL, character);
        token->type = TOKEN_PIPE;
    }
    else if (character == '\0')
    {
        token->content = add_char(NULL, character);
        token->type = TOKEN_EOF;
    }
    else
    {
        token->content = add_char(NULL, character);
        token->type = TOKEN_STRING;
    }
    return (token);
}

This function creates a new token with the given character as its content. The
type of the token is determined by the character. If the character is '<', '>',
'|', or '\0', the token is a special type (infile, outfile, pipe, or EOF). If
the character is anything else, the token is a string.

Process Tokens:

```c
void process_tokens(char *line, t_command **lexer)
{
    int i;
    t_token *token;
    char c;

    i = 0;
    token = NULL;
    while (line[i])
    {
        while (line[i] == ' ' || line[i] == '\t')
            i++;
        if (!line[i])
            break;
        token = malloc(sizeof(t_token));
        token->content = NULL;
        while (line[i])
        {
            if (c == 0 && (line[i] == '\'' || line[i] == '\"'))
                c = line[i];
            else if (c == line[i])
                c = 0;
            if (c == 0 && (line[i] == '<' || line[i] == '>'
                    || line[i] == '|' || line[i] == ' ' || line[i] == '\t'))
                break;
            token->content = add_char(token->content, line[i]);
            i++;
        }
        if (token->content)
        {
            token->type = TOKEN_STRING;
            ft_lstadd_back(lexer, ft_lstnew(token));
        }
        if (line[i] == '<' || line[i] == '>')
        {
            token = create_token(line[i], line[i + 1]);
            if (line[i] == line[i + 1])
                i += 2;
            else
                i++;
            ft_lstadd_back(lexer, ft_lstnew(token));
        }
        else if (line[i] == '|')
        {
            token = create_token(line[i++], '\0');
            ft_lstadd_back(lexer, ft_lstnew(token));
        }
    }
    if (line[i] == '\0')
    {
        token = create_token(line[i], line[i]);
        ft_lstadd_back(lexer, ft_lstnew(token));
    }
}

Minishell Project Guide

Tokenization

This function processes a line of input and creates a linked list of tokens.
It skips over whitespace and then creates a new token for each word or special
character in the line. If a word is enclosed in quotes, it is treated as a
single token, even if it contains spaces or special characters. The function
also handles special cases for the '<', '>', and '|' characters, creating a
new token for each one and adding it to the list.

Syntax Error Checking

After tokenizing the input, we check for syntax errors. This includes checking
for invalid sequences of tokens such as "<>", "<|", ">|", "<<|", ">>|", "|",
"||", and also checking for unbalanced quotes like a single '"', '"', "'".

Parsing

Once we have our tokens and have ensured they are syntactically correct, we
move on to parsing. In this step, we extract commands and files from the
tokens. We use linked lists to store these commands and files. A linked list
is a linear data structure where each element is a separate object. Each
element (we will call it a node) of a list consists of two items - the data
and a reference to the next node.

Expansion

After parsing, we perform expansion. This is where we handle environment
variables (tokens that start with $). The shell replaces the variable with its
value before executing the command.

Here Documents

Here documents (or here-docs) are a feature of shell that allow the input of a
sequence of lines to a command directly from the command line. This can be
useful when a command expects input from a user and we want to automate that
process.

After parsing, we have a struct that contains commands and files as linked
lists. If we need to work with here-docs, we take this struct and loop over
each node. Each node can have more than one here-doc, so we need to use
another loop that iterates over the files.

In our enum, the here_doc type is 4. When we encounter a here_doc, we create a
pipe and then fork a new process. In the child process, we write to the write
end of the pipe (fd[1]) and close the read end (fd[0]). We use an infinite
loop with the readline function to read the here-doc content and write it to
fd[1]. After that, we close fd[1].

In the parent process, we wait for the child process to finish and save the
read end of the pipe (fd[0]) in a variable in our struct for later use.

Execution

Finally, we execute the commands that we have parsed and expanded. This
involves creating a new process and replacing its image with the command to be
executed.

Execution Implementation

The execution part of our Minishell involves taking the parsed commands and
files and running them. This is where the actual actions specified by the
user's input take place.

We start by implementing a function that gets the commands from our parsed
data. If you've worked on the pipex project, this will be familiar to you.

Next, we use the global struct that contains our commands and files. We loop
over these commands and files, forking a new process for each command. Inside
each child process, we open the necessary files and execute the command.

We use the dup2 function to redirect input and output to the appropriate files,
and execve to replace the current process image with the new command. It's
important to carefully manage the opening and closing of file descriptors in
this step to avoid leaks and ensure that data is being read from and written
to the correct places.

This is a challenging part of the project, but it's also where everything comes
together. By carefully managing processes and file descriptors, we can create
a shell that behaves just like a real one.

Conclusion

In this project, we've built a simple shell from scratch. We started by
implementing a Lexer to tokenize the user's input. We then checked for syntax
errors and parsed the tokens into commands and files. We expanded any
environment variables and handled here-docs. Finally, we executed the commands,
managing processes and file descriptors to ensure that input and output went
to the right places.

This project is a great demonstration of many important concepts in systems
programming, including process management, file I/O, and parsing. It's also a
fun way to get a better understanding of how shells work under the hood.

Remember, this is a general guide and the exact implementation details may
vary depending on the specific project guidelines provided by the school.
Always follow the guidelines provided by the school and respect the honor code
when working on your projects.
