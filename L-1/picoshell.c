#include <unistd.h>
#include <sys/wait.h>
#include <stdlib.h>

int picoshell(char **cmds[])
{
	int   i       = 0;
	int   prev_in = -1;
	int   fd[2];
	int   st;
	int   res     = 0;
	pid_t pid;

	if (!cmds || !cmds[0])
	{
		return (1);
	}

	while (cmds[i])
	{
		int has_next = (cmds[i + 1] != 0);

		if (has_next)
		{
			if (pipe(fd) == -1)
			{
				if (prev_in != -1)
				{
					close(prev_in);
					prev_in = -1; /* prevent double close later */
				}
				res = 1;
				break;
			}
		}

		pid = fork();
		if (pid == 0)
		{
			if (prev_in != -1)
			{
				if (dup2(prev_in, 0) == -1)
				{
					exit(1);
				}
			}
			if (has_next)
			{
				if (dup2(fd[1], 1) == -1)
				{
					exit(1);
				}
			}
			if (prev_in != -1)
			{
				close(prev_in);
			}
			if (has_next)
			{
				close(fd[0]);
				close(fd[1]);
			}
			if (!cmds[i][0])
			{
				exit(1);
			}
			execvp(cmds[i][0], cmds[i]);
			exit(1);
		}
		else if (pid == -1)
		{
			if (has_next)
			{
				close(fd[0]);
				close(fd[1]);
			}
			if (prev_in != -1)
			{
				close(prev_in);
				prev_in = -1; /* prevent double close later */
			}
			res = 1;
			break;
		}

		if (prev_in != -1)
		{
			close(prev_in);
		}
		if (has_next)
		{
			close(fd[1]);
			prev_in = fd[0];
		}
		else
		{
			prev_in = -1;
		}

		i++;
	}

	if (prev_in != -1)
	{
		close(prev_in);
		prev_in = -1;
	}

	while (wait(&st) != -1)
	{
		if (st != 0)
		{
			res = 1;
		}
	}

	return (res);
}

// int main(void)
// {
// 	char **cmds[4];

// 	// static char *cmd1[] = {"cat", NULL};
//    	// static char *cmd2[] = {"cat", NULL};
//    	// static char *cmd3[] = {"ls", "-l", NULL};

// 	static char *cmd1[] = {"ls", NULL};
//     static char *cmd2[] = {"grep", "l", NULL};
//     static char *cmd3[] = {"wc", NULL};//essayer commande inexistante (comme "wololo")

//     cmds[0] = cmd1;
//     cmds[1] = cmd2;
//     cmds[2] = cmd3;
//     cmds[3] = NULL;
	
// 	int res = picoshell(cmds);
// 	if (res)
// 		write(1, "Returned 1\n", 11);
// 	return (0);
// }