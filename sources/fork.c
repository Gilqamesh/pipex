/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:50:28 by edavid            #+#    #+#             */
/*   Updated: 2021/08/31 16:27:05 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	redirectInputFromFile(t_pipex *mystruct)
{
	if (dup2(mystruct->file[0], STDIN_FILENO) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2() failed at line %d \
			in file %s\n", __LINE__, __FILE__);
	if (close(mystruct->file[0]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d \
			in file %s\n", __LINE__, __FILE__);
}

/*
** Creates connection between input file and first CMD then runs execve on CMD
*/
void	handle_inputFile_firstCmd(t_pipex *mystruct, char *argv[], char *envp[])
{
	if (!mystruct->isHereDoc)
	{
		mystruct->file[0] = open(argv[1], O_RDONLY);
		if (mystruct->file[0] == -1)
			error_handler(mystruct, PIPEX_EFOPEN, "Could not open file %s\n",
				argv[1]);
		redirectInputFromFile(mystruct);
	}
	mystruct->openPipes[0][0] = false;
	if (close(mystruct->pipes[0][0]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line \
			%d in file %s\n", __LINE__, __FILE__);
	if (dup2(mystruct->pipes[0][1], STDOUT_FILENO) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2() failed at line %d \
			in file %s\n", __LINE__, __FILE__);
	mystruct->openPipes[0][1] = false;
	if (close(mystruct->pipes[0][1]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d \
			in file %s\n", __LINE__, __FILE__);
	if (mystruct->isHereDoc)
		read_until_delimiter(mystruct);
	if (execve(mystruct->commands[0][0], mystruct->commands[0], envp) == -1)
		error_handler(mystruct, PIPEX_ECMD, "Unable to execute %s\n",
			mystruct->commands[0][0]);
}

/*
** Transfers the data read from last CMD to output file through a pipe
*/
static void	transfer_data(t_pipex *mystruct)
{
	char	*line;
	int		ret;

	while (1)
	{
		ret = get_next_line(mystruct->pipes[mystruct->nOfCmds - 1][0], &line);
		if (ret == -1)
			error_handler(mystruct, PIPEX_ERR, "get_next_line() failed at \
				line %d in file %s\n", __LINE__, __FILE__);
		if (write(mystruct->file[1], line, ft_strlen(line)) == -1)
		{
			free(line);
			error_handler(mystruct, PIPEX_ERR, "write() failed at line %d \
				in file %s\n", __LINE__, __FILE__);
		}
		free(line);
		if (ret)
			if (write(mystruct->file[1], "\n", 1) == -1)
				error_handler(mystruct, PIPEX_ERR, "write() failed at line %d \
					in file %s\n", __LINE__, __FILE__);
		if (ret == 0)
			break ;
	}
}

void	handle_lastCmd_outputFile(t_pipex *mystruct, int argc, char *argv[])
{
	wait_childProcess(mystruct);
	if (mystruct->openPipes[mystruct->nOfCmds - 1][1])
	{
		mystruct->openPipes[mystruct->nOfCmds - 1][1] = false;
		if (close(mystruct->pipes[mystruct->nOfCmds - 1][1]) == -1)
			error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d \
				in file %s\n", __LINE__, __FILE__);
	}
	mystruct->file[1] = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC,
			0777);
	if (mystruct->file[1] == -1)
		error_handler(mystruct, PIPEX_EFOPEN, "Could not open file %s\n",
			argv[argc - 1]);
	transfer_data(mystruct);
	if (close(mystruct->file[1]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d \
			in file %s\n", __LINE__, __FILE__);
	if (mystruct->openPipes[mystruct->nOfCmds - 1][0])
	{
		mystruct->openPipes[mystruct->nOfCmds - 1][0] = false;
		if (close(mystruct->pipes[mystruct->nOfCmds - 1][0]) == -1)
			error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d \
				in file %s\n", __LINE__, __FILE__);
	}
}

/*
** Closes open pipes up to but excluding 'upToPipeNum'
*/
void	closePreviousPipes(t_pipex *mystruct, int upToPipeNum)
{
	int	i;

	i = -1;
	while (++i < upToPipeNum)
	{
		if (mystruct->openPipes && mystruct->openPipes[i][0])
		{
			mystruct->openPipes[i][0] = false;
			if (close(mystruct->pipes[i][0]) == -1)
				error_handler(mystruct, PIPEX_EFCLOSE, "Pipe failed to \
					close() at line %d in file %s\n", __LINE__, __FILE__);
		}
		if (mystruct->openPipes && mystruct->openPipes[i][1])
		{
			mystruct->openPipes[i][1] = false;
			if (close(mystruct->pipes[i][1]) == -1)
				error_handler(mystruct, PIPEX_EFCLOSE, "Pipe failed to \
					close() at line %d in file %s\n", __LINE__, __FILE__);
		}
	}
}
