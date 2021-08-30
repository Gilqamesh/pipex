/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   here_doc.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/24 13:33:36 by edavid            #+#    #+#             */
/*   Updated: 2021/08/27 11:41:48 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

static void	redirect_stdin(t_pipex *mystruct)
{
	if (close(mystruct->hereDocPipe[1]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	if (dup2(mystruct->hereDocPipe[0], STDIN_FILENO) == -1)
		error_handler(mystruct, PIPEX_EDUP2, "dup2() failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	if (close(mystruct->hereDocPipe[0]) == -1)
		error_handler(mystruct, PIPEX_EFCLOSE, "close() failed at line %d in \
			file %s\n", __LINE__, __FILE__);
}

void	read_until_delimiter(t_pipex *mystruct)
{
	char	*line;
	int		ret;

	if (pipe(mystruct->hereDocPipe) == -1)
		error_handler(mystruct, PIPEX_EPIPE, "%d %s\n", __LINE__, __FILE__);
	while (1)
	{
		ret = get_next_line(STDIN_FILENO, &line);
		if (ret == -1)
			error_handler(mystruct, PIPEX_ERR, "%d %s\n", __LINE__, __FILE__);
		if (!ft_strcmp(line, mystruct->delimiter))
			ret = 0;
		else if (write(mystruct->hereDocPipe[1], line, ft_strlen(line)) == -1)
		{
			free(line);
			error_handler(mystruct, PIPEX_ERR, "%d %s\n", __LINE__, __FILE__);
		}
		free(line);
		if (ret && write(mystruct->hereDocPipe[1], "\n", 1) == -1)
			error_handler(mystruct, PIPEX_ERR, "%d %s\n", __LINE__, __FILE__);
		if (ret == 0)
			break ;
	}
	redirect_stdin(mystruct);
}
