#include <pthread.h>
#include <unistd.h>

void reader(void* args){

}

void writer(void* args){

}


int main(void){
    
    
    
    pthread_t reader_arr[2];
    pthread_t writer_arr[2];

    writer_arr[0] = pthread_create(NULL, NULL, writer, NULL);
    writer_arr[1] = pthread_create(NULL, NULL, writer, NULL);

    reader_arr[0] = pthread_create(NULL, NULL, reader, NULL);
    reader_arr[1] = pthread_create(NULL, NULL, reader, NULL);



    return 0;
}