/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   pipex.h                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/19 16:24:08 by edavid            #+#    #+#             */
/*   Updated: 2021/08/27 18:26:43 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#ifndef PIPEX_H
# define PIPEX_H

# include "../mylib/mylib.h"
# include <stdio.h>
# include <unistd.h>
# include <sys/wait.h>
# include <fcntl.h>

typedef struct s_pipex
{
	char	***commands;
	int		nOfCmds;
	int		file[2];
	int		(*pipes)[2];
	bool	(*openPipes)[2];
	bool	isHereDoc;
	int		hereDocPipe[2];
	char	*delimiter;
	t_list	*alloced_lst;
}	t_pipex;

void	error_handler(t_pipex *mystruct, int errcode, char *message, ...);
void	handle_inputFile_firstCmd(t_pipex *mystruct, char *argv[],
			char *envp[]);
void	handle_lastCmd_outputFile(t_pipex *mystruct, int argc, char *argv[]);
void	destroy_mystruct(t_pipex *mystruct);
void	initialize_mystruct(int argc, char *argv[], char *envp[],
			t_pipex *mystruct);
void	cmd_path(t_pipex *mystruct, char **cmd, char *envp[]);
void	closePreviousPipes(t_pipex *mystruct, int upToPipeNum);
void	read_until_delimiter(t_pipex *mystruct);
void	wait_childProcess(t_pipex *mystruct);

/*
** Error codes
*/
# define PIPEX_ECMD		1	/* Command was not found */
# define PIPEX_EUSAGE	2	/* Wrong usage */
# define PIPEX_EMALLOC	3	/* Malloc failed */
# define PIPEX_ERR		4	/* Unexpected error */
# define PIPEX_EFOPEN	5	/* Could not open file */
# define PIPEX_EFCLOSE	6	/* Could not close file */
# define PIPEX_EPIPE	7	/* pipe() failed */
# define PIPEX_EFORK	8	/* fork() failed */
# define PIPEX_EDUP2	9	/* dup2() failed */
# define PIPEX_ESTATUS	10	/* Status code error */
# define PIPEX_EEXIT	11	/* Process has not exited properly */

#endif
