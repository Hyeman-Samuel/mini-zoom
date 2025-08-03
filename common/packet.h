#ifndef PROTOCOL_H
#define PROTOCOL_H

#define MSG_TYPE_CHAT   1
#define MSG_TYPE_FILE   2
#define MSG_TYPE_AUDIO  3
#define MSG_TYPE_VIDEO  4


typedef struct {
    int type;          
    int length;
    char data[1024];
} Packet;

#endif
