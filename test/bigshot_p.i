void abort(){} 
void reach_error(){}





void __VERIFIER_assert(int expression) { if (!expression) {  {reach_error();abort();}}; return; }

char *v;

void *thread1(void * arg)
{
  v = calloc(8, sizeof(char));
  return 0;
}

void *thread2(void *arg)
{
  if (v) strcpy(v, "Bigshot");
  return 0;
}


int main()
{
  pthread_t t1, t2;

  pthread_create(&t1, 0, thread1, 0);
  pthread_create(&t2, 0, thread2, 0);
  pthread_join(t1, 0);
  pthread_join(t2, 0);

  __VERIFIER_assert(!v || v[0] == 'B');

  return 0;
}

