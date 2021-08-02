/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   parser.c                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: ssobchak <ssobchak@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2021/06/01 17:14:16 by lpeggy            #+#    #+#             */
/*   Updated: 2021/08/02 22:23:03 by lpeggy           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "parser.h"

static char	*dollarsign(char *str, int *i, t_vars *vars)
{
	int		j;
	char	*key;
	char	*rkey;

	j = *i;
	if (!ft_strcmp(str + *i, "$?"))
		return (str = dollarswap(str, ft_itoa(g_exit_status), i + 2, j));
				//test libft with tester
	while (str[++(*i)])
	{
		if (!(str[*i] == '_' || ft_isalnum(str[*i])))
			break ;
	}
	if (*i == j + 1)
		return (str);
	key = ft_substr(str, j + 1, *i - j - 1);
	rkey = get_env_var(vars->env, key);
	//!rkey && (str = dollarswap(str, NULL, i, j));
	if (!rkey)
		str = dollarswap(str, NULL, i, j);
	str = dollarswap(str, rkey, i, j);
	free(key);
	return (str);
}

/*
char	*slash(char *str, int *i)
{
	char	*bef;
	char	*aft;

	bef = ft_substr(str, 0, *i);
	aft = ft_strdup(str + *i + 1);
	free(str);
	bef = ft_strjoin_free(bef, aft);
	(*i)++;
	free(aft);
	return (bef);
}
*/

static char	*quotes(char *str, int *i)
{
	int		j;
	char	*bef;
	char	*into;
	char	*aft;

	j = *i;
	while (str[++(*i)])
	{
		if (str[*i] == '\'')
			break ;
	}
	bef = ft_substr(str, 0, j);
	into = ft_substr(str, j + 1, *i - j - 1);
	aft = ft_strdup(str + *i + 1);
	bef = ft_strjoin_free(bef, into);
	bef = ft_strjoin_free(bef, aft);
	free(into);
	free(aft);
	free(str);
	return (bef);
}

static char	*doublequotes(char *str, int *i, t_vars *vars)
{
	int		j;
	char	*bef;
	char	*into;
	char	*aft;

	j = *i;
	while (str[++(*i)])
	{
		//if (str[*i] == '\\' && (str[*i + 1] == '\"' || str[*i + 1] == '$'
		//		|| str[*i + 1] == '\\'))
		//	str = slash(str, i);
		//(str[*i] == '$') && (str = dollarsign(str, i, vars));
		if (str[*i] == '$')
			str = dollarsign(str, i, vars);
		if (str[*i] == '\"')
			break ;
	}
	bef = ft_substr(str, 0, j);
	into = ft_substr(str, j + 1, *i - j - 1);
	aft = ft_strdup(str + *i + 1);
	bef = ft_strjoin_free(bef, into);
	bef = ft_strjoin_free(bef, aft);
	free(str);
	free(into);
	free(aft);
	return (bef);
}

char	*parser(char *str, t_vars *vars)
{
	int		i;
	char	*tmp;

	i = -1;
	tmp = ft_strtrim(str, " \n\f\v\r\t");
	free(str);
	str = tmp;
	while (str[++i])
	{
		(str[i] == '\'') && (str = quotes(str, &i));
		//(str[i] == '\\') && (str = slash(str, &i));
		(str[i] == '\"') && (str = doublequotes(str, &i, vars));
		(str[i] == '$') && (str = dollarsign(str, &i, vars));
	}
	return (str);
}
