

void abort(){} 
void __VERIFIER_error(){}
 
void reach_error(){__VERIFIER_error();}




int i = 3, j = 6;


void *t1(void *arg) {
  {
int k = 0;
while( k < 5){
    
    i = j + 1;
    
   k;k=k+1;
}
}
  pthread_exit(NULL);
}

void *t2(void *arg) {
  {
int k = 0;
while( k < 5){
    
    j = i + 1;
    
   k;k=k+1;
}
}
  pthread_exit(NULL);
}

int main(int argc, char **argv) {
  pthread_t id1, id2;
  int condI,condJ;

  pthread_create(&id1, NULL, t1, NULL);
  pthread_create(&id2, NULL, t2, NULL);

  
  condI = i > (2*5+6);
  

  
  condJ = j > (2*5+6);
  

  if (condI || condJ) {
     {reach_error();abort();}
  }

  return 0;
}
