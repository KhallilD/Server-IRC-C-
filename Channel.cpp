#include "Channel.h"

Channel::Channel() {
	this->channels.head = NULL;
}

void Channel::new_channel(const std::string &channel)
{
	channel_list_node * newNode = new channel_list_node;
	newNode->name = channel;
	newNode->next = NULL;
	if (this->channels.head == NULL)
		this->channels.head = newNode;
	else
	{
		channel_list_node * e = this->channels.head;
		while (e->next != NULL)
		{
			e = e->next;
		}
		e->next = newNode;
	}
}

void Channel::delete_channel(int fd, const std::string &channel)
{
	const char *msg = "Erreur, vous devez être le seul utilisateur dans le channel pour pouvoir le supprimer !\n";
	if (this->channels.head != NULL)
	{
		channel_list_node * e = this->channels.head;
		channel_list_node * previous = NULL;

		while (e != NULL)
		{
			if (e->name == channel)
			{
				if ((e->users.size() == 1 && e->users.find(fd) != e->users.end()) || e->users.size() == 0)
				{
					if (previous == NULL && e->next != NULL)
						this->channels.head = e->next;
					else if (previous == NULL && e->next == NULL)
						this->channels.head = NULL;
					else if  (e->next != NULL)
						previous->next = e->next;
					else
						previous->next = NULL;
					delete(e);
					msg = "Channel deleted\n";
					write(fd, msg, strlen(msg));
					return;
				}
				else
				{
					write(fd, msg, strlen(msg));
					return;
				}
			}
			previous = e;
			e = e->next;
		}
	}
	else
		return;
}  

bool Channel::remove_channel(const std::string &channel)
{
	channel_list_node * e = channels.head;
	channel_list_node * previous = channels.head;

	if (channels.head == NULL) {
		return false;
	}

	if (e->name == channel) {
		channels.head = e->next;
		return true;
	}

	while (e->next != NULL) {
		e = e->next;
		if (e->name == channel) {
			previous->next = e->next;
			return true;
		}
		previous = e;
	}
	return false;
}

std::vector<std::string> Channel::get_users(const std::string &channel)
{
	channel_list_node * e = channels.head;
	std::vector<std::string> username_list;
	if (channels.head == NULL)
		return username_list;

	while (e != NULL) {
		if (e->name == channel) {
			for(std::map<int, std::string>::iterator i = e->users.begin(); i != e->users.end(); i++)
			{
				username_list.push_back(i->second);
			}
			return username_list;
		}
		e = e->next;
	}
	return username_list;
}

bool Channel::add_user(const std::string &channel, int fd, const std::string &username)
{
	channel_list_node * e = channels.head;
	if (channels.head == NULL) {
		return false;
	}

	while (e != NULL)
	{
		if (e->name == channel)
		{
			e->users.insert(std::make_pair(fd, username));
			return true;
		}
		e = e->next;
	}
	return false;
}

bool Channel::remove_user(const std::string &channel, int fd)
{
	channel_list_node * e = channels.head;

	if (channels.head == NULL)
		return false;

	while (e != NULL) {
		if (e->name == channel) {
			e->users.erase(fd);
			return true;
		}
		e = e->next;
	}
	return false;
}

bool Channel::in_channel(const std::string &channel, int fd)
{
	channel_list_node * e = channels.head;

	if (channels.head == NULL)
		return false;

	while (e != NULL) {
		if (e->name == channel) {

			for (std::map<int, std::string>::iterator i = e->users.begin(); i != e->users.end(); i++) {
				if (i->first == fd) {
					return true;
				}
			}
		}
		e = e->next;
	}
	return false;
}

bool Channel::exist(const std::string &channel)
{
	channel_list_node * e;

	if (channels.head == NULL)
		return false;

	e = channels.head;
	while (e != NULL) {
		if (e->name == channel) {
			return true;
		}
		e = e->next;
	}
	return false;
}

std::vector<std::string> Channel::get_channels(const std::string &args)
{
	channel_list_node * e = this->channels.head;
	std::vector<std::string> channel_list;
	int	i = 0;
	int	pos;

	if (this->channels.head == NULL) {
		return channel_list;
	}
	if (args != "")
	{
		while (e != NULL)
		{
			pos = e->name.find(args);
			if (pos >= 0)
			{
				i++;
				channel_list.push_back(e->name);
			}
			e = e->next;
		}
		if (i == 0)
			return channel_list;
	}
	else
	{
		while (e != NULL) {
			channel_list.push_back(e->name);
			e = e->next;
		}
	}
	return channel_list;
}

bool	Channel::change_username(const std::string &channel, int fd, const std::string &username)
{
	channel_list_node * e = channels.head;

	if (channels.head == NULL)
		return false;

	while (e != NULL) {
		if (e->name == channel)
		{
			for (std::map<int, std::string>::iterator i = e->users.begin(); i != e->users.end(); i++)
			{
				if (i->first == fd)
				{
					i->second = username;
					return true;
				}
			}
		}
		e = e->next;
	}
	return false;
}

void	Channel::send_message(const char * sender, const std::string &channel, const char *message)
{
	channel_list_node * e = this->channels.head;

	while (e != NULL)
	{
		if (e->name == channel)
		{
			for(std::map<int, std::string>::iterator i = e->users.begin(); i != e->users.end(); i++)
			{
				write(i->first, sender, strlen(sender));
				write(i->first, " : ", 3);
				write(i->first, message, strlen(message));
				write(i->first, "\n", 1);
			}
		}
		e = e->next;
	}
}
