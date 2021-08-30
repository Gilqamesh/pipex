/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:42:42 by edavid            #+#    #+#             */
/*   Updated: 2021/08/30 11:20:29 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	handleChildProcess(t_pipex *mystruct, int curPipeNum, char *envp[])
{
	if (mystruct->openPipes[curPipeNum][0])
	{
		mystruct->openPipes[curPipeNum][0] = false;
		if (close(mystruct->pipes[curPipeNum][0]) == -1)
			error_handler(mystruct, PIPEX_EFCLOSE, "Pipe failed to \
				close() at line %d in file %s\n", __LINE__, __FILE__);
	}
	if (dup2(mystruct->pipes[curPipeNum - 1][0], STDIN_FILENO) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2 failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	if (dup2(mystruct->pipes[curPipeNum][1], STDOUT_FILENO) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2 failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	if (mystruct->openPipes[curPipeNum][1])
	{
		mystruct->openPipes[curPipeNum][1] = false;
		if (close(mystruct->pipes[curPipeNum][1]) == -1)
			error_handler(mystruct, PIPEX_EFCLOSE, "Pipe failed to \
				close() at line %d in file %s\n", __LINE__, __FILE__);
	}
	closePreviousPipes(mystruct, curPipeNum);
	if (execve(mystruct->commands[curPipeNum][0],
		mystruct->commands[curPipeNum], envp) == -1)
		error_handler(mystruct, PIPEX_ECMD, "Unable to execute %s\n",
			mystruct->commands[curPipeNum][0]);
}

/*
** Makes a pipe between two commands
** And waits for the previous child process
*/
static void	createPipe_betweenTwoCmds(t_pipex *mystruct, int curPipeNum,
char *envp[])
{
	pid_t	pid;

	wait_childProcess(mystruct);
	pipe(mystruct->pipes[curPipeNum]);
	mystruct->openPipes[curPipeNum][0] = true;
	mystruct->openPipes[curPipeNum][1] = true;
	pid = fork();
	if (pid == 0)
		handleChildProcess(mystruct, curPipeNum, envp);
}

void	wait_childProcess(t_pipex *mystruct)
{
	pid_t	pid;
	int		statusCode;
	int		wstatus;

	pid = wait(&wstatus);
	if (pid == -1)
		error_handler(mystruct, PIPEX_ERR, "wait failed at line %d in file \
			%s\n", __LINE__, __FILE__);
	if (WIFEXITED(wstatus))
	{
		statusCode = WEXITSTATUS(wstatus);
		if (statusCode)
			error_handler(mystruct, PIPEX_ESTATUS, "Child process of PID %d\
					has executed with a status code of %d\n", pid, statusCode);
	}
	else
		error_handler(mystruct, PIPEX_EEXIT, "Child process of PID %d has \
			not exited properly\n", pid);
}

int	main(int argc, char *argv[], char *envp[])
{
	pid_t	pid;
	int		i;
	t_pipex	mystruct;

	initialize_mystruct(argc, argv, envp, &mystruct);
	if (pipe(mystruct.pipes[0]) == -1)
		error_handler(&mystruct, PIPEX_EPIPE, "pipe() failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	mystruct.openPipes[0][0] = true;
	mystruct.openPipes[0][1] = true;
	pid = fork();
	if (pid == -1)
		error_handler(&mystruct, PIPEX_EFORK, "fork() failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	if (pid == 0)
		handle_inputFile_firstCmd(&mystruct, argv, envp);
	i = 0;
	while (++i < mystruct.nOfCmds)
		createPipe_betweenTwoCmds(&mystruct, i, envp);
	closePreviousPipes(&mystruct, i - 1);
	handle_lastCmd_outputFile(&mystruct, argc, argv);
	destroy_mystruct(&mystruct);
	return (0);
}
