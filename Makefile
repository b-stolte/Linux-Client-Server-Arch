main:
	g++ -std=c++11 -O -o client client.cpp
	g++ -std=c++11 -O -o server server.cpp
	groff -Tps -man client.man > client.ps
	groff -Tps -man server.man > server.ps
	ps2pdf client.ps client.pdf
	ps2pdf server.ps server.pdf
	rm client.ps
	rm server.ps

client:
	g++ -std=c++11 -O -o client client.cpp 

server:
	g++ -std=c++11 -O -o server server.cpp

clean:
	rm client
	rm server
	rm client.pdf
	rm server.pdf
