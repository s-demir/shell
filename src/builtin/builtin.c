/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:59:13 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/08 17:21:21 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	env_or_pwd_cmd(char *_cmd, t_env *env, int con, int *_out_fd)
{
	int					a;
	char				*abs_pwd;

	a = -1;
	if (str_cmp(_cmd, "env", NULL))
	{
		if (con)
			print_export_declaration_to_fd(env, _out_fd);
		else
		{
			while (env->parsed_env[++a])
				print_env_var_to_fd(
					env->parsed_env[a][0], env->parsed_env[a][1], _out_fd[1]);
		}
		return (0);
	}
	abs_pwd = get_current_working_directory(100, 5, _out_fd[1]);
	if (abs_pwd)
	{
		ft_putendl_fd(abs_pwd, _out_fd[1]);
		return (0);
	}
	return (256);
}

char	**export_cmd(char **_cmd, t_env *env, int *_out_fd, int **s)
{
	int				a;
	int				b;

	a = 1;
	while (_cmd[a])
	{
		b = export_statment_check(_cmd[a]);
		if (b > 0)
		{
			if (b >= 1 && _cmd[a][b] == '+')
				append_env_var(_cmd[a], env);
			else
				update_environment_var(_cmd[a], env);
		}
		else
		{
			ft_putstr_fd("   err: export(\'", _out_fd[1]);
			ft_putstr_fd(_cmd[a], _out_fd[1]);
			ft_putendl_fd("\') : Not a valid thing",
				_out_fd[1]);
			**s = 256;
		}
		a++;
	}
	return (_cmd);
}

char	**unset_or_export_cmd(char **_cmd, t_env *env, int *_out_fd, int *s)
{
	int				a;
	int				c;

	a = 1;
	*s = 0;
	if (_cmd[a] && str_cmp(_cmd[0], "unset", NULL) && env->parsed_env[0])
	{
		while (_cmd[a])
		{
			c = locate_env_var_index(env, _cmd[a]);
			if (c >= 0)
				remove_env_entry(env, c);
			else
				*s = 256;
			a++;
		}
	}
	else if (str_cmp(_cmd[0], "export", NULL))
	{
		if (is_export_with_args(_cmd))
			_cmd = export_cmd(_cmd, env, _out_fd, &s);
		else
			env_or_pwd_cmd("env", env, 1, _out_fd);
	}
	return (_cmd);
}

static int new_path(t_env *env)
{
	char	*new_path;

	new_path = get_current_working_directory(100, 5, 2);
	if (new_path)
	{
		update_pwd_env(new_path, env, -1);
		free(new_path);
		return (0);
	}
	return (256);
}
int	cd_cmd(char **_cmd, t_env *env, int *_out_fd)
{
	int		a;

	if (_cmd[1] && _cmd[2])
		ft_putendl_fd("  err: cd(): Not a cd thing", _out_fd[1]);
	else
	{
		if (change_current_directory(_cmd[1], env) < 0)
			ft_putendl_fd(
				"  err: cd(): Only EXISTING destinations", _out_fd[1]);
		else
		{
			a = locate_env_var_index(env, "PWD");
			if (a >= 0)
			{
				update_old_pwd(env, a);
				remove_env_entry(env, a);
			}
			return (new_path(env));
		}
	}
	return (256);
}
