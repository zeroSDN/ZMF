/**
* @author Jan Strauß
* @date created on 7/7/15.
*/

#include <util/NetworkUtil.hpp>

Poco::Net::NetworkInterface zmf::util::NetworkUtil::getNetworkInterface(Poco::Net::IPAddress::Family ip_version) {
    Poco::Net::NetworkInterface::Map m = Poco::Net::NetworkInterface::map();
    if (ip_version == Poco::Net::IPAddress::IPv4) {
        for (Poco::Net::NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it) {
            if (it->second.supportsIPv4() &&
                it->second.firstAddress(Poco::Net::IPAddress::IPv4).isUnicast() &&
                !it->second.isLoopback() &&
                !it->second.isPointToPoint()) {
                return it->second;
            }
        }
    }
#ifdef POCO_HAVE_IPv6
    else if (ip_version == Poco::Net::IPAddress::IPv6) {
        for (Poco::Net::NetworkInterface::Map::const_iterator it = m.begin(); it != m.end(); ++it) {
            if (it->second.supportsIPv6() &&
                it->second.firstAddress(Poco::Net::IPAddress::IPv6).isUnicast() &&
                !it->second.isLoopback() &&
                !it->second.isPointToPoint()) {
                return it->second;
            }
        }
    }
#endif // POCO_HAVE_IPv6

    throw Poco::NotFoundException("No multicast-eligible network interface found.");
}

Poco::Net::NetworkInterface zmf::util::NetworkUtil::getNetworkInterface(std::string name,
                                                                        Poco::Net::IPAddress::Family ip_version) {
    return Poco::Net::NetworkInterface::forName(name, ip_version);
}
