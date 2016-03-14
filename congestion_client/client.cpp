#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <string.h>
#include <sys/time.h>
#include "congestion.h"

using namespace std;

u_int32_t g_loss_rate=0;
int g_running = 1;

void *send_fb(void *arg);

int main() {
    int ss;
    struct sockaddr_in addr_serv,from;
    socklen_t from_len = sizeof(from);

    pthread_t fb_thread;
    int ret = -1;

    ss = socket(AF_INET,SOCK_DGRAM,0);
    if(ss < 0){
        printf("socket init error\n");
        exit(-1);
    }

    memset(&addr_serv,0,sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_serv.sin_port = htons(9003);

    if(bind(ss,(struct sockaddr*)&addr_serv,sizeof(addr_serv)) < 0){
        printf("bind error\n");
        exit(-1);
    }

    ret = pthread_create(&fb_thread,NULL,send_fb,NULL);
    if(ret != 0){
        printf("create thread error\n");
        exit(-1);
    }

    char recv_buf[1500];
    int recv_len;
    int new_frame_counter = 0;
    int old_frame_counter = 0;
    int frame_receive = 0;
    int frame_total;

    while(g_running){
        recv_len = recvfrom(ss,recv_buf,1500,0,(struct sockaddr*)&from,&from_len);
        printf("receive %d byte\n",recv_len);

        packet_header header;
        header.packet_counter = *((u_int32_t *)&recv_buf[0]);
        header.frame_counter =  *((u_int32_t *)&recv_buf[4]);
        header.frame_packet_number = *((u_int32_t *)&recv_buf[8]);
        header.tv_sec = *((long *)&recv_buf[12]);
        header.tv_usec = *((long *)&recv_buf[20]);


        printf("packet_counter %d frame_counter %d frame_packet_number %d\n",
               header.packet_counter,header.frame_counter,header.frame_packet_number);
        printf("time sec %ld time usec %ld\n",header.tv_sec,header.tv_usec);

        new_frame_counter = header.frame_counter;
        if(new_frame_counter == old_frame_counter){
            frame_receive++;
            frame_total = header.frame_packet_number;
        }else{
            printf("receive %d packet\n",frame_receive);
            g_loss_rate = (frame_total-frame_receive)/frame_total*10000;
            frame_receive = 1;
        }

        old_frame_counter = new_frame_counter;
    }

    return 0;
}

void *send_fb(void *arg){
    fb_header header;

    int ss = socket(AF_INET,SOCK_DGRAM,0);
    if(ss < 0){
        printf("socket init error\n");
        exit(-1);
    }

    struct sockaddr_in addr_serv;
    bzero(&addr_serv,sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = inet_addr("172.16.5.200");
    addr_serv.sin_port = htons(9002);

    struct timeval now, begin, delay;
    int counter = 0;
    delay.tv_sec = 0;
    delay.tv_usec = 50*1000;
    gettimeofday(&begin,NULL);

    u_int32_t time_difference;
    char *send_buf[100];
    int send_n = 0;

    while(g_running){
        gettimeofday(&now,NULL);
        time_difference = (now.tv_sec - begin.tv_sec)*1000 + (now.tv_usec - begin.tv_usec)/1000;
//        printf("time difference %d\n",time_difference);
        while(time_difference < 1000*counter){
            select(0,NULL,NULL,NULL,&delay);
            gettimeofday(&now,NULL);
            time_difference = (now.tv_sec - begin.tv_sec)*1000 + (now.tv_usec - begin.tv_usec)/1000;
        }
        header.loss_rate = g_loss_rate;
        header.receiver_rate = 0;

        memcpy(send_buf,&header,FB_HEADER_LENGTH);
        send_n = sendto(ss,send_buf,FB_HEADER_LENGTH,0,(struct sockaddr*)&addr_serv,sizeof(addr_serv));
        if(send_n < 0){
            printf("send fb error");
            exit(-1);
        }
        counter++;
    }
}
