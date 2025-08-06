/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   utils3.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 17:51:11 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:23:42 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void	gfree_string_array(char **arr)
{
	(void)arr;
	// int				a;

	// a = 0;
	// while (arr[a] != 0)
	// {
	// 	gfree(arr[a]);
	// 	a += 1;
	// }
	// gfree(arr);
}

int	strtoint(char *str)
{
	int					num;
	int					a;

	num = 0;
	a = 0;
	while (str[a])
	{
		if (str[a] <= '9' && str[a] >= '0')
			num = (num * 10) + (str[a] - 48);
		a++;
	}
	return (num);
}
int	count_array_strings(char **arr)
{
	int			a;

	a = 0;
	while (arr[a] != 0)
		a++;
	return (a);
}
