/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec_init2.c                                   :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:55:40 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:10:57 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	check_directory_status(char *path_, char *file, int *status)
{
	struct stat		s;

	if (file && str_cmp(file, ".", NULL))
		*status = 2;
	else if (str_cmp(file, "..", NULL)
		|| str_cmp(file, ",", ""))
	{
		*status = 1;
		errno = 2;
	}
	else if (!stat(path_, &s)
		&& S_ISDIR(s.st_mode))
	{
		*status = 2;
		ft_putstr_fd(" err: this \'", 2);
		ft_putstr_fd(path_, 2);
		ft_putendl_fd("\' Is a directory", 2);
		errno = 13;
	}
}

int	report_command_error(char *file, int _status)
{
	if (_status == 1)
	{
		_status = get_exit_status(errno);
		ft_putstr_fd("   err: \'", 2);
		ft_putstr_fd(file, 2);
		ft_putstr_fd("\' ", 2);
		ft_putendl_fd(strerror(errno), 2);
		return (_status);
	}
	else if (_status)
	{
		ft_putstr_fd("   minishell(\'", 2);
		ft_putstr_fd(file, 2);
		ft_putendl_fd("': command not found", 2);
	}
	return (_status);
}

void verify_path(t_ast_node *head, char **env, int *status)
{
    char *path;
    path = find_cmd_path(head->args[0], env, "PWD", R_OK);
    if (!path)
        *status = 0;
    else
	{
        check_directory_status(path, head->args[0], status);
        // gfree(path);
	}
    *status = report_command_error(head->args[0], *status);
}

int	check_cmd_perms(t_ast_node *head, char **env)
{
    int status;
    status = 0;
    if (head->args && !check_if_command_is_builtin(head->args[0]) && head->file_type == READ_FILE)
        verify_path(head, env, &status);
    if (!status && head->left)
        status = check_cmd_perms(head->left, env);
    if (!status && head->right)
        status = check_cmd_perms(head->right, env);
    return status;
}
