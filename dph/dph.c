#include <stdio.h>
#include <unistd.h>
#include <math.h>
#include <pthread.h>
#include <ctype.h>
#include <errno.h>
#include <signal.h>
#include <stdlib.h>

#include <sys/types.h>
#include <sys/stat.h>
#include <sys/wait.h>

//定义一个最大运行时间，当超过该时间，哲学家们离开餐桌
#define MAX_TIME 100

//定义五个哲学家，编号为0，1，2，3，4
int ph_num[] = {0,1,2,3,4};

//定义五根筷子为互斥锁
pthread_mutex_t chop[5];


//产生高斯分布的随机数来当作时间
int randomGaussian_r(int mean, int stddev, unsigned int* state) {
	double mu = 0.5 + (double) mean;
	double sigma = fabs((double) stddev);
	double f1 = sqrt(-2.0 * log((double) rand_r(state) / (double) RAND_MAX));
	double f2 = 2.0 * 3.14159265359 * (double) rand_r(state) / (double) RAND_MAX;
	if (rand_r(state) & (1 << 5))
		return (int) floor(mu + sigma * cos(f2) * f1);
	else
		return (int) floor(mu + sigma * sin(f2) * f1);
}


void pickup_forks(int philosopher_number){
      int num = philosopher_number;
      printf("Philosopher %d want to have a dinner\n",num);
}

void return_forks(int philosopher_number){
    int num = philosopher_number;
      printf("Philosopher %d have finished a dinner\n",num);
} 


//线程初始化调用该函数
void* process(void *ph_num)
{
  //线程初始化传入的参数
	int id = *(int*)ph_num; 

  //将第i个哲学家的左边的筷子定义为i，右边的定义为（i+1)%5
	int chopLeft = id;
	int chopRight = (id+1)%5;
	int temp;
	
 //定义一个记录运行的时间（不计思考的时间，即吃饭的总时间）
	int run_time = 0;
	
 //当运行时间小于规定时间，则继续执行
	while(run_time<MAX_TIME)
	{
		pthread_mutex_lock(&chop[id]);
    pickup_forks(id);
		if(!pthread_mutex_trylock(&chop[chopRight])==0)	
		{
      
			pthread_mutex_unlock(&chop[id]);

			//重新进入思考，等待下一次就餐
			temp = abs(randomGaussian_r(1, 5, (unsigned int*) ph_num));
			printf("Philosopher %d is thinking for %d seconds\n",id,temp);

			sleep(temp);
		}
		else
		{
			// feeding randomGaussian with stddev, mean, and ph_num (as random seed)
			
			temp = abs(randomGaussian_r(9,3, (unsigned int*) ph_num));			

			run_time += temp;
			printf("Philosopher %d is eating for %d seconds\n",id,temp);
			sleep(temp);
			
			pthread_mutex_unlock(&chop[id]);			
      pthread_mutex_unlock(&chop[chopRight]);
      return_forks(id);
			
			temp = abs(randomGaussian_r(11,7, (unsigned int*) ph_num));
			printf("Philosopher %d is thinking for %d seconds\n",id,temp);

			sleep(temp);
			
		}
		
	}
	//超过最大时间，哲学家们离开餐桌
	printf("Philosopher %d has left the table\n",id);
	return 0;
}

int main (void)
{
  int i, rd;
  int pid;
  
  //创建五个线程
  pthread_t philosopher[5];
  
    for(i = 0; i < 5; i++)
    {
      //互斥锁初始化
        if(pthread_mutex_init(&chop[i], NULL) != 0)
            printf("Failed to initialize mutex\n");
    }
     //线程初始化
    for(i = 0; i < 5; i++)
    {
        if(pthread_create(&philosopher[i], NULL, process, (void *) &ph_num[i])  != 0)
            printf("Failed to initialize thread\n") ;
    }
    
    for(i = 0; i < 5; i++)
    {
        pthread_join(philosopher[i], NULL);
    }
    for(i = 0; i < 5; i++)
    {
        pthread_mutex_destroy(&chop[i]);
    }
	
	return 0;
}