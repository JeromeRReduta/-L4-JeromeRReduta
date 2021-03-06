#include <stdbool.h>
#include <stdio.h>
#include <unistd.h>
#include <sys/wait.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>

struct command_line {
    char **tokens;
    bool stdout_pipe;
    char *stdout_file;
};

/* Function prototypes */
int run_process(struct command_line *cmds);

void execute_pipeline(struct command_line *cmds)
{
    /**
     * TODO: design an algorithm that sets up a pipeline piece by piece.
     * Solutions will probably either iterate over the pieces of the pipeline or
     * work recursively. Imagine you have three commands in a pipeline:
     *
     *  - cat
     *  - tr
     *  - sed
     *
     * Use 'stdout_pipe' to determine when you've reached the last command in
     * the pipeline, and 'stdout_file' to decide whether the final result gets
     * written to a file or the terminal.
     *
     * If we aren't at the last command, then we need to set up a pipe for the
     * current command's output to go into. For example, let's say our command
     * is `cat file.txt`. We will create a pipe and have the stdout of the
     * command directed to the pipe. Before running the next command, we'll set
     * up the stdin of the next process to come from the pipe, and
     * execute_pipeline will run whatever command comes next (for instance,
     * `tr '[:upper:]' '[:lower:]'`).
     *
     * Here's some pseudocode to help:
     *
     * create a new pipe
     * fork a new process
     * if pid is the child:
     *     dup2 stdout to pipe[1]
     *     close pipe[0]
     *     execvp the command
     * if pid is the parent:
     *     dup2 stdin to pipe[0]
     *     close pipe[1]
     *     move on to the next command in the pipeline
     *
     * The special case here is when there are no more commands left. In that
     * case, you can simply execvp the command (no need to create another pipe).
     * If you created a handler process in main(), then it will be replaced by
     * this last call to execvp.
     */

    int num_of_commands = sizeof(struct command_line) / sizeof(cmds);
    printf("SIZEOF COMMANDLINE[3]:\t%d\n", num_of_commands);

    for (int i = 0; i < num_of_commands; i++) {
        int success = run_process((cmds + i));

        if (success == -1) {
            return;
        }
    }
  
        

}

int run_process(struct command_line *cmds)
{
   if (cmds->stdout_pipe == false) {
       if (cmds->stdout_file != NULL) {
     
           int open_flags = O_RDWR | O_CREAT | O_TRUNC;
           int open_perms = 0666;
           int fd = open(cmds->stdout_file, open_flags, open_perms);

           if (fd == -1) {
               perror("open");
               return -1;
           }
           dup2(fd, STDOUT_FILENO);
       }

       if (execvp(cmds->tokens[0], cmds->tokens) == -1) {
           perror("execvp");
           return -1;
       }
   }
   
    // Create new pipe
    int fd[2];

    if (pipe (fd) == -1) {
         return -1;
     }

    pid_t child = fork();

    
    if (child == -1) {
        perror("fork");
        return -1;
    }
    // Child case - execute program
    else if (child == 0) {
        printf("DOING STUFF:\n");
        dup2(fd[1], STDOUT_FILENO);
        close(fd[0]);
        execvp(cmds->tokens[0], cmds->tokens);
    }
    else {
        // Parent
        dup2(fd[0], STDIN_FILENO);
        close(fd[1]);
    }

    return 0;

}

int main(int argc, char *argv[])
{
    char *input_file = NULL;
    char *output_file = NULL;

    if (argc < 2 || argc > 3) {
        printf("Usage: %s file-to-leetify [output-file]\n", argv[0]);
        return 1;
    }

    input_file = argv[1];

    if (argc == 3) {
        output_file = argv[2];
    }

    /* TODO: Next steps:
     *   finish setting up the array of command_line structs
     *   fork() to create a 'handler' process that will call execute_pipeline
     *   wait for the handler to finish
     */

    char *command1[] = { "cat", input_file, (char *) NULL };
    char *command2[] = { "tr", "[:upper:]", "[:lower:]", (char *) NULL };
    char *command3[] = { "sed", output_file, (char *) NULL };


    // Show command line as struct
    struct command_line cmds[3] = { 0 };
    cmds[0].tokens = command1;
    cmds[0].stdout_pipe = true;
    cmds[0].stdout_file = NULL;

    cmds[1].tokens = command2;
    cmds[1].stdout_pipe = true;
    cmds[1].stdout_file = NULL;

    cmds[2].tokens = command3;
    cmds[2].stdout_pipe = false;
    cmds[2].stdout_file = NULL;


    execute_pipeline(cmds);

    return 0;
}


