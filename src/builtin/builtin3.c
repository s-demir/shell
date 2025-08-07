/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:02:22 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:10:57 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*build_pwd_string(char *new_path)
{
	char	*pwd_str;
	int		len;
	int		idx;

	len = string_length(new_path, '\0') + 5;
	pwd_str = galloc(len * sizeof(char));
	if (!pwd_str)
		return (NULL);
	duplicate_string(pwd_str, "PWD=", 0, 4);
	idx = 0;
	while (new_path[idx])
	{
		pwd_str[4 + idx] = new_path[idx];
		idx++;
	}
	pwd_str[4 + idx] = '\0';
	return (pwd_str);
}

void	__exit(char **_cmd_)
{
	int	status;

	status = 0;
	if (_cmd_[1] && _cmd_[2])
		status = 1;
	else if (_cmd_[1] && !is_numeric_string(_cmd_[1]))
		status = 255;
	else if (_cmd_[1])
		status = strtoint(_cmd_[1]);
	//gfree_string_array(_cmd_);
	clear_garbage();
	exit(status);
}

int	is_export_with_args(char **cmd)
{
	int	a;

	a = 1;
	while (cmd[0] && cmd[a])
	{
		if (cmd[a])
			return (1);
		a++;
	}
	return (0);
}

int	export_statment_check(char *_cmd_)
{
	int	a;
	int	b;

	a = string_length(_cmd_, '=');
	if (a > 1 && _cmd_[a - 1] == '+')
		a -= 1;
	if (a)
	{
		b = 0;
		while (b < a)
		{
			if (!b && !ft_isalpha(_cmd_[b]) && _cmd_[b] != '_')
				return (-1);
			else if (b && !ft_isalnum(_cmd_[b]) && _cmd_[b] != '_')
				return (-1);
			b++;
		}
	}
	return (a);
}
