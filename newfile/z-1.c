int  num;
pthread_mutex_t  m;
pthread_cond_t  empty, full;
void reach_error(){return;}
void abort(){return;}
void * thread1(void * arg){  
            pthread_mutex_lock(&m);  
            while (num > 0)     
                        pthread_cond_wait(&empty, &m);    
            num;num=num+1;
  
            pthread_mutex_unlock(&m);  
            pthread_cond_signal(&full);  
            return 0;
}
void * thread2(void * arg){  
            pthread_mutex_lock(&m);  
            while (num == 0)
                        pthread_cond_wait(&full, &m);  
            num;num=num-1;
    
            pthread_mutex_unlock(&m);  
            pthread_cond_signal(&empty);  
            return 0;
}
int main(){  
            pthread_t  t1, t2;  
            num = 1;  
            pthread_mutex_init(&m, 0);  
            pthread_cond_init(&empty, 0);  
            pthread_cond_init(&full, 0);    
            pthread_create(&t1, 0, thread1, 0);  
            pthread_create(&t2, 0, thread2, 0);    
            pthread_join(t1, 0);  
            pthread_join(t2, 0);  
            if (num!=1)  {    
                 {reach_error();abort();}    
                ;  
            }  
            return 0;  
}
