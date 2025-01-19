#include <iostream>
#include "functions.hpp"

int create_networkd_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
//DEBUG BEGIN
    std::cout<<"Networkd started"<<std::endl;
//DEBUG END
    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    if (!unconfigured) {
//DEBUG BEGIN
    std::cout<<"Interface is configured"<<std::endl;
//DEBUG END
        std::string cmd = "/usr/bin/grep -l "+interface+" /etc/systemd/network/*.network";
        filename = exec(cmd.c_str());
        filename.pop_back();
//DEBUG BEGIN
    std::cout<<"Filename: "<<filename<<std::endl;
//DEBUG END
        dns_server = get_nameserver(filename, "DNS", "=");
        std::vector<std::string> nameservers = split(dns_server, " ");
        for(int i = 0; i < nameservers.size(); i++) {
            if(is_ipv4_address((nameservers[i]))) {
                ipv4_nameserver += nameservers[i]+" ";
            }
            else if (is_ipv6_address(nameservers[i])) {
                ipv6_nameserver += nameservers[i]+" ";
            }
        }
        //Remove last space
        if (!ipv4_nameserver.empty() && ipv4_nameserver[ipv4_nameserver.length()-1] == ' ') {
            ipv4_nameserver.erase(ipv4_nameserver.length()-1);
        }
        if (!ipv6_nameserver.empty() && ipv6_nameserver[ipv6_nameserver.length()-1] == ' ') {
            ipv6_nameserver.erase(ipv6_nameserver.length()-1);
        }
        search = get_dns_domains(filename, "Domains", "=");
        //Remove last space
        if (!search.empty() && search[search.length()-1] == ' ') {
            search.erase(search.length()-1);
        }
    }
    std::cout<<"Filename: "<<filename<<std::endl;
    if (filename == "") {
        filename = "/etc/systemd/network/"+interface+".network";
    }
    if(ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured") {
        std::remove(filename.c_str());
        return 0;
    }
    std::ofstream file;
    file.open(filename);
    if (!file.is_open()) {
        std::cerr<<std::endl<<"Error! Could not open file "<<filename<<std::endl<<std::endl;
        return 1;
    }
    file<<"[Match]"<<std::endl;
    file<<"Name="<<interface<<std::endl<<std::endl;
    file<<"[Network]"<<std::endl;
    if (ipv4_assignment == "dhcp" && (ipv6_assignment == "auto" || ipv6_assignment == "dhcp")) {
//DEBUG BEGIN
    std::cout<<"DHCP 4 + 6"<<std::endl;
//DEBUG END
        file<<"DHCP=yes"<<std::endl;
    }
    else if (ipv4_assignment == "dhcp") {
//DEBUG BEGIN
    std::cout<<"DHCP 4"<<std::endl;
//DEBUG END
        file<<"DHCP=ipv4"<<std::endl;
    }
    else if (ipv6_assignment == "auto" || ipv6_assignment == "dhcp") {
//DEBUG BEGIN
    std::cout<<"DHCP 6"<<std::endl;
//DEBUG END
        file<<"DHCP=ipv6"<<std::endl;
    }
//DEBUG BEGIN
    std::cout<<"no DHCP"<<std::endl;
//DEBUG END
    if (ipv4_address != "" && ipv4_assignment == "static") {
//DEBUG BEGIN
    std::cout<<"IPv4 Address: "<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
//DEBUG END
        file<<"Address="<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
    }
    if (ipv4_gateway != "" && ipv4_assignment == "static") {
//DEBUG BEGIN
    std::cout<<"IPv4 Gateway: "<<ipv4_gateway<<std::endl;
//DEBUG END
        file<<"Gateway="<<ipv4_gateway<<std::endl;
    }
    if (ipv4_nameserver != "") {
        std::vector<std::string> nameserver = split(ipv4_nameserver, " ");
        for(int i = 0; i < nameserver.size(); i++) {
            file<<"DNS="<<nameserver[i]<<std::endl;
        }
    }
    if (search != "") {
        file<<"Domains="<<search<<std::endl;
    }
    if (ipv6_address != "" && ipv6_assignment == "static") {
//DEBUG BEGIN
    std::cout<<"IPv6 Address: "<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
//DEBUG END
        file<<"Address="<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
    }
    if (ipv6_gateway != "" && ipv6_assignment == "static") {
//DEBUG BEGIN
    std::cout<<"IPv6 Gateway: "<<ipv6_gateway<<std::endl;
//DEBUG END
        file<<"Gateway="<<ipv6_gateway<<std::endl;
    }
    if (ipv6_nameserver != "") {
        std::vector<std::string> nameserver = split(ipv6_nameserver, " ");
        for(int i = 0; i < nameserver.size(); i++) {
            file<<"DNS="<<nameserver[i]<<std::endl;
        }
    }
    if (ipv6_accept_ra == "0") {
        file<<"IPv6AcceptRA=false"<<std::endl;
    }
    else {
        file<<"IPv6AcceptRA=true"<<std::endl;
    }
    std::string restart_networkd_cmd = "/usr/bin/systemctl restart systemd-networkd";
    system(restart_networkd_cmd.c_str());
    return 0;
}
