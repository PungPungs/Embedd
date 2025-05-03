#include "func.h"
#include <string>

using namespace std;

int main() {
	HANDLE gps = serial_Handler("GPS", 9600);

	//HANDLE motion = serial_Handler("Motion", 9600);

	HANDLE sender = serial_Handler("Sender", 9600);

	while (true) {
		string nav = read_data(gps);
		//string hdt = read_data(motion);
		string hdt = "$GPHDT,172815.0,3723.46587704,N,12202.26957864,W,2,6,1.2,18.893,M,-25.669,M,2.0 0031*4F";
		string data = nav + "\r\n" + hdt;
		send_data(sender, &data);
		cout << "ÀÛµ¿ Áß...." << endl;
	}
	exit(0);
}

