/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:47:38 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/07 21:12:51 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*build_path_from_env(char *env_var, char *file, int *indx_s)
{
	char	*tmp_path;
	int		a;
	int		b;
	int		file_size;

	advance_path_index(env_var, indx_s, &a);
	file_size = string_length(file, ' ');
	tmp_path = galloc((indx_s[1] - a) + file_size + 2);
	if (!tmp_path)
		return (NULL);
	b = 0;
	while (b < ((indx_s[1] - a) + file_size + 1))
	{
		if (indx_s[3] && (b < indx_s[1] - a))
			tmp_path[b] = env_var[a + b];
		else if (indx_s[3]
			&& env_var[indx_s[1] - 1] != '/'
			&& (b == indx_s[1] - a))
			tmp_path[b] = '/';
		else
			tmp_path[b] = file[b - (indx_s[1] - a) - indx_s[3]];
		b++;
	}
	tmp_path[b] = '\0';
	return (tmp_path);
}

int	find_substr_index(char **haystack, char *needle, int n_length)
{
	int	a;
	int	b;

	a = 0;
	while (haystack[a] != 0)
	{
		if (haystack[a][0] == needle[0])
		{
			b = 0;
			while (haystack[a][b]
				&& haystack[a][b] == needle[b])
			{
				if (b == n_length - 1)
					return (a);
				b++;
			}
		}
		a++;
	}
	return (-1);
}

int	check_path_access(char *path, int mode)
{
	if (access(path, mode) < 0)
		return (0);
	return (1);
}

void	advance_path_index(char *env_var, int *indx_s, int *a)
{
	if (indx_s[3])
	{
		indx_s[1] += 1;
		*a = indx_s[1];
		while (env_var[indx_s[1]] != '\0'
			&& env_var[indx_s[1]] != ':')
			indx_s[1] += 1;
	}
	else
		*a = indx_s[1];
}

int	count_tokens(char *str, char del)
{
	int	a;
	int	b;
	int	res;

	a = 0;
	b = 1;
	res = 0;
	while (str && str[a])
	{
		if (str[a] != del)
		{
			if (b)
				res += 1;
			b = 0;
		}
		else
			b = 1;
		a += 1;
	}
	return (res);
}
