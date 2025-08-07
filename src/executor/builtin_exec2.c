/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:27:08 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:10:57 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	**combine_command_arguments(char **f_args, char **cmd)
{
	size_t index;
    char **new_args;
    index = 1;

    if (!f_args)
        return NULL;
    while (cmd[index])
        index++;
    new_args = galloc((index + 1) * sizeof(char *));
    if (!new_args)
        return NULL;
    index = 0;
    if (f_args[0])
        new_args[0] = strcopy(f_args[0]);
    else
        new_args[0] = strcopy("");
    index = 1;
    while (cmd[index])
    {
        new_args[index] = strcopy(cmd[index]);
        index++;
    }
    new_args[index] = NULL;
    gfree_string_array(f_args);
    return new_args;
}

int	is_numeric_string(char *str)
{
	size_t i;

	i = 0;
	while (str[i])
	{
        if (!ft_isdigit(str[i]))
            return (0);
		i++;
    }
    return (1);
}

int	execute_builtin(char **args, int *fd, t_env *env, int *piped)
{
	if (str_cmp(args[0], "exit", NULL))
	{
        int exit_status = 0;
        if (args[1] && args[2])
            return 1;
        if (args[1] && !is_numeric_string(args[1]))
            exit_status = 255;
		else if (args[1])
            exit_status = strtoint(args[1]);
        gfree_string_array(args);
        ft_putendl_fd("EXIT", STDOUT_FILENO);
        cleanup_shell(env, exit_status);
    }
    else if (!piped[PIPE_REDIR_COMBO])
		return execute_builtin_with_piping(args, fd, env, piped);
	else
		return execute_builtin_with_simple_piping(args, fd, env, piped);
    return run_redir_builtin(args, fd, env, piped);
}
