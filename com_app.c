#include <sys/stat.h>
#include <unistd.h>
#include <stdio.h>
#include <stdlib.h>
#include <sys/socket.h>
#include <sys/types.h>
#include <string.h>
#include <asm/types.h>
#include <linux/netlink.h>
#include <linux/socket.h>
#include <errno.h>

#define MAX_PAYLOAD 1024
#define NETLINK_TEST 25
#define L 300

int main(int argc, char *argv[])
{
    //Create a message
    //int isreg[1001]={0};
    int intid;
    char *id = argv[1];
    char *type=argv[2];
    char str1[60] = "Registration. id=" ;
    strcat(str1, id );
    sscanf(str1,"Registration. id=%d",&intid);
    char str2[60] = ", type=";
    strcat(str1, str2);
    strcat(str1, type);
    if(intid<1||intid>1000)
    {
        printf("Fail\n");
        return;
    }
    //printf("---New Registration! id=%s, type=%s---\n", id, type);
    //
    int state, retval;
    int state_smg = 0;
    struct sockaddr_nl src_addr, dest_addr;
    struct nlmsghdr *nlh = NULL;
    struct iovec iov;
    struct msghdr msg;
    int sock_fd;

    // Create a socket
    sock_fd = socket(AF_NETLINK, SOCK_RAW, NETLINK_TEST);
    if(sock_fd == -1)
    {
        printf("error getting socket: %s", strerror(errno));
        return -1;
    }

    // To prepare binding
    memset(&src_addr, 0, sizeof(src_addr));
    src_addr.nl_family = AF_NETLINK;
    src_addr.nl_pid = intid; //設定源端埠號
    src_addr.nl_groups = 0;

    //Bind
    retval = bind(sock_fd, (struct sockaddr*)&src_addr, sizeof(src_addr));
    if (retval < 0)
    {
        printf("Fail\n");
        //printf("bind failed: %s", strerror(errno));
        close(sock_fd);
        return -1;
    }

    // To orepare create mssage
    nlh = (struct nlmsghdr *)malloc(NLMSG_SPACE(MAX_PAYLOAD));
    if (!nlh)
    {
        printf("malloc nlmsghdr error!\n");
        close(sock_fd);
        return -1;
    }

    //
    //

    memset(&dest_addr,0,sizeof(dest_addr));
    dest_addr.nl_family = AF_NETLINK;
    dest_addr.nl_pid = 0; //設定目的埠號
    dest_addr.nl_groups = 0;
    nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
    nlh->nlmsg_pid =intid; //設定源埠                !!!
    nlh->nlmsg_flags = 0;
    strcpy(NLMSG_DATA(nlh), str1); //設定訊息體     !!!
    iov.iov_base = (void *)nlh;
    iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

    //Create mssage
    memset(&msg, 0, sizeof(msg));
    msg.msg_name = (void *)&dest_addr;
    msg.msg_namelen = sizeof(dest_addr);
    msg.msg_iov = &iov;
    msg.msg_iovlen = 1;
//////////////////////////////////////////////////////////////////send message
    //printf("state_smg\n");
    state_smg = sendmsg(sock_fd,&msg,0);
    if (state_smg == -1)
    {
        printf("get error sendmsg = %s\n",strerror(errno));
    }
    memset(nlh, 0, NLMSG_SPACE(MAX_PAYLOAD));

    //receive message
    //printf("waiting received!\n");
    state = recvmsg(sock_fd, &msg, 0);
    if (state < 0)
    {
        printf("state<1");
    }
    printf("%s\n", (char*)NLMSG_DATA(nlh));

    char *back=(char*)NLMSG_DATA(nlh);
    char get[301];
    if(strcmp(back,"Fail")==0)
    {
        close(sock_fd);
        return 0;
    }

    else
    {
        while(1)
        {
            fgets(get,268,stdin);
            strcpy(NLMSG_DATA(nlh), get); //設定訊息體     !!!

            memset(&dest_addr,0,sizeof(dest_addr));
            dest_addr.nl_family = AF_NETLINK;
            dest_addr.nl_pid = 0; //設定目的埠號
            dest_addr.nl_groups = 0;
            nlh->nlmsg_len = NLMSG_SPACE(MAX_PAYLOAD);
            nlh->nlmsg_pid = intid; //設定源埠                !!!
            nlh->nlmsg_flags = 0;
            iov.iov_base = (void *)nlh;
            iov.iov_len = NLMSG_SPACE(MAX_PAYLOAD);

            //Create mssage
            memset(&msg, 0, sizeof(msg));
            msg.msg_name = (void *)&dest_addr;
            msg.msg_namelen = sizeof(dest_addr);
            msg.msg_iov = &iov;
            msg.msg_iovlen = 1;

            state_smg = sendmsg(sock_fd,&msg,0);
            state = recvmsg(sock_fd, &msg, 0);
            printf("%s", (char*)NLMSG_DATA(nlh));
            //printf("Received message: %s\n", (char*)NLMSG_DATA(nlh));
            //while(getchar() !='\n');


        }
    }
    close(sock_fd);
    return 0;
}
/*
 //
*/