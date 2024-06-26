#include "tcp_socket.h"
#include "exception.h"

using namespace std;

void TCPSocket::listen(const int backlog)
{
  check_syscall(::listen(fd_num(), backlog));
}

TCPSocket TCPSocket::accept()
{
  return { check_syscall(::accept(fd_num(), nullptr, nullptr)) };
}
