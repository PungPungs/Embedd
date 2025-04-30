#include <string.h>
#include <iostream>

using namespace std;

string get_gphdt(string &buffer) {
	string line;
	size_t header = buffer.find("$GPHDT");
	size_t end = buffer.find("\r\n", header);
	if (header != string::npos && end != string :: npos) {
		line = buffer.substr(header, end + 2 - header);
		buffer.erase(0,end + 2);
		return line;
	}
	return "";
}

int main(void) {
	string hdt = "$GPGGA,215235.670,3735.0064,N,12701.6746,E,1,03,50.0,0.0,M,19.6,M,0.0,0000*4\r\n$GPRMC,215235.670,A,3735.0064,N,12701.6746,E,0.000000,,060905,,*12\r\n$GPHDT,123.456,T*00\r\n$GPGGA,215235.670,3735.0064,N,12701.6746,E,1,03,50.0,0.0,M,19.6,M,0.0,0000*4\r\n$GPRMC,215235.670,A,3735.0064,N,12701.6746,E,0.000000,,060905,,*12\r\n$GPHDT,123.456,T*00\r\n$GPGGA,215235.670,3735.0064,N,12701.6746,E,1,03,50.0,0.0,M,19.6,M,0.0,0000*4\r\n$GPRMC,215235.670,A,3735.0064,N,12701.6746,E,0.000000,,060905,,*12\r\n$GPHDT,123.456,T*00\r\n$GPGGA,215235.670,3735.0064,N,12701.6746,E,1,03,50.0,0.0,M,19.6,M,0.0,0000*4\r\n$GPRMC,215235.670,A,3735.0064,N,12701.6746,E,0.000000,,060905,,*12\r\n$GPHDT,123.456,T*00\r\n";
	while(hdt.size() > 0){
		cout << get_gphdt(hdt) << endl;
	}
	return -1;
}
