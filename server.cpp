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
#include <fcntl.h>
#include <ctime>
#include <iomanip>
#include <map>
#include <chrono>


//provided Trans() function
int TransSave = 0;
void Trans( int n ) {
	long i, j;

	// Use CPU cycles 
	j = 0;
	for( i = 0; i < n * 100000; i++ ) {
		j += i ^ (i+1) % (i+n);
	}
	TransSave += j;
	TransSave &= 0xff;
}


double get_epoch_time(){
	std::chrono::duration<double, std::milli> millitime = std::chrono::system_clock::now().time_since_epoch();
	return((double)millitime.count()/1000);
}

//map will keep track of transactions from all clients
std::map<std::string, int> transactions;
int transactionCount = 0;

int main(int argc, char* argv[]){
	std::cout << std::fixed << std::setprecision(2);
	//read in port number
	int portNum;
	if(argc < 2){
		std::cout << "Not enough command line arguments" << std::endl;
		return(-1);
	}
	else if(argc == 2){
		portNum = atoi(argv[1]);
		if(portNum < 5000 || portNum > 64000){
			std::cout << "Invalid port number" << std::endl;
		}
	}
	else{
		std::cout << "Too many command line arguments" << std::endl;
		return(-1);
	}
	
	//configure port for listening
	std::cout << "Using port " << portNum << std::endl;
    	int fd = socket(AF_INET, SOCK_STREAM, 0);
	fcntl(fd, F_SETFL, O_NONBLOCK);
	struct sockaddr_in serv_addr; 

	// setup server address
	serv_addr.sin_family = AF_INET;
	serv_addr.sin_addr.s_addr = INADDR_ANY;
	serv_addr.sin_port = htons(portNum); // port

	if (bind(fd, (struct sockaddr*) &serv_addr, sizeof(serv_addr)) < 0) // bind socket to the server address
		return 1;

	if (listen(fd, 100) < 0)
		return 1;
	double lastTransTime = get_epoch_time();
	double firstTransTime;
	bool firstTransDone = false;
	int timeout = 30;
	while(get_epoch_time() - lastTransTime < timeout){
		struct sockaddr_in client_address; // client address
		socklen_t len = sizeof(client_address); 

		int client_fd = accept(fd, (struct sockaddr*) &client_address, &len);  // accept connection

		if (client_fd < 0) // bad connection
		    continue; // discard

		char buffer[1024];
		int count = 0;
		std::string hostname;
		//read hostname
		count = read(client_fd, buffer, sizeof(buffer));
		buffer[count] = '\0';
		hostname = buffer;
	
		//send confirmation
		std::string conf = "H";
		count = write(client_fd, conf.c_str(), conf.size());

		//read n
		std::string n;
		count = read(client_fd, buffer, sizeof(buffer));
		buffer[count] = '\0';
		n = buffer;
		transactionCount++;
		++transactions[hostname];
		if(!firstTransDone){
			firstTransTime = get_epoch_time();
			firstTransDone = true;
		}
		std::cout  << get_epoch_time() << ": # " << transactionCount << " (T " << n << ") from " << hostname << std::endl;
		Trans(stoi(n));
		std::cout << get_epoch_time() << ": # " << transactionCount << " (Done) from " << hostname << std::endl;
		//send confirmation
		conf = std::to_string(transactionCount);
		count = write(client_fd, conf.c_str(), conf.size());
		
		close(client_fd);
		lastTransTime = get_epoch_time();
	}

	//print summary
	double serverTime = lastTransTime - firstTransTime;
	double tps = transactionCount/serverTime;
	std::cout << std::endl << "SUMMARY" << std::endl;
	for(auto t : transactions){
		std::cout << "    " << t.second << " transactions from " << t.first << std::endl;
	}
	std::cout << tps << " transactions/second   (" << transactionCount << "/" << serverTime << ")" << std::endl;
	return(0);
}








