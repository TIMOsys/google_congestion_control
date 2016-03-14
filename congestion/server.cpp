#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <strings.h>
#include <string.h>
#include <sys/time.h>

#include "congestion.h"
using namespace std;

int g_running = 1;
u_int32_t g_loss_rate = 0;
u_int32_t g_length = 0;

int main() {
    char *video = "/home/sjtu/movie/video.mp4";
    struct timeval now, begin, delay;
    int frame_counter = 0;

    int ss = socket(AF_INET,SOCK_DGRAM,0);
    if(ss < 0){
        printf("socket init error\n");
        exit(0);
    }

    struct sockaddr_in addr_clie;
    bzero(&addr_clie,sizeof(addr_clie));
    addr_clie.sin_family = AF_INET;
    addr_clie.sin_port = htons(9003);
    addr_clie.sin_addr.s_addr = inet_addr("172.16.5.139");
//    addr_clie.sin_addr.s_addr = inet_addr("172.16.5.200");


    u_int32_t time_difference;
    gettimeofday(&begin,NULL);
    delay.tv_sec = 0;
    delay.tv_usec = 50*1000;
    g_length = 400*1000;

    pthread_t receive_thread;
    int ret = -1;
    ret = pthread_create(&receive_thread,NULL,receive_fb,NULL);
    if(ret < 0){
        printf("create receive thread error\n");
        exit(-1);
    }

    while(g_running){
        gettimeofday(&now,NULL);
        time_difference = (now.tv_sec - begin.tv_sec)*1000 + (now.tv_usec - begin.tv_usec)/1000;
//        printf("time difference %d\n",time_difference);
        while(time_difference < 1000*frame_counter){
            select(0,NULL,NULL,NULL,&delay);
            gettimeofday(&now,NULL);
            time_difference = (now.tv_sec - begin.tv_sec)*1000 + (now.tv_usec - begin.tv_usec)/1000;
        }
        printf("the %d cycle\n",frame_counter);
        if(g_length < 1000*1000){
            printf("g_length %d KB\n",g_length/1000);
        }else{
            int length_tmp = g_length;
            printf("g_length %.2f MB\n",(float)length_tmp/(1000*1000));
        }
        send_frame(video,g_length,frame_counter,ss,addr_clie);
        frame_counter++;
    }

    return 0;
}