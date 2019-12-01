#include <stdio.h>
#include <stdint.h>
#include <poll.h>
#include <stdlib.h>
#include <fcntl.h>
#include <errno.h>
#include <string.h>
#include <sys/types.h>
#include <unistd.h>
#include <sys/wait.h>

/**
 * @brief Simple method to encapsule an sh interpretation
 * 
 * @param argv 
 * @return int EXIT_SUCCESS 
 */
int fork_sh_execute(const char *command)
{
	char *tab[4];
	pid_t pid;
	int status;

	/* sh init args */
	tab[0] = "sh";
	tab[1] = "-c";
	tab[2] = (char *)command;
	tab[3] = NULL;
	status = 0;

	/* do fork and sh exec */
	if ((pid = fork()) < 0)
	{
		/* parent exec */
		fprintf(stderr, "Error fork: %s\n", strerror(errno));
		return EXIT_FAILURE;
	}
	else if (pid == 0)
	{
		/* child exec */
		int ret = 0;
		execvp(tab[0], &tab[0]);
		if (ret != 0)
		{
			fprintf(stderr, "Error exec: %s\n", strerror(errno));
			fprintf(stderr, "Error command: %s\n", command);
		}
		_exit(ret);
	}
	else
	{
		waitpid(pid, &status, 1);
		return status;
	}
}