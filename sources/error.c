/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   error.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 14:42:39 by edavid            #+#    #+#             */
/*   Updated: 2021/08/31 16:56:16 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

void	error_handler(t_pipex *mystruct, int errcode, char *message, ...)
{
	va_list	ap;

	PRINT_HERE();
	destroy_mystruct(mystruct);
	PRINT_HERE();
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

	PRINT_HERE();
	closePreviousPipes(mystruct, mystruct->nOfCmds);
	PRINT_HERE();
	i = -1;
	if (mystruct->commands)
		while (++i < mystruct->nOfCmds)
			ft_destroy_str_arr(mystruct->commands + i);
	PRINT_HERE();
	ft_lstmallocfree(&mystruct->alloced_lst);
}
