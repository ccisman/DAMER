extern void abort(void); 
extern void __VERIFIER_error();
void reach_error(){__VERIFIER_error();}

#include <pthread.h>
#include <assert.h>

pthread_mutex_t  mutex;
int data = 0;

void *thread1(void *arg)
{
  pthread_mutex_lock(&mutex);
  data++;
  pthread_mutex_unlock(&mutex);
  return 0;
}


void *thread2(void *arg)
{
  pthread_mutex_lock(&mutex);
  data+=2;
  pthread_mutex_unlock(&mutex);
  return 0;
}


void *thread3(void *arg)
{
  pthread_mutex_lock(&mutex);
  if (data >= 3){
    ERROR: {reach_error();abort();}
    ;
  }
  pthread_mutex_unlock(&mutex);    
  return 0;
}


int main()
{
  pthread_t t1, t2, t3;
  pthread_mutex_init(&mutex, 0);

  pthread_create(&t1, 0, thread1, 0);
  pthread_create(&t2, 0, thread2, 0);
  pthread_create(&t3, 0, thread3, 0);

  pthread_join(t1, 0);
  pthread_join(t2, 0);
  pthread_join(t3, 0);
  
  return 0;
}
