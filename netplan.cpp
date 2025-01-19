#include <iostream>
#include <filesystem>
#include "functions.hpp"

int create_netplan_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
//Debug start
    std::cout<<"Neplan config started<br>"<<std::endl;

//Debug endl

    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    if (!unconfigured) {
//Debug start
        std::cout<<"Interface is configured<br>"<<std::endl;
//Debug end


        std::string cmd = "/usr/bin/grep -l "+interface+" /etc/netplan/*.yaml";
        filename = exec(cmd.c_str());
        filename.pop_back();
        std::filesystem::path p = filename;
        filename = p.replace_extension();
//Debug begin
        std::cout<<"Filename is "<<filename<<std::endl;
//Debig end

        std::string dns_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".nameservers.addresses | /usr/bin/grep -v null";
        dns_server = exec(dns_cmd.c_str());
        replace(dns_server.begin(), dns_server.end(), '\n', ' ');
        replace(dns_server.begin(), dns_server.end(), '-', ' ');
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
        std::string search_cmd = "/usr/sbin/netplan get network.ethernets."+interface+".nameservers.search | /usr/bin/grep -v null";
        search = exec(search_cmd.c_str());
        replace(search.begin(), search.end(), '\n', ' ');
        replace(search.begin(), search.end(), '-', ' ');
        //Remove last space
        if (!search.empty() && search[search.length()-1] == ' ') {
            search.erase(search.length()-1);
        }
    }
    if (filename == "") {
        filename = interface;
    }
//Debug start
        std::cout<<"Filename:"<<filename<<std::endl;
//Debug end
    //remove old config
    std::string remove_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+"=\"{addresses: null, dhcp4: false, dhcp6: false, routes: null, nameservers.addresse: null, nameservers.search: null}\"";
    std::system(remove_cmd.c_str());
    std::system("/usr/sbin/netplan apply >> /dev/null 2>&1");
    if (ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured") {
         std::cout<<"Start netplan apply..."<<std::endl;
        int test = std::system("/usr/sbin/netplan apply");
        std::cout<<test<<std::endl;
        return 0;
    }
    if (ipv4_assignment == "dhcp" || ipv6_assignment == "dhcp" || ipv6_assignment == "auto") {
//Debug start
        std::cout<<"DHCP is used<br>"<<std::endl;
//Debug end



        std::string dhcp = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+"=\"{";
        if (ipv4_assignment == "dhcp") {
            dhcp += "dhcp4: true, ";
        }
        else {
            dhcp += "dhcp4: false, ";
        }
        if (ipv6_assignment == "dhcp" || ipv6_assignment == "auto") {
            dhcp += "dhcp6: true}\"";
        }
        else {
            dhcp += "dhcp6: false}\"";
        }
 //Debug start
        std::cout<<"dhcp command:"<<dhcp<<std::endl;
//Debug end
        std::system(dhcp.c_str());
    }
    if (ipv6_accept_ra != "0") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+".accept-ra=true";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_assignment == "static") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+".addresses=["+ipv4_address+"/"+ipv4_netmask+"]";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" ethernets."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv4_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv6_assignment == "static") {
       std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+".addresses=["+ipv6_address+"/"+ipv6_netmask+"]";
       std::system(netplan_cmd.c_str());
    }
    if (ipv6_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" ethernets."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv6_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.ethernets."+interface+".nameservers.addresses=["+ipv4_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
    if (ipv6_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.ethernets."+interface+".nameservers.addresses=["+ipv6_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
     if (search != "") {
        std::string netplan_add_search = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.ethernets."+interface+".nameservers.search=["+search+"\"]";
        std::system(netplan_add_search.c_str());
    }
    std::cout<<"Start netplan apply..."<<std::endl;
    int test = std::system("/usr/sbin/netplan apply");
    std::cout<<test<<std::endl;
    return 0;
}
