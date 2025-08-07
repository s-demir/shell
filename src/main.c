/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.c                                             :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 14:59:59 by amsagman          #+#    #+#             */
/*   Updated: 2025/08/06 18:37:09 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

t_token	*parse_user_input(char *input, int *status)
{
	t_token	*tokens;
	char	*cleaned_input;

	if (!input || *input == '\0')
	{
		*status = 258;
		return NULL;
	}
	cleaned_input = ft_strtrim(input, " \t\n\v\f\r");
	if (!cleaned_input)
	{
		*status = 258;
		return NULL;
	}
	if (check_syntax(cleaned_input))
	{
		*status = 258;
		return NULL;
	}
	tokens = tokenize_input(cleaned_input);
	if (!tokens)
		*status = 258;
	return tokens;
}

static void	handle_input(char *input, t_env *env, int *status)
{
	t_ast_node	*ast;
	t_token		*tokens;

	tokens = parse_user_input(input, status);
	if (tokens && *status == 0)
	{
		ast = parse_tokens(&tokens);
		if (ast)
		{
			execute_commands(ast, env, status);
			clear_ast(ast);
		}
	}
	set_env_status(env, *status, "?=");
}

void	execute_shell_loop(t_env *env)
{
	char	*input;
	int		status;

	while (1)
	{
		status = 0;
		input = readline("mineshell$ ");
		if (!input)
		{
			write(STDOUT_FILENO, "exit\n", 5);
			free(input);
			cleanup_shell(env, 0);
		}
		add_history(input);
		handle_input(input, env, &status);
		free(input);
	}
}

int	main(int argc, char **argv, char **envp)
{
	t_env	*env;

	(void)argv;
	configure_signal_handlers();
	env = galloc(sizeof(t_env));
	if (!env || !isatty(STDIN_FILENO) || !isatty(STDOUT_FILENO))
	{
		clear_garbage();
		return 0;
	}

	if (argc == 1 && start_shell(env, envp))
	{
		execute_shell_loop(env);
		cleanup_shell(env, 0);
	}
	// gfree(env);
	return 0;
}
