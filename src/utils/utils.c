/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils.c                                            :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:21:34 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:26:20 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

int	get_exit_status(int err)
{
	if (err == 2)
		return (127);
	else if (err == 13)
		return (126);
	return (err);
}

int	count_in_digits(int num)
{
	int			a;

	a = 0;
	if (!num)
		return (1);
	while (num)
	{
		num /= 10;
		a++;
	}
	return (a);
}

void	gfree_env_var(char ***array)
{
	(void)array;
	// int				a;

	// a = 0;
	// while (array[a])
	// {
	// 	// gfree(array[a][0]);
	// 	// gfree(array[a][1]);
	// 	// gfree(array[a]);
	// 	a += 1;
	// }
	// // gfree(array);
}

void	cleanup_shell(t_env *env, int status)
{
	if (env)
	{
		clear_garbage();
	}
	exit(status);
}

// void	close_pipe_fds(int fd1, int fd2)
// {
// 	close(fd1);
// 	close(fd2);
// }

void safe_close(int fd)
{
    if (fd != -1 && fd > 2)  // stdin, stdout, stderr'i kapatma
    {
        close(fd);
    }
}

void close_pipe_fds(int fd1, int fd2)
{
    safe_close(fd1);
    safe_close(fd2);
}

// Redirection fd'lerini temizlemek için yardımcı fonksiyon
void cleanup_redirection_fds(int *piped)
{
    if (piped[PIPE_HAS_INPUT] && piped[PIPE_INPUT_FD] != -1)
        safe_close(piped[PIPE_INPUT_FD]);
    if (piped[PIPE_HAS_OUTPUT] && piped[PIPE_OUTPUT_FD] != -1)
        safe_close(piped[PIPE_OUTPUT_FD]);
}
