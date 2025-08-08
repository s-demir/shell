/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec3.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:29:39 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/08 15:17:44 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void handle_basic_fds(int *piped, int *fd, int *pipe_fd)
{
    if (piped[PIPE_ACTIVE] && piped[PIPE_ACTIVE] <= piped[PIPE_TOTAL])
        dup2(fd[0], 0);
    if (piped[PIPE_ACTIVE] > 1)
        dup2(pipe_fd[1], 1);
    else
        safe_close(fd[0]);        // close yerine safe_close - invalid fd warning'ini önler
    close_pipe_fds(pipe_fd[0], pipe_fd[1]);
}

int run_basic_cmd(char **cmd, int *fd, char **env, int *piped)
{
    pid_t pid;
    int pipe_fd[2];

    pipe_fd[0] = -1;                // Başlangıçta güvenli değerler ata - pipe hatasında invalid close önlenir
    pipe_fd[1] = -1;                // Başlangıçta güvenli değerler ata - pipe hatasında invalid close önlenir

    if (pipe(pipe_fd) == -1)        // Pipe başarısızlığını kontrol et - sistem hatasını yakala
    {
        perror("pipe");             // Hata mesajı yazdır - debug bilgisi
        return (1);                 // Hata kodu döndür - function failure indicator
    }
    pid = fork();
    signal(SIGINT, child_ctrl_c);
    signal(SIGQUIT, child_ctrl_c);
    if (!pid)
	{
        handle_basic_fds(piped, fd, pipe_fd);
        //while (*env)
        //{
        //    // gfree(*env);
        //    env++;
        //}
        execve(cmd[0], cmd, env);
        ft_putendl_fd(strerror(errno), 2);
        clear_garbage();
        exit(127);
	}
    close_pipe_fds(pipe_fd[1], fd[0]);
    if (piped[PIPE_ACTIVE] > 1)
        fd[0] = pipe_fd[0];
    else
        safe_close(pipe_fd[0]);   // close yerine safe_close - invalid fd warning'ini önler
    return 1;
}

int	execute_builtin_with_piping(char **_cmd_, int *_fd, t_env *env, int *_piped)
{
	int				status;
	int				_out_fd[2];

	_out_fd[1] = 1;
	if (_piped[PIPE_ACTIVE] > 1)
	{
        _out_fd[0] = -1;            // Güvenli başlangıç değeri - pipe error durumunda safety
        _out_fd[1] = -1;            // Güvenli başlangıç değeri - pipe error durumunda safety
        if (pipe(_out_fd) == -1)    // Pipe başarısızlığını kontrol et - critical system call check
        {
            perror("pipe");         // Hata mesajı yazdır - error reporting
            return (1);             // Hata kodu döndür - propagate error upward
        }
	}
	status = run_builtin_in_child(_cmd_, env, _out_fd, _piped);
	if (_piped[PIPE_ACTIVE] > 1)
	{
		close(_out_fd[1]);
		_fd[0] = _out_fd[0];
	}
	//close(_out_fd[0]); //ben ekledim (hüma)
	return (status);
}

int	execute_builtin_with_simple_piping(
		char **_cmd_, int *_fd, t_env *env, int *_piped)
{
	int				status;
	int				_out_fd[2];

	_out_fd[1] = 1;
	if (_piped[PIPE_REDIR_COMBO] && _piped[PIPE_HAS_OUTPUT])
		_out_fd[1] = _piped[PIPE_OUTPUT_FD];
	if (_piped[PIPE_ACTIVE] > 1 && (!_piped[PIPE_REDIR_COMBO] || !_piped[PIPE_HAS_OUTPUT]))
	{
        _out_fd[0] = -1;            // Initialize before pipe - prevent invalid fd usage
        _out_fd[1] = -1;            // Initialize before pipe - prevent invalid fd usage
        if (pipe(_out_fd) == -1)    // Error check for pipe creation - system call validation
        {
            perror("pipe");         // Error reporting - diagnostic output
            return (1);             // Return error code - function failure indication
        }
	}
	status = run_builtin_in_child(_cmd_, env, _out_fd, _piped);
	if (_piped[PIPE_REDIR_COMBO] && _piped[PIPE_HAS_OUTPUT])
	{
		close(_out_fd[1]);
		_piped[PIPE_HAS_OUTPUT] = 0;
	}
	if (!_piped[PIPE_HAS_INPUT] && !_piped[PIPE_HAS_OUTPUT])
		_piped[PIPE_REDIR_COMBO] = 0;
	if (_piped[PIPE_ACTIVE] > 1 && (!_piped[PIPE_REDIR_COMBO] || !_piped[PIPE_HAS_OUTPUT]))
	{
		close(_out_fd[1]);
		_fd[0] = _out_fd[0];
	}
	return (status);
}
