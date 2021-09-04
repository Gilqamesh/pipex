/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/24 13:33:36 by edavid            #+#    #+#             */
/*   Updated: 2021/09/04 17:32:53 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	redirect_stdin(t_pipex *mystruct)
{
	if (close(mystruct->hereDocPipe[1]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed\n");
	mydup2(mystruct, mystruct->hereDocPipe[0], STDIN_FILENO);
	if (close(mystruct->hereDocPipe[0]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed\n");
}

void	read_until_delimiter(t_pipex *mystruct)
{
	char	*line;
	int		ret;

	if (pipe(mystruct->hereDocPipe) == -1)
		error_handler(mystruct, PIPEX_EPIPE, "pipe() failed\n");
	while (1)
	{
		ret = get_next_line(STDIN_FILENO, &line);
		if (ret == -1)
			error_handler(mystruct, PIPEX_ERR, "get_next_line() failed\n");
		if (!ft_strcmp(line, mystruct->delimiter))
			ret = 0;
		else if (write(mystruct->hereDocPipe[1], line, ft_strlen(line)) == -1)
		{
			free(line);
			error_handler(mystruct, PIPEX_ERR, "write() failed\n");
		}
		free(line);
		if (ret && write(mystruct->hereDocPipe[1], "\n", 1) == -1)
			error_handler(mystruct, PIPEX_ERR, "write() failed\n");
		if (ret == 0)
			break ;
	}
	redirect_stdin(mystruct);
}
