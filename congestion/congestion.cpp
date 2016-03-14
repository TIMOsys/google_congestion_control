//
// Created by sjtu on 16-3-12.
//

#include <sys/types.h>
#include <netinet/in.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/time.h>
#include <string.h>

#include "congestion.h"

extern u_int32_t g_loss_rate;
extern u_int32_t g_length;
extern int g_running;

u_int32_t all_packet_counter = 0;

int send_frame(char *file,int length,int frame_counter,int ss,struct sockaddr_in to_addr){
    FILE * fp = fopen(file,"r");
    if(fp == NULL){
        printf("open file error\n");
        exit(-1);
    }
    char * send_frame = (char *)malloc(sizeof(char)*length);

    fread(send_frame,sizeof(char),length,fp);

    packet_header header;
    struct timeval now_time, send_delay;
    int send_number  = length/UDP_PACKET_LENGTH - 1;
    send_delay.tv_sec = 0;
    send_delay.tv_usec = 0.9*1000*1000/send_number;

    int send_count = 0;
    int send_n = 0;
    char send_buf[UDP_PACKET_LENGTH+PACKET_HEADER_LENGTH];
    for(send_count=0;send_count<send_number;send_count++){
        header.frame_counter = frame_counter;
        gettimeofday(&now_time,NULL);
        header.frame_packet_number = send_number;
        header.tv_sec = now_time.tv_sec;
        header.tv_usec = now_time.tv_usec;

        all_packet_counter++;
        header.packet_counter = all_packet_counter;
        *((u_int32_t *)&send_buf[0]) = header.packet_counter;
        *((u_int32_t *)&send_buf[4]) = header.frame_counter;
        *((u_int32_t *)&send_buf[8]) = header.frame_packet_number;
        *((long *)&send_buf[12]) = header.tv_sec;
        *((long *)&send_buf[20]) = header.tv_usec;

        memcpy(&send_buf[PACKET_HEADER_LENGTH],&send_frame[UDP_PACKET_LENGTH*send_count],UDP_PACKET_LENGTH);

        send_n = sendto(ss,send_buf,UDP_PACKET_LENGTH+PACKET_HEADER_LENGTH,0,
                        (struct sockaddr*)&to_addr,sizeof(to_addr));
        if(send_n < 0){
            printf("send frame error");
            exit(-1);
        }
        select(0,NULL,NULL,NULL,&send_delay);
    }

    fclose(fp);
}

void *receive_fb(void *arg){
    int ss;
    ss = socket(AF_INET,SOCK_DGRAM,0);
    if(ss < 0){
        printf("socket init error\n");
        exit(-1);
    }

    struct sockaddr_in addr_serv,from;
    socklen_t from_len = sizeof(from);

    memset(&addr_serv,0,sizeof(addr_serv));
    addr_serv.sin_family = AF_INET;
    addr_serv.sin_addr.s_addr = htonl(INADDR_ANY);
    addr_serv.sin_port = htons(9002);

    if(bind(ss,(struct sockaddr*)&addr_serv,sizeof(addr_serv)) < 0){
        printf("bind error\n");
        exit(-1);
    }

    char recv_buf[100];
    int recn = 0;

    fb_header header;

    while(g_running){
        recn = recvfrom(ss,recv_buf,100,0,(struct sockaddr*)&from,&from_len);
//        printf("receive %d byte feedback message\n",recn);
        memcpy(&header,recv_buf,8);
        g_loss_rate = header.loss_rate;
        printf("g_loss_rate %d\n",g_loss_rate);
        set_g_length();
    }

}

void set_g_length(){
    //As in kbps
    float As = g_length*8/1000;
    float fl = (float)g_loss_rate/10000.00;

    if(g_loss_rate <10000*0.02){
        As = 1.05*(As+1);
        printf("increase\n");
    }else if(g_loss_rate > 10000*0.1){
        As = As*(1.0-0.5*fl);
        printf("decrease\n");
    }else{
        As = As;
        printf("hold\n");
    }
//    printf("As %f\n",As);
    g_length = As*1000/8;
}