//
// Created by sjtu on 16-3-12.
//

#ifndef CONGESTION_CLIENT_CONGESTION_H
#define CONGESTION_CLIENT_CONGESTION_H

#define FB_HEADER_LENGTH 8

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

#define UDP_PACKET_LENGTH 1400
#define PACKET_HEADER 24

#endif //CONGESTION_CLIENT_CONGESTION_H
