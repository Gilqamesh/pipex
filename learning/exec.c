#include <stdio.h>
#include <unistd.h>

int main(int argc, char *argv[], char *envp[])
{
	// int		i;

	// i = 0;
	// while (envp[i])
	// 	printf("%s\n", envp[i++]);
	
	// exec and then combination of v/l, p, e
	// l - list of arguments, v - vector as an argument (string arr)
	// p - function uses the path variable so we dont have to explicitely
	//	name it in each string
	// e - environment vector (char *envp[])

	execve(argv[1], argv, envp);
	return (0);
}
