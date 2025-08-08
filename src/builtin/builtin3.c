/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin3.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:02:22 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/08 17:21:25 by sedemir          ###   ########.fr       */
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

void	close_session(char **cmd)
{
	int	status;

	status = 0;
	if (cmd[1] && cmd[2])
		status = 1;
	else if (cmd[1] && !is_numeric_string(cmd[1]))
		status = 255;
	else if (cmd[1])
		status = strtoint(cmd[1]);
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

int	echo_cmd(char **_cmd, int *_out_fd)
{
	int				a;
	int				op_n;

	op_n = 0;
	if (_cmd[0] && _cmd[1] && is_valid_echo_option(_cmd[1]))
		op_n = 1;
	a = op_n + 1;
	while (op_n && _cmd[a] && is_valid_echo_option(_cmd[a]))
		a++;
	if ((_cmd[0] && _cmd[a]) || string_length(_cmd[a], '\0'))
	{
		while (1)
		{
			ft_putstr_fd(_cmd[a], _out_fd[1]);
			a++;
			if (_cmd[a])
				write(_out_fd[1], " ", 1);
			else
				break ;
		}
	}
	if (!op_n)
		write(_out_fd[1], "\n", 1);
	return (0);
}
