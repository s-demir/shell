/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   heredoc.c                                          :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: amsagman <amsagman@student.42kocaeli.co    +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/08/02 20:33:50 by amsagman          #+#    #+#             */
/*   Updated: 2025/08/07 23:47:33 by amsagman         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

static void write_heredoc_line(int std_out, char *buf, t_env *env, int expandable)
{
	int flags[3];

	if (expandable)
	{
		buf[string_length(buf, '\n')] = '\0';
		ft_memset(flags, 0, 3 * sizeof(int));
		buf = recursively_expand_variables(buf, env, 0, flags);
		ft_memset(flags, 0, 3 * sizeof(int));
		buf = recursively_expand_variables(buf, env, 1, flags);
	}
	write(std_out, buf, string_length(buf, '\0'));
	write(std_out, "\n", 1);

}

static void process_heredoc_input(int std_out, char *limiter, t_env *env, int expandable)
{
	char *buf;
	char *clean_limiter;

	clean_limiter = remove_quotes_from_str(limiter, 0, 0, 0);
	while (1)
	{
		buf = readline(">> ");
		if (!buf || compare_strings(clean_limiter, buf, string_length(buf, '\n')))
		{
			free(buf);
			break;
		}
		write_heredoc_line(std_out, buf, env, expandable);
		free(buf);
	}
}

void manage_heredoc_fds(int *out_fd, int *piped, int status)
{
	close(out_fd[1]);
	piped[PIPE_INPUT_FD] = out_fd[0];
	piped[PIPE_RESERVED1] = WEXITSTATUS(status) - 1;
	if (piped[PIPE_RESERVED1] < 0)
		piped[PIPE_RESERVED1] += 2;
	piped[PIPE_EXEC_STATE] = status;
	close(out_fd[0]); //ben ekldim (hüma) ama terminal bunu eklediğimde loopta kalıyor
	//eklemediğimde de fd kapanmıyor 6dan 7ye çıkıyor açık kalan fd sayısı. 
}

int run_heredoc(char *limiter, int *piped, t_env *env)
{
	int out_fd[2];
	pid_t pid;
	int status;

	out_fd[0] = -1;                 // Safe fd initialization - prevents invalid close on error
	out_fd[1] = -1;                 // Safe fd initialization - prevents invalid close on error

	if (pipe(out_fd) == -1)         // Check pipe system call - error detection
	{
		perror("pipe");             // Error reporting - diagnostic output
		return (1);                 // Return error - failure indication
	}
	pid = fork();
	signal(SIGINT, SIG_IGN);
	if (!pid)
	{
		signal(SIGINT, exit_heredoc);
		close(out_fd[0]);
		process_heredoc_input(out_fd[1], limiter, env, has_quotes(limiter));
		close(out_fd[1]);
		clear_garbage();
		exit(1);
	}
	waitpid(pid, &status, 0);
	manage_heredoc_fds(out_fd, piped, status);
	return piped[PIPE_RESERVED1];
}
