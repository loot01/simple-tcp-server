#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>
#include <arpa/inet.h>
#include <netinet/in.h>
#include <signal.h>
#include <sys/wait.h>
#include <errno.h>

#define BUF_SIZE 4096

void	sigchld_handler(int s)
{
	(void)s;
	int	saved_errno;

	saved_errno = errno;
	while (waitpid(-1, NULL, WNOHANG) > 0);
	errno = saved_errno;
}

void	handle_conn(int client_fd, char *buf)
{
	int	disconnected = 0;
	char	c;
	int	i;
	int	num_bytes;

	while (!disconnected)
	{
		i = 0;
		while (i < BUF_SIZE - 1)
		{
			num_bytes = recv(client_fd, &c, 1, 0);
			if (num_bytes == -1)
			{
				perror("recv");
				disconnected = 1;
				break;
			}
			if (num_bytes == 0)
			{
				printf("Client disconnected\n");
				disconnected = 1;
				break;
			}
			buf[i++] = c;
			if (c == '\n')
				break;
		}
		if (disconnected)
			break;
		buf[i] = '\0';
		printf("%s", buf);
	}
}

int	main(int argc, char **argv)
{
	int	status;
	int	server_fd;
	int	client_fd;
	char	*port;
	char	buf[BUF_SIZE];
	char	ipstr[INET6_ADDRSTRLEN];
	struct addrinfo	hints;
	struct addrinfo	*servinfo;
	struct sockaddr_storage their_addr;
	struct sigaction sa;
	socklen_t addr_size;
	
	if (argc != 2)
	{
		fprintf(stderr, "usage: ./server <port>\n");
		exit(1);
	}
	client_fd = 0;
	port = argv[1];
	memset(&hints, 0, sizeof(hints));
	hints.ai_family = AF_UNSPEC;
	hints.ai_socktype = SOCK_STREAM;
	hints.ai_flags = AI_PASSIVE;

	if ((status = getaddrinfo(NULL, port, &hints, &servinfo)) != 0)
	{
		fprintf(stderr, "getaddrinfo error: %s\n", gai_strerror(status));
		exit(1);
	}
	inet_ntop(servinfo->ai_family, servinfo->ai_addr, ipstr, sizeof ipstr);
	printf("Server: listening on port %s\n", port);

	if ((server_fd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1)
	{
		perror("socket");
		exit(1);
	}

	if (bind(server_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		perror("bind");
		exit(1);
	}

	if (listen(server_fd, 20) == -1)
	{
		perror("listen");
		exit(1);
	}
	sa.sa_handler = sigchld_handler;
	sigemptyset(&sa.sa_mask);
	sa.sa_flags = SA_RESTART;
	if (sigaction(SIGCHLD, &sa, NULL) == -1)
	{
		perror("sigaction");
		exit(1);
	}

	printf("Server: Waiting for connections..\n");
		if (client_fd == -1)
	{
		perror("accept");
	}
	
	while (1)
	{
		addr_size = sizeof(their_addr);
		client_fd = accept(server_fd, (struct sockaddr *)&their_addr,
				&addr_size);
		if (client_fd == -1)
		{
			perror("accept");
			continue ;
		}
		
		printf("Server: new client connected\n");
		if (!fork())
		{
			handle_conn(client_fd, buf);
			close(client_fd);
			exit(0);
		}
		close(client_fd);

	}
	
	close(server_fd);
	freeaddrinfo(servinfo);
}
