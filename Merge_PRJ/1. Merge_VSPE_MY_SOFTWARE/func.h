#include <string>
#include <Windows.h>
#include <iostream>
using namespace std;
#pragma once
HANDLE serial_Handler(string equip,int baudrate);
string read_data(HANDLE handler);
int send_data(HANDLE handler, string *data);