int nondet_num_int=0;

void abort(){} 
void reach_error(){}
int __VERIFIER_nondet_int()
		{
			nondet_num_int = nondet_num_int + 1;
			return nondet_num_int;
		}






typedef struct {
  int element[(20)];
  int head;
  int tail;
  int amount;
} QType;

pthread_mutex_t m;
int __VERIFIER_nondet_int();
int stored_elements[(20)];
int enqueue_flag, dequeue_flag;
QType queue;

void init(QType *q)
{
  q->head=0;
  q->tail=0;
  q->amount=0;
}

int empty(QType * q) 
{
  if (q->head == q->tail) 
  { 
    printf("queue is empty\n");
    return (-1);
  }
  else 
    return 0;
}

int full(QType * q) 
{
  if (q->amount == (20)) 
  {  
    printf("queue is full\n");
    return (-2);
  } 
  else
    return 0;
}

int enqueue(QType *q, int x) 
{
  q->element[q->tail] = x;
  q->amount++;
  if (q->tail == (20)) 
  {
    q->tail = 1;
  } 
  else 
  {
    q->tail++;
  }

  return 0;
}

int dequeue(QType *q) 
{
  int x;

  x = q->element[q->head];
  q->amount--;
  if (q->head == (20)) 
  {
    q->head = 1;
  } 
  else 
    q->head++;

  return x;
}

void *t1(void *arg) 
{
  int value, i;

  pthread_mutex_lock(&m);
  if (enqueue_flag)
  {
    {
i=0;
while( i<(20)){  
    {
      value = __VERIFIER_nondet_int();
      enqueue(&queue,value);
      stored_elements[i]=value;
    } i++;}
}
    enqueue_flag=(0);
    dequeue_flag=(1);
  }	
  pthread_mutex_unlock(&m);

  return 0;
}

void *t2(void *arg) 
{
  int i;

  pthread_mutex_lock(&m);
  if (dequeue_flag)
  {
    {
i=0;
while( i<(20)){  
    {
      if (empty(&queue)!=(-1))
        if (!dequeue(&queue)==stored_elements[i]) {
           {reach_error();abort();}
        }
    } i++;}
}
    dequeue_flag=(0);
    enqueue_flag=(1);
  }
  pthread_mutex_unlock(&m);

  return 0;
}

int main(void) 
{
  pthread_t id1, id2;

  enqueue_flag=(1);
  dequeue_flag=(0);

  init(&queue);

  if (!empty(&queue)==(-1)) {
     {reach_error();abort();}
  }

  pthread_mutex_init(&m, 0);

  pthread_create(&id1, 0, t1, &queue);
  pthread_create(&id2, 0, t2, &queue);

  pthread_join(id1, 0);
  pthread_join(id2, 0);

  return 0;
}

