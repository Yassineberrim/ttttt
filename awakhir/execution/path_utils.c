/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   path_utils.c                                       :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yberrim <yberrim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/10/09 19:45:32 by yberrim           #+#    #+#             */
/*   Updated: 2023/10/10 16:26:13 by yberrim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"

char	*ft_genv(t_env *env, char *str)
{
	t_env	*tmp;

	tmp = env;
	while (tmp)
	{
		if (!ft_strcmp(tmp->name, str))
			return (tmp->value);
		tmp = tmp->next;
	}
	printf("minishell: No such file or directory\n");
	return (NULL);
}

size_t	ft_envsize(t_env *env)
{
	size_t	i;

	i = 0;
	while (env)
	{
		i++;
		env = env->next;
	}
	return (i);
}

char	**lincke_list_toaraay(t_env *env)
{
	t_env	*tmp;
	char	**env_arr;
	int		i;
	char	*tmpp;

	i = 0;
	tmp = env;
	env_arr = (char **)malloc(sizeof(char *) * (ft_envsize(env) + 1));
	while (tmp)
	{
		tmpp = ft_strjoin(ft_strdup(tmp->name), "=");
		env_arr[i] = ft_strjoin(tmpp, tmp->value);
		// free(tmpp);
		// tmpp = NULL;
		tmp = tmp->next;
		i++;
	}
	env_arr[i] = NULL;
	return (env_arr);
}

void	free_env(t_env *head)
{
	t_env	*tmp;

	tmp = head;
	while (tmp)
	{
		head = head->next;
		free(tmp->name);
		free(tmp->value);
		free(tmp);
		tmp = head;
	}
}
