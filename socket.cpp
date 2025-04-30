#include <stdio.h>
#include <stdlib.h>
#include <cstring.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>


#define PORT 12346
#define BUF_SIZE 1024


using namespace std;

int main(){
	int socket_fd, accepted_fd;
	struct sockaddr_in host_addr, client_addr;
	socklen_t size;
	int recv_length;
	char buffer[BUF_SIZE];

	socket_fd = socket(PF_INET, SOCK_STREAM,0);
	
	host_addr.sin_family = AF_INET;
	

}
