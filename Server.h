#ifndef __SERVER__
# define __SERVER__

#include "include.h"
#include "Channel.h"
#include "User.h"

#define FD_FREE 0
#define FD_CLIENT 1
#define FD_SERVER 2

struct s_env;

std::vector<std::string> divide_str(const std::string &str, char sep);

class Server {
	private:
		int open_server_socket(struct s_env * e);

	public:
		Channel channels;
		User users;
		void exec_command(int fd, char *cmd);
		void read_input(struct s_env *e, int socket);
		void add_user(int fd);
		void create_channel(int fd, const std::string &args);
		void delete_channel(int fd, const std::string &args);
		void enter_channel(int fd, const std::string &args);
		void leave_channel(int fd, const std::string &args);
		void list_channels(int fd, const std::string &args);
		void send_message_all(int fd, const char * msg);
		void send_message_channel(int fd, const std::string &args);
		void send_message_user(int fd, const char *message, const std::string &username);
		void get_users_in_channel(int fd, const std::string &args);
		void get_all_users(int fd);
		void change_username(int fd, const std::string &args);
		void run_server(struct s_env * e);
		void add_client(struct s_env * e, int serverSocket);
		void send_help(int fd);
};

typedef struct s_env
{
	char fd_type[255];
	void (Server::*fct_read[255])(struct s_env *, int);
	void (Server::*fct_write[255])(struct s_env *, int);
	int port;
}		t_env;

#endif /*__SERVER__*/
