#include <unistd.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <fcntl.h>
#include <sys/resource.h>
#include <string.h>
#include <iostream>

#define MAXEPOLLSIZE 10240

using namespace std;

int setnonblocking(int sockfd) {
    if (fcntl(sockfd, F_SETFL, fcntl(sockfd, F_GETFD, 0) | O_NONBLOCK) == -1) {
        return -1;
    }
    return 0;
}

int handle(int fd) {
    cout << "handling fd=" << fd << endl;
    return 0;
}

int main(int argc, char **argv) {
    struct rlimit rt;
    rt.rlim_max = rt.rlim_cur = MAXEPOLLSIZE;
    if (setrlimit(RLIMIT_NOFILE, &rt) == -1) {
        cerr << "setrlimit failed" << endl;
        return -1;
    }

    int udp_listener;
    if ((udp_listener = socket(PF_INET, SOCK_DGRAM, 0)) == -1) {
        cerr << "socket create failed" << endl;
        return -1;
    }

    int opt = SO_REUSEADDR;
    setsockopt(udp_listener, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));

    setnonblocking(udp_listener);

    struct sockaddr_in local_addr;
    memset(&local_addr, 0, sizeof(local_addr));

    int server_port = 8080;
    local_addr.sin_family = PF_INET;
    local_addr.sin_port = htons(server_port);
    local_addr.sin_addr.s_addr = INADDR_ANY;
    if (bind(udp_listener, (struct sockaddr *) &local_addr, sizeof(struct sockaddr)) == -1) {
        cerr << "bind failed" << endl;
        return -1;
    }

    int epoll_fd = epoll_create(MAXEPOLLSIZE);
    struct epoll_event ev;
    struct epoll_event events[MAXEPOLLSIZE];
    ev.events = EPOLLIN | EPOLLET;
    ev.data.fd = udp_listener;
    if (epoll_ctl(epoll_fd, EPOLL_CTL_ADD, udp_listener, &ev) < 0) {
        cerr << "epoll_ctl error with fd=" << epoll_fd << endl;
        return -1;
    }

    for ( ; ; ) {
        int n_fds = epoll_wait(epoll_fd, events, 10000, -1);
        if (n_fds == -1) {
            cerr << "epoll_wait error" << endl;
            return -1;
        }

        for (int i = 0; i < n_fds; i++) {
            int fd = events[i].data.fd;
            handle(fd);
        }
    }
    return 0;
}
