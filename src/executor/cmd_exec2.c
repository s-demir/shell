/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec2.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:34:02 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/08 15:31:54 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	initialize_or_reset_pipe_state(int *_piped, int f)
{
	int			a;

	a = 0;
	_piped[PIPE_ACTIVE] = _piped[PIPE_TOTAL];
	if (f)
		while (a < 12)
			_piped[a++] = 0;
	else if (_piped[PIPE_TOTAL])
		_piped[PIPE_ACTIVE] += 1;
	_piped[PIPE_EXEC_STATE] = 1;
}

int	switch_fds_identifier(
		int *_piped, int index, int index_2, int con)
{
	if (con)
	{
		if (_piped[index])
			safe_close(_piped[index_2]);
		_piped[index] = 1;
	}
	else
	{
		ft_putendl_fd("err: file not found", 2);
		_piped[PIPE_HAS_INPUT] = 0;
	}
	return (1);
}

int	open_file_for_redirection(
		t_ast_node *head, int *_piped, t_env *env, int status)
{
	int			mode;

	if (head->file_type == READ_FILE)
	{
		switch_fds_identifier(_piped, PIPE_HAS_INPUT, PIPE_INPUT_FD, 1);
		_piped[PIPE_INPUT_FD] = open(head->args[0], O_RDONLY);
		if (_piped[PIPE_INPUT_FD] < 0)
			status = switch_fds_identifier(_piped, 0, 0, 0);
	}
	else if (head->file_type == READ_FROM_APPEND)
	{
		switch_fds_identifier(_piped, PIPE_HAS_INPUT, PIPE_INPUT_FD, 1);
		status = run_heredoc(head->args[0], _piped, env);
		signal(SIGINT, handle_ctrl_c);
	}
	else
	{
		switch_fds_identifier(_piped, PIPE_HAS_OUTPUT, PIPE_OUTPUT_FD, 1);
		mode = O_TRUNC;
		if (head->file_type == WRITE_FILE_APPEND)
			mode = O_APPEND;
		_piped[PIPE_OUTPUT_FD] = open(head->args[0], O_WRONLY | O_CREAT | mode, 0666);
	}
	return (status);
}

int	check_if_command_is_builtin(char *_cmd)
{
	char		*tmp_cmd;
	int			status;

	status = 0;
	tmp_cmd = galloc(string_length(_cmd, ' ') + 1);
	if (!tmp_cmd)
		return (0);
	duplicate_string(tmp_cmd, _cmd, 0, string_length(_cmd, ' '));
	if (str_cmp(tmp_cmd, "echo", "cd")
		|| str_cmp(tmp_cmd, "pwd", "export")
		|| str_cmp(tmp_cmd, "unset", "env")
		|| str_cmp(tmp_cmd, "exit", NULL))
		status = 1;
	return (status);
}

int	run_builtin_in_child(char **cmd, t_env *env, int *_out_fd, int *_piped)
{
	int			status;

	status = 0;
	(void)_piped;
	if (str_cmp(cmd[0], "echo", NULL))
		status = echo_cmd(cmd, _out_fd);
	else if (str_cmp(cmd[0], "pwd", "env"))
		status = env_or_pwd_cmd(cmd[0], env, 0, _out_fd);
	else if (str_cmp(cmd[0], "export", "unset"))
		cmd = unset_or_export_cmd(cmd, env, _out_fd, &status);
	else if (str_cmp(cmd[0], "cd", NULL))
		status = cd_cmd(cmd, env, _out_fd);
	else if (str_cmp(cmd[0], "exit", NULL))
		close_session(cmd);
	gfree_string_array(cmd);
	return (status);
}
