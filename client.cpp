#include <iostream>
#include <time.h>
#include <sys/time.h>
#include <string>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <unistd.h>
#include <sys/types.h> 
#include <sys/socket.h>
#include <netinet/in.h>  
#include <arpa/inet.h> 
#include <ctime>
#include <iomanip>
#include <chrono>


void Sleep( int n ) {
	struct timespec sleep;

	// Make sure pass a valid nanosecond time to nanosleep
	if( n <= 0 || n >= 100 ) {
		n = 1;
	}

	// Sleep for less than one second
	sleep.tv_sec  = 0;
        sleep.tv_nsec = n * 10000000;
	if( nanosleep( &sleep, NULL ) < 0 ) {
		perror ("NanoSleep" );
	}
}

double get_epoch_time(){
	std::chrono::duration<double, std::milli> millitime = std::chrono::system_clock::now().time_since_epoch();
	return((double)millitime.count()/1000);
}

int transactionCount = 0;
int main(int argc, char* argv[]){
	std::cout << std::fixed << std::setprecision(2);
	//std::cout << get_epoch_time() << std::endl;
	//determine hostname.pid
	char host[64];
	gethostname(host, 64);
	std::string hostname = host;
	int pid = getpid();
	hostname.append(".");
	hostname += std::to_string(pid);
	//read in port and ip address
	int portNum;
	std::string IP_addr;
	if(argc < 3){
		std::cout << "Not enought input arguments" << std::endl;
		return(-1);
	}
	else if(argc == 3){
		portNum = atoi(argv[1]);
		IP_addr = argv[2];
		if(portNum < 5000 || portNum > 64000){
			std::cout << "Invalid port number" << std::endl;
			return(-1);
		}
	}
	else{
		std::cout << "Too many input arguments" << std::endl;
		return(-1);
	}
	//print intial log
	std::cout << "Using port " << portNum << std::endl;
	std::cout << "Using server address " << IP_addr << std::endl;
	std::cout << "Host " << hostname << std::endl;
	
	//make connection fd
	int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
	if(conn_fd < 0){ 
		std::cout << "Connnection error" << std::endl;
		return(-1);
	}

	struct sockaddr_in serv_addr; 
    	serv_addr.sin_family = AF_INET;
    	serv_addr.sin_addr.s_addr = inet_addr(IP_addr.c_str());
    	serv_addr.sin_port = htons(portNum); // port
	
	//read from stdin and start coms with server if T<n> is seen
	std::string task;
	for (task; std::getline(std::cin, task);){
		std::string n = task.substr(1,task.size() - 1);
		if(task.substr(0,1) == "T"){
			//Only when the T is seen should the connection be made again
			//make connection fd
			int conn_fd = socket(AF_INET, SOCK_STREAM, 0);
			if(conn_fd < 0){ 
				std::cout << "Connnection error" << std::endl;
				return(-1);
			}
			if (connect(conn_fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
        			{return -1;}

			char buffer[1024];
			int count = 0;
			//send hostname to the server
			count = write(conn_fd, hostname.c_str(), hostname.size());
			//get reponse
			count = read(conn_fd, buffer, sizeof(buffer));
			buffer[count] = '\0';
			//send n
			std::cout << std::fixed << std::setprecision(2) << get_epoch_time() << ": Send (T " << n << ")" << std::endl;
			count = write(conn_fd, n.c_str(), n.size());
			
			//get response
			count = read(conn_fd, buffer, sizeof(buffer));
			buffer[count] = '\0';
			std::cout << get_epoch_time() << ": Recv (D " << buffer << ")" << std::endl;
			transactionCount++;
			close(conn_fd);
		}
		else{
			std::cout << "Sleep " << n << " units" << std::endl;
			Sleep(std::stoi(n));
		}
	}
	std::cout << "Sent " << transactionCount << " transactions" << std::endl;
	
}






