#include <linux/init.h>
#include <linux/module.h>
#include <linux/timer.h>
#include <linux/time.h>
#include <linux/types.h>
#include <net/sock.h>
#include <net/netlink.h>
#include <linux/string.h>
#include "com_kmodule.h"

#define NETLINK_TEST 25
#define MAX_MSGSIZE 1024

struct sock *nl_sk = NULL;

int isreg[1001]= {0};
struct mailbox pig[1001];

void send_msg(char *str, int pid)
{
    printk(KERN_INFO "Send received:%s\n", str);
    char msg[258]="";
    struct sk_buff *skb;
    struct nlmsghdr *nlh;
    int len = NLMSG_SPACE(MAX_MSGSIZE);
    ////////////////////////////////////抓id跟type
    int id=pid;//port
    char four[5],type[10],word[260]="";
    sscanf(str, "%4s", four);
    if(strcmp(four,"Regi")==0)
    {
        sscanf(str,"Registration. id=%d, type=%s", &id, type);
        //sscanf(str,"Registration. id=%s, type=%s", idstr, type);
        //printk(KERN_INFO "id:%d type:%s\n", id, type);
        if(id<1||id>1000)
        {
            strcpy(msg,"Fail");
            return;
        }
        else if(strlen(type)==6)
        {
            if((strcmp(type,"queued")==0)&&(isreg[id]==0))
            {
                strcpy(msg,"Success");
                isreg[id]=1;
                pig[id].msg_data_head = kmalloc(sizeof(struct msg_data),0);
                pig[id].msg_data_tail = kmalloc(sizeof(struct msg_data),0);
                pig[id].msg_data_count = 0;
                pig[id].type = '1';
            }
            else
                strcpy(msg,"Fail");
        }
        else if(strlen(type)==8)
        {
            if((strcmp(type,"unqueued")==0)&&(isreg[id]==0))
            {
                strcpy(msg,"Success");
                isreg[id]=1;
                pig[id].msg_data_head = kmalloc(sizeof(struct msg_data),0);
                pig[id].msg_data_tail = kmalloc(sizeof(struct msg_data),0);
                pig[id].msg_data_count = 0;
                pig[id].type = '0';
            }
            else
                strcpy(msg,"Fail");
        }
        else
        {
            strcpy(msg,"Fail");
        }

    }
    else if(strcmp(four,"Send")==0)
    {
        char delim[]=" ";
        char *token;
        char *token2;
        char idstr[10];
        int id;
        printk(KERN_INFO "1.before str:%s\n", str);
        token=strsep(&str,delim);
        printk(KERN_INFO "2.after one str:%s\n", str);
        token2=strsep(&str,delim);
        printk(KERN_INFO "  excp: token2=%s\n", token2);
        printk(KERN_INFO "3.after two str:%s\n", str);


        strcpy(idstr,token2);
        sscanf(idstr,"%d",&id);
        //printk(KERN_INFO "4.is:%d\n", id);
        int i;
        for(i=0; i<strlen(str)-1; i++)
        {
            word[i]=str[i];
            printk(KERN_INFO "word[%d]=:%c\n", i, word[i]);
        }
        printk(KERN_INFO "strlen(str)=:%d\n", strlen(str));
        printk(KERN_INFO "strlen(word)=:%d\n", strlen(word));
        /*
        char pi[258]="";
        sscanf(str,"Send %s %258s", idstr, send);
        printk(KERN_INFO "strlen(str)=%d\n",strlen(str))
        int i;
        pi[0]=str[6+strlen(idstr)];
        for(i=0; i<strlen(str)-4-2-strlen(idstr); i++)
        {
            word[i]=str[6+strlen(idstr)+i];
            printk(KERN_INFO "word[%d]=str[%d]=%c\n",i,7+i+strlen(idstr),str[4+1+strlen(idstr)+1+i+1]);
        }

            printk(KERN_INFO "before trim ,strlen(word)=%d\n",strlen(word));
        for(i=strlen(str)-4-2-strlen(idstr);i<strlen(word);i++){
            word[i]="";
            printk(KERN_INFO "TTT word[%d]=""\n",i);
        }
            printk(KERN_INFO "after trim ,strlen(word)=%d\n",strlen(word));
        */

        if(strlen(word)==255)
        {
            printk(KERN_INFO "255 ,word=%s\n",word);
        }
        //printk(KERN_INFO "@@word=%s\n",word);

        if(id<1||id>1000)
        {
            strcpy(msg,"Fail\n");
        }
        else if(isreg[id]==0)
        {
            strcpy(msg,"Fail\n");
            printk(KERN_INFO "isreg[id]==0\n");
        }
        else if(strlen(word)>255)
        {
            strcpy(msg,"Fail\n");
            printk(KERN_INFO ">255 ,word=%s\n",word);
        }
        else
        {
            struct msg_data *now;
            now=kmalloc(sizeof(struct msg_data),0);
            if(pig[id].type=='0')//unqueued 刷新
            {
                if(pig[id].msg_data_count==0)
                {
                    strcpy(pig[id].msg_data_head->buf,word);
                    pig[id].msg_data_head->next=NULL;
                    pig[id].msg_data_count=1;
                    strcpy(msg,"Success\n");
                }
                if(pig[id].msg_data_count==1)
                {
                    strcpy(pig[id].msg_data_head->buf,word);
                    strcpy(msg,"Success\n");
                }
            }
            else if(pig[id].type=='1')//queued linked list
            {
                if(pig[id].msg_data_count==0)//0 create
                {
                    strcpy(pig[id].msg_data_head->buf,word);
                    strcpy(pig[id].msg_data_tail->buf,word);
                    pig[id].msg_data_count++;
                    pig[id].msg_data_head->next=NULL;
                    pig[id].msg_data_tail->next=NULL;
                    strcpy(msg,"Success\n");
                }
                else if(pig[id].msg_data_count==1)
                {

                    pig[id].msg_data_count++;
                    strcpy(now->buf,word);
                    pig[id].msg_data_tail=now;
                    pig[id].msg_data_head->next=pig[id].msg_data_tail;
                    pig[id].msg_data_tail->next=NULL;
                    now=NULL;
                    strcpy(msg,"Success\n");
                }
                else if(pig[id].msg_data_count==2)
                {
                    pig[id].msg_data_count++;
                    strcpy(now->buf,word);
                    pig[id].msg_data_tail->next=now;
                    pig[id].msg_data_tail=pig[id].msg_data_tail->next;
                    pig[id].msg_data_tail->next=NULL;
                    now=NULL;
                    strcpy(msg,"Success\n");
                }
                else if(pig[id].msg_data_count==3)
                {
                    strcpy(msg,"Fail\n");
                }

            }
        }
    }
    else if(strcmp(four,"Recv")==0)
    {
        if(pig[pid].type=='0')
        {
            if(pig[pid].msg_data_count==0)
            {
                strcpy(msg,"Fail\n");
                //printk(KERN_INFO "Recv unqueued\n");
            }
            else
            {
                strcpy(msg,pig[pid].msg_data_head->buf);
                if(msg[strlen(msg)-1]!='\n')
                    strcat(msg,"\n");
            }
        }
        else if(pig[pid].type=='1')
        {
            if(pig[pid].msg_data_count==0)
            {
                strcpy(msg,"Fail\n");
                //printk(KERN_INFO "Empty queued\n");
            }
            else
            {
                //printk(KERN_INFO "Recv queued\n");
                strcpy(msg,pig[pid].msg_data_head->buf);
                if(msg[strlen(msg)-1]!='\n')
                    strcat(msg,"\n");
                pig[pid].msg_data_head=pig[pid].msg_data_head->next;
                pig[pid].msg_data_count--;
                if(pig[pid].msg_data_count==0)
                {
                    pig[id].msg_data_head = kmalloc(sizeof(struct msg_data),0);
                }
            }
        }
    }
    ////////////////////////////////////////////////
    if (!str || !nl_sk)
    {
        return;
    }
    //printk(KERN_INFO "pid:%d\n", pid);
    /////////////////////////////////////
    skb = alloc_skb(len, GFP_KERNEL);
    if (!skb)
    {
        printk(KERN_ERR "send_msg:alloc_skb error\n");
        return;
    }
    nlh = nlmsg_put(skb, 0, 0, 0, MAX_MSGSIZE, 0);
    NETLINK_CB(skb).portid = 0;
    NETLINK_CB(skb).dst_group = 0;
    /////////////////////////////////////
    strcpy(NLMSG_DATA(nlh), msg);
    printk(KERN_INFO "my_net_link:send message '%s'.\n\n",(char *)NLMSG_DATA(nlh));
    netlink_unicast(nl_sk, skb, pid, MSG_DONTWAIT);
}

void recv_nlmsg(struct sk_buff *skb)
{
    int pid;
    struct nlmsghdr *nlh = nlmsg_hdr(skb);
    /////////////////////////////////////
    if (nlh->nlmsg_len < NLMSG_HDRLEN || skb->len < nlh->nlmsg_len)
        return;
    //printk(KERN_INFO "Message received:%s\n", (char*)NLMSG_DATA(nlh));
    char* str=(char*)NLMSG_DATA(nlh);
    pid = nlh->nlmsg_pid;
    /////////////////////////////////////Registration. id=2, type=queued

    send_msg(str, pid);
    //printk(KERN_INFO "Send!!\n");
}

struct netlink_kernel_cfg nl_kernel_cfg =
{
    .groups = 0,
    .flags = 0,
    .input = recv_nlmsg,
    .cb_mutex = NULL,
    .bind = NULL,
    .compare = NULL,
};

int netlink_init(void)
{
    nl_sk = netlink_kernel_create(&init_net, NETLINK_TEST, &nl_kernel_cfg);
    if (!nl_sk)
    {
        printk(KERN_ERR "my_net_link: create netlink socket error.\n");
        return 1;
    }
    printk(KERN_INFO "netlink_init: create netlink socket ok.\n");
    return 0;
}

static void netlink_exit(void)
{
    if (nl_sk != NULL)
        sock_release(nl_sk->sk_socket);

    printk(KERN_INFO "my_net_link: self module exited\n");
}


module_init(netlink_init);
module_exit(netlink_exit);

/*
module_init(com_kmodule_init);

module_exit(com_kmodule_exit);
*/