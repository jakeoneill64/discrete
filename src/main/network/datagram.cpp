//
// Created by Jake M O'Neill on 26/01/2024.
//

#include <sys/socket.h>
#include "datagram.h"
#include <unistd.h>
#include <netinet/in.h>
#include <arpa/inet.h>

void DatagramSocket::bind(u_int32_t port) {

}
// TODO we would potentially want to support AF_UNIX in the future.
DatagramSocket::DatagramSocket(bool ipv6) {
    //TODO handling issues.
    m_fileDescriptor = socket(ipv6 ? AF_INET6 : AF_INET, SOCK_DGRAM, 0);
}

DatagramSocket::~DatagramSocket() {
     close(m_fileDescriptor);
}

void DatagramSocket::send(DatagramPacket packet, const char *addr, uint32_t port) {
    sockaddr_in destinationAddress{};
    destinationAddress.sin_port = htons(port);
    destinationAddress.sin_addr.s_addr = inet_addr(addr);
    destinationAddress.sin_family = AF_INET;
    sendto(m_fileDescriptor, packet.m_data, packet.m_size, 0, reinterpret_cast<const sockaddr *>(&destinationAddress), sizeof(destinationAddress));
}

std::shared_ptr<DatagramPacket> DatagramSocket::receive() {
}

void DatagramSocket::updatePacketPool() {

}
