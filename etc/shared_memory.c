/*
struct shmid_ds
{
   struct         ipc_perm shm_perm;    // 퍼미션
   int            shm_segsz;            // 메모리 공간의 크기
   time_t         shm_dtime;            // 마지막 attach 시간 
   time_t         shm_dtime;            // 마지막 detach 시간 
   time_t         shm_ctime;            // 마지막 변경 시간
   unsigned short shm_cpid;             // 생성프로세스의 pid
   unsigned short shm_lpid;             // 마지막으로 작동한 프로세스의 pid
   short          shm_nattch;           // 현재 접근한 프로세스의 수
};
*/

#include <sys/shm.h>
#include <sys/ipc.h>
#include <stdio.h>
#include <unistd.h>
#include <stdlib.h>

int main( )
{
   int shmid;
   int pid;

   int *cal_num;
   void *shared_memory = (void *)0;

   shmid = shmget((key_t)1234, sizeof(int), 0666 | IPC_CREAT);
   printf("shmid1 : %d\n",shmid);

   if(shmid == -1)
   {
      perror("shmget");
      exit(0);
   }

   shared_memory = shmat(shmid, (void *)0,0);
   printf("shm1 : %d\n",shared_memory);
   // descriptor of the shared memory, start address of the shared memory, mode
   if(shared_memory == (void *)-1)
   {
      perror("shmat");
      exit(0);
   }

   cal_num = (int *)shared_memory;
   *cal_num = 1;
   pid = fork();
   if(pid == 0)
   {
      shmid = shmget((key_t)1234, sizeof(int), 0666 | IPC_CREAT);
      printf("shmid2 : %d\n",shmid);
      if(shmid == -1)
      {
         perror("shmget");
         exit(0);
      }
      shared_memory = shmat(shmid,(void *)0, 0);
      printf("shm2 : %d\n",shared_memory);
      if(shared_memory == (void *)-1)
      {
         perror("shmat");
         exit(0);
      }
      cal_num = (int *)shared_memory;
      *cal_num = 1;

      while(1)
      {
         if(*cal_num==0) break;
         *cal_num = *cal_num + 1;
         sleep(1);
      }
      shmdt(shared_memory);
   }

   else if(pid > 0)
   {
      char c;
      while(1)
      {
         scanf(" %c",&c);
         if(c=='e') break;
         printf("%d\n", *cal_num);
      }
      *cal_num = 0;
      shmdt(shared_memory);
      shmctl(shmid, IPC_RMID, NULL);
   }
   return 0;
}
