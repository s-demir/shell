/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   start_shell.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 15:12:20 by amsagman          #+#    #+#             */
/*   Updated: 2025/08/08 17:42:10 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	update_old_pwd(t_env *env, int idx)
{
	char	*old_pwd;
	int		oldpwd_idx;
	char	*oldpwd_str;

	if (idx >= 0)
	{
		old_pwd = strcopy(env->parsed_env[idx][1]);
		oldpwd_idx = locate_env_var_index(env, "OLDPWD");
		if (oldpwd_idx >= 0)
			remove_env_entry(env, oldpwd_idx);
		oldpwd_str = galloc(string_length(old_pwd, '\0') + 8); // "OLDPWD=" + pwd + '\0'
		if (oldpwd_str)
		{
			duplicate_string(oldpwd_str, "OLDPWD=", 0, 7);
			duplicate_string(oldpwd_str + 7, old_pwd, 0, string_length(old_pwd, '\0'));
			oldpwd_str[string_length(old_pwd, '\0') + 7] = '\0';
			update_environment_var(oldpwd_str, env);
		}
	}
}

static void update_pwd(t_env *env, int idx)
{
	char	*pwd;

	if (idx >= 0)
		update_old_pwd(env, idx);
	pwd = get_current_working_directory(100, 5, 2);
	if (pwd)
	{
		if (idx >= 0)
			remove_env_entry(env, idx);
		update_pwd_env(pwd, env, idx);
	}
}

static void set_defaults(t_env *env, int idx)
{
    idx = locate_env_var_index(env, "SHELL");
    if (idx >= 0)
        remove_env_entry(env, idx);
    update_environment_var("SHELL=minishell", env);
    update_environment_var("?=0", env);
    idx = locate_env_var_index(env, "PWD");
    update_pwd(env, idx);
}

static void update_shlvl(t_env *env, int idx)
{
    int value;
    value = 0;
    if (idx >= 0)
        value = strtoint(env->parsed_env[idx][1]);
    set_env_status(env, value + 1, "SHLVL=");
}

int start_shell(t_env *env, char **orig_env)
{
    int status;
    int idx;
    if (!env)
        return 0;
    status = setup_env(env, orig_env, 0);
    idx = locate_env_var_index(env, "SHLVL");
    update_shlvl(env, idx);
    set_defaults(env, 0);
    return status;
}
