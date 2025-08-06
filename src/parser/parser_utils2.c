/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser_utils2.c                                    :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: sedemir <sedemir@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/07/31 16:19:27 by sedemir           #+#    #+#             */
/*   Updated: 2025/08/06 18:31:44 by sedemir          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "minishell.h"

void init_node(t_ast_node *node, t_token_type type)
{
    node->type = type;
    node->args = NULL;
    node->left = NULL;
    node->right = NULL;
}

t_ast_node *make_node(t_token_type type)
{
    t_ast_node *node;
    node = galloc(sizeof(t_ast_node));
    if (!node)
    {
        return NULL;
    }
    init_node(node, type);
    return node;
}

static void clear_args(char **args, int idx)
{
    (void)idx;
    (void)args;
    // if (!args || !args[idx])
    //     return ;
    // gfree(args[idx]);
    // clear_args(args, idx + 1);
    // if (idx == 0)
    // {
    //     gfree(args);
    // }
}

void clear_ast(t_ast_node *node)
{
    int idx;
    idx = 0;
    if (!node)
        return ;
    if (node->type == TOKEN_WORD && node->args)
    {
        clear_args(node->args, idx);
    }
    clear_ast(node->left);
    clear_ast(node->right);
    // gfree(node);
}
