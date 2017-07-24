
#include <stdint.h>

enum types{
   QUERY_BROADCAST ,
   QUERY_UNICAST ,
   HELLO_RESPONSE,
   CHAT,
   CHAT_ACK,
   EXITING
} en_pack_type;

struct query_bcast{
  unsigned char type;
  unsigned char requester_name[10];
  unsigned char requester_surname[10];
};

struct query_ucast{
  unsigned char type;
  unsigned char requester_name[10];
  unsigned char requester_surname[10];
  unsigned char target_name[10];
  unsigned char target_surname[10];

};

struct hello_resp{
  unsigned char type;
  unsigned char responder_name[10];
  unsigned char responder_surname[10];
  unsigned char queryier_name[10];
  unsigned char queryier_surname[10];

};

struct chat{
  unsigned char type;
  unsigned char length[2];
  unsigned char packet_id;
  unsigned char message[256];

};

struct chat_ack{
  unsigned char type;
  unsigned char packet_id;
};

struct exiting{
  unsigned char type;
  unsigned char name[10];
  unsigned char surname[10];
};

struct mac_file{
    unsigned char mac[6];
    unsigned char name[10];
    unsigned char surname[10];
};

typedef struct node
{
    uint8_t mac[6];
    unsigned char name[10];
    unsigned char surname[10];
    struct node* next;
} node;

static void fill_query_bcast(struct query_bcast *q);

static void fill_query_ucast(struct query_ucast *q, unsigned char *target_name, unsigned char *target_surname);

static void fill_hello_resp(struct hello_resp *q, unsigned char *target_name, unsigned char *target_surname);

static void fill_chat(struct chat *q, unsigned char *message);

static void fill_chat_ack(struct chat_ack *q);

static void fill_exiting(struct exiting *q);

//static void write_macs_to_file(unsigned char *mac, unsigned char *name, unsigned char *surname)

static unsigned char *hex_print(void *pack, size_t len);

static void decode_ucast();

static void decode_bcast();

static void decode_hello_resp();

static void decode_chat();

static void decode_chat_ack();

static void decode_exiting();

static int write_mac_to_file( unsigned char *smac, unsigned char *name, unsigned char *surname);

static int check_mac_is_exist();

static unsigned char *read_mac_file(unsigned char *name, unsigned char *surname);

unsigned char *hex_to_string(unsigned char *string);

struct node* create(unsigned char *mac, unsigned char *name, unsigned char *surname, struct node* next);

node* append(node* head, unsigned char *mac, unsigned char *name, unsigned char *surname);

node* remove_front(node* head);

node* remove_back(node* head);

node* remove_any(node* head,node* nd);

void display(node* n);

node* search(node* head,unsigned char *mac);

int count(node *head);







//int struct_to_hex(unsigned char *input, int output);
//unsigned char hex_to_struct(int input, unsigned char *output);
