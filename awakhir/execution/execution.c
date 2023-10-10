/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   execution.c                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: yberrim <yberrim@student.42.fr>            +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2023/09/14 17:43:17 by yberrim           #+#    #+#             */
/*   Updated: 2023/10/10 18:24:52 by yberrim          ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../minishell.h"


static int	exec(t_cmd *cmd, char **env, int **pipe_fd)
{
	if (cmd->fd_out != 1)
	{
		dup2(cmd->fd_out, 1);
		close(cmd->fd_out);
	}
	if (cmd->fd_in != 0)
	{
		dup2(cmd->fd_in, 0);
		close(cmd->fd_in);
	}
	if (cmd->out_redir_type != IN_NONE || cmd->in_redir_type != OUT_NONE)
	{
		dup2(*pipe_fd[1], 1);
		close(*pipe_fd[1]);
	}
	/*maarftch alach khasse nncommante hade fd0,fd1*/
	close(*pipe_fd[0]);
	close(*pipe_fd[1]);
	(void)env;
	execve(cmd->cmd_path, cmd->cmd, lincke_list_toaraay(cmd->env));
	exit(g_exit_status = 127);
	return (g_exit_status = 127);
}

static void	ft_close(t_cmd *cmd, int **pipe_fd)
{
	if (cmd->out_redir_type != IN_NONE || cmd->in_redir_type != OUT_NONE)
		close(*pipe_fd[1]);
	if (cmd->fd_out != 1)
		close(cmd->fd_out);
	if (cmd->fd_in != 0)
		close(cmd->fd_in);
}

int	built_in(t_cmd *cmd)
{
	if (is_buildin(cmd))
	{
		execution_builtin(cmd, 0);
		return (g_exit_status);
	}
	else if (!cmd->cmd_path)
	{
		ft_putstr_fd("minishell: command not found\n", 2);
		return (g_exit_status = 127);
	}
	return (-1);
}

void execute_command_v1(t_cmd *cmd , char **env)
{
	int		*pipe_fd;
	t_cmd	*head;
	head = cmd;

	
	pipe_fd = malloc(sizeof(int) * 2);
	pipe(pipe_fd);
	check_redirections(cmd);
	cmd->cmd_path = find_abs_path(head->env, cmd->cmd[0]);
	if(cmd->cmd_path == NULL)
	{
		ft_putstr_fd("minishell: command not found\n", 2);
		exit(127);
	}
	exec(cmd, env, &pipe_fd);
	free(pipe_fd);
	exit(g_exit_status);
}

void execute_command_v2(t_cmd *cmd , char **env)
{
	int		*pipe_fd;
	int		j;
	t_cmd	*head;
	
	j = 0;
	head = cmd;
	while (cmd)
	{
		pipe_fd = malloc(sizeof(int) * 2);
		if (is_buildin(cmd) == 0)
			cmd->cmd_path = find_abs_path(head->env, cmd->cmd[0]);
		if (cmd && cmd->next)
		{
			pipe(pipe_fd);
			cmd->fd_out = pipe_fd[1];
			cmd->next->fd_in = pipe_fd[0];
		}
		check_redirections(cmd);
		if (!((is_buildin(cmd)) && built_in(cmd) != -1))
		{
			cmd->child_pid = fork();
			if (cmd->child_pid == 0)
			{
				j = exec(cmd, env, &pipe_fd);
				exit(j);
			}
		}
		free(cmd->cmd_path);
		ft_close(cmd, &pipe_fd);
		free(pipe_fd);
		cmd = cmd->next;
	}
}
void dup_chek(t_cmd *cmd)
{
	int backup_output = dup(1);
	int backup_input = dup(0);
	check_redirections(cmd);
	built_in(cmd);
	if(cmd->fd_out != 1)
		dup2(backup_output, 1);
	if(cmd->fd_in != 0)
		dup2(backup_input, 0);
}
int	execution_proto(t_cmd *cmd, char **env)
{
	int		j;
	t_cmd	*head;

	j = 0;
	head = cmd;
	if (!cmd->next)
	{
		if (is_buildin(cmd))
			dup_chek(cmd);
		else
		{
			if ((cmd->child_pid = fork()) == 0)
				execute_command_v1(cmd, env);
			waitpid(cmd->child_pid, &g_exit_status, 0);
			if (WIFEXITED(g_exit_status))
				g_exit_status = WEXITSTATUS(g_exit_status);
			else if (WIFSIGNALED(g_exit_status))
				g_exit_status = WTERMSIG(g_exit_status) + 128;
		}
		return (g_exit_status);
	}
	execute_command_v2(cmd, env);
	while (wait(&g_exit_status) > 0)
	{
					if (WIFEXITED(g_exit_status))
			g_exit_status = WEXITSTATUS(g_exit_status);
		else if (WIFSIGNALED(g_exit_status))
			g_exit_status = WTERMSIG(g_exit_status) + 128;
	}
	return (g_exit_status);
}
