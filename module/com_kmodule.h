#ifndef COM_KMODULE_H
#define COM_KMODULE_H
#include <linux/module.h>
struct mailbox
{
    //0: unqueued
    //1: queued
    unsigned char type;
    unsigned char msg_data_count;
    struct msg_data *msg_data_head;
    struct msg_data *msg_data_tail;
};

struct msg_data
{
    char buf[256];
    struct msg_data *next;
};

MODULE_LICENSE("GPL");
MODULE_AUTHOR("Apple pie");
MODULE_DESCRIPTION("A Simple Hello World module");

#endif  //ifndef COM_KMODULE_H
