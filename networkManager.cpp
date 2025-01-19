#include <iostream>
#include "functions.hpp"


int create_nm_connection (bool unconfigured, std::string interface, std::string connection, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    if (ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured") {
        std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" ipv4.method disabled ipv6.method disabled ipv4.address \"\" ipv4.gateway \"\" ipv6.address \"\" ipv6.gateway \"\"";
        std::system(nmcli_command.c_str());
    }
    else {
//Debug BEGIN
        std::cout<<"ipv6_autoconf"<<ipv6_autoconf<<std::endl;
//DEBUG END
        std::string argument;
        if (ipv4_assignment == "dhcp") {
            argument += "ipv4.method auto ipv4.address \"\" ipv4.gateway \"\"";
        }
        else if (ipv4_assignment == "static") {
            argument += "ipv4.method manual ipv4.address "+ipv4_address+"/"+ipv4_netmask;
            if (ipv4_gateway != "") {
                argument += " ipv4.gateway "+ipv4_gateway;
            }
        }
        else {
            argument += "ipv4.method disabled";
        }
        if (ipv6_autoconf == "1") {
            argument += " ipv6.method auto ipv6.address \"\" ipv6.gateway \"\"";
        }
        else if (ipv6_assignment == "dhcp") {
            argument += " ipv6.method dhcp ipv6.address \"\" ipv6.gateway \"\"";
        }
        else if (ipv6_assignment == "static") {
            argument += " ipv6.method manual ipv6.address "+ipv6_address+"/"+ipv6_netmask;
            if (ipv6_gateway != "") {
                argument += " ipv6.gateway "+ipv6_gateway;
            }
        }
        else {
            argument += " ipv6.method disabled";
        }
        if (ipv6_accept_ra == "0") {
            std::string accept_ra_cmd = "echo 0 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
            system(accept_ra_cmd.c_str());
        }
        else if (ipv6_accept_ra == "1") {
            std::string accept_ra_cmd = "echo 1 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
            system(accept_ra_cmd.c_str());
        }
        else {
            std::string accept_ra_cmd = "echo 2 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
            system(accept_ra_cmd.c_str());
        }
        if (unconfigured) {
//Debug BEGIN
            std::cout<<"Interface unconfigured"<<std::endl;
//DEBUG END
            std::string nmcli_command = "/usr/bin/nmcli connection add type ethernet ifname "+interface+" con-name "+interface+" "+argument;
            std::system(nmcli_command.c_str());
        }
        else {
//DEBUG BEGIN
            std::cout<<"Interface configured"<<std::endl;
// DEBUG END
            std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" "+argument;
            std::system(nmcli_command.c_str());
        }
    }
    std::string activate_conn_cmd = "/usr/bin/nmcli connection up "+connection;
    std::system(activate_conn_cmd.c_str());
    return 0;
}
