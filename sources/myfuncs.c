/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/09/04 19:23:43 by edavid            #+#    #+#             */
/*   Updated: 2021/09/04 19:24:22 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

/*
** fork() with error handling
*/
pid_t	myfork(t_pipex *mystruct)
{
	pid_t	pid;

	pid = fork();
	if (pid == -1)
		error_handler(mystruct, PIPEX_EFORK, "fork() failed\n");
	return (pid);
}

/*
** dup2() with error handling
*/
void	mydup2(t_pipex *mystruct, int fromFd, int toFd)
{
	if (dup2(fromFd, toFd) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2() failed\n");
}

/*
** Opens 'pipeNumber'
** In case there is an error with pipe() it calls error_handler
*/
void	openPipe(t_pipex *mystruct, int pipeNumber)
{
	if (pipe(mystruct->pipes[pipeNumber]) == -1)
		error_handler(mystruct, PIPEX_EPIPE, "pipe() failed");
	mystruct->openPipes[pipeNumber][0] = true;
	mystruct->openPipes[pipeNumber][1] = true;
}

/*
** Closes 'pipeNumber' read/write end for mystruct->pipes
** In case there is an error with close() it calls error_handler
*/
void	closePipe(t_pipex *mystruct, int pipeNumber, int read_or_write_end)
{
	if (mystruct->openPipes[pipeNumber][read_or_write_end])
	{
		mystruct->openPipes[pipeNumber][read_or_write_end] = false;
		if (close(mystruct->pipes[pipeNumber][read_or_write_end]) == -1)
			error_handler(mystruct, PIPEX_EFCLOSE, "close() failed\n");
	}
}
