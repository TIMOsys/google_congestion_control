//
// Created by sjtu on 16-3-12.
//

#ifndef CONGESTION_CONGESTION_H
#define CONGESTION_CONGESTION_H

#define UDP_PACKET_LENGTH 1400
#define PACKET_HEADER_LENGTH 28

typedef struct packet_header{
    u_int32_t packet_counter;
    u_int32_t frame_counter;
    u_int32_t frame_packet_number;
    long tv_sec;
    long tv_usec;
}packet_header;

typedef struct fb_header{
    u_int32_t loss_rate;
    u_int32_t receiver_rate;
}fb_header;

int send_frame(char *file,int length,int frame_counter,int ss,struct sockaddr_in to_addr);
void *receive_fb(void *arg);
void set_g_length();
#endif //CONGESTION_CONGESTION_H
