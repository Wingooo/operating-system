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
      int *ptr;       //指向共享缓冲区的指针
      int param;     //命令行传入的负指数分布参数
      pthread_mutex_t lock;  //用于线程之间的互斥
      int current;   //用于访问缓冲区时记录第几个buffer
}share;

void *func(void *atr);

int main(int argc,char **argv){
      //信号量的初始化
      sem_t *empty;
      sem_t *full;
      empty = sem_open("empty",O_RDWR,0667);
      full = sem_open("full",O_RDWR,0667);
      //共享内存的初始化
      const char *name = "OS";
      int shm_fd;
      void *ptr;
      shm_fd = shm_open(name,O_RDONLY, 0667);
      ptr = mmap(0,Buffer_Size, PROT_READ, MAP_SHARED, shm_fd, 0);

      //给结构体赋值
      share *temp = (share*)malloc(sizeof(share));
      temp->param = atoi(argv[1]);
      temp->ptr = (int*)ptr;
      temp->empty = empty;
      temp->full = full;
      temp->current = 0;
      pthread_mutex_init(&(temp->lock), NULL);
      
      //创建三个线程
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
      //获取进程和线程id
      share *share = atr;
      pid_t pid = (int)getpid();
      pid_t tid = (int)syscall(__NR_gettid);

      //按照负指数分布来控制生产者的间隔
      int sleep_time;
      double x;
      while(1){
            //由于负指数分布可能会产生很小的值，相当于零，故要控制x值
            //产生0~99的随机数，并且除以100，作为自变量
            x = (rand()%100)/100.0;
            sleep_time = share->param * exp(share->param * x * -1);
            sleep(sleep_time);

            //线程之间的互斥锁
            pthread_mutex_lock(&share->lock);
            sem_wait(share->full);
            //随机产生一个数并且传入共享内存的缓冲区
            printf("%d\t%d\t%d\n",pid,tid,share->ptr[share->current]);
            share->current += 1;
            if(share->current == 20)
                  share->current = 0;
            sem_post(share->empty);
            pthread_mutex_unlock(&share->lock);
            //sem_post(share->put);
      }
}
