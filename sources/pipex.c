/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:42:42 by edavid            #+#    #+#             */
/*   Updated: 2021/09/05 19:07:07 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	sendEOFtoInput(t_pipex *mystruct)
{
	if (pipe(mystruct->tmpFd) == -1)
		error_handler(mystruct, PIPEX_EPIPE, "pipe() failed\n");
	mydup2(mystruct, mystruct->tmpFd[0], STDIN_FILENO);
	close(mystruct->tmpFd[0]);
	close(mystruct->tmpFd[1]);
}

static void	handleChildProcess(t_pipex *mystruct, int curPipeNum, char *envp[])
{
	closePipe(mystruct, curPipeNum, 0);
	if (mystruct->openPipes[curPipeNum - 1][0])
	{
		mydup2(mystruct, mystruct->pipes[curPipeNum - 1][0], STDIN_FILENO);
		closePipe(mystruct, curPipeNum - 1, 0);
	}
	else
		sendEOFtoInput(mystruct);
	mydup2(mystruct, mystruct->pipes[curPipeNum][1], STDOUT_FILENO);
	closePipe(mystruct, curPipeNum, 1);
	if (execve(mystruct->commands[curPipeNum][0],
		mystruct->commands[curPipeNum], envp) == -1)
		error_handler(mystruct, PIPEX_EEXIT, "command not found\n");
}

/*
** Makes a pipe between two commands
** And waits for the previous child process
*/
static void	createPipe_betweenTwoCmds(t_pipex *mystruct, int curPipeNum,
char *envp[])
{
	pid_t	pid;
	int		wstatus;

	closePipe(mystruct, curPipeNum - 1, 1);
	wstatus = wait_childProcess();
	if (wstatus)
		closePipe(mystruct, curPipeNum - 1, 0);
	openPipe(mystruct, curPipeNum);
	pid = myfork(mystruct);
	if (pid == 0)
		handleChildProcess(mystruct, curPipeNum, envp);
	closePipe(mystruct, curPipeNum - 1, 0);
	closePipe(mystruct, curPipeNum, 1);
}

int	wait_childProcess(void)
{
	pid_t	pid;
	int		statusCode;
	int		wstatus;

	statusCode = 0;
	pid = wait(&wstatus);
	if (pid == -1)
		return (PIPEX_ERR);
	if (WIFEXITED(wstatus))
	{
		statusCode = WEXITSTATUS(wstatus);
		if (statusCode)
			return (PIPEX_ESTATUS);
	}
	else
		return (PIPEX_EEXIT);
	return (statusCode);
}

int	main(int argc, char *argv[], char *envp[])
{
	pid_t	pid;
	int		i;
	t_pipex	mystruct;
	int		statusCode;

	initialize_mystruct(argc, argv, envp, &mystruct);
	openPipe(&mystruct, 0);
	pid = myfork(&mystruct);
	if (pid == 0)
		handle_inputFile_firstCmd(&mystruct, argv, envp);
	i = 0;
	while (++i < mystruct.nOfCmds)
		createPipe_betweenTwoCmds(&mystruct, i, envp);
	statusCode = handle_lastCmd_outputFile(&mystruct);
	destroy_mystruct(&mystruct);
	return (statusCode);
}
