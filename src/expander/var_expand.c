/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   var_expand.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:17:59 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:10:57 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

char	*replace_variable_with_value(
		char *old_var, char *__new, int st, int end)
{
	int							size;
	char						*new__;
	int							unsize;

	unsize = string_length(__new, '\0');
	size = st + (string_length(old_var, '\0') - end) + unsize;
	new__ = galloc(size + 1);
	if (!new__)
		return (NULL);
	duplicate_string(new__, old_var, 0, st);
	duplicate_string(new__ + st, __new, 0, unsize);
	duplicate_string(new__ + st + unsize, old_var, end, string_length(old_var, '\0'));
	// gfree(old_var);
	return (new__);
}

char	*expand_variable_in_string(char *var, t_env *env, int a, int *f_hole)
{
	int							hole_size;
	int							c;
	int							b;
	char						*new_var;

	b = a + 1;
	while (var[a] && is_valid_var_start(var, b, 0))
		b++;
	hole_size = b - a;
	new_var = galloc(hole_size + 1);
	duplicate_string(new_var, var, a + 1, b);
	if (str_cmp(new_var, "?", NULL) && g_var_thing)
		set_env_status(env, g_var_thing, "?=");
	g_var_thing = 0;
	c = locate_env_var_index(env, new_var);
	// gfree(new_var);
	*f_hole = 0;
	if (c >= 0)
	{
		new_var = replace_variable_with_value(var, env->parsed_env[c][1], a, b);
		*f_hole = string_length(env->parsed_env[c][1], '\0') + a;
	}
	else
		new_var = replace_variable_with_value(var, "", a, b);
	return (new_var);
}

char	*recursively_expand_variables(
		char *var, t_env *env, int __con, int *f_arr)
{
	char						*new_var;

	while (var[f_arr[0]])
	{
		if (var[f_arr[0]] == 39)
		{
			f_arr[0]++;
			f_arr[1]++;
			while (!(f_arr[2] % 2) && var[f_arr[0]] && var[f_arr[0]] != 39)
				f_arr[0]++;
		}
		if (var[f_arr[0]] == 34)
			f_arr[2]++;
		if (is_valid_var_start(var, f_arr[0], 1)
			&& ((!(f_arr[2] % 2) && __con) || (f_arr[2] % 2 && !__con)))
			return (new_var = expand_variable_in_string(
					var, env, f_arr[0], &f_arr[0]),
				recursively_expand_variables(new_var,
					env, __con, f_arr));
		f_arr[0]++;
	}
	return (var);
}

char	**refactore_args_array(char **args, int *quick_norm_fix)
{
	int						a;
	int						b;
	int						c;
	char					**new_args;

	a = 0;
	b = 0;
	*quick_norm_fix = 0;
	c = count_array_strings(args);
	new_args = galloc((count_arg_flaws(args) + c + 1) * sizeof(char **));
	if (!new_args)
		return (NULL);
	while (args[a])
	{
		c = is_flawed_str(args[a], 0, 0, 0);
		if (c)
			b = simplified_refactor_thing(new_args, b, args[a], 0);
		else
			new_args[b++] = strcopy(args[a]);
		a++;
	}
	new_args[b] = 0;
	//gfree_string_array(args);
	return (new_args);
}

void	expand_variables_in_ast(t_ast_node *head, t_env *env)
{
	int							a;
	int							f_arr[3];

	if (head->file_type != FILE_READY && head->args
		&& head->file_type != READ_FROM_APPEND)
	{
		a = -1;
		while (head->args[++a])
			((ft_memset(f_arr, 0, 3 * sizeof(int))),
				(head->args[a] = recursively_expand_variables(
						head->args[a], env, 1, f_arr)));
		head->args = refactore_args_array(head->args, &a);
		while (head->args[a])
		{
			ft_memset(f_arr, 0, 3 * sizeof(int));
			head->args[a] = recursively_expand_variables(
					head->args[a], env, 0, f_arr);
			head->args[a] = remove_quotes_from_str(head->args[a], 0, 0, 0);
			a++;
		}
	}
	if (head->left)
		expand_variables_in_ast(head->left, env);
	if (head->right)
		expand_variables_in_ast(head->right, env);
}
