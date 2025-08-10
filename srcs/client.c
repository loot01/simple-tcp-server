#include <sys/socket.h>
#include <sys/types.h>
#include <netdb.h>
#include <stdio.h>
#include <string.h>
#include <unistd.h>
#include <stdlib.h>

int	main(int argc, char **argv)
{
	int	status;
	int	server_fd;
	char	buf[1024];
	char	*port;
	struct addrinfo	hints;
	struct addrinfo	*servinfo;
	
	if (argc != 2)
	{
		fprintf(stderr, "usage: stc <port>\n");
		exit(1);
	}
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
	printf("Client connected to port: %s\n", port);
	if ((server_fd = socket(servinfo->ai_family, servinfo->ai_socktype,
			servinfo->ai_protocol)) == -1)
	{
		perror("socket");
		exit(1);
	}
	if (connect(server_fd, servinfo->ai_addr, servinfo->ai_addrlen) == -1)
	{
		perror("connect");
		exit(1);
	}
	while (1)
	{
		if(fgets(buf, sizeof(buf), stdin) == NULL)
			break;
		if (send(server_fd, buf, strlen(buf), 0) == -1)
			break;
	}
	close(server_fd);
	freeaddrinfo(servinfo);
}
