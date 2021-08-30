/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   initialize.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: edavid <edavid@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/08/23 15:50:33 by edavid            #+#    #+#             */
/*   Updated: 2021/08/30 11:21:37 by edavid           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../headers/pipex.h"

/*
** Initializes command arguments and store them
** Warning, delimiter used for each argument is ' ', so stuff like awk is not
** handled, ex.: "awk '{print $3}'" would be interpreted as:
** "awk" "'{print" "$3}'"
*/
static void	initialize_Cmds(t_pipex *mystruct, char *argv[], char *envp[])
{
	int	i;
	int	firstArgIndex;

	if (mystruct->isHereDoc)
		firstArgIndex = 3;
	else
		firstArgIndex = 2;
	i = -1;
	while (++i < mystruct->nOfCmds)
	{
		mystruct->commands[i] = ft_split(argv[i + firstArgIndex], ' ');
		if (mystruct->commands[i] == NULL)
			error_handler(mystruct, PIPEX_ERR, "Something went wrong with \
				ft_split at line %d in file %s\n", __LINE__, __FILE__);
		if (mystruct->commands[i][0] == NULL)
			error_handler(mystruct, PIPEX_EUSAGE, "Empty command\n");
		cmd_path(mystruct, &mystruct->commands[i][0], envp);
	}
}

/*
** Initializes values related to here_doc in 'mystruct'
*/
static void	init_hereDoc(t_pipex *mystruct, char *argv[])
{
	mystruct->isHereDoc = true;
	mystruct->nOfCmds--;
	if (mystruct->nOfCmds < 1)
		error_handler(mystruct, PIPEX_EUSAGE,
			"Usage: ./pipex here_doc LIMITER cmd [commands ...] outfile\n");
	mystruct->delimiter = ft_strdup(argv[2]);
	if (mystruct->delimiter == NULL)
		error_handler(mystruct, PIPEX_ERR, "Malloc failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	ft_lstadd_front(&mystruct->alloced_lst, ft_lstnew(mystruct->delimiter));
}

/*
** Initializes t_pipex variable
*/
void	initialize_mystruct(int argc, char *argv[], char *envp[],
t_pipex *mystruct)
{
	ft_bzero(mystruct, sizeof(*mystruct));
	mystruct->nOfCmds = argc - 3;
	if (mystruct->nOfCmds < 1)
		error_handler(mystruct, PIPEX_EUSAGE,
			"Usage: ./pipex infile cmd1 [additional commands ...] outfile\n");
	if (!ft_strcmp(argv[1], "here_doc"))
		init_hereDoc(mystruct, argv);
	mystruct->commands = ft_lstmallocwrapper(&mystruct->alloced_lst,
			(mystruct->nOfCmds + 1) * sizeof(*mystruct->commands), false);
	if (mystruct->commands == NULL)
		error_handler(mystruct, PIPEX_EMALLOC, "Malloc failed at line %d in \
			file %s\n", __LINE__, __FILE__);
	initialize_Cmds(mystruct, argv, envp);
	mystruct->commands[mystruct->nOfCmds] = NULL;
	mystruct->pipes = ft_lstmallocwrapper(&mystruct->alloced_lst,
			mystruct->nOfCmds * sizeof(*mystruct->pipes), false);
	mystruct->openPipes = ft_lstmallocwrapper(&mystruct->alloced_lst,
			mystruct->nOfCmds * sizeof(*mystruct->openPipes), true);
	if (mystruct->pipes == NULL)
		error_handler(mystruct, PIPEX_EMALLOC, "Malloc failed at line %d in \
			file %s\n", __LINE__, __FILE__);
}

/*
** Returns allocated string, that is the path for 'cmd'
** It tries out all the possibilities in the string array 'paths'
** If none of them are valid paths for 'cmd', the function returns NULL
*/
static char	*get_cur_path(t_pipex *mystruct, char **paths, char **cmd)
{
	int		i;
	char	*cur_path;

	i = -1;
	while (paths[++i])
	{
		cur_path = ft_strjoin_free(ft_strdup(paths[i]), ft_strjoin("/", *cmd));
		if (cur_path == NULL)
			error_handler(mystruct, PIPEX_ERR, "Something went wrong with\
				ft_strjoin_free at line %d in file %s\n", __LINE__, __FILE__);
		if (!access(cur_path, F_OK))
			break ;
		free(cur_path);
	}
	if (paths[i] == NULL)
		cur_path = NULL;
	return (cur_path);
}

/*
** Allocates and returns a string that is the path of the UNIX command 'cmd'
** 'envp' is the environment variable from the shell
** Returns NULL on failure
*/
void	cmd_path(t_pipex *mystruct, char **cmd, char *envp[])
{
	int		i;
	char	**paths;
	char	*cur_path;

	i = -1;
	while (envp[++i] && ft_strncmp(envp[i], "PATH=", 5))
		;
	paths = ft_split(envp[i] + 5, ':');
	if (paths == NULL)
		error_handler(mystruct, PIPEX_ERR, "Something went wrong with\
				ft_split at line %d in file %s\n", __LINE__, __FILE__);
	cur_path = get_cur_path(mystruct, paths, cmd);
	if (cur_path == NULL)
		error_handler(mystruct, PIPEX_ECMD, "No path found for %s\n", *cmd);
	ft_destroy_str_arr(&paths);
	free(*cmd);
	*cmd = cur_path;
}
