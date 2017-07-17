#ifndef LIBANT_INET_UTILS_HPP_
#define LIBANT_INET_UTILS_HPP_

#include <string>
#include <utility>

/**
 * @brief get ipaddr of the underlying peer sockfd
 * @param peerfd peer sockfd
 * @return true on success, false on failure. if true is returned, pair::first holds the ipaddr;
 *			if false is returned, pair::first holds the error message.
 */
std::pair<std::string, bool> stat_get_peer_ipaddr(int peerfd);

/**
 * @brief 用于通过网卡接口（eth0/eth1/lo...）获取对应的IP地址。支持IPv4和IPv6。
 * @param nif 网卡接口。eth0/eth1/lo...
 * @param af 网络地址类型。1：IPv4，2：IPv6。
 * @return 成功返回对应的IP，失败返回空字符串。
 */
std::string stat_get_ip_addr(const char* nif, int af);

#endif // LIBANT_INET_UTILS_HPP_
