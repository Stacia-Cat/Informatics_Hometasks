#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>

void Judge (int N)
{
	printf("Judge: is here\n");
}

void Runners (int i, int N)
{
	printf("Runner %d: has come\n", i);
}

int main(int argc, char *argv[])
{
	int N = atoi((argv[1])); //Определяется количество бегунов
	int idJ = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666); //Создаются очереди для судьи и бегунов
	pid_t p = fork();
	if (p == 0)
	{
		Judge(N); //Начало работы судьи
		return 0;
	}
	int i;
	for (i = 1; i < N+1; i++)
	{
		p = fork();
		if (p == 0)
		{
		Runners(i, N); //Начало работы i бегуна
		return 0;
		}
	}
	for (i = 1; i < N+2; i++) //Ждем завершения всех процессов
	{
		wait(NULL);
	}
	msgctl(idJ, IPC_RMID, 0); //Закрытие очереди
	return 0;
}
