#ifndef __USER__
# define __USER__

#include "include.h"

typedef struct user_list_node {
    std::string name;
    int id;
    std::vector<std::string> channels;
    struct user_list_node * next;
} user_list_node;

typedef struct user_list {
    user_list_node * head;
} user_list;

class User {
	user_list users;

public:
	User();

	void init();
	void print();
	void new_user(int fd);
	void new_user();
	bool remove_user(int fd);
	bool set_name(int fd, const std::string &name);
	std::string get_name(int fd);
	int  get_fd(const std::string &name);
	std::vector<std::string> get_users();
	bool set_channel(int fd, const std::string &channel);
	std::vector<std::string> get_channels(int fd);
	bool remove_channel(int fd, const std::string &channel);
};

#endif
