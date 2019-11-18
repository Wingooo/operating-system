#include <stdio.h>
#include <stdlib.h>
#include <unistd.h>
#include <string.h>
#include <fcntl.h>
#include <pthread.h>
#include <unistd.h>
#include <math.h>
#include <semaphore.h>
#include <sys/shm.h>
#include <sys/stat.h>
#include <sys/mman.h>
#include <sys/types.h>
#include <sys/syscall.h>  

#define Buffer_Size sizeof(int)*20
typedef struct share{
      sem_t *empty;  
      sem_t *full;
      int *ptr;       //ָ����������ָ��
      int param;     //�����д���ĸ�ָ���ֲ�����
      pthread_mutex_t lock;  //�����߳�֮��Ļ���
      int current;   //���ڷ��ʻ�����ʱ��¼�ڼ���buffer
}share;

void *func(void *atr);

int main(int argc,char **argv){
      //�ź����ĳ�ʼ��
      sem_t *empty;
      sem_t *full;
      empty = sem_open("empty",O_RDWR,0667);
      full = sem_open("full",O_RDWR,0667);
      //�����ڴ�ĳ�ʼ��
      const char *name = "OS";
      int shm_fd;
      void *ptr;
      shm_fd = shm_open(name,O_RDONLY, 0667);
      ptr = mmap(0,Buffer_Size, PROT_READ, MAP_SHARED, shm_fd, 0);

      //���ṹ�帳ֵ
      share *temp = (share*)malloc(sizeof(share));
      temp->param = atoi(argv[1]);
      temp->ptr = (int*)ptr;
      temp->empty = empty;
      temp->full = full;
      temp->current = 0;
      pthread_mutex_init(&(temp->lock), NULL);
      
      //���������߳�
      pthread_t tid[3];
      pthread_attr_t attr; 
     	pthread_attr_init(&attr);
      pthread_create(&tid[0],&attr,func,temp);
     	pthread_create(&tid[1],&attr,func,temp);
     	pthread_create(&tid[2],&attr,func,temp); 
	    for(int i=0;i<3;i++){
			    pthread_join(tid[i],NULL);
 	  	}
      shm_unlink(name);
      return 0;
}
void *func(void *atr){
      //��ȡ���̺��߳�id
      share *share = atr;
      pid_t pid = (int)getpid();
      pid_t tid = (int)syscall(__NR_gettid);

      //���ո�ָ���ֲ������������ߵļ��
      int sleep_time;
      double x;
      while(1){
            //���ڸ�ָ���ֲ����ܻ������С��ֵ���൱���㣬��Ҫ����xֵ
            //����0~99������������ҳ���100����Ϊ�Ա���
            x = (rand()%100)/100.0;
            sleep_time = share->param * exp(share->param * x * -1);
            sleep(sleep_time);

            //�߳�֮��Ļ�����
            pthread_mutex_lock(&share->lock);
            sem_wait(share->full);
            //�������һ�������Ҵ��빲���ڴ�Ļ�����
            printf("%d\t%d\t%d\n",pid,tid,share->ptr[share->current]);
            share->current += 1;
            if(share->current == 20)
                  share->current = 0;
            sem_post(share->empty);
            pthread_mutex_unlock(&share->lock);
            //sem_post(share->put);
      }
}
