#include <stdio.h>
#include <sys/socket.h>
#include <stdlib.h>
#include <netinet/in.h>
#include <string.h>
#include <unistd.h>
#include <arpa/inet.h>
#include <sys/stat.h>
#include <fcntl.h>

#define PORT 8000


void send_files(char * input_str, int new_socket)
{
    int fd,i,j,k,valread;
    long long int file_size;
    char message[1060],buffer[1060];
    char * token = strtok(input_str," ");
    token = strtok(NULL," ");

    while(token!=NULL)
    {
        printf("Searching for %s\n",token);
        fd = open(token , O_RDONLY);
        if(fd<0)
        {
            strcpy(message,"Error in sending ");
            strcat(message,token);
            strcat(message," \n");
            printf("%s\n",message);
            send(new_socket,message,strlen(message),0);
            bzero(message,1024);

        }
        else
        {
            strcpy(message,"Sending ");
            strcat(message,token);
            strcat(message," \n");
            printf("%s\n",message);
            send(new_socket,message,strlen(message),0);
            bzero(message,1024);

            for(;;)
            {
                valread = read(new_socket , buffer, 1024);
                if(valread>0)
                {
                    if(strcmp(buffer,"Ready")==0)
                    {
                        break;
                    }
                    else
                    {
                        printf("WE GOT PROBLEM\n");
                        fflush(stdout);
                        break;
                    }  
                }
            }
            // printf("Name SENT and notif\n");
            // fflush(stdout);
            file_size = lseek(fd,0,SEEK_END);
            lseek(fd,-file_size,SEEK_CUR);
            // printf("1");
            // fflush(stdout);
            sprintf(message,"%lld",file_size);
            // printf("2");
            // fflush(stdout);
            send(new_socket,message,strlen(message),0);
            bzero(message,1024);

            // printf("SIZE SENT");
            // fflush(stdout);
            for(;;)
            {
                valread = read(new_socket , buffer, 1024);
                if(valread>0)
                {
                    if(strcmp(buffer,"Ready")==0)
                    {
                        break;
                    }
                    else
                    {
                        printf("WE GOT PROBLEM\n");
                        fflush(stdout);
                        break;
                    }  
                }
            }
            // printf("SENT SIZE and now sending file\n");
            // fflush(stdout);

            for(;;)
            {
                if(file_size<1024)
                {
                    break;
                }
                else
                {
                    read(fd,message,1024);
                    send(new_socket,message,strlen(message),0);
                    // printf("I am in 1 %s\n",message);
                    // fflush(stdout);
                    file_size-=1024;
                    bzero(message,1024);
                }
                // usleep(1);
            }
            if(file_size>0)
            {
                read(fd,message,file_size);
                send(new_socket,message,strlen(message),0);
                file_size-=file_size;
                // printf("I am in 2 %s HERE\n",message);
                // fflush(stdout);
                bzero(message,1024);

            }
            close(fd);
            printf("DONE");
            fflush(stdout);
            // sleep(1);
        }
        
        token = strtok(NULL," ");
        // sleep(1);
    }
    sleep(1);
    strcpy(message,"All Files Fetched");
    send(new_socket,message,strlen(message),0);
    bzero(message,1024);

    // sleep(10000);
}

int main(int argc, char const *argv[])
{
    int count=0,i,j,k;
    char temp_string_idk[1024];
    int server_fd, new_socket, valread;
    struct sockaddr_in address;  
    int opt = 1;
    int addrlen = sizeof(address);
    char buffer[1024] = {0};
    char *hello = "Hello from server";

    // Creating socket file descriptor
    if ((server_fd = socket(AF_INET, SOCK_STREAM, 0)) == 0)  // creates socket, SOCK_STREAM is for TCP. SOCK_DGRAM for UDP
    {
        perror("socket failed");
        exit(EXIT_FAILURE);
    }

    // This is to lose the pesky "Address already in use" error message
    if (setsockopt(server_fd, SOL_SOCKET, SO_REUSEADDR | SO_REUSEPORT,
                                                  &opt, sizeof(opt))) // SOL_SOCKET is the socket layer itself
    {
        perror("setsockopt");
        exit(EXIT_FAILURE);
    }
    address.sin_family = AF_INET;  // Address family. For IPv6, it's AF_INET6. 29 others exist like AF_UNIX etc. 
    address.sin_addr.s_addr = INADDR_ANY;  // Accept connections from any IP address - listens from all interfaces.
    address.sin_port = htons( PORT );    // Server port to open. Htons converts to Big Endian - Left to Right. RTL is Little Endian

    // Forcefully attaching socket to the port 8080
    if (bind(server_fd, (struct sockaddr *)&address,
                                 sizeof(address))<0)
    {
        perror("bind failed");
        exit(EXIT_FAILURE);
    }

    // Port bind is done. You want to wait for incoming connections and handle them in some way.
    // The process is two step: first you listen(), then you accept()
    if (listen(server_fd, 3) < 0) // 3 is the maximum size of queue - connections you haven't accepted
    {
        perror("listen");
        exit(EXIT_FAILURE);
    }

    // returns a brand new socket file descriptor to use for this single accepted connection. Once done, use send and recv
    if ((new_socket = accept(server_fd, (struct sockaddr *)&address,
                       (socklen_t*)&addrlen))<0)
    {
        perror("accept");
        exit(EXIT_FAILURE);
    }
    for(;;)
    {
        valread = read(new_socket , buffer, 1024);  // read infromation received into the buffer
        if(valread>0)
        {
            strcpy(temp_string_idk,buffer);
            char * token = strtok(temp_string_idk," ");
            if(token!=NULL && strcmp(token,"get")==0)
            {
                send_files(buffer,new_socket);
                bzero(buffer,1024);
            }
            else if(token!=NULL && strcmp(token,"exit")==0)
            {
                break;
            }
        }
    }
    // printf("%s\n",buffer);
    // count = atoi(buffer);
    // printf("%d\n",count);
    // for(i=0;i<count;i++)
    // {
    //     valread = read(new_socket , buffer, 1024);
    //     printf("%s\n",buffer);
    // }
    // send(new_socket , hello , strlen(hello) , 0 );  // use sendto() and recvfrom() for DGRAM
    // printf("Hello message sent\n");
}
