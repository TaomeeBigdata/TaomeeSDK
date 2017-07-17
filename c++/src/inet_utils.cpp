#include <cerrno>
#include <cstring>

#include <ifaddrs.h>
#include <sys/socket.h>
#include <netdb.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <sys/types.h>
#include <sys/socket.h>

#include "inet_utils.h"

using namespace std;

pair<string, bool> stat_get_peer_ipaddr(int peerfd)
{
	sockaddr_storage saddr;
	socklen_t slen = sizeof(saddr);
	if (getpeername(peerfd, reinterpret_cast<sockaddr*>(&saddr), &slen) == -1) {
		return make_pair(strerror(errno), false);
	}

	bool   ret = false;
	string ip;
	char   ipaddr[INET6_ADDRSTRLEN];
	switch (saddr.ss_family) {
	case AF_INET: {
		sockaddr_in* s = reinterpret_cast<sockaddr_in*>(&saddr);
		if (inet_ntop(AF_INET, reinterpret_cast<void*>(&s->sin_addr), ipaddr, sizeof(ipaddr))) {
			ret = true;
			ip  = ipaddr;
		} else {
			ip  = strerror(errno);
		}
		break;
	}
	case AF_INET6: {
		sockaddr_in6* s = reinterpret_cast<sockaddr_in6*>(&saddr);
		if (inet_ntop(AF_INET6, reinterpret_cast<void*>(&s->sin6_addr), ipaddr, sizeof(ipaddr))) {
			ret = true;
			ip	= ipaddr;
		} else {
			ip	= strerror(errno);
		}
		break;
	}
	default:
		ip = "Unsupported Address Family!";
		break;
	}
	
	return make_pair(ip, ret);
}

string stat_get_ip_addr(const char* nif, int af)
{
	if (af == 1) {
		af = AF_INET;
	} else {
		af = AF_INET6;
	}

	string ip;
	// get a list of network interfaces
	ifaddrs* ifaddr;
	if (getifaddrs(&ifaddr) < 0) {
		return ip;
	}
	// walk through linked list
	char ipaddr[128];
	int  ret_code = -1;
	for (ifaddrs* ifa = ifaddr; ifa != 0; ifa = ifa->ifa_next) {
		if ((ifa->ifa_addr == 0) || (ifa->ifa_addr->sa_family != af)
				|| strcmp(ifa->ifa_name, nif)) {
			continue;
		}
		// convert binary form ip address to numeric string form
		ret_code = getnameinfo(ifa->ifa_addr,
								(af == AF_INET) ? sizeof(sockaddr_in) : sizeof(sockaddr_in6),
								ipaddr, sizeof(ipaddr), 0, 0, NI_NUMERICHOST);
		break;
	}

	freeifaddrs(ifaddr);

	if (ret_code == 0) {
		ip = ipaddr;
	}
	return ip;
}
