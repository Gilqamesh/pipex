/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:42:39 by edavid            #+#    #+#             */
/*   Updated: 2021/09/01 14:39:58 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

void	error_handler(t_pipex *mystruct, int errcode, char *message, ...)
{
	va_list	ap;

	destroy_mystruct(mystruct);
	va_start(ap, message);
	ft_printf(message, ap);
	va_end(ap);
	perror(NULL);
	exit(errcode);
}

/*
** Frees all allocated memory associated with 'mystruct'
*/
void	destroy_mystruct(t_pipex *mystruct)
{
	int	i;

	close(mystruct->file[1]);
	closePreviousPipes(mystruct, mystruct->nOfCmds);
	i = -1;
	if (mystruct->commands)
		while (++i < mystruct->nOfCmds && mystruct->commands[i])
			ft_destroy_str_arr(mystruct->commands + i);
	ft_lstmallocfree(&mystruct->alloced_lst);
}

/*
** Helper function for initialize_mystruct in initialize.c
*/
void	initOutFile(t_pipex *mystruct, int argc, char **argv)
{
	mystruct->file[1] = open(argv[argc - 1], O_WRONLY | O_CREAT | O_TRUNC,
			0777);
	if (mystruct->file[1] == -1)
		error_handler(mystruct, PIPEX_EFOPEN, "Could not open file %s\n",
			argv[argc - 1]);
}
