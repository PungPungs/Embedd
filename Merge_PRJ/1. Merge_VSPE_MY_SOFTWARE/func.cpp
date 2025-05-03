#include <string>
#include <Windows.h>
#include <iostream>
#include <cstdlib>

using namespace std;

HANDLE serial_Handler(string equip, int baudrate) {
	wstring port;
	char check = 'N';

	while (check == 'N') {
		cout << equip << "�ø��� ��Ʈ�� �Է����ּ���!!" << endl;
		wcin >> port;
		wcout << port << " : �ش� ��Ʈ�� �³���? �´ٸ� Y, Ʋ���� N�� �Է����ּ���" << endl;
		cin >> check;
		if (check != 'Y' || check == 'N') {
			cout << "�߸� �Է��ϼ̽��ϴ�.  ó������ �ٽ� ���ư��ϴ�.." << endl;
			check = 'N';
		}
	}
	HANDLE hSerial = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	
	if (hSerial == INVALID_HANDLE_VALUE) {
		cerr << equip << "�� �ø��� ��Ʈ�� �� �� �����ϴ�.";
		exit(1);
	}
	
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.BaudRate = baudrate;
	// Default
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;


	if (!SetCommState(hSerial, &dcbSerialParams)) {
		cerr << equip << "�ø��� ��Ʈ ���� ����";
		exit(1);
	}

	// Default
	COMMTIMEOUTS timeouts = { 0 };
	timeouts.ReadIntervalTimeout = 50;
	timeouts.ReadTotalTimeoutConstant = 50;
	timeouts.ReadTotalTimeoutMultiplier = 10;

	SetCommTimeouts(hSerial, &timeouts);
	return hSerial;
}


string read_data(HANDLE handler) {
	char buffer[1];
	DWORD bytesRead;
	string ncomingLine;
	while(true) {
		if (ReadFile(handler, buffer, 1, &bytesRead, nullptr) && bytesRead > 0) {
			char m = buffer[0];
			if (m != '\r') {
				ncomingLine += m;
			}
			if (m == '\n') {
				return ncomingLine;
			}
		}
	}
}

int send_data(HANDLE handler, string *data) {
	char buffer[1];
	DWORD bytesRead;
	string ncomingLine;
	// 3. ������ ����
	DWORD bytesWritten;

	if (!WriteFile(handler, data->c_str(), data->length(), &bytesWritten, NULL)) {
		cout << "���� ����" << endl;
		CloseHandle(handler);
		exit(1);
	}

}
