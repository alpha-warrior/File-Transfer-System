// Client side C/C++ program to demonstrate Socket programming
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

void save_file(int sock)
{
    long long int file_size,recv_size,percentage;
    char buffer[1024],message[1024],percent[10];
    int fd,valread;
    recv_size=0;
    printf("%s",buffer);
    fflush(stdout);
    bzero(buffer,1024);
    char * token = strtok(NULL," ");
    fd = open(token,O_WRONLY | O_CREAT | O_TRUNC , 0600);
    fflush(stdout);
    strcpy(message,"Ready");
    send(sock,message,strlen(message),0);
    bzero(message,1024);

    for(;;)
    {
        valread = read(sock,buffer,1024);

        if(valread>0)
        {
            file_size = atoi(buffer);
            break;
        }
    }
    printf("File size is %lld\n",file_size);
    fflush(stdout);
    strcpy(message,"Ready");
    send(sock,message,strlen(message),0);
    bzero(buffer,1024);

    // usleep(100);
    percent[0]='\b';
    percent[1]='\b';
    percent[2]='\b';
    percent[3]='\b';
    percent[4]='0';
    percent[5]='0';
    percent[6]='0';
    percent[7]='%';
    percent[8]='\0';

    printf("Percentage of File Copied:       ");
    printf("%s",percent);
    fflush(stdout);
    for(;;)
    {
        valread = read(sock,buffer,1024);
        if(valread>0)
        {
            recv_size+=valread;
            write(fd,buffer,valread);
            percentage = ((recv_size*100)/file_size);
            // printf("%lld %lld\n",recv_size,percentage);
            // fflush(stdout);

            percent[4] = percentage/100 + '0';
            percentage%=100;
            percent[5] = percentage/10 + '0';
            percentage%=10;
            percent[6] = percentage + '0';
            printf("%s",percent);
            // printf("\b\b\b%d",(recv_size/file_size)*100);
            // printf("%lld %lld\n",recv_size,percentage);
            fflush(stdout);
            bzero(buffer,1024);
        }
        if(recv_size>=file_size)
        {
            break;
        }
    }
    close(fd);
    // printf("DONE \n");
    return;
}

void get_files(char * input_str,int sock)
{
    // printf("%s",input_str);
    int count=-1,i,j,k,valread,fd;
    long long int file_size,recv_size=0,percentage=0;
    char str_count[10],temp_str[1000],buffer[1024],temp_buffer[1024],message[1024],percent[10];
    input_str[strlen(input_str)-1]='\0';
    // buffer[0]='\0';
    send(sock,input_str,strlen(input_str),0);
    for(;;)
    {
        bzero(buffer,1024);
        // printf("DIED");
        valread = read(sock,buffer,1024);
        if (valread>0)
        {
            strcpy(temp_buffer,buffer);
            char * token = strtok(temp_buffer," ");
            if(token!=NULL && strcmp(token,"Error")==0)
            {
                printf("%s",buffer);
                fflush(stdout);
                bzero(buffer,1024);
            }
            else if(token!=NULL && strcmp(token,"Sending")==0)
            {
                save_file(sock);
                printf("\nFile fetched\n");
                fflush(stdout);
            }
            else if(token!=NULL && strcmp(token,"All")==0)
            {
                printf("\nAll files fetched\n");
                fflush(stdout);
                bzero(buffer,1024);
                return;
            }


        }
    }
    // strcpy(temp_str,input_str);
    // char * token = strtok(temp_str," ");
    // while(token!=NULL)
    // {
    //     count+=1;
    //     token=strtok(NULL," ");
    // }
    // sprintf(str_count,"%d",count);
    // send(sock , str_count , strlen(str_count) , 0 ); 
    // token = strtok(input_str," ");
    // for(i=0;i<count;i++)
    // {
    //     token = strtok(NULL," ");
    //     send(sock,token,strlen(token),0);
    // }

}

int main(int argc, char const *argv[])
{
    size_t size = 1000;
    char * input_str;
    input_str = (char*)malloc(sizeof(char)*1000);
    char temp_str[1000];
    struct sockaddr_in address;
    int sock = 0, valread;
    struct sockaddr_in serv_addr;
    char *hello = "Hello from client";
    char buffer[1024] = {0};
    if ((sock = socket(AF_INET, SOCK_STREAM, 0)) < 0)
    {
        printf("\n Socket creation error \n");
        return -1;
    }

    memset(&serv_addr, '0', sizeof(serv_addr)); // to make sure the struct is empty. Essentially sets sin_zero as 0
                                                // which is meant to be, and rest is defined below

    serv_addr.sin_family = AF_INET;
    serv_addr.sin_port = htons(PORT);

    // Converts an IP address in numbers-and-dots notation into either a 
    // struct in_addr or a struct in6_addr depending on whether you specify AF_INET or AF_INET6.
    if(inet_pton(AF_INET, "127.0.0.1", &serv_addr.sin_addr)<=0)
    {
        printf("\nInvalid address/ Address not supported \n");
        return -1;
    }

    if (connect(sock, (struct sockaddr *)&serv_addr, sizeof(serv_addr)) < 0)  // connect to the server address
    {
        printf("\nConnection Failed \n");
        return -1;
    }
    printf("Connection established successfully\n");
    for(;;)
    {
        printf("client>>");
        getline(&input_str,&size,stdin);
        if(input_str!=NULL)
        {
            strcpy(temp_str,input_str);
            char * token = strtok(temp_str," ");
            // printf("%s",token);
            if(strcmp(token,"get")==0)
            {
                get_files(input_str,sock);
            }
            else if(strcmp(token,"exit\n")==0||strcmp(token,"exit")==0)
            {
                send(sock,"exit now",strlen("exit now"),0);
                return 0;
            }
            else
            {
                printf("Invalid Command\n");
            }
        }
    }
    // send(sock , hello , strlen(hello) , 0 );  // send the message.
    // printf("Hello message sent\n");
    // valread = read( sock , buffer, 1024);  // receive message back from server, into the buffer
    // printf("%s\n",buffer);
    // return 0;
}
