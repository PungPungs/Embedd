#include <string>
#include <Windows.h>
#include <iostream>
#include <cstdlib>

using namespace std;

HANDLE serial_Handler(string equip, int baudrate) {
	wstring port;
	char check = 'N';

	while (check == 'N') {
		cout << equip << "시리얼 포트를 입력해주세요!!" << endl;
		wcin >> port;
		wcout << port << " : 해당 포트가 맞나요? 맞다면 Y, 틀리면 N을 입력해주세요" << endl;
		cin >> check;
		if (check != 'Y' || check == 'N') {
			cout << "잘못 입력하셨습니다.  처음으로 다시 돌아갑니다.." << endl;
			check = 'N';
		}
	}
	HANDLE hSerial = CreateFile(port.c_str(), GENERIC_READ | GENERIC_WRITE, 0, nullptr, OPEN_EXISTING, 0, nullptr);
	
	if (hSerial == INVALID_HANDLE_VALUE) {
		cerr << equip << "의 시리얼 포트를 열 수 없습니다.";
		exit(1);
	}
	
	DCB dcbSerialParams = { 0 };
	dcbSerialParams.BaudRate = baudrate;
	// Default
	dcbSerialParams.ByteSize = 8;
	dcbSerialParams.StopBits = ONESTOPBIT;
	dcbSerialParams.Parity = NOPARITY;


	if (!SetCommState(hSerial, &dcbSerialParams)) {
		cerr << equip << "시리얼 포트 설정 실패";
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
	// 3. 데이터 쓰기
	DWORD bytesWritten;

	if (!WriteFile(handler, data->c_str(), data->length(), &bytesWritten, NULL)) {
		cout << "쓰기 실패" << endl;
		CloseHandle(handler);
		exit(1);
	}

}
