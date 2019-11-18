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

//����һ���������ʱ�䣬��������ʱ�䣬��ѧ�����뿪����
#define MAX_TIME 100

//���������ѧ�ң����Ϊ0��1��2��3��4
int ph_num[] = {0,1,2,3,4};

//�����������Ϊ������
pthread_mutex_t chop[5];


//������˹�ֲ��������������ʱ��
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


//�̳߳�ʼ�����øú���
void* process(void *ph_num)
{
  //�̳߳�ʼ������Ĳ���
	int id = *(int*)ph_num; 

  //����i����ѧ�ҵ���ߵĿ��Ӷ���Ϊi���ұߵĶ���Ϊ��i+1)%5
	int chopLeft = id;
	int chopRight = (id+1)%5;
	int temp;
	
 //����һ����¼���е�ʱ�䣨����˼����ʱ�䣬���Է�����ʱ�䣩
	int run_time = 0;
	
 //������ʱ��С�ڹ涨ʱ�䣬�����ִ��
	while(run_time<MAX_TIME)
	{
		pthread_mutex_lock(&chop[id]);
    pickup_forks(id);
		if(!pthread_mutex_trylock(&chop[chopRight])==0)	
		{
      
			pthread_mutex_unlock(&chop[id]);

			//���½���˼�����ȴ���һ�ξͲ�
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
	//�������ʱ�䣬��ѧ�����뿪����
	printf("Philosopher %d has left the table\n",id);
	return 0;
}

int main (void)
{
  int i, rd;
  int pid;
  
  //��������߳�
  pthread_t philosopher[5];
  
    for(i = 0; i < 5; i++)
    {
      //��������ʼ��
        if(pthread_mutex_init(&chop[i], NULL) != 0)
            printf("Failed to initialize mutex\n");
    }
     //�̳߳�ʼ��
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