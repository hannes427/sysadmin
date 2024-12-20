#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "functions.hpp"
#include <boost/program_options.hpp>

std::string interface;
std::string ipv4_address;
std::string ipv4_netmask;
std::string ipv4_gateway;
std::string ipv4_assignment;
std::string ipv6_address;
std::string ipv6_netmask;
std::string ipv6_gateway;
std::string ipv6_assignment;
std::string ipv6_autoconf;
std::string ipv6_accept_ra;

int main () {
    std::string cmd = "/usr/sbin/netplan get network.ethernets.enX1.nameservers.addresses";
    std::string dns_server = exec(cmd.c_str());
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    //return 0;
    std::regex test;
    replace(dns_server.begin(), dns_server.end(), '\n', ' ');
    replace(dns_server.begin(), dns_server.end(), '-', ' ');
    //std::cout<<"Dns-Server: a"<<dns_server<<"e"<<std::endl;
    std::vector<std::string> nameservers = split(dns_server, " ");
    for(int i = 0; i < nameservers.size(); i++) {
        std::cout<<"Nameserver: "<<nameservers[i]<<"test"<<std::endl;
        if(is_ipv4_address((nameservers[i]))) {
            int already_listed = ipv4_nameserver.find(nameservers[i]);
            if (already_listed == std::string::npos) {
                ipv4_nameserver += nameservers[i]+" ";
            }
        }
        else if (is_ipv6_address(nameservers[i])) {
            int already_listed = ipv6_nameserver.find(nameservers[i]);
            if (already_listed == std::string::npos) {
                ipv6_nameserver += nameservers[i]+" ";
            }
        }
    }
    //Remove last space
    if (!ipv4_nameserver.empty() && ipv4_nameserver[ipv4_nameserver.length()-1] == ' ') {
        ipv4_nameserver.erase(ipv4_nameserver.length()-1);
    }
    if (!ipv6_nameserver.empty() && ipv6_nameserver[ipv6_nameserver.length()-1] == ' ') {
        ipv6_nameserver.erase(ipv6_nameserver.length()-1);
    }
    std::cout<<"v4: "<<ipv4_nameserver<<" v6: "<<ipv6_nameserver<<std::endl;

    //std::string search = get_dns_domains("/tmp/nameserver.txt", "Domains", "=");
    std::string search_cmd = "/usr/sbin/netplan get network.ethernets.enX1.nameservers.search";
    std::string search = exec(search_cmd.c_str());
    replace(search.begin(), search.end(), '\n', ' ');
    replace(search.begin(), search.end(), '-', ' ');
    std::cout<<"Search: "<<search<<std::endl;
    return 0;
}
