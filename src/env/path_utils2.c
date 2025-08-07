/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils2.c                                      :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:49:35 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:30:42 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static char	*check_path_without_env(char *file, int mode)
{
	char				*tmp_path;
	int					b;

	b = string_length(file, ' ');
	tmp_path = galloc(b + 1);
	if (!tmp_path)
		return (NULL);
	duplicate_string(tmp_path, file, 0, b);
	if (check_path_access(tmp_path, mode))
		return (tmp_path);
	// gfree(tmp_path);
	return (NULL);
}

char *check_env_path(char *env_path, char *file, int *indices, int mode)
{
    char *path;
    path = build_path_from_env(env_path, file, indices);
    if (!path)
        return NULL;
    if (check_path_access(path, mode))
        return path;
    // // gfree(path);
    return NULL;
}

char *find_cmd_path(char *file, char **envp, char *env_var, int mode)
{
    int indices[4];
    char *result;

    indices[3] = 0;
    indices[1] = string_length(env_var, '\0');
    indices[0] = find_substr_index(envp, env_var, indices[1]);
    if (indices[0] < 0 || (file[0] && file[1] && file[0] == '.' && file[1] == '/'))
	{
        result = check_path_without_env(file, mode);
        return result;
	}
    indices[2] = string_length(envp[indices[0]], '\0');
    if (string_length(file, ' ') != string_length(file, '\0') && !check_path_access(file, mode))
        return NULL;
    while (envp[indices[0]][indices[1]])
	{
        result = check_env_path(envp[indices[0]], file, indices, mode);
        if (result)
            return result;
        if (!indices[3])
            indices[3] = 1;
	}
    return NULL;
}

char	*extract_next_substring(char *str, char del, int *index)
{
	char		*sub;
	int			size;
	int			a;

	while (str[index[0]] && str[index[0]] == del)
		index[0] += 1;
	size = string_length(str + index[0], del);
	sub = galloc(size + 1);
	if (!sub)
		return (NULL);
	a = 0;
	while (str[index[0]] && str[index[0]] != del)
	{
		if (str[index[0]] != 34 && str[index[0]] != 39)
			sub[a++] = str[index[0]];
		index[0] += 1;
	}
	sub[a] = '\0';
	index[0] += 1;
	return (sub);
}

char	**prepare_cmd_arguments(char *cmd, char **envp, int c)
{
	char		**cmd_arr;
	char		*cmd_holder;
	int			i[3];

	i[1] = count_tokens(cmd, ' ');
	cmd_arr = galloc((i[1] + 1) * sizeof(char *));
	i[0] = 0;
	while (c < i[1])
	{
		cmd_holder = extract_next_substring(cmd, '\0', i);
		if (!c && !check_if_command_is_builtin(cmd_holder))
		{
			cmd_arr[c] = find_cmd_path(cmd_holder, envp, "PATH", X_OK);
			if (!cmd_arr[c])
				cmd_arr[c] = ft_strdup(cmd_holder);
			// // gfree(cmd_holder);
		}
		else
			cmd_arr[c] = cmd_holder;
		if (!cmd_arr[c])
			return (NULL);
		c += 1;
	}
	cmd_arr[c] = NULL;
	return (cmd_arr);
}
