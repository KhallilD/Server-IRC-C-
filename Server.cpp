#include "Server.h"

const char * help = "\nCommand : \n/nick - Modifier votre nom\n/users - Affiche les utilisateurs connec\
tes\n/new_channel - Cree un nouveau channel\n/delete_channel - Supprime un channel\n/list || /list [string] - Affiche les channels existants\n/join [channel] - Rejoins le channel\n/part [channel]- Quitte le channel\n/names [channel] - Affiche les utilisateurs connectes au channel\n[message] - Envoi le texte dans les channels ou vous etes\n/msg [ user || channel ] [message] - Envoi le texte a la destination choisis\n/command - Affiche cette aide\n\n";

std::vector<std::string> divide_str(const std::string &str, char sep)
{
    std::vector<std::string> v_str;
    std::stringstream ss(str);
    std::string word;

    while (std::getline(ss, word, sep)) {
      v_str.push_back(word);
    }

    return v_str;
}

int Server::open_server_socket(t_env * e)
{
  int sock =  socket(PF_INET, SOCK_STREAM, 0);
  if (sock < 0) {
    perror("socket");
    exit( -1 );
  }
  struct sockaddr_in serverIPAddress;

  serverIPAddress.sin_family = AF_INET;
  serverIPAddress.sin_addr.s_addr = INADDR_ANY;
  serverIPAddress.sin_port = htons(e->port);

  int optval = 1;
  setsockopt(sock, SOL_SOCKET, SO_REUSEADDR, (char *) &optval, sizeof(int));

  if(bind(sock,(struct sockaddr *)&serverIPAddress, sizeof(serverIPAddress)) == -1)
    {
      perror("bind");
      exit(-1);
    }

  if (listen(sock, 42) == -1)
    {
      perror("listen");
      exit(-1);
    }
  e->fd_type[sock] = FD_SERVER;
  e->fct_read[sock] = &Server::add_client;
  e->fct_write[sock] = NULL;

  return sock;
}

void Server::add_client(t_env *e, int sock)
{
  struct sockaddr_in clientIPAddress;
  const char * msg = "\nBienvenue sur l'IRC\n";

  int alen = sizeof(clientIPAddress);

  int c_sock = accept(sock, (struct sockaddr *)&clientIPAddress, (socklen_t*)&alen);
  if (c_sock < 0) {
    perror("accept");
    exit(-1);
  }

  write(c_sock, msg, strlen(msg));
  add_user(c_sock);
  e->fd_type[c_sock] = FD_CLIENT;
  e->fct_read[c_sock] = &Server::read_input;
  e->fct_write[c_sock] = NULL;
}

void Server::run_server(t_env * e)
{
  open_server_socket(e);
  int   i, fd_max;
  fd_set fd_read;
  int return_select;
  struct timeval tv;

  tv.tv_sec = 2;
  tv.tv_usec = 0;

  void (Server::*fct)(struct s_env *, int);
 
  while (1)
    {
      FD_ZERO(&fd_read);
      fd_max = 0;
      for (i = 0; i < 255; i++)
	if (e->fd_type[i] != FD_FREE)
	  {
	    FD_SET(i, &fd_read);
	    fd_max = i;
	  }
      return_select = select(fd_max + 1, &fd_read, NULL, NULL, &tv);
      if (return_select == -1)
      {
    	  perror("select");
      }

    for (i = 0; i < 255; i++)
	if (FD_ISSET(i, &fd_read))
	  {
	    fct = e->fct_read[i];
	    (this->*fct)(e,i);
	  }
    }
}

void Server::exec_command(int fd, char *cmd)
{
  int	args_size = 0;
  std::string cmdline = cmd;
  std::string command;
  std::string args = "";
  std::vector<std::string> arg_list = divide_str(cmdline, ' ');
  if (arg_list.size() >= 2)
    {
      command = arg_list[0];
      args = arg_list[1];

      args_size = arg_list.size();
      if (args_size >= 3)
	{
	  int i = 2;
	  args = args + " ";
	  while (args_size > 2)
	    {
	      args = args + (arg_list[i] + " ");
	      i++;
	      args_size--;
	    }
	}
    }
  else
    command = arg_list[0];


  if (cmd[0] != '/') {
    send_message_all(fd, cmd);
  } else if (command == "/users") {
    get_all_users(fd);
  } else if (command == "/nick") {
    change_username(fd,args);
  } else if (command == "/new_channel") {
    create_channel(fd, args);
  } else if (command == "/command") {
    send_help(fd);
  } else if (command == "/list") {
    list_channels(fd, args);
  } else if (command == "/join") {
    enter_channel(fd, args);
  } else if (command == "/part") {
    leave_channel(fd, args);
  } else if (command == "/names") {
    get_users_in_channel(fd, args);
  } else if (command == "/delete_channel") {
    delete_channel(fd, args);
  } else if (command == "/msg") {
    send_message_channel(fd, args);
  } else {
    const char * msg =  "commande inconnue\n";
    write(fd, msg, strlen(msg));
  }
}

void Server::read_input(t_env *e, int fd)
{
  char cmd[256];
  int cmd_l = 0;
  unsigned char n_char = 0;

  while (cmd_l < 256 && read(fd, &n_char, 1) > 0)
    {
      if (n_char == '\n') {
    	  break;
      }
      cmd[cmd_l] = n_char;
      cmd_l++;
    }
  cmd_l--;
  if (cmd_l == -1)
     {
       std::vector<std::string> user_channels =  users.get_channels(fd);
       for (std::vector<std::string>::iterator i = user_channels.begin(); i != user_channels.end(); i++)
     	  channels.remove_user(*i, fd);
       users.remove_user(fd);
       e->fd_type[fd] = FD_FREE;
       e->fct_read[fd] = 0;
       e->fct_write[fd] = NULL;
       close(fd);
     }

  cmd[cmd_l] = 0;
  if (cmd_l > 0)
      exec_command(fd, cmd);
}

void Server::send_help(int fd)
{
  write(fd, help, strlen(help));
}

void Server::add_user(int fd)
{
  users.new_user(fd);
  send_help(fd);
}

void Server::change_username(int fd, const std::string &args)
{
  std::vector<std::string> arguments;
  const char * msg;
  arguments = divide_str(args, ' ');
  if (arguments.size() < 1)
    {
      msg = "Error, please enter a name\n";
      write(fd, msg, strlen(msg));
      return;
    }
  std::vector<std::string> user_channels;

  user_channels = users.get_channels(fd);
  if (users.set_name(fd, args))
    {
      for (std::vector<std::string>::iterator i = user_channels.begin(); i != user_channels.end(); i++)
	{
	  channels.change_username(*i, fd, args);
	}
    }
}

void Server::create_channel(int fd, const std::string &args)
{
  const char * msg;
  std::string channel;
  std::vector<std::string> arguments;

  arguments = divide_str(args, ' ');
  if (arguments.size() != 1)
    {
      msg = "Veuillez entrez UN nom de channel\n";
      write(fd, msg, strlen(msg));
      return;
    }

  channel = arguments[0];

  if (!channels.exist(channel))
    {
      channels.new_channel(channel);
      msg = "Channel created\n";
      write(fd, msg, strlen(msg));
      return;
    } else {
    msg = "Channel already exist\n";
    write(fd, msg, strlen(msg));
    return;
  }
}

void Server::delete_channel(int fd, const std::string &args)
{
  std::string channel;
  const char * msg;
  std::vector<std::string> arguments;

  arguments = divide_str(args, ' ');
  if (arguments.size() != 1)
    {
      msg = "Veuillez entrez UN nom de channel\n";
      write(fd, msg, strlen(msg));
      return;
    }
  channel = arguments[0];
  if (channels.exist(channel))
    {
      channels.delete_channel(fd, channel);
      users.remove_channel(fd, channel);
      
      return;
    }
  else
    {
      msg = "Channels doesn't exist\n";
      write(fd, msg, strlen(msg));
      return;
    }
}

void Server::enter_channel(int fd, const std::string &args)
{
  const char * msg;
  std::string channel;
  std::vector<std::string> arguments;

  arguments = divide_str(args, ' ');
  if (arguments.size() != 1)
    {
      msg = "Veuillez entrez UN nom de channel\n";
      write(fd, msg, strlen(msg));
      return;
    }
  channel = arguments[0];

  if (channels.exist(args)) {
    if (channels.in_channel(channel, fd)) {
      msg = "already in channel\n";
      write(fd, msg, strlen(msg));
      return;
    } else if (channels.add_user(channel, fd, users.get_name(fd))) {
      msg = "user added\n";
      users.set_channel(fd, channel);
      write(fd, msg, strlen(msg));
      return;
    } 

  } 
  else {
    msg = "Channel doesn't exist\n";
    write(fd, msg, strlen(msg));
    return;
  }
}

void Server::leave_channel(int fd,  const std::string &args)
{
  const char * msg;
  std::string channel;
  std::vector<std::string> arguments;

  arguments = divide_str(args, ' ');
  if (arguments.size() != 1)
    {
      msg = "Veuillez entrez UN nom de channel\n";
      write(fd, msg, strlen(msg));
      return;
    }
  channel = arguments[0];


  if (channels.in_channel(channel, fd)) {
    if (channels.remove_user(channel, fd)) {
      msg = "you've been removed\n";
      users.remove_channel(fd, channel);
      write(fd, msg, strlen(msg));
      return;
    } else {
      msg = "DENIED\n";
      write(fd, msg, strlen(msg));
      return;
    }
  } else {
    msg = "ERROR\n";
    write(fd, msg, strlen(msg));
    return;
  }
}

void Server::list_channels(int fd, const std::string &args)
{
	const char * msg;
	std::vector<std::string> null;
	std::vector<std::string> channelList;

	channelList = channels.get_channels(args);
	if (channelList != null)
	{
		std::sort(channelList.begin(), channelList.end());
		for (std::vector<int>::size_type i = 0; i != channelList.size(); i++)
		{
			msg = channelList[i].c_str();
			write(fd, msg, strlen(msg));
			msg = "\n";
			write(fd, msg, strlen(msg));
		}
	}
	else
	{
		msg = "No channels\n";
		write(fd, msg, strlen(msg));
		return;
	}
}

void Server::send_message_all(int fd, const char * message)
{
  std::vector<std::string> _channels;
  const char * msg;

  _channels = users.get_channels(fd);
  if (_channels.size() == 0)
    {
      msg = "Veuillez rentrez dans un channel avant d'envoyer un message\n";
      write(fd, msg, strlen(msg));
    }
  else
    {
      for(std::vector<std::string>::iterator i = _channels.begin(); i != _channels.end(); i++)
    	  channels.send_message(users.get_name(fd).data(),*i, message);
    }
}

void Server::send_message_channel(int fd, const std::string &args)
{
  std::vector<std::string> arguments;
  std::string message;
  const char *msg;
  arguments = divide_str(args, ' ');
  if (arguments.size() < 2)
    {
      msg = "/msg [ user || channel ] [message]";
      write(fd, msg, strlen(msg));
    }
  for (std::vector<std::string>::iterator i = arguments.begin() + 1; i != arguments.end(); i++)
    {
      message += *i + " ";
    }
  std::vector<std::string> channel_list = channels.get_channels("");
  if (std::find(channel_list.begin(), channel_list.end(), arguments[0]) != channel_list.end())
    {
      if (users.get_fd(arguments[0]) != -1)
	{
	  send_message_user(fd, message.data(), arguments[0]);
	  channels.send_message(users.get_name(fd).data(), arguments[0], message.data());
	}
      else
	{
	  channels.send_message(users.get_name(fd).data(), arguments[0], message.data());
	}
    }
  else if (users.get_fd(arguments[0]) != -1)
    {
      send_message_user(fd, message.data(), arguments[0]);
    }
  else
    {
      msg = "Erreur, aucun user ou channel correspondant\n";
      write (fd, msg, strlen(msg));
    }
}

void Server::send_message_user(int fd, const char * message, const std::string &username)
{
  const char *sender_name = users.get_name(fd).data();
  int	receiver_fd = users.get_fd(username);

  write(receiver_fd, sender_name, strlen(sender_name));
  write(receiver_fd, " : ", 3);
  write(receiver_fd, message, strlen(message));
  write(receiver_fd, "\n", 1);
}

void Server::get_users_in_channel(int fd, const std::string &args)
{
  const char * msg;
  std::string channel;
  std::vector<std::string> arguments;

  arguments = divide_str(args, ' ');

  if (arguments.size() != 1)
    {
      msg = "Veuillez entrez UN nom de channel\n";
      write(fd, msg, strlen(msg));
      return;
    }
  channel = arguments[0];

  if (channels.exist(channel)) {
    std::vector<std::string> username_list;
    std::vector<std::string> null;

    username_list = channels.get_users(channel);
    if (username_list == null) {
      msg = "Channel empty\n";
      write(fd, msg, strlen(msg));
      return;
    } 
    else {
      std::sort(username_list.begin(), username_list.end());
      for (std::vector<int>::size_type i = 0; i != username_list.size(); i++)
    	{
	  msg = username_list[i].c_str();
	  write(fd, msg, strlen(msg));
	  msg = "\n";
	  write(fd, msg, strlen(msg));
    	}
      return;
    }
  } else {
    msg = "Channel not existing\n";
    write(fd, msg, strlen(msg));
    return;
  }
}

void Server::get_all_users(int fd)
{
  const char * msg;
  std::vector<std::string> username_list;
  std::vector<std::string> null;

  username_list = users.get_users();
  if (username_list == null)
    {
      msg = "\n";
      write(fd, msg, strlen(msg));
      return;
    } 
  else 
    {
      std::sort(username_list.begin(), username_list.end());
      for (std::vector<int>::size_type i = 0; i != username_list.size(); i++)
	{
	  msg = username_list[i].c_str();
	  write(fd, msg, strlen(msg));
	  msg = "\n";
	  write(fd, msg, strlen(msg));
	}
      msg = "\n";
      write(fd, msg, strlen(msg));
      return;
    }
}

