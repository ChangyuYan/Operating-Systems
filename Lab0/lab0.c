/*
------------------------------
Name: Changyu Yan
ID: 304-566-451
------------------------------
Credits: 
- The template for getopt_long is from the official gnu website:
https://www.gnu.org/software/libc/manual/html_node/Getopt-Long-Option-Example.html#Getopt-Long-Option-Example
*/

#include <stdio.h>
#include <stdlib.h>
#include <sys/types.h> 
#include <sys/stat.h>
#include <getopt.h>
#include <fcntl.h>
#include <string.h>
#include <errno.h>
#include <unistd.h>
#include <signal.h>

void print_usage(); 
void segfault_handler(int num); 
void read_write(); 

int main (int argc, char **argv)
{
	int c;
	int segfault_flag = 0; // segmentation fault flag
	char* i_file = NULL; // input file
	char* o_file = NULL; // output file
	char* nPtr = NULL; // null pointer, used to cause a segmentation fault
	int i_fd = 0; // input file descriptor; stdin
	int o_fd = 1; // output file descriptor; stdout
  

	while (1)
	{
		static struct option long_options[] =
		{
	    	/* These options don’t set a flag.
	    	We distinguish them by their indices. */
	    	{"input",			required_argument,	0, 'i'},
	    	{"output",		required_argument,	0, 'o'},
	    	{"segfault",	no_argument,		0, 's'},
	    	{"catch",			no_argument,		0, 'c'},
	   	 	{0, 0, 0, 0}
		};
	  
		/* getopt_long stores the option index here. */
		int option_index = 0;

		c = getopt_long(argc, argv, "i:o:sc", long_options, &option_index);

		/* Detect the end of the options. */
		if (c == -1)
	    	break;

    switch (c)
    	{
			case 'i':
				i_file = optarg; 
				break;

			case 'o':
				o_file = optarg; 
				break;
			
			case 's':
				segfault_flag = 1; 
				break;
			
			case 'c':
				signal(SIGSEGV, segfault_handler);
				break;

			default:
				fprintf(stderr, "ERROR: Argument Not Recognized! --%s\n%s\n", optarg, strerror(errno));
				print_usage(); 
				_exit(1);
				break;
		}	
	}

	if (i_file)
	{
		i_fd = open(i_file, O_RDONLY);

		if (i_fd >= 0)
		{
			// Setting input file to stdin
	  	close(0);
	  	dup2(i_fd,0); 
	  	close(i_fd);
		}
    else
		{
	  	fprintf(stderr, "ERROR: Unable to open input file: --input=%s\n%s\n", i_file, strerror(errno));
	  	_exit(2);
		}
	}

	if (o_file)
	{
		// Creating the file, giving user read, write, and execute permission
		o_fd = creat(o_file, S_IRWXU);
    if (o_fd >= 0)
		{
			// Setting output file to stdout
			close(1);
			dup2(o_fd,1);
			close(o_fd); 
		}
    else
		{
			fprintf(stderr, "ERROR: Unable to create output file: --output=%s\n%s\n", o_file, strerror(errno));
			_exit(3);
		}
	}

	if (segfault_flag)
	{
		*nPtr = 'x'; 
	}
	
	// Now we need to read from stdin and write to stdout
	
	read_write(); 
	_exit(0);
}

/****************************************************
Function implementations:
*****************************************************/
void print_usage()
{
	printf("Usage: lab0 --input=filename --output=filename --segfault --catch \n");
}

void segfault_handler(int num) {
  fprintf(stderr, "Segmentation fault catched! by `--catch'. Signal number: %d\ncatch signal: %s\n", num, strerror(errno));
  _exit(4);
}

void read_write()
{
	char* buffer = (char*) malloc(sizeof(char));

	// read from stdin, one character at a time until the end of line
	// ssize_t read(int fd, void *buf, size_t count);
  	ssize_t ch = read(0, buffer, 1);
  	while(ch > 0)
  	{
		// write to stdout, one character at a time
    	write(1, buffer, 1);
    	ch = read(0, buffer, 1);
  	}
 	free(buffer);
}