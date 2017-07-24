#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <sys/socket.h>
#include <arpa/inet.h>
#include <arpa/inet.h>
#include <linux/if_packet.h>
#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <sys/ioctl.h>
#include <sys/socket.h>
#include <net/if.h>
#include <netinet/ether.h>
#include <stdint.h>

#include "packets.h"

#define BCAST_MAC0	0xFF
#define BCAST_MAC1	0xFF
#define BCAST_MAC2	0xFF
#define BCAST_MAC3	0xFF
#define BCAST_MAC4	0xFF
#define BCAST_MAC5	0xFF
#define ETHER_TYPE 0x1234
#define DEFAULT_IF	"enp3s0"
#define BUF_SIZ	1024
#define MAX_NAME_SIZE 10
#define MY_NAME "cihan"
#define MY_SURNAME "alp"
#define TARGET_NAME "okan"
#define TARGET_SURNAME "erdogan"
#define MAX_MESSAGE_SIZE 256
#define MAX_MAC_SIZE 6
#define LENGTH 2




int main(int argc, unsigned char *argv[])

{
  int sockfd;
  struct ifreq if_idx;
  struct ifreq if_mac;
  int tx_len = 0;
  int i,j;
  unsigned char sendbuf[BUF_SIZ];
  struct ether_header *eh = (struct ether_header *) sendbuf;
  struct sockaddr_ll socket_address;
  unsigned char ifName[IFNAMSIZ];
  unsigned char type;
  unsigned char *target_name = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);
  unsigned char *target_surname = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);
  unsigned char *message = malloc(sizeof(unsigned char)*MAX_MESSAGE_SIZE);
  unsigned char *to_mac = malloc(sizeof(unsigned char)*MAX_MAC_SIZE);

  uint8_t mac0;
  uint8_t mac1;
  uint8_t mac2;
  uint8_t mac3;
  uint8_t mac4;
  uint8_t mac5;




  struct query_bcast query_bcast;
  struct query_ucast query_ucast;
  struct chat chat;
  struct exiting exiting;

  unsigned char *packet_bcast = malloc(sizeof(query_bcast)) ;
  unsigned char *packet_ucast = malloc(sizeof(query_ucast)) ;
  unsigned char *packet_chat = malloc(sizeof(chat)) ;
  unsigned char *packet_exiting = malloc(sizeof(exiting));

  memset(&query_bcast, 0, sizeof(struct query_bcast));
  memset(&query_ucast, 0, sizeof(struct query_ucast));
  memset(&chat, 0, sizeof(struct chat));
  memset(&exiting, 0, sizeof(struct exiting));

  strcpy(ifName, DEFAULT_IF);
  /* Get interface name */


  /* Open RAW socket to send on */
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

  /* Construct the Ethernet header */
  memset(sendbuf, 0, BUF_SIZ);
  /* Ethernet header */
  eh->ether_dhost[0] = BCAST_MAC0;
  eh->ether_dhost[1] = BCAST_MAC1;
  eh->ether_dhost[2] = BCAST_MAC2;
  eh->ether_dhost[3] = BCAST_MAC3;
  eh->ether_dhost[4] = BCAST_MAC4;
  eh->ether_dhost[5] = BCAST_MAC5;
  eh->ether_shost[0] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[0];
  eh->ether_shost[1] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[1];
  eh->ether_shost[2] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[2];
  eh->ether_shost[3] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[3];
  eh->ether_shost[4] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[4];
  eh->ether_shost[5] = ((uint8_t *)&if_mac.ifr_hwaddr.sa_data)[5];

  /* Ethertype field  ETH_P_IP */
  eh->ether_type = htons(ETHER_TYPE);
  tx_len += sizeof(struct ether_header);


  /* Packet data */

  /*
  sendbuf[tx_len++] = 0xde;
    sendbuf[tx_len++] = 0xad;
    sendbuf[tx_len++] = 0xbe;
  sendbuf[tx_len++] = 0xef;
  *//* Index of the network device */


  if (argc > 1){
    type = atoi(argv[1]);
    if (type==1 && argc !=4)
    {
        printf("\n Usage for QUERY_BROADCAST: ./example1 [TYPE=0]  \n");
        printf("\n Usage for QUERY_UNICAST: ./example1 [TYPE=1] [TARGET_NAME] [TARGET_SURNAME]\n");
        printf("\n Usage for CHAT :  ./example1 [TYPE=3] [TARGET_NAME] [TARGET_SURNAME] [MESSAGE]\n");
        printf("\n Usage for EXITING: ./example1 [TYPE=5]\n");
        exit(-1);
    }
    if (type==2)
    {
        printf("\n Usage for QUERY_BROADCAST: ./example1 [TYPE=0]  \n");
        printf("\n Usage for QUERY_UNICAST: ./example1 [TYPE=1] [TARGET_NAME] [TARGET_SURNAME]\n");
        printf("\n Usage for CHAT :  ./example1 [TYPE=3] [TARGET_NAME] [TARGET_SURNAME] [MESSAGE]\n");
        printf("\n Usage for EXITING: ./example1 [TYPE=5]\n");
        exit(1);
    }
    if (type==3 && argc < 5)
    {
        printf("\n Usage for QUERY_BROADCAST: ./example1 [TYPE=0]  \n");
        printf("\n Usage for QUERY_UNICAST: ./example1 [TYPE=1] [TARGET_NAME] [TARGET_SURNAME]\n");
        printf("\n Usage for CHAT :  ./example1 [TYPE=3] [TARGET_NAME] [TARGET_SURNAME] [MESSAGE]\n");
        printf("\n Usage for EXITING: ./example1 [TYPE=5]\n");
        exit(-1);
    }

    if (type==5 && argc != 2)
    {
        printf("\n Usage for QUERY_BROADCAST: ./example1 [TYPE=0]  \n");
        printf("\n Usage for QUERY_UNICAST: ./example1 [TYPE=1] [TARGET_NAME] [TARGET_SURNAME]\n");
        printf("\n Usage for CHAT :  ./example1 [TYPE=3] [TARGET_NAME] [TARGET_SURNAME] [MESSAGE]\n");
        printf("\n Usage for EXITING: ./example1 [TYPE=5]\n");
        exit(-1);
    }
    switch (type) {
        case QUERY_BROADCAST:

          printf("Broadcasting Query Packet..\n");
          fill_query_bcast(&query_bcast);
          packet_bcast = hex_print((void *) &query_bcast, sizeof(struct query_bcast));

          for(j = 0; j < (sizeof(query_bcast)); j++){
            sendbuf[14+j] = packet_bcast[j];
            tx_len++;
            //printf("%d\n", sendbuf[14+j]);
          }
          break;
          case QUERY_UNICAST:
            printf("Unicasting\n");
            strcpy(target_name, argv[2]);
            strcpy(target_surname, argv[3]);
            /*to_mac = read_mac_file(target_name, target_surname);
            eh->ether_dhost[0] = to_mac[0];
            eh->ether_dhost[1] = to_mac[1];
            eh->ether_dhost[2] = to_mac[2];
            eh->ether_dhost[3] = to_mac[3];
            eh->ether_dhost[4] = to_mac[4];
            eh->ether_dhost[5] = to_mac[5];*/
            fill_query_ucast(&query_ucast, target_name, target_surname);
            packet_ucast = hex_print((void *) &query_ucast, sizeof(struct query_ucast));
            for(j = 0; j < (sizeof(query_ucast)); j++){
              sendbuf[14+j] = packet_ucast[j];
              tx_len++;
            }
            break;
          case CHAT:
            printf("Chat Message Sending\n");
            unsigned char *to_name = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);
            unsigned char *to_surname = malloc(sizeof(unsigned char)*MAX_NAME_SIZE);

            strcpy(to_name, argv[2]);
            strcpy(to_surname, argv[3]);

            for(i = 4; i < argc; i++){
                strcat(message, argv[i]);
                strcat(message, " ");
            }



            unsigned char *length = malloc(sizeof(unsigned char)*2);
            to_mac = read_mac_file(to_name, to_surname);
            for(int k = 0; k < 6; k++){
                //printf("%02x ", to_mac[k]);
            }
            eh->ether_dhost[0] = to_mac[0];
            eh->ether_dhost[1] = to_mac[1];
            eh->ether_dhost[2] = to_mac[2];
            eh->ether_dhost[3] = to_mac[3];
            eh->ether_dhost[4] = to_mac[4];
            eh->ether_dhost[5] = to_mac[5];
            fill_chat(&chat, message);
            packet_chat = hex_print((void *) &chat, sizeof(struct chat));
            for(j = 0; j < (sizeof(chat)); j++){
              sendbuf[14+j] = packet_chat[j];
              tx_len++;
            }
            break;
        case EXITING:
          printf("EXITING Message Broadcasting\n");
          fill_exiting(&exiting);
          packet_exiting = hex_print((void *) &exiting, sizeof(struct exiting));
          for(j = 0; j < (sizeof(exiting)); j++){
            sendbuf[14+j] = packet_exiting[j];
            tx_len++;
          }
          break;



          default: printf("default");

            break;

      }
    }
    else{
        printf("\n Usage for QUERY_BROADCAST: ./example1 [TYPE=0]  \n");
        printf("\n Usage for QUERY_UNICAST: ./example1 [TYPE=1] [TARGET_NAME] [TARGET_SURNAME]\n");
        printf("\n Usage for CHAT :  ./example1 [TYPE=3] [TARGET_NAME] [TARGET_SURNAME] [MESSAGE]\n");
        printf("\n Usage for EXITING: ./example1 [TYPE=5]\n");
        exit(-1);
    }
    printf("\n" );

    socket_address.sll_ifindex = if_idx.ifr_ifindex;
    // Address length
    socket_address.sll_halen = ETH_ALEN;
    // Destination MAC
    /*socket_address.sll_addr[0] = to_mac[0];
    socket_address.sll_addr[1] = to_mac[1];
    socket_address.sll_addr[2] = to_mac[2];
    socket_address.sll_addr[3] = to_mac[3];
    socket_address.sll_addr[4] = to_mac[4];
    socket_address.sll_addr[5] = to_mac[5];*/

  //printf("%X\n",packet[6]);
  //printf("%s\n",sendbuf);
  /* Send packet */
    if (sendto(sockfd, sendbuf, tx_len, 0, (struct sockaddr*)&socket_address, sizeof(struct sockaddr_ll)) < 0)
      printf("Send failed\n");
}

/**
 * @brief
 *
 * @param[out] q
 */
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
    snprintf(q->target_name, MAX_NAME_SIZE, "%s", target_name);
    snprintf(q->target_surname, MAX_NAME_SIZE, "%s", target_surname);
}

static void fill_chat(struct chat *q, unsigned char *message)
{
    q->type = CHAT;
    snprintf(q->length, LENGTH, "%ld", (strlen(message)));
    q->packet_id = 1;
    snprintf(q->message, MAX_MESSAGE_SIZE, "%s", message);
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

static unsigned char *read_mac_file(unsigned char *name, unsigned char *surname)
{
    FILE *fp;

    unsigned char *mac = malloc(sizeof(unsigned char)*6);
    unsigned char *macc = malloc(sizeof(unsigned char)*6);
    unsigned char *f_name = malloc(sizeof(unsigned char)*10);
    unsigned char *f_surname = malloc(sizeof(unsigned char)*10);
    int check_name,  check_surname;

    fp = fopen("mac.txt", "r");
    while(!feof(fp)){


        fscanf(fp, "%s %s %s\n", mac, f_name, f_surname);
            //printf("%s\n", mac);

        check_name = strcmp(name, f_name);
        check_surname = strcmp(surname, f_surname);
        if(check_name == 0 && check_surname==0){
            macc=hex_to_string(mac);
            for(int k = 0; k < 6; k++){
                //printf("%02x ", macc[k]);
            }
        }

        //printf("%s %s %s\n", mac, f_name, f_surname);

    }
    fclose(fp);
    return macc;
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
