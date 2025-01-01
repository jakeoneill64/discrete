#include <sys/socket.h>
#include "datagram.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <errno.h>

int DatagramSocket::bind(u_int32_t port) {
    sockaddr_in server_address{};
    memset(&server_address, 0, sizeof(server_address));
    server_address.sin_family = AF_INET;
    server_address.sin_addr.s_addr = htonl(INADDR_ANY);
    server_address.sin_port = htons(port);

    // Bind the socket to the specified address (optional for client sockets)
    return ::bind(m_fileDescriptor, (struct sockaddr*)&server_address, sizeof(struct sockaddr_in));
}
// TODO we would potentially want to support AF_UNIX in the future.
DatagramSocket::DatagramSocket(bool ipv6) {
    //TODO handling issues.
    m_fileDescriptor = socket(ipv6 ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
    int x = bind(1637);
    int u = 0;
}

DatagramSocket::~DatagramSocket() {
     close(m_fileDescriptor);
}

void DatagramSocket::send(DatagramPacket packet, const char *addr, uint32_t port) {
    sockaddr_in destinationAddress{};
    destinationAddress.sin_port = htons(port);
    destinationAddress.sin_addr.s_addr = inet_addr(addr);
    destinationAddress.sin_family = AF_INET;
    int q = sendto(m_fileDescriptor, packet.m_data, packet.m_size, 0, (sockaddr *)(&destinationAddress), sizeof(destinationAddress));
    auto x = errno;

    // Get a detailed string representation of the error using strerror
    int y = 0 ;
}

std::shared_ptr<DatagramPacket> DatagramSocket::receive() {
}

void DatagramSocket::updatePacketPool() {

}
