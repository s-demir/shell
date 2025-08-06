/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   builtin_exec.c                                     :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:23:38 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:29:18 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void setup_child_fds(int *fd, int *out_fd, int *piped, int *child_fd)
{
    if (piped[PIPE_ACTIVE] && piped[PIPE_ACTIVE] <= piped[PIPE_TOTAL])
        dup2(child_fd[0], 0);
    if (piped[PIPE_ACTIVE] > 1)
        dup2(fd[1], 1);
    else
        safe_close(child_fd[0]);  // close yerine safe_close - invalid fd warning'ini önler
    close_pipe_fds(fd[0], fd[1]);
    dup2(1, out_fd[1]);
}

static void manage_redir_fds(int *out_fd, int *piped)
{
    out_fd[1] = 1;
    if (piped[PIPE_REDIR_COMBO] && piped[PIPE_HAS_OUTPUT])
        out_fd[1] = piped[PIPE_OUTPUT_FD];
    if (piped[PIPE_ACTIVE] > 1 && (!piped[PIPE_REDIR_COMBO] || !piped[PIPE_HAS_OUTPUT]))
        pipe(out_fd);
}

int run_builtin_child(char **cmd, int *fd, t_env *env, int *piped)
{
    pid_t pid;
    int pipe_fd[2];
    int out_fd[2];
    int status;

    pipe_fd[0] = -1;                // Safe initialization - prevents invalid fd close
    pipe_fd[1] = -1;                // Safe initialization - prevents invalid fd close
    if (pipe(pipe_fd) == -1)        // Check pipe creation success - error handling
    {
        perror("pipe");             // Error message - debug information
        return (1);                 // Error return - failure indication
    }
    pid = fork();
    if (!pid)
	{
        setup_child_fds(pipe_fd, out_fd, piped, fd);
        status = run_builtin_in_child(cmd, env, out_fd, piped);
        clear_garbage();
        exit(WEXITSTATUS(status));
	}
    close_pipe_fds(pipe_fd[1], fd[0]);
    if (piped[PIPE_ACTIVE] > 1)
        fd[0] = pipe_fd[0];
    else
        safe_close(pipe_fd[0]);   // close yerine safe_close - invalid fd warning'ini önler
    return 1;
}

int	run_redir_builtin(char **cmd, int *fd, t_env *env, int *piped)
{
    pid_t pid;
    int out_fd[2];
    int status;

    if (piped[PIPE_ACTIVE] > 1 && (!piped[PIPE_REDIR_COMBO] || !piped[PIPE_HAS_OUTPUT]))
    {
        out_fd[0] = -1;             // Initialize fd array - safety measure
        out_fd[1] = -1;             // Initialize fd array - safety measure
        if (pipe(out_fd) == -1)     // Validate pipe creation - system call check
        {
            perror("pipe");         // Report error - diagnostic message
            return (1);             // Return failure - error propagation
        }
        manage_redir_fds(out_fd, piped);  // Mevcut fonksiyonunu kullan - utilize existing function
    }
    pid = fork();
    if (!pid)
    {
        if (fd[0] != -1)            // fd parametresini kullan - compiler warning'ini önler
            dup2(fd[0], 0);         // Input redirection - stdin'e yönlendir
        if (piped[PIPE_ACTIVE] > 1 && (!piped[PIPE_REDIR_COMBO] || !piped[PIPE_HAS_OUTPUT]))
            dup2(out_fd[1], 1);     // Output'u pipe'a yönlendir
        safe_close(out_fd[1]);      // Child'da yazma ucunu kapat
        clear_garbage();          // Bellek temizliği - clear garbage
        exit(execute_builtin(cmd, fd, env, piped));  // fd'yi builtin'e geçir
    }
    safe_close(out_fd[1]);          // Parent'ta yazma ucunu kapat
    waitpid(pid, &status, 0);       // Child process'i bekle
    return (get_exit_status(status)); // Exit status'u döndür
}

void	builtin_n_exit(char **_cmd_, t_env *env, int *_out_fd, int *_piped)
{
	int				status;

	status = run_builtin_in_child(_cmd_, env, _out_fd, _piped);
	exit(WEXITSTATUS(status));
}
