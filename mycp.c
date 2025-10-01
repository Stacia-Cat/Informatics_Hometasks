#include <fcntl.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>

void copy(int fd, int dest_path)
{
	while (1)
	{
		char buff[4096];
		ssize_t n = read(fd, buff, sizeof(buff));
		if (n > 0)
		{
			n = write(dest_path, buff, n);
		}
		else if (n == 0)
			break;
		else
		{
			perror("ERROR while copying process");
			break;
		}
	}
}

int main(int argc, char *argv[])
{
	struct stat path_stat;
	if (argc == 1)
	{
		perror("mycp: Error: No file operand"); //сообщение об ошибке
	}
	else if (argc == 2)
	{
		perror("mycp: Error: missing destination file operand");
	}
	else if (argc >= 3)
	{
		//если нет ключей
		int i;
		int fd;
		if (stat(argv[argc-1], &path_stat) != 0)
		{
		        perror("Incorrect destination");
			return 0;
		}
		int ch = 0;
		int dest_path;
		if (S_ISREG(path_stat.st_mode))
		{
			dest_path = open(argv[argc-1], O_WRONLY | O_CREAT | O_TRUNC);
			ch = 0;
		}
		else if (S_ISDIR(path_stat.st_mode))
		{
			ch = 1;
		}
		for (i = 1; i < argc-1; i++)
		{
			fd = open(argv[i], O_RDONLY);
			if (fd < 0)
			{
				perror("ERROR happened");
				break;
			}
			if (ch==1)
			{
				char s[1024];
				snprintf(s, sizeof(s), "%s/%s", argv[argc-1], argv[i]);
				dest_path = open(s, O_WRONLY | O_CREAT | O_TRUNC);
			}
			copy(fd, dest_path);
			close(fd);
		}
		close(dest_path);
	}
	return 0;
}
