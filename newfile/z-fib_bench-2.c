void abort(){} 
void __VERIFIER_error(){}

void reach_error(){__VERIFIER_error();}






int i=1, j=1;


void *
t1(void* arg)
{
  int k = 0;

  {
k = 0;
while( k < 5){
    
    i=i+j;
    
   k;k=k+1;
}
}
  pthread_exit(NULL);
}

void *
t2(void* arg)
{
  int k = 0;

  {
k = 0;
while( k < 5){
    
    j=j+i;
    
   k;k=k+1;
}
}
  pthread_exit(NULL);
}

int
main(int argc, char **argv)
{
  pthread_t id1, id2;
  int condI,condJ;

  pthread_create(&id1, NULL, t1, NULL);
  pthread_create(&id2, NULL, t2, NULL);

  
  condI = i >= 144;
  

  
  condJ = j >= 144;
  

  if (condI || condJ) {
     {reach_error();abort();}
  }

  return 0;
}
