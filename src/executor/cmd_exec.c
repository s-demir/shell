/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   cmd_exec.c                                         :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:32:07 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:29:36 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	run_redir_cmd(
		char **_cmd_, int *_fd, char **env, int *_piped)
{
	pid_t				pid;
	int					fd_[2];

	fd_[0] = -1;                    // Başlangıçta güvenli değerler ata - pipe başarısızlığında invalid fd önlenir
	fd_[1] = -1;                    // Başlangıçta güvenli değerler ata - pipe başarısızlığında invalid fd önlenir

	if (pipe(fd_) == -1)            // Pipe başarısızlığını kontrol et - sistem çağrısı hatasını yakala
	{
		perror("pipe");             // Hata mesajı yazdır - debug için gerekli
		return (1);                 // Hata kodu döndür - caller'a hata durumu bildir
	}
	pid = fork();
	signal(SIGINT, child_ctrl_c);
	signal(SIGQUIT, child_ctrl_c);
	if (!pid)
	{
		child_fds_managment(_piped, _fd, fd_);
		execve(_cmd_[0], _cmd_, env);
		ft_putendl_fd(strerror(errno), 2);
		clear_garbage();
		exit(127);
	}
	parent_fds_managment(_piped, _fd, fd_);
	//gfree_string_array(_cmd_);
	return (1);
}

static char **build_cmd_args(char *cmd, char **env, char **args)
{
    char **first_args;
    char **full_args;
    first_args = prepare_cmd_arguments(cmd, env, 0);
    full_args = combine_command_arguments(first_args, args);
    return full_args;
}

int prep_run_cmd(char **cmd, int *fd, int *piped, t_env *env)
{
    char **cmd_args;
    int status;
    cmd_args = build_cmd_args(cmd[0], env->original_env, cmd);
    if (check_if_command_is_builtin(cmd_args[0]))
        status = execute_builtin(cmd_args, fd, env, piped);
    else
	{
        piped[PIPE_CHILD_COUNT]++;
        if (!piped[PIPE_REDIR_COMBO])
		{
            status = run_basic_cmd(cmd_args, fd, env->original_env, piped);
            //gfree_string_array(cmd_args);
		}
        else
            status = run_redir_cmd(cmd_args, fd, env->original_env, piped);
	}
    if (piped[PIPE_ACTIVE] > 1)
        piped[PIPE_ACTIVE]--;
    return status;
}

int	wait_children(int status, int *_piped)
{
	if (status != 2 && status != 127
		&& status != 126 && _piped[PIPE_CHILD_COUNT]
		&& _piped[PIPE_EXEC_STATE])
	{
		while (_piped[PIPE_CHILD_COUNT])
		{
			wait(&status);
			_piped[PIPE_CHILD_COUNT] -= 1;
		}
		signal(SIGINT, handle_ctrl_c);
		signal(SIGQUIT, SIG_IGN);
		if (!g_var_thing)
			return (WEXITSTATUS(status));
		else
			return (g_var_thing);
	}
	return (status);
}

void	execute_commands(t_ast_node *head, t_env *env, int *status)
{
	int	_piped[PIPE_STATE_SIZE];
	int	_status;

	initialize_or_reset_pipe_state(_piped, 1);
	count_redirects(head, _piped);
	initialize_or_reset_pipe_state(_piped, 0);
	set_node_types(head);
	expand_variables_in_ast(head, env);
	_status = check_cmd_perms(head, env->original_env);
	if (!_status)
		*status = execute_ast_node(head, _piped, env);
}
