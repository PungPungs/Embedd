// 폴링 방식으로 작성했는데 상당히 비효율적인 코드인 거 같다.. 찾아보니 select 란 방식이 있어 해당사항을 적용해보려 한다.


#include <iostream>
#include <string>
#include <stdlib.h>
#include <vector>

// Linux library
#include <termios.h>
#include <errno.h>
#include <unistd.h>
#include <fcntl.h>

using namespace std;


int main(){
	string gport;
	string mport;
	string sport;
	int gps_fd;
	int motion_fd;
	int sender_fd;

	struct termios gnewtio = {0};
	struct termios goldtio = {0};

	struct termios mnewtio = {0};
	struct termios moldtio = {0};

	struct termios snewtio = {0};
	struct termios soldtio = {0};

	cout << "gps 시리얼 포트를 입력해주세요(리눅스)" << endl;
	cin >> gport;
	cout << "motion 시리얼 포트를 입력해주세요(리눅스)" << endl;
	cin >> mport;
	cout << "전달 받을 시리얼 포트를 입력해주세요(리눅스)" << endl;
	cin >> sport;

	gps_fd =  open(gport.c_str(), O_RDWR | O_NOCTTY);
	motion_fd =  open(mport.c_str(), O_RDWR | O_NOCTTY);
	sender_fd =  open(sport.c_str(), O_RDWR | O_NOCTTY);

	if (gps_fd < 0 || motion_fd < 0 || sender_fd < 0) {
		perror("시리얼 포트 열기 실패");
		return -1;
	}

	// 시리얼 포트는 시스템 자원이라서 설정 변경 후 원상복귀를 안 할 시 오류가 발생할 수 있다.
	tcgetattr(gps_fd, &goldtio);
	tcgetattr(motion_fd, &moldtio);
	tcgetattr(sender_fd, &soldtio);

	gnewtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	gnewtio.c_iflag = 0;
	gnewtio.c_oflag = 0;
	gnewtio.c_lflag = 0;
	gnewtio.c_cc[VTIME] = 0;
	gnewtio.c_cc[VMIN] = 1;


	mnewtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	mnewtio.c_iflag = 0;
	mnewtio.c_oflag = 0;
	mnewtio.c_lflag = 0;
	mnewtio.c_cc[VTIME] = 0;
	mnewtio.c_cc[VMIN] = 1;

	snewtio.c_cflag = B115200 | CS8 | CLOCAL | CREAD;
	snewtio.c_iflag = 0;
	snewtio.c_oflag = 0;
	snewtio.c_lflag = 0;
	snewtio.c_cc[VTIME] = 0;
	snewtio.c_cc[VMIN] = 1;

	tcflush(gps_fd, TCIOFLUSH);
	tcsetattr(gps_fd, TCSANOW, &gnewtio);
	fcntl(gps_fd, F_SETFL, O_NONBLOCK);

	tcflush(motion_fd, TCIOFLUSH);
	tcsetattr(motion_fd, TCSANOW, &mnewtio);
	fcntl(motion_fd, F_SETFL, O_NONBLOCK);

	tcflush(sender_fd, TCIOFLUSH);
	tcsetattr(sender_fd, TCSANOW, &snewtio);
	fcntl(sender_fd, F_SETFL, O_NONBLOCK);

	unsigned char gbuffer[256] = {0};
	unsigned char mbuffer[256] = {0};
	string gdummy;
	string mdummy;


	while (true) {
		int g_ret = read(gps_fd, gbuffer, sizeof(gbuffer));
		int m_ret = read(motion_fd, mbuffer, sizeof(mbuffer));

		if(g_ret > 0) {
			gdummy = processing(gbuffer,g_ret);
			if(!gdummy.empty()){
				write(sender_fd,gdummy.c_str(), gdummy.size());
			}
		}
		if(m_ret > 0) {
			mdummy = processing(mbuffer,m_ret);
			if (!mdummy.empty()) {
				write(sender_fd,mdummy.c_str(), mdummy.size());
			}
		}

		usleep(5000);
	}
	
	tcsetattr(gps_fd, TCSANOW, &goldtio);
	tcsetattr(motion_fd, TCSANOW, &moldtio);
	tcsetattr(sender_fd, TCSANOW, &soldtio);
	
	close(gps_fd);
	close(motion_fd);
	close(sender_fd);
}

string processing(unsigned char *buffer, int len) {
	string incoiming;
	for (int i = 0; i < len; i++) {
		if (buffer[i] == '\n') {
			return incoiming;
		}
		else if (buffer[i] != '\r') {
			incoiming += buffer[i];
		}
	}
	return "";
}