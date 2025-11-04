#include <unistd.h>
#include <sys/msg.h>
#include <sys/wait.h>
#include <sys/types.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/ipc.h>

struct relay_msg {
	long type;
	long sender;
};
const size_t msg_size = sizeof(struct relay_msg) - sizeof(long);

void Judge (int idJ, int N)
{
	printf("Judge: is here\n");

	const int J_id = N+1;
	const int J_reg = N+2;
	struct relay_msg msg = {};
	int R_ready = 0;
	while (R_ready < N)
	{
		msgrcv(idJ, &msg, msg_size, J_reg, 0);
		printf("Judge: runner %ld registered\n", msg.sender);
		R_ready = R_ready + 1;
	}
	printf("Judge: start\n");

	msg.type = 1;
	msg.sender = J_id;
	//msg = {1, J_id}; //Передача жетона к первому
	msgsnd(idJ, &msg, msg_size, 0);
	printf("Judge: baton passed to the Runner 1\n");

	if (msgrcv(idJ, &msg, msg_size, N+1, 0)<0)
	{
		perror("failed to receive a message\n");
	}
	printf("Judge: received the baton from the Runner %ld\n", msg.sender); //Получение жетона назад от последнего
	printf("Judge: competition finished\n"); //Окончание эстафеты
	exit(0);
}

void Runners (int idJ, int idn, int N)
{
	const int J_id = N+1;
	struct relay_msg msg = {J_id+1, idn};
	printf("Runner %d: is here ready\n", idn);
	msgsnd(idJ, &msg, msg_size, 0);

	int rcv_stat = msgrcv(idJ, &msg, msg_size, idn, 0);
	if (rcv_stat < 0)
	{
		perror("failed to receive a message\n");
	}
	printf("Runner %d: baton received from the ", idn);
	if (msg.sender == J_id)
	{
		printf("judge\n");
	}
	else
	{
		printf("Runner %ld\n", msg.sender);
	}


	msg.type = idn+1;
	msg.sender = idn;
	//msg = {idn+1, idn};
	printf("Runner %d: baton passed to the ", idn);
	msgsnd(idJ, &msg, msg_size, 0);
	if (idn == N)
	{
		printf("judge\n");
	}
	else
	{
		printf("Runner %ld\n", msg.type);
	}
	exit(0);
}

int main(int argc, char *argv[])
{
	int N = atoi((argv[1])); //Определяется количество бегунов
	int idJ = msgget(IPC_PRIVATE, IPC_CREAT | IPC_EXCL | 0666); //Создаются очереди для судьи и бегунов
	pid_t p = fork();
	if (p == 0)
	{
		Judge(idJ, N); //Начало работы судьи
		return 0;
	}
	int i;
	for (i = 1; i < N+1; i++)
	{
		p = fork();
		if (p == 0)
		{
		Runners(idJ, i, N); //Начало работы i бегуна
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
