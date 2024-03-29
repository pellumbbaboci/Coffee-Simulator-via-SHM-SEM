/*

The PARENT
 */
#include "local.h"
int main(int argc, char *argv[])
{

    static struct MEMORY memory;

    static unsigned int short start_val[4] = {15, 0, 1, 0};

    int semid, shmid, croaker;

    char *shmptr;

    pid_t p_id, c_id, pid = getpid();

    union semun arg;

    memory.head = memory.tail = 0;
    
    struct MEMORY *memptr;

    pid_t gid = getgid();

    // Check command line arguments

    if (argc != 3)
    {

        cerr << argv[0] << " producer_time consumer_time" << endl;

        return 1;
    }

    // Create, attach, clear segment

    if ((shmid = shmget((int)pid, sizeof(memory),

                        IPC_CREAT | 0600)) != -1)
    {

        if ((shmptr = (char *)shmat(shmid, 0, 0)) == (char *)-1)
        {

            perror("shmptr -- parent -- attach ");

            return 2;
        }

        memcpy(shmptr, (char *)&memory, sizeof(memory));
    }
    else
    {
        perror("shmid -- parent -- creation ");

        return 3;
    }

    // Create & initialize semaphores

    if ((semid = semget((int)pid, 4, IPC_CREAT | 0666)) != -1)
    {

        arg.array = start_val;

        if (semctl(semid, 0, SETALL, arg) == -1)
        {

            perror("semctl -- parent -- initialization");

            return 4;
        }
    }
    else
    {

        perror("semget -- parent -- creation ");

        return 5;
    }


    cout<< "\n***Welcome To CinsBucks Cafee!***\n"<<endl;

    // Fork PRODUCER process
    int i;
    for (i = 0; i < 3; i++)
    {
        if ((p_id = fork()) == -1)
        {
            perror("fork -- producer");
            return 6;
        }
        else if (p_id == 0)
        {
            execl("producer", "./producer", argv[1], (char *)0);
            perror("execl -- producer");
            return 7;
        }
    }

    // Fork CONSUMER process

    int k;
    for (k = 0; k < 50; k++)
    {
        if ((c_id = fork()) == -1)
        {

            perror("fork -- consumer");

            return 8;
        }
        else if (c_id == 0)
        {

            execl("consumer", "./consumer", argv[2], (char *)0);

            perror("execl -- consumer");

            return 9;
        }
    }

    
    // Wait for 1 to die -

    while (1)
    {
        croaker = (int)wait((int *)0); // kill remaining process
    }

    
    //kill(((croaker == p_id) ? c_id : p_id), SIGKILL);

    shmdt(shmptr);

    shmctl(shmid, IPC_RMID, (struct shmid_ds *)0);

    semctl(semid, 0, IPC_RMID, 0);
    
    return 0;
}
