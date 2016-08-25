#include "User.h"

User::User() {
    this->users.head = NULL;
}

void User::new_user(int fd)
{
  std::string user_name;
  user_name = "user";
  user_name.push_back(fd + '0');
  if (this->users.head == NULL)
    {
      user_list_node * e = new user_list_node;
      e->id = fd;

      e->name = user_name;

      e->next = NULL;
      this->users.head = e;
    }
  else
    {
      user_list_node * e = this->users.head;
      user_list_node * new_node = new user_list_node;

      new_node->id = fd;
      new_node->name = user_name;;
      new_node->next = NULL;

 
     while (e->next != NULL) {
        e = e->next;
      }
     e->next = new_node;
    }
  
}

bool User::remove_user(int fd)
{
    user_list_node * e = this->users.head;
    user_list_node * previous = this->users.head;

    if (this->users.head == NULL) {
        return false;
    }
    if (e->id == fd) {
        this->users.head = e->next;
        return true;
    }
    while (e->next != NULL) 
      {
        e = e->next;
        if (e->id == fd) {
            previous->next = e->next;
	    delete(e);
            return true;
        }
        previous = e;
    }
    return false;
}

bool	User::remove_channel(int fd, const std::string &channel)
{
  user_list_node *e = this->users.head;
  if (this->users.head == NULL)
    return false;
  else
    while (e != NULL)
      {
    	if (e->id == fd)
    	{
    		e->channels.erase(std::remove(e->channels.begin(), e->channels.end(), channel), e->channels.end());
    		return true;
    	}
	e = e->next;
      }
  return false;
}

bool	User::set_channel(int fd, const std::string &channel)
{
 user_list_node *e = this->users.head;
  
  if (this->users.head == NULL)
    return false;
  else
    while (e != NULL)
      {
	if (e->id == fd)
	  {
	    e->channels.push_back(channel);
	    return true;
	  }
	e = e->next;
      }
  return false;
}

std::vector<std::string>	User::get_channels(int fd)
{
  user_list_node *e = this->users.head;
  std::vector<std::string> error;
  if (this->users.head == NULL)
    return error;
  else
    while (e != NULL)
      {
	if (e->id == fd)
	  {
	    return e->channels;
	  }
	e = e->next;
      }
  return error;
}

bool	User::set_name(int fd, const std::string &name)
{
  user_list_node *e = this->users.head;
  
  if (this->users.head == NULL)
    return false;
  else
    while (e != NULL)
      {
	if (e->id == fd)
	  {
	    e->name = name;
	    return true;
	  }
	e = e->next;
      }
  return false;
}

std::string	User::get_name(int fd)
{
  user_list_node * e = this->users.head;
  while (e != NULL)
    {
      if (fd == e->id)
	return e->name;
      e = e->next;
    }
  return NULL;
}

int	User::get_fd(const std::string &name)
{
  user_list_node * e = this->users.head;
  while (e != NULL)
    {
      if (name == e->name)
	return e->id;
      e = e->next;
    }
  return -1;
}

std::vector<std::string> User::get_users()
{
    user_list_node * e = this->users.head;
    int i = 0;
    std::vector<std::string> username_list;

    if (this->users.head == NULL) {
        return username_list;
    }

    while (e != NULL)
    {
    	i++;
        username_list.push_back(e->name);
        e = e->next;
    }
    return username_list;
}
