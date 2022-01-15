// C library headers
#include <stdio.h>
#include <string.h>

// Linux headers
#include <fcntl.h> // Contains file controls like O_RDWR
#include <errno.h> // Error integer and strerror() function
#include <termios.h> // Contains POSIX terminal control definitions
#include <unistd.h> // write(), read(), close()
#include <pthread.h>
#include <stdlib.h>
#include <time.h>

int USB;
char led[1];
pthread_mutex_t mutex_serial;
pthread_t pt[2];
void config(){
    struct termios tty;

    cfsetospeed (&tty, (speed_t)B115200);
    cfsetispeed (&tty, (speed_t)B115200);

    /* Setting other Port Stuff */
    tty.c_cflag     &=  ~PARENB;            // Make 8n1
    tty.c_cflag     &=  ~CSTOPB;
    tty.c_cflag     &=  ~CSIZE;
    tty.c_cflag     |=  CS8;

    tty.c_cflag     &=  ~CRTSCTS;           // no flow control
    tty.c_cc[VMIN]   =  1;                  // read doesn't block
    tty.c_cc[VTIME]  =  5;                  // 0.5 seconds read timeout
    tty.c_cflag     |=  CREAD | CLOCAL;     // turn on READ & ignore ctrl lines

    tty.c_cc[VTIME] = 10;    // Wait for up to 1s (10 deciseconds), returning as soon as any data is received.
    tty.c_cc[VMIN] = 0;

    tcflush( USB, TCIFLUSH );
    if ( tcsetattr ( USB, TCSANOW, &tty ) != 0) {
        fprintf(stderr, "Error\n"); 
    }
}

void leer(){
    
        FILE * fd;  
        // Allocate memory for read buffer, set size according to your needs
        char read_buf [8];
        int num_bytes;

        fd = fopen("datostemp.txt", "w+");
    while(1){
        // Normally you wouldn't do this memset() call, but since we will just receive
        // ASCII data for this example, we'll set everything to 0 so we can
        // call printf() easily.
        //memset(&read_buf, '\0', sizeof(char)*5);

        // Read bytes. The behaviour of read() (e.g. does it block?,
        // how long does it block for?) depends on the configuration
        // settings above, specifically VMIN and VTIME
        //pthread_mutex_lock(&mutex_serial);
        num_bytes = read(USB, &read_buf, sizeof(char)*8);
        if ((read_buf[0] == '0') && (read_buf[1] == '0') && (read_buf[2] == '0')){
            fprintf(fd, "%s\n", &read_buf[3]);
        }
        //pthread_mutex_unlock(&mutex_serial);

        //fprintf(fd, "%s\n", read_buf);
        //printf("bytes leidos: %i . Mensaje recibido: %s", num_bytes, read_buf);
        fflush(fd);
       
    }
}
void escribir(){
unsigned char dato[2];
printf("Opcion 1: Pulsar el boton azul para leer temperaturas\n");
printf("Opcion 2: Introducir el numero 1 del teclado para encender el led verde\n");
printf("Opcion 3: Introducir el numero 0 del teclado para encender el led azul\n");
    while(1){
      
        scanf("%s",led);
        
        /*if(led==1){
        
        dato[0]=1;
        }*/
        //pthread_mutex_lock(&mutex_serial);
        //write(USB,dato,sizeof(int));
        if(write(USB,led,sizeof(char))!=sizeof(char)){
            printf("Error en el envio \n");
        }
        //pthread_mutex_unlock(&mutex_serial);
        
        //printf("%s",dato);
    
    }
}


int main (){
    USB = open( "/dev/ttyACM0", O_RDWR| O_NOCTTY );
    config();

    pthread_create(&pt[0],NULL, (void *) leer,NULL);
    pthread_create(&pt[1],NULL, (void *) escribir,NULL);
    pthread_join(pt[0], NULL);
    pthread_join(pt[1], NULL);

    close(USB);
}
