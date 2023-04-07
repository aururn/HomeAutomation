// GPIO Part
#include <stdio.h> //printf(), perror()
#include <stdlib.h> //atof()
#include <sys/socket.h> //socket()
#include <sys/ioctl.h> //ioctl()
#include <netinet/in.h> //htons(), inet_addr()
#include <string.h> //memset()
#include <unistd.h> //close()

#include <wiringPi.h> //gpio()

// Declare status
const int PortNumber = 5000;
const int BufLength = 1024;

void Gpio(int Pin, int Value)
{
    pinMode(Pin, OUTPUT);

    if(Value == 0)
    {
        digitalWrite(Pin, LOW);
        printf("GPIO%d is OFF\n\n", Pin);
    }
    else if(Value == 1)
    {
        digitalWrite(Pin, HIGH);
        printf("GPIO%d is OFF\n\n", Pin);
    }
    else
    {
        printf("GPIO Value error\n\n");
    }
}

// take out data from buff and check data
void Getdata(char *buff, int ini_pin[], int ini_element)
{
    char rev_data[100];
    int i=4,j=0;

    while(buff[i] != '\0')
    {
        rev_data[j] = buff[i];
        i++,j++;
    }

    char Pin_s[100];
    char Value_s[100];

    sscanf(rev_data, "%s %s", Pin_s, Value_s); // Divide buff into Pin and Value
 
    int Pin = atoi(Pin_s);  // Take out Pin number
    int Value = atoi(Value_s);  // Take out pin Value

//check if the pin number is in the array of allowed pin
    for(int k=0; k<ini_element; k++)
    {
        if(Pin == ini_pin[k]) // if correct pin
        {
            Gpio(Pin, Value);
        }
    }
}

int check_ini(int ini_pin[], ini_value[])
{
// configure initial data from txt file
    FILE *fp = NULL;
    char ini_data[100]; //initial [pin value]
    int i=0;

    fp = fopen("ini.txt", "r"); // open txt file

    if(fp == NULL)
    {
        printf("file open error\n");
        return -1;
    }

    char header[1024];
    fgets(header, sizeof(header), fp);  // remove file header

    while(fgets(ini_data, 100, fp) != NULL)
    {
        sscanf(ini_data, "%d %d", &ini_pin[i], &ini_value[i]);  // take out data
        i++;
    }
    fclose(fp); // close file

// set up initial gpio
    wiringPisetupGpio();  

    int Line = i;

    printf("< initial set up >\n");
    for(int j=0; j<line; j++)
    {
        pinMode(ini_pin[j], OUTPUT);
        if(ini_value[j] == 0)
        {
            digitalWrite(ini_pin[j], LOW);
            printf("GPIO%d is OFF\n\n", ini_pin[j]);
        }
        else if(ini_value[j] == 1)
        {
            digitalWrite(ini_pin[j], HIGH);
            printf("GPIO%d is ON\n\n", ini_pin[j]);
        }
    }
    return Line;
}

// communication Part
int main(int argc, char** argv)
{
// read the input file and set initial value for gpio
    int ini_pin[100];
    int ini_value[100];
    int ini_element = check_ini(ini_pin, ini_value);

// create address
    struct sockaddr_in addr , from_addr;
    int bind_status;
    int sock;
    char buff[BufferLength];
    socklen_t from_addr_size;

    memset(buff, 0x00, BufLength);

// create socket
    sock = socket(AF_INET, SOCK_DGRAM, 0);

// if create socket is failed
    if(sock < 0)
    {
        printf("Could not make a socket\n");
        return -1;
    }

// communication setting
    addr.sin_family = AF_INET; // IPv4
    addr.sin_port = htons(PortNumber); // PortNumber:5000
    addr.sin_addr.s_addr = INADDR_ANY; // client(not specify)
    bind_status = bind(sock, (struct sockaddr *)&addr, sizeof(addr) ); // resister socket

// if resister socket is failed
    if( bind_status < 0)
    {
        printf("bind error\n");
        return -1;
    }
    printf("<<bind success>>\n\n");

// receive data
    while(1)
    {
        int recv_status;
    
        recv_status = recvfrom(sock, buff, BufLength + 1, 0, (struct sockaddr *)&from_addr, &from_addr_size);

        if ( recv_status < 0)
        {
            continue;
        }

        buff[BufLength - 1]= '\0'; 
        printf("received data:%s\n", buff);

        Getdata(buff, ini_pin, ini_element);     
    }

    close(sock_df); // close socket

    return 0;
}