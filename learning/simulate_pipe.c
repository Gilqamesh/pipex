#include <unistd.h>
#include <fcntl.h>
#include <sys/wait.h>
#include <stdio.h>

int	main(int argc, char *argv[], char *envp[])
{
	int		fd[2];
	int		openfd;
	int		openfd_copy;
	int		status_code;
	pid_t	pid;

	if (argc != 5)
	{
		return (1);
	}
	pid = fork();
	if (pid < 0)
		return (2);
	if (pid == 0)
	{
		// Child process

		// By using bitwise OR on the flags, open is going to know that both or
		// multiple flags are turned on.
		// O_CREAT creates the file in case it doesn't exist, third argument
		// is permission type which we set with an octal
		// openfd = open(argv[1], O_RDONLY | O_CREAT, 0777);
		// if (openfd == -1)
		// 	return (4);
		// File descriptors can be the same for 2 processes, but cant for the
		// same process. Its a key to an input/output resource (file, terminal
		// input/output, pipe, etc..). Every time a process is started,
		// automatically a file descriptor 0 is linked and open to STDIN.
		// Similarly 1 is linked to STDOUT, 2 is linked and open to STDERR.

		// Returns another file descriptor that points to the same file as
		// openfd does.
		// openfd_copy = dup(openfd);

		// Closes the second parameter FD and sets it to where openfd is
		// pointing at. Return value is the second parameter.
		openfd_copy = dup2(openfd, STDOUT_FILENO);
		// In this example if we called printf(), it would actually be printing
		// to the file that openfd was pointing to originally, so to our file.
		// Finally, close previous FD as we dont need it anymore.
		close(openfd);

		// Important to call exec on the child process as the parent process
		// has to wait for its child processes, otherwise the parent process
		// will get replaced by the exec function and we lose reference
		// to all the child processes (zombie processes)
		if (execve("/bin/cat", (char *[]){"/bin/cat", argv[1], NULL}, envp) == -1)
			return (3); // Could not find program to execute and this process
						// keeps on running if we wouldnt return an error
	}
	else
	{
		// Parent process
		// either wait(NULL) works to wait for the execution of the child
		// (wait for state change), if we pass in an *int variable, it can
		// also be observed to see if the child process got terminated
		// normally (or not in the case of SIGKILL for example or other
		// errors that might have occured)
		wait(&pid);
		if (WIFEXITED(pid))
		{
			// normal execution of the child process

			// the status code of a process is what we return from the main,
			// at the end of main it's usually return (0)
			status_code = WEXITSTATUS(pid);
			if (status_code == 0)
			{
				// successful execution of child process
				printf("Successful execution of child process\n");
			}
			else
			{
				// failed execution of the child process
				printf("Failed execution of the child process with an \
					execution code of %d\n", status_code);
			}
		}
		printf("Some post processing goes here\n");
	}

	// Close all ends of the pipe, otherwise child processes that are reading
	// from the pipe might be waiting for the pipes to be closed
	close(fd[0]); 
	close(fd[1]); 

	return (0);
}

