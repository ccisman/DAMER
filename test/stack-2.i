int nondet_num_uint=0;
void abort(){} 
void reach_error(){}
void abort(){} 
void assume_abort_if_not(int cond) { 
  if(!cond) {abort();}
}





int __VERIFIER_nondet_uint()
		{
			nondet_num_uint = nondet_num_uint + 1;
			return nondet_num_uint;
		}

static int top=0;
static unsigned int arr[(5)];
pthread_mutex_t m;
int flag=(0) ;

void error(void) 
{ 
   {reach_error();abort();}
  return;
}

void inc_top(void)
{
  top++;
}

void dec_top(void)
{
  top--;
}

int get_top(void)
{
  return top;
}

int stack_empty(void)
{
  return (top==0) ? (1) : (0) ;
}

int push(unsigned int *stack, int x)
{
  if (top==(5)) 
  {
    printf("stack overflow\n");
    return (-1);
  } 
  else 
  {
    stack[get_top()] = x;
    inc_top();
  }
  return 0;
}

int pop(unsigned int *stack)
{
  if (get_top()==0) 
  {
    printf("stack underflow\n");	
    return (-2);
  } 
  else 
  {
    dec_top();
    return stack[get_top()];  
  }
  return 0;
}

void *t1(void *arg) 
{
  int i;
  unsigned int tmp;

  {
i=0;
while( i<(5)){
  {
    pthread_mutex_lock(&m);
    tmp = __VERIFIER_nondet_uint();
    assume_abort_if_not(tmp < (5));
    if (push(arr,tmp)==(-1))
      error();
    flag=(1);
    pthread_mutex_unlock(&m);
  } i++;}
}
  return 0;
}

void *t2(void *arg) 
{
  int i;

  {
i=0;
while( i<(5)){
  {
    pthread_mutex_lock(&m);
    if (flag)
    {
      if (!(pop(arr)!=(-2)))
        error();
    }
    pthread_mutex_unlock(&m);
  } i++;}
}
  return 0;
}


int main(void) 
{
  pthread_t id1, id2;

  pthread_mutex_init(&m, 0);

  pthread_create(&id1, 0, t1, 0);
  pthread_create(&id2, 0, t2, 0);

  pthread_join(id1, 0);
  pthread_join(id2, 0);

  return 0;
}

