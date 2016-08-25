# Server-IRC-C++ Running on Linux
IRC Server which allow users to create and use discussion channels
The server is non-blocking, the use of select allow to get multiples inputs (new connection/command input)
3 objects : Server/Channel/User
Chained list are used to manage groups of users and channels.
Usage : ./server <port>
