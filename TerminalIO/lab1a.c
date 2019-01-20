#include <stdio.h>
#include <stdlib.h>

#include <unistd.h>
#include <getopt.h>

#include <termios.h>
#include <sys/types.h>
#include <poll.h>
#include <signal.h>
#include <sys/wait.h>


#define SHELL 's'
const char cr = '\r';
const char lf = '\n';

void reset(); 
void get_exit_status(); 
void handler(int signal_num);


struct termios ORIGINAL_ATTRIBUTES; // Keep original attributes
pid_t CHILD_PID = -1; 


int main(int argc, char *argv[])
{
    
    //************** SETTING INPUT MODE ****************
    struct termios terminal_attr;

    // Make sure stdin is a terminal.
    if (!isatty(0))
    {
        fprintf(stderr, "ERROR: stdin in not a terminal!.\n");
        exit(1);
    }

    // Save the terminal attributes so we can restore them later.
    tcgetattr(0, &ORIGINAL_ATTRIBUTES);
    atexit(reset);

    // Set terminal modes.
    tcgetattr(0, &terminal_attr);
    terminal_attr.c_oflag = 0;      // no processing
    terminal_attr.c_lflag = 0;      // no processing
    terminal_attr.c_iflag = ISTRIP; // only lower 7 bits

    terminal_attr.c_cc[VTIME] = 0;
    terminal_attr.c_cc[VMIN] = 1;

    tcsetattr(0, TCSANOW, &terminal_attr);

    struct option long_options[] =
        {
            {"shell", no_argument, NULL, SHELL},
            {0, 0, 0, 0}};

    while (1)
    {
        int ret = getopt_long(argc, argv, "", long_options, NULL);

        if (ret == -1)
        {
            break;
        }
        switch (ret)
        {
        case SHELL:
        {
            int from_child[2];
            int to_child[2];
            

            atexit(get_exit_status);
            signal(SIGPIPE, handler);
            signal(SIGINT, handler);

            if (pipe(to_child) == -1)
            {
                fprintf(stderr, "ERROR: pipe() failed!\n");
                exit(1);
            }
            if (pipe(from_child) == -1)
            {
                fprintf(stderr, "ERROR: pipe() failed!\n");
                exit(1);
            }

            CHILD_PID = fork();

            
            //child process
            if (CHILD_PID == 0)
            {
                close(to_child[1]);
                close(from_child[0]);
                
                dup2(from_child[1], 1);
                dup2(from_child[1], 2);
                dup2(to_child[0], 0);

                close(to_child[0]);
                close(from_child[1]);

                char *execvp_argv[2];
                char filename[] = "/bin/bash";
                execvp_argv[0] = filename;
                execvp_argv[1] = NULL;
                if (execvp(filename, execvp_argv) == -1)
                {
                    fprintf(stderr, "ERROR: execvp() failed!\n");
                    exit(1);
                }
            }

            //parent process
            if (CHILD_PID > 0)
            {
                close(to_child[0]);
                close(from_child[1]);

                char buffer[3000];
                int count = 0;

                struct pollfd fd[2];
                
                fd[0].events = POLLERR | POLLIN | POLLHUP;
                fd[1].events = POLLERR | POLLIN | POLLHUP;
                fd[0].fd = 0;
                fd[1].fd = from_child[0];

                int poll_ret = 0;
                while (1)
                {
                    poll_ret = poll(fd, 2, -1);
                    if (poll_ret > 0)
                    {
                        if ((fd[0].revents & POLLIN) == POLLIN)
                        {
                            count = read(0, buffer, 3000);
                            int i = 0;
                            char tbuf;
                            while (i < count)
                            {
                                tbuf = buffer[i];

                                if (tbuf == 0x03)
                                {
                                    printf("^C\n");
                                    kill(CHILD_PID, SIGINT);
                                }
                                if (tbuf == 0x04)
                                {
                                    printf("^D\n");
                                    close(to_child[1]);
                                    close(from_child[0]);
                                    kill(CHILD_PID, SIGHUP);
                                    exit(0);
                                }
                                if (tbuf == lf || tbuf == cr)
                                {
                                    write(1, &cr, 1);
                                    write(1, &lf, 1);
                                    write(to_child[1], &lf, 1);
                                }
                                else
                                {
                                    write(1, &tbuf, 1);
                                    write(to_child[1], &tbuf, 1);
                                }
                                i++;
                            }
                        }
                        else if ((fd[1].revents & POLLIN) == POLLIN)
                        {
                            count = read(from_child[0], buffer, 3000);
                            int i = 0;
                            char tbuf;
                            while (i < count)
                            {
                                tbuf = buffer[i];
                                if (tbuf == lf)
                                {
                                    write(1, &cr, 1);
                                    write(1, &lf, 1);
                                }
                                else
                                {
                                    write(1, &tbuf, 1);
                                }
                                i++;
                            }
                        }
                        else if (((fd[0].revents & POLLERR) == POLLERR) || ((fd[1].revents & POLLERR) == POLLERR))
                        {
                            kill(CHILD_PID, SIGINT);
                            exit(1);
                        }

                        else if (((fd[0].revents & POLLHUP) == POLLHUP) || ((fd[1].revents & POLLHUP) == POLLHUP))
                        {
                            kill(CHILD_PID, SIGHUP);
                            exit(0);
                        }
                        
                    }
                }
            }

            else //fork() failed
            { 
                fprintf(stderr, "ERROR: fork() failed!\n");
                exit(1);
            }
            break;
        }



        default:
        {
            fprintf(stderr, "ERROR: Argument Not Recognized, correct usage includes: --shell");
            exit(1);
        }
        }
    }

    char c;

    while (1)
    {
        if (read(0, &c, 1) < 0)
        {
            fprintf(stderr, "Read Error");
            exit(1);
        }

        if (c == 0x04)
            break;

        if (c == lf || c == cr)
        {
            write(1, &cr, 1);
            write(1, &lf, 1);
        }
        else
            write(1, &c, 1);
    }
    exit(0);
}

// *************************************************
// Functions Implementation

void get_exit_status()
{
    int exit_status;
    waitpid(CHILD_PID, &exit_status, 0);
    fprintf(stderr, "SHELL EXIT SIGNAL=%d STATUS=%d\n", WTERMSIG(exit_status), WEXITSTATUS(exit_status));
}

void reset()
{
    tcsetattr(0, TCSANOW, &ORIGINAL_ATTRIBUTES);
}

void handler(int signal_num)
{
    if (signal_num == SIGINT)
        kill(CHILD_PID, SIGINT);

    else if (signal_num == SIGPIPE)
        exit(1);
}