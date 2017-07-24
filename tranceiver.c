#include <stdio.h>
#include <stdlib.h>
#include <stdint.h>
#include <ctype.h>
#include <string.h>
#include "packets.h"
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <linux/ip.h>
#include <linux/udp.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <sys/poll.h>
#include <unistd.h>

#define hello 1

#define BCAST_MAC0	0xFF
#define BCAST_MAC1	0xFF
#define BCAST_MAC2	0xFF
#define BCAST_MAC3	0xFF
#define BCAST_MAC4	0xFF
#define BCAST_MAC5	0xFF
#define DEST_MAC0	0xD8
#define DEST_MAC1	0xCB
#define DEST_MAC2	0x8A
#define DEST_MAC3	0x84
#define DEST_MAC4	0x29
#define DEST_MAC5	0x94
#define ETHER_TYPE	0x1234
#define DEFAULT_IF	"enp3s0"
#define BUF_SIZ		1024
#define HEAD_SIZE		14
#define MY_NAME "cihan"
#define MY_SURNAME "alp"
#define TARGET_NAME "okan"
#define TARGET_SURNAME "erdogan"
#define MAX_NAME_SIZE 10
#define MAX_MESSAGE_SIZE 10
#define LENGTH 2
#define MAC_SIZE 6
#define BEGIN_NAME 1
#define END_NAME 10
#define BEGIN_SURNAME 11
#define END_SURNAME 20
static uint8_t bcast[sizeof(struct query_bcast)];
static uint8_t ucast[sizeof(struct query_ucast)];
static uint8_t hresp[sizeof(struct hello_resp)];
static uint8_t chatting [sizeof(struct chat)];
static uint8_t chata[sizeof(struct chat_ack)];
static uint8_t ex[sizeof(struct exiting)];
static unsigned char dmac[MAC_SIZE];

unsigned char my_mac[] = {0xd8, 0xcb, 0x8a, 0x84, 0x29, 0x94};


//static unsigned char smac[MAC_SIZE];
//static unsigned char srcmac[MAC_SIZE];
static uint8_t type;
int k;

int main(int argc, unsigned char *argv[])

{
    //printf("sldkşalgkşlfdkglş");

    int sockfd,ret;
    int sockopt;
    struct ifreq if_idx;
    struct ifreq if_mac;
    struct ifreq ifopts;
    struct sockaddr_storage their_addr;
    int tx_len = 0;
    int i,j;
    unsigned char sendbuf[BUF_SIZ];
    struct ether_header *eh = (struct ether_header *) sendbuf;
    struct sockaddr_ll socket_address;
    struct pollfd ufds[1]; /*******POLL************/
    unsigned char ifName[IFNAMSIZ];
    unsigned char type;
    unsigned char *target_name = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);
    unsigned char *target_surname = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);
    unsigned char *smac = malloc(sizeof(unsigned char)*MAC_SIZE);
    unsigned char *srcmac = malloc(sizeof(unsigned char)*MAC_SIZE);

    uint8_t mac0;
    uint8_t mac1;
    uint8_t mac2;
    uint8_t mac3;
    uint8_t mac4;
    uint8_t mac5;
    uint8_t buf[BUF_SIZ];
    uint8_t data[BUF_SIZ-HEAD_SIZE];
    ssize_t numbytes;

    struct query_bcast query_bcast;
    struct query_ucast query_ucast;
    struct hello_resp hello_resp;
    struct chat chat;
    struct chat_ack chat_ack;
    struct exiting exiting;

    struct query_bcast *decoded_query_bcast;
    struct query_ucast *decoded_query_ucast;
    struct hello_resp  *decoded_hello_resp;
    struct chat        *decoded_chat;
    struct chat_ack    *decoded_chat_ack;
    struct exiting     *decoded_exiting;


    unsigned char *packet_bcast    = malloc(sizeof(query_bcast)) ;
    unsigned char *packet_ucast    = malloc(sizeof(query_ucast)) ;
    unsigned char *packet_hello    = malloc(sizeof(hello_resp)) ;
    unsigned char *packet_chat     = malloc(sizeof(query_bcast)) ;
    unsigned char *packet_chat_ack = malloc(sizeof(query_ucast)) ;
    unsigned char *packet_exit     = malloc(sizeof(hello_resp)) ;

    int rv;




    memset(&query_bcast, 0, sizeof(struct query_bcast));
    memset(&query_ucast, 0, sizeof(struct query_ucast));
    memset(&hello_resp, 0, sizeof(struct hello_resp));


    strcpy(ifName, DEFAULT_IF);
    memset(sendbuf, 0, BUF_SIZ);



    if ((sockfd = socket(PF_PACKET, SOCK_RAW, htons(ETHER_TYPE))) == -1) {
        perror("socket");
    }


    /* Get the index of the interface to send on */
    memset(&if_idx, 0, sizeof(struct ifreq));
    strncpy(if_idx.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFINDEX, &if_idx) < 0)
        perror("SIOCGIFINDEX");
    /* Get the MAC address of the interface to send on */
    memset(&if_mac, 0, sizeof(struct ifreq));
    strncpy(if_mac.ifr_name, ifName, IFNAMSIZ-1);
    if (ioctl(sockfd, SIOCGIFHWADDR, &if_mac) < 0)
        perror("SIOCGIFHWADDR");

    /* Set interface to promiscuous mode - do we need to do this every time? */
    strncpy(ifopts.ifr_name, ifName, IFNAMSIZ-1);
  	ioctl(sockfd, SIOCGIFFLAGS, &ifopts);
  	ifopts.ifr_flags |= IFF_PROMISC;
  	ioctl(sockfd, SIOCSIFFLAGS, &ifopts);

    /* Allow the socket to be reused - incase connection is closed prematurely */
  	if (setsockopt(sockfd, SOL_SOCKET, SO_REUSEADDR, &sockopt, sizeof sockopt) == -1) {
  		perror("setsockopt");
  		close(sockfd);
  		exit(EXIT_FAILURE);
  	}
  	/* Bind to device */
  	if (setsockopt(sockfd, SOL_SOCKET, SO_BINDTODEVICE, ifName, IFNAMSIZ-1) == -1)	{
  		perror("SO_BINDTODEVICE");
  		close(sockfd);
  		exit(EXIT_FAILURE);
  	}
    /*eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
    eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
    eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
    eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
    eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
    eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
    eh->ether_dhost[0] = BCAST_MAC0;
    eh->ether_dhost[1] = BCAST_MAC1;
    eh->ether_dhost[2] = BCAST_MAC2;
    eh->ether_dhost[3] = BCAST_MAC3;
    eh->ether_dhost[4] = BCAST_MAC4;
    eh->ether_dhost[5] = BCAST_MAC5;
    eh->ether_type = htons(ETHER_TYPE);
    tx_len += sizeof(struct ether_header);*/


    while(1){
        tx_len = 0;
        ufds[0].fd = sockfd;
        ufds[0].events = POLLIN;
        rv = poll(ufds, 1, 5000);
        if (rv == -1) {
            perror("poll"); // error occurred in poll()
        }
        else if (rv == 0) {
            //printf("Timeout occurred!  No data after 0.1 mseconds.\n");
        }
        else {
        // check for events on socket:
            if (ufds[0].revents & POLLIN) {
                numbytes = recvfrom(sockfd, buf, BUF_SIZ, 0, NULL, NULL); // receive normal data
                //printf("%s\n", buf);
                //printf("%s\n", buf);

                if (    eh->ether_dhost[0] == DEST_MAC0 || BCAST_MAC0 &&
              			eh->ether_dhost[1] == DEST_MAC1 || BCAST_MAC1 &&
              			eh->ether_dhost[2] == DEST_MAC2 || BCAST_MAC2 &&
              			eh->ether_dhost[3] == DEST_MAC3 || BCAST_MAC3 &&
              			eh->ether_dhost[4] == DEST_MAC4 || BCAST_MAC4 &&
              			eh->ether_dhost[5] == DEST_MAC5 || BCAST_MAC5) {
              		//printf("Correct destination MAC address\n");
              	} else {
              		printf("Wrong destination MAC: %x:%x:%x:%x:%x:%x\n",
              						eh->ether_dhost[0],
              						eh->ether_dhost[1],
              						eh->ether_dhost[2],
              						eh->ether_dhost[3],
              						eh->ether_dhost[4],
              						eh->ether_dhost[5]);
              		continue;
                }

                printf("-------------------------------------\n");
              	printf("Dest MAC: ");

              	for (i=0; i<numbytes; i++) {
                    if(i < 6){
                        printf("%02x:", buf[i]);
                        dmac[i] = buf[i];
                    }
                    else if(i==6)
                        printf("\nSrc MAC: " );
                    if(i >= 6 && i < HEAD_SIZE-2){

                        smac[i-6] = buf[i];

                        printf("%02x:", buf[i] );

                  }


                }


                printf("\n");
                eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
                eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
                eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
                eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
                eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
                eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];
                eh->ether_dhost[0] = BCAST_MAC0;
                eh->ether_dhost[1] = BCAST_MAC1;
                eh->ether_dhost[2] = BCAST_MAC2;
                eh->ether_dhost[3] = BCAST_MAC3;
                eh->ether_dhost[4] = BCAST_MAC4;
                eh->ether_dhost[5] = BCAST_MAC5;
                eh->ether_type = htons(ETHER_TYPE);
                tx_len += sizeof(struct ether_header);




                socket_address.sll_ifindex = if_idx.ifr_ifindex;
                /* Address length*/
                socket_address.sll_halen = ETH_ALEN;
                /* Destination MAC */
                socket_address.sll_addr[0] = smac[0];
                socket_address.sll_addr[1] = smac[1];
                socket_address.sll_addr[2] = smac[2];
                socket_address.sll_addr[3] = smac[3];
                socket_address.sll_addr[4] = smac[4];
                socket_address.sll_addr[5] = smac[5];

                struct ether_header *eth;
                printf("\n");
                type = buf[HEAD_SIZE];
                printf("TYPE CHECK---%d\n", type);
                switch (type) {
                    case QUERY_BROADCAST:

                        //eth = (struct ether_header *) &buf[0];
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          bcast[i-HEAD_SIZE] = buf[i];

                        }
                        printf("Decoding Broadcast Query\n");
                        decode_bcast();
                        decoded_query_bcast = (struct query_bcast*) bcast;

                        printf("%s\n",decoded_query_bcast->requester_name);
                        write_mac_to_file(smac,
                            decoded_query_bcast->requester_name,
                            decoded_query_bcast->requester_surname
                        );


                        printf("Sending Hello Packet to Broadcast Query..\n");

                        fill_hello_resp(&hello_resp,
                            decoded_query_bcast->requester_name,
                            decoded_query_bcast->requester_surname
                        );

                        packet_hello = hex_print((void *) &hello_resp, sizeof(struct hello_resp));
                        for(j = 0; j < (sizeof(hello_resp)); j++){
                          sendbuf[14+j] = packet_hello[j];
                          tx_len++;
                        }

                        if (
                            sendto(sockfd,
                            sendbuf, tx_len,
                            0,
                            (struct sockaddr*)&socket_address,
                            sizeof(struct sockaddr_ll)) < 0
                        )
                            printf("Send failed\n");
                        break;


                    case QUERY_UNICAST:
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          ucast[i-HEAD_SIZE] = buf[i];
                        }
                        decoded_query_ucast = (struct query_ucast*) ucast;

                        if(
                            strcmp(decoded_query_ucast->target_name, (unsigned char*)MY_NAME)
                            &&
                            strcmp(decoded_query_ucast->target_name,(unsigned char*)MY_SURNAME)
                        ){
                            printf("a Unicast packet received that doesnt belong to me\n");
                            break;
                        }
                        printf("Decoding Unicast Query\n");

                        decode_ucast();
                        printf("Sending Hello Packet to Unicast Query..\n");

                        fill_hello_resp(&hello_resp,
                            decoded_query_ucast->requester_name,
                            decoded_query_ucast->requester_surname
                        );

                        packet_hello = hex_print((void *) &hello_resp, sizeof(struct hello_resp));
                        for(j = 0; j < (sizeof(hello_resp)); j++){
                          sendbuf[14+j] = packet_hello[j];
                          tx_len++;
                        }

                        write_mac_to_file(smac,
                            decoded_query_ucast->requester_name,
                            decoded_query_ucast->requester_surname
                        );
                        printf("%s\n",decoded_query_ucast->requester_name );

                        if (
                            sendto(sockfd,
                            sendbuf, tx_len,
                            0,
                            (struct sockaddr*)&socket_address,
                            sizeof(struct sockaddr_ll)) < 0
                        )

                            printf("Send failed\n");

                        break;


                    case HELLO_RESPONSE:
                        printf("HELLO_RESPONSE has been received:\n");
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          hresp[i-HEAD_SIZE] = buf[i];
                        }
                        decode_hello_resp();
                        decoded_hello_resp = (struct hello_resp*) hresp;

                        write_mac_to_file(smac,
                            decoded_hello_resp->responder_name,
                            decoded_hello_resp->responder_surname
                        );
                        break;

                    case CHAT:
                        printf("You have a MESSAGE!!!!!!:\n");
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          chatting[i-HEAD_SIZE] = buf[i];
                        }
                        decode_chat();
                        decoded_chat = (struct chat*) chatting;
                        printf("Sending Chat ACK to CHAT..\n");

                        fill_chat_ack(&chat_ack);

                        packet_chat_ack = hex_print((void *) &chat_ack, sizeof(struct chat_ack));
                        for(j = 0; j < (sizeof(chat_ack)); j++){
                          sendbuf[14+j] = packet_chat_ack[j];
                          tx_len++;
                        }
                        eh->ether_dhost[0] = smac[0];
                        printf("%02x\n", smac[0]);
                        eh->ether_dhost[1] = smac[1];
                        eh->ether_dhost[2] = smac[2];
                        eh->ether_dhost[3] = smac[3];
                        eh->ether_dhost[4] = smac[4];
                        eh->ether_dhost[5] = smac[5];

                        if (
                            sendto(sockfd,
                            sendbuf, tx_len,
                            0,
                            (struct sockaddr*)&socket_address,
                            sizeof(struct sockaddr_ll)) < 0
                        )
                        printf("Send failed\n");
                        break;


                    case CHAT_ACK:

                        printf("CHAT_ACK has been received:\n");
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          chata[i-HEAD_SIZE] = buf[i];
                        }
                        decode_chat_ack();
                        break;


                    case EXITING:
                        printf("EXITING a USER:\n");
                        for (i = HEAD_SIZE; i<numbytes; i++) {
                          ex[i-HEAD_SIZE] = buf[i];
                        }
                        decode_exiting();
                        break;
                    default: printf("Default Value has been received\n");
                        break;
                }
            }
        }
    }
    close(sockfd);
  	return 0;
}



static void fill_query_bcast(struct query_bcast *q)
{
    q->type = QUERY_BROADCAST;
    snprintf(q->requester_name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->requester_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
}

/**
 * @brief
 *
 * @param[out] q
 */
static void fill_query_ucast(struct query_ucast *q, unsigned char *target_name, unsigned char *target_surname)
{
    q->type = QUERY_UNICAST;
    snprintf(q->requester_name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->requester_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
    snprintf(q->target_name, MAX_NAME_SIZE, "%s", TARGET_NAME);
    snprintf(q->target_surname, MAX_NAME_SIZE, "%s", TARGET_SURNAME);
}
/**
 * @brief
 *
 * @param[out] q
 */
static void fill_hello_resp(struct hello_resp *q, unsigned char *target_name, unsigned char *target_surname)
{
    q->type = HELLO_RESPONSE;
    snprintf(q->responder_name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->responder_surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
    snprintf(q->queryier_name, MAX_NAME_SIZE, "%s", target_name);
    snprintf(q->queryier_surname, MAX_NAME_SIZE, "%s", target_surname);
}

static void fill_chat(struct chat *q, unsigned char *message)
{
    q->type = CHAT;
    snprintf(q->length, LENGTH, "%s", (unsigned char *)strlen(message));
    q->packet_id = 0;
    snprintf(q->message, MAX_MESSAGE_SIZE, "%s", message);
}

static void fill_chat_ack(struct chat_ack *q)
{
    q->type = CHAT_ACK;
    q->packet_id = 0;
}

static void fill_exiting(struct exiting *q)
{
    q->type = EXITING;
    snprintf(q->name, MAX_NAME_SIZE, "%s", MY_NAME);
    snprintf(q->surname, MAX_NAME_SIZE, "%s", MY_SURNAME);
}



/**
 * @brief generic print of struct in hexadecimal
 * format
 *
 * @param[in] pack
 * @param[in] len
 */
static unsigned char *hex_print(void *pack, size_t len)
{
    int i = 0;
    unsigned char *arr = malloc(sizeof(unsigned char)*len);
    for (i = 0; i < len; i++) {
        arr[i] = ((uint8_t *) pack)[i];

    }
    return arr;
}

static void decode_bcast()
{
    struct query_bcast *q;
    q = (struct query_bcast*) bcast;

    fprintf(stdout, "q->type: %d\n", q->type);
    fprintf(stdout, "q->requester_name: %s\n", q->requester_name);
    fprintf(stdout, "q->requester_surname: %s\n", q->requester_surname);
}
static void decode_ucast()
{
      struct query_ucast *q;
      q = (struct query_ucast*) ucast;

      fprintf(stdout, "q->type: %d\n", q->type);

      fprintf(stdout, "q->requester_name: %s\n", q->requester_name);
      fprintf(stdout, "q->requester_surname: %s\n", q->requester_surname);

      fprintf(stdout, "q->target_name: %s\n", q->target_name);
      fprintf(stdout, "q->target_surname: %s\n", q->target_surname);
      printf("--------------------------------------\n");
}

static void decode_hello_resp()
{
      struct hello_resp *q;
      q = (struct hello_resp*) hresp;

      fprintf(stdout, "q->type: %d\n", q->type);

      fprintf(stdout, "q->responder_name: %s\n", q->responder_name);
      fprintf(stdout, "q->responder_surname: %s\n", q->responder_surname);

      fprintf(stdout, "q->queryier_name: %s\n", q->queryier_name);
      fprintf(stdout, "q->queryier_surname: %s\n", q->queryier_surname);
      printf("--------------------------------------\n");
}

static void decode_chat()
{
      struct chat *q;
      q = (struct chat*) chatting;

      fprintf(stdout, "q->type: %d\n", q->type);

      fprintf(stdout, "q->length: %s\n", q->length);
      fprintf(stdout, "q->packet_id: %d\n", q->packet_id);
      fprintf(stdout, "q->message: %s\n", q->message);


      printf("--------------------------------------\n");
}

static void decode_chat_ack()
{
      struct chat_ack *q;
      q = (struct chat_ack*) chata;

      fprintf(stdout, "q->type: %d\n", q->type);
      fprintf(stdout, "q->packet_id: %d\n", q->packet_id);

      printf("--------------------------------------\n");
}

static void decode_exiting()
{
      struct exiting *q;
      q = (struct exiting*) ex;

      fprintf(stdout, "q->type: %d\n", q->type);
      fprintf(stdout, "q->name: %s\n", q->name);
      fprintf(stdout, "q->surname: %s\n", q->surname);

      printf("--------------------------------------\n");
}

static int write_mac_to_file(
    unsigned char *smac,
    unsigned char *name,
    unsigned char *surname)
{

    int k, check = 1;
    FILE *fp;
    fp = fopen("mac.txt","a+");

    unsigned char *mac = malloc(sizeof(unsigned char)*6);
    unsigned char *macc = malloc(sizeof(unsigned char)*6);
    unsigned char *f_name = malloc(sizeof(unsigned char)*10);
    unsigned char *f_surname = malloc(sizeof(unsigned char)*10);

    while(!feof(fp)){


        fscanf(fp, "%s %s %s\n", mac, f_name, f_surname);
            //printf("%s\n", mac);

        macc=hex_to_string(mac);
        //printf("%s %s %s\n", mac, f_name, f_surname);
        check = strcmp(smac, macc);
        //printf("%d\n", check);
        if(check == 0){
            break;
        }
    }


    if(check != 0){
        for(k = 0; k < 6; k++){
            fprintf(fp, "%02x", smac[k]);
        }
        fprintf(fp, " %s %s\n", name, surname);
    }
    else return 0;

    fclose(fp);
}



unsigned char *hex_to_string(unsigned char *string)
{
    unsigned char *pos = string;
    unsigned char *val = malloc(sizeof(string)*10);
    size_t count = 0;
    for(count = 0; count <= sizeof(val)/sizeof(val[0]); count++) {
        sscanf(pos, "%2hhx", &val[count]);
        pos += 2;
    }
    return val;
}
