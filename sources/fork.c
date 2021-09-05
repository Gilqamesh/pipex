/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   fork.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:50:28 by edavid            #+#    #+#             */
/*   Updated: 2021/09/05 19:06:53 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	redirectInputFromFile(t_pipex *mystruct)
{
	mydup2(mystruct, mystruct->file[0], STDIN_FILENO);
	if (close(mystruct->file[0]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed");
}

/*
** Creates connection between input file and first CMD then runs execve on CMD
*/
void	handle_inputFile_firstCmd(t_pipex *mystruct, char *argv[], char *envp[])
{
	closePipe(mystruct, 0, 0);
	if (!mystruct->isHereDoc)
	{
		mystruct->file[0] = open(argv[1], O_RDONLY);
		if (mystruct->file[0] == -1)
			error_handler(mystruct, PIPEX_EFOPEN, "Could not open infile\n");
		redirectInputFromFile(mystruct);
	}
	mydup2(mystruct, mystruct->pipes[0][1], STDOUT_FILENO);
	closePipe(mystruct, 0, 1);
	if (mystruct->isHereDoc)
		read_until_delimiter(mystruct);
	if (execve(mystruct->commands[0][0], mystruct->commands[0], envp) == -1)
		error_handler(mystruct, PIPEX_EEXIT, "command not found\n");
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
		ret = get_next_line(STDIN_FILENO, &line);
		if (ret == -1)
			error_handler(mystruct, PIPEX_ERR, "get_next_line() failed\n");
		if (write(mystruct->file[1], line, ft_strlen(line)) == -1)
		{
			free(line);
			error_handler(mystruct, PIPEX_ERR, "write() failed at line\n");
		}
		free(line);
		if (ret)
			if (write(mystruct->file[1], "\n", 1) == -1)
				error_handler(mystruct, PIPEX_ERR, "write() failed\n");
		if (ret == 0)
			break ;
	}
	if (close(mystruct->file[1]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed\n");
}

int	handle_lastCmd_outputFile(t_pipex *mystruct)
{
	int	statusCode;

	closePipe(mystruct, mystruct->nOfCmds - 1, 1);
	statusCode = wait_childProcess();
	mydup2(mystruct, mystruct->pipes[mystruct->nOfCmds - 1][0], STDIN_FILENO);
	closePipe(mystruct, mystruct->nOfCmds - 1, 0);
	transfer_data(mystruct);
	return (statusCode);
}

/*
** Closes open pipes up to but excluding 'upToPipeNum'
*/
void	closePreviousPipes(t_pipex *mystruct, int upToPipeNum)
{
	int	i;

	if (mystruct->openPipes == NULL)
		return ;
	i = -1;
	while (++i < upToPipeNum)
	{
		closePipe(mystruct, i, 0);
		closePipe(mystruct, i, 1);
	}
}
