//
// Created by Jake M O'Neill on 24/01/2024.
//

#ifndef DISCRETE_DATAGRAM_H
#define DISCRETE_DATAGRAM_H

#include <memory>
#include <vector>
#include <cinttypes>
#include <cstddef>

struct DatagramPacket{
    char *m_data;
    std::size_t m_size;
};

class DatagramSocket{
    public:
        explicit DatagramSocket(bool ipv6);
        void bind(u_int32_t port);
        ~DatagramSocket();
        // TODO max size to send should be 508 bytes and we should enforce this.
        // TODO change the signature of this. I want this to support hostnames. maybe use boost::url
        void send(DatagramPacket packet, const char *addr, uint32_t port);
        std::shared_ptr<DatagramPacket> receive();
    private:
        void updatePacketPool();
        int m_fileDescriptor;
        std::vector<std::shared_ptr<DatagramPacket>> m_packetsAvailable;
        std::vector<std::shared_ptr<DatagramPacket>> m_packetsUsed;

};

#endif //DISCRETE_DATAGRAM_H
