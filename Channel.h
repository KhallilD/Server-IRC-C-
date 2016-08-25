#ifndef __CHANNEL__
#define __CHANNEL__

#include "include.h"

typedef struct channel_list_node {
  std::string name;
  std::map<int, std::string> users;
  struct channel_list_node * next;
} channel_list_node;

typedef struct channel_list {
    channel_list_node * head;
} channel_list;

class Channel {
    channel_list channels;
public:
        Channel();

        void init();
        void new_channel(const std::string &channel);
        void delete_channel(int user, const std::string &channel);
        bool exist(const std::string &channel);
        bool remove_channel(const std::string &channel);
        std::vector<std::string> get_users(const std::string &channel);
        std::vector<std::string> get_channels(const std::string &args);
        bool add_user(const std::string &channel, int fd, const std::string &username);
        bool remove_user(const std::string &channel, int fd);
        bool change_username(const std::string &channel, int fd, const std::string &username);
        bool add_message(const std::string &channel, std::string &user, const std::string &message);
        bool in_channel(const std::string &channel, int fd);
        void send_message(const char * sender, const std::string &channel, const char * message);
};

#endif /*__CHANNEL__*/
