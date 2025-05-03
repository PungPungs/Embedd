// C++
#include <iostream>
#include <cstring>
#include <signal.h>
#include <vector>

// Linux
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/select.h>
#include <errno.h>

using namespace std;

volatile bool running = true;

string gps_line_buffer;
string motion_line_buffer;

/*
vector<string> list_serial_ports() {
	vector<string> ports;
	DIR *dir;
	struct dirent *ent;

	dir = opendir("/dev/");
	if (dir != NULL) {
		while ((ent = readdir(dir)) != NULL) {
		if(fnmatch("ttyS*", ent->d_name,0) == 0 ||
		   fnmatch("ttyUSB*", ent->d_name, 0) == 0 ||
		   fnmatch("ttyAMA*", ent->d_name,0) == 0 ||
		   fnmatch("ttyACM*", ent->d_name,0) == 0) {
		ports.push_back(string("/dev/" + ent->d_name);
		}
	}
	closedir(dir);
} else {
	perror("디렉토리 열기 실패");
}

return ports;
}
*/

void signal_handler(int signum) {
    cout << "[INFO] 종료 시그널 감지.. 프로그램 종료 중.." << endl;
    running = false;
}

bool setup_serial_port(int fd, speed_t buadrate = B57600) {
    struct  termios newtio = {0};

    newtio.c_cflag = buadrate | CS8 | CLOCAL | CREAD;
    newtio.c_iflag = 0;
    newtio.c_oflag = 0;
    newtio.c_lflag = 0;
    newtio.c_cc[VMIN] = 1;
    newtio.c_cc[VTIME] = 0;

    tcflush(fd, TCIOFLUSH);

    if(tcsetattr(fd, TCSANOW, &newtio) != 0) {
        perror("tcsetattr error");
        return false;
    }

    return true;
    
}

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

string get_gps(string &buffer, int num) {
	string line;
	size_t pos;
	for (int i = 0; i < num; ++i){
		pos = buffer.find("\r\n",pos);
	}

	if (pos !=  string::npos) {
		line = buffer.substr(0, pos);
		return line;
	}
	return "";
}

int main() {

    int gps_fd = open("/dev/ttyUSB0", O_RDWR | O_NOCTTY);
    int motion_fd = open("/dev/ttyUSB1", O_RDWR | O_NOCTTY);
    int sender_fd = open("/dev/ttyUSB4", O_RDWR | O_NOCTTY);

    if (gps_fd < 0) {
        perror("GPS 입력 시리얼 포트 오류 발생");
        return -1;
    }

	if (motion_fd < 0) {
		perror("TOGS 입력 시리얼 포트 오류 발생");
		return -1;
	}

	if (sender_fd < 0) {
		perror("출력 시리얼 포트 오류 발생");
		return -1;
    }

    unsigned char g_buffer[256];
    unsigned char m_buffer[256];
    if (!setup_serial_port(gps_fd)) {
        perror("GPS 시리얼 포트 오류");
        return -1;
    }
    if (!setup_serial_port(motion_fd)) {
        perror("TOGS 시리얼 포트 오류");
        return -1;
    }
    if (!setup_serial_port(sender_fd)) {
        perror("출력 시리얼 포트 오류");
        return -1;
    }
    signal(SIGINT, signal_handler);

    while (running)
    {
        memset(g_buffer, 0, sizeof(g_buffer));
        memset(m_buffer, 0, sizeof(m_buffer));

        fd_set read_fds;
        FD_ZERO(&read_fds);
        FD_SET(gps_fd,&read_fds);
        FD_SET(motion_fd,&read_fds);

        int max_fd = (gps_fd > motion_fd ? gps_fd : motion_fd) + 1;

        struct  timeval timeout;
        timeout.tv_sec = 1;
        timeout.tv_usec = 0;
        
        int ret = select(max_fd, &read_fds, NULL, NULL, &timeout);

        if (ret < 0) {
            perror("select 에러");
            break;
        } else if (ret == 0) {
            cout << "No data" << endl;
            continue;
        }

        if (FD_ISSET(gps_fd, &read_fds)) {
            int len = read(gps_fd, g_buffer, sizeof(g_buffer));
            if (len > 0){
                gps_line_buffer.append((char*)g_buffer, len);
                string line = get_gps(gps_line_buffer,3);
                ssize_t written = write(sender_fd, line.c_str(), line.size());
				if (written < 0) {
					perror("GPS 시리얼 포트 오류 발생");
				}
               }
        }
        if (FD_ISSET(motion_fd, &read_fds)) {
            int len = read(motion_fd, m_buffer, sizeof(m_buffer));
            if (len > 0){
                motion_line_buffer.append((char*)m_buffer, len);
                string line = get_gphdt(motion_line_buffer);
                ssize_t written = write(sender_fd, line.c_str(), line.size());
				if (written < 0) {
					perror("TOGS 시리얼 포트 오류 발생");
				}
            }
        }
		cout << "running..." << endl;
    }
    close(gps_fd);
    close(motion_fd);
    close(sender_fd);
}
