/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   preparser.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: lpeggy <marvin@42.fr>                      +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/07/13 18:25:59 by lpeggy            #+#    #+#             */
/*   Updated: 2021/07/24 23:09:12 by lpeggy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static int	skip_until(char *str, int i, int sym)
{
	while (str[i])
	{
		if (str[i] == sym && str[i - 1] != '\\')
			return (i);
		i++;
	}
	return (-1);
}

int	if_quotes(char *str, int i)
{
	if (str[i] == '\'')
		i = skip_until(str, i + 1, '\'');
	if (str[i] == '\"')
		i = skip_until(str, i + 1, '\"');
	if (str[i] == '\\')
		i += 2;
	return (i);
}

/*
char	**cut_cmds(char *str, t_vars *vars)
{
	int		i;
	int		start;
	int		num;
	char	**cmd_line;

	cmd_line = ft_calloc(sizeof(char *), (vars->cmd_nbr + 1));// null last
	if (!cmd_line)
		return (NULL);
	num = 0;
	if (vars->cmd_nbr == 1)
	{
		cmd_line[num] = ft_strdup(str);
		return (cmd_line);
	}
	
	start = 0;
	i = 0;
	while (str[i])
	{
		i = if_quotes(str, i);
		if (str[i] == '|')
		{
			cmd_line[num] = ft_substr(str, start, i - start);
			i = skip_symbs(str, i, " |");// " " ?
			num++;
			start = i;
			continue ;
		}
		//if (num == vars->cmd_nbr - 1)
		if (num == vars->pipe_nbr)//after last pipe
		{
			cmd_line[num] = ft_strdup(str + i);
			break ;
		}
		i++;
	}
	return (cmd_line);
}*/

int	cut_args(char **args, char *str, int *i, int k, t_util *util)
{
	if (str[*i] == '\'')
		util->flag_q++;
	if (str[*i] == '\"')
		util->flag_dq++;
	if ((ft_strchr(" \n\f\v\r\t", str[*i]) || str[*i + 1] == '\0')
		&& str[*i - 1] != '\\' && !(util->flag_q % 2) && !(util->flag_dq % 2))
	{
		args[k] = ft_substr(str, util->start, *i - util->start);// + 1 ?
		return (*i = skip_symbs(str, *i, " \n\t\f\v\r"));
	}
	return (0);
}

int	cut_cmds(char **cmd_line, char *str, int *i, int k, t_util *util)
{
	*i = if_quotes(str, *i);
	if (str[*i] == '|')
	{
		cmd_line[k] = ft_substr(str, util->start, *i - util->start);
		return (*i = skip_symbs(str, *i, "|"));// " " ?
	}
	return (0);
}

char	**split_arr_if(char *str, int elem_nbr, t_util *util, int (*func)())
{
	int 	i;
	int		k;
	char	**arr;

	arr = ft_calloc(sizeof(char *), elem_nbr + 1);//ret check
	if (!arr)
		return (NULL);
	k = 0;
	if (elem_nbr == 1)
	{
		arr[k] = ft_strdup(str);
		return (arr);
	}
	i = skip_symbs(str, 0, " \n\t\f\v\r");//looks safe here
	util->start = i;
	while (str[i] && k < elem_nbr)
	{
		if (k == elem_nbr - 1)
		{
			arr[k] = ft_strdup(str + i);
			break ;
		}
		if (func(arr, str, &i, k, util))
		{
			!(util->start = i) || k++;
			continue ;
		}
		i++;
	}
	return (arr);
}


static int word_after(char *str, int j, char *divider)//if_word_after
{
	int		i;

	i = j;
	i++;
	i = skip_symbs(str, i, divider);
	i = skip_symbs(str, i, " \n\f\v\r\t");
	if (!str[i])
		return (0);
	return (1);
}

int	skim(char *str)
{
	int		i;

	if (!str[0])
		return (0);
	i = skip_symbs(str, 0, " \n\f\v\r\t");
	if (str[i] == '|')
	{
		write(1, "sh: syntax error\n", 17);
		return (0);
	}
	while (str[i])
	{
		i = if_quotes(str, i);
		if (i == -1 || (ft_strchr(">|<", str[i]) && !word_after(str, i, ">|<")))
		{
			write(1, "sh: syntax error\n", 17);
			return (0);
		}
		i++;
	}
	return (1);
}

void	deal_spec_symbs(void *ptr, t_vars *vars)
{
	int		i;

	i = -1;
	while (((t_proc *)ptr)->args[++i])
		((t_proc *)ptr)->args[i] = parser(((t_proc *)ptr)->args[i], vars);
}

void	pre_parser(char *str, t_vars *vars)
{
	t_util	util;
	char	**cmd_line;

	ft_memset(&util, 0, sizeof(t_util));

	(skim(str) && (vars->cmd_nbr = count_elems(str, "|")))
		|| (vars->parse_err = 1);
	if (vars->parse_err)
		return ;
	DEBUG_PARSER && printf(GREY"cmd_nbr %d"RESET, vars->cmd_nbr);
	vars->pipe_nbr = vars->cmd_nbr - 1;
	((cmd_line = split_arr_if(str, vars->cmd_nbr, &util, cut_cmds))
	 && (make_cmd_list(cmd_line, vars, &util))) || (vars->parse_err = 1);
	if (vars->parse_err)
		return ;
	//DEBUG_PARSER && printf(GREY"\tbefore dealing spec symbs"RESET);
	//DEBUG_PARSER && _print_list(&vars->cmd_arr);
	ft_lstiter_param(vars->cmd_arr, &deal_spec_symbs, vars);

	DEBUG_PARSER && printf(GREY"\tafter all parse"RESET);
	DEBUG_PARSER && _print_list(&vars->cmd_arr);
	//printf(RED"type redir %d\n"RESET, ((t_proc *)(vars->cmd_arr)->content)->type_redir);
	//printf(RED"type redir %s\n"RESET, ((t_proc *)(vars->cmd_arr)->content)->args[0]);
}
