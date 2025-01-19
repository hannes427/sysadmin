#include <iostream>
#include "functions.hpp"
#include <filesystem>






#include <algorithm>
#include <iterator>


std::string interface;
std::string managed_by;
std::string connection;
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

int remove_interface (std::string interface);
int create_ifupdown_file(bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra);
int create_netplan_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra);
int create_networkd_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra);
int create_nm_connection (bool unconfigured, std::string interface, std::string connection, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra);

int main (int argc, char **argv) {
    int current_uid = getuid();
    bool unconfigured = false;
//DEBUG BEGIN
    if( argc > 1 )
    {
        int i;
        std::cout << "Command line: "<<argv[0]<<" ";

        std::copy( argv+1, argv+argc, std::ostream_iterator<const char*>( std::cout, " " ) ) ;
    }
//DEBUG END
    std::cout<<std::endl;
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("interface", boost::program_options::value<std::string>()->required(), "name of the interface")
        ("managed-by", boost::program_options::value<std::string>(), "which way is used to configure this interface (ifupdown, netplan, networkd, network-manager)")
        ("connection", boost::program_options::value<std::string>(), "NetworkManager connection (if NetworkManager is used")
        ("ipv4-address", boost::program_options::value<std::string>(), "IPv4 address/netmask (IP/MASK")
        ("ipv4-gateway", boost::program_options::value<std::string>(), "IPv4 default gateway")
        ("ipv4-assignment", boost::program_options::value<std::string>()->required(), "Method to set IPv4 addres (valid input: static, dhcp or unconfigured")
        ("ipv6-address", boost::program_options::value<std::string>(), "IPv6 address")
        ("ipv6-gateway", boost::program_options::value<std::string>(), "IPv6 default gateway")
        ("ipv6-assignment", boost::program_options::value<std::string>()->required(), "Method to set IPv6 addres (valid input: static, dhcp, auto or unconfigured")
        ("ipv6-autoconf", boost::program_options::value<std::string>(), "Use ipv6 autoconf (0=off, 1=on)")
        ("ipv6-accept-ra", boost::program_options::value<std::string>(), "Accept router advertisements (0=off, 1=on, 2=on+forwarding)");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
        }
        if (vm.count("interface")) {
            interface = vm["interface"].as<std::string>();
        }
        if (vm.count("managed-by")) {
            managed_by = vm["managed-by"].as<std::string>();
        }
        if (vm.count("connection")) {
            connection = vm["connection"].as<std::string>();
        }
        if (vm.count("ipv4-assignment")) {
            if (vm["ipv4-assignment"].as<std::string>() == "static") {
                ipv4_assignment = "static";
            }
            else if (vm["ipv4-assignment"].as<std::string>() == "dhcp") {
                ipv4_assignment = "dhcp";
            }
            else if (vm["ipv4-assignment"].as<std::string>() == "unconfigured") {
                ipv4_assignment = "unconfigured";
            }
            else {
                std::cerr<<std::endl<<"Error! IPv4 assignment method! "<<vm["ipv4-assignment"].as<std::string>()<<std::endl<<std::endl;
                return 1;
            }
        }
        if (ipv4_assignment == "static" && vm.count("ipv4-address")) {
            std::vector<std::string> temp_ipv4_address = split(vm["ipv4-address"].as<std::string>(), "/");
            if (temp_ipv4_address.size() <= 1) {
                std::cerr<<std::endl<<"Error! Invalid ipv4 format: "<<ipv4_address<<"Please supply address and netmask (format: ip/netmask)"<<std::endl<<std::endl;
                return 1;
            }
            if(is_ipv4_address((temp_ipv4_address[0]))) {
                ipv4_address = temp_ipv4_address[0];
                if (temp_ipv4_address[1].find_first_not_of("0123456789") != std::string::npos) {
                    std::cerr<<std::endl<<"Error! Invalid Netmask supplied: "<<temp_ipv4_address[1]<<std::endl<<std::endl;
                    return 1;
                }
                else {
                    ipv4_netmask = temp_ipv4_address[1];
                }
            }
            else {
                std::cerr<<std::endl<<"Error! Invalid IPv4 address! "<<ipv4_address<<std::endl<<std::endl;
                return 1;
            }
        }
         if (vm.count("ipv4-gateway")) {
            ipv4_gateway = vm["ipv4-gateway"].as<std::string>();
        }
        if (vm.count("ipv6-assignment")) {
            if (vm["ipv6-assignment"].as<std::string>() == "static") {
                ipv6_assignment = "static";
            }
            else if (vm["ipv6-assignment"].as<std::string>() == "dhcp") {
                ipv6_assignment = "dhcp";
            }
            else if (vm["ipv6-assignment"].as<std::string>() == "auto") {
                ipv6_assignment = "auto";
            }
            else if (vm["ipv6-assignment"].as<std::string>() == "unconfigured") {
                ipv6_assignment = "unconfigured";
            }
            else {
                std::cerr<<std::endl<<"Error! IPv6 assignment method! "<<vm["ipv6-assignment"].as<std::string>()<<std::endl<<std::endl;
                return 1;
            }
        }
        if (ipv6_assignment == "static" &&  vm.count("ipv6-address")) {
        std::vector<std::string> temp_ipv6_address = split(vm["ipv6-address"].as<std::string>(), "/");
            if (temp_ipv6_address.size() <= 1) {
                std::cerr<<std::endl<<"Error! Invalid ipv6 format: "<<ipv6_address<<"Please supply address and netmask (format: ip/netmask)"<<std::endl<<std::endl;
                return 1;
            }
            if(is_ipv6_address((temp_ipv6_address[0]))) {
                ipv6_address = temp_ipv6_address[0];
                if (temp_ipv6_address[1].find_first_not_of("0123456789") != std::string::npos) {
                    std::cerr<<std::endl<<"Error! Invalid Netmask supplied: "<<temp_ipv6_address[1]<<std::endl<<std::endl;
                    return 1;
                }
                else {
                    ipv6_netmask = temp_ipv6_address[1];
                }
            }
            else {
                std::cerr<<std::endl<<"Error! Invalid IPv6 address! "<<ipv6_address<<std::endl<<std::endl;
                return 1;
            }
        }
        if (vm.count("ipv6-autoconf")) {
            if (vm["ipv6-autoconf"].as<std::string>() == "0") {
                ipv6_autoconf = "0";
            }
            else if (vm["ipv6-autoconf"].as<std::string>() == "1") {
                 ipv6_autoconf = "1";
            }
            else {
                std::cerr<<std::endl<<"Error! IPv6 autoconf! "<<vm["ipv6-autoconf"].as<std::string>()<<std::endl<<std::endl;
                return 1;
            }
        }
        if (vm.count("ipv6-accept-ra")) {
            if (vm["ipv6-accept-ra"].as<std::string>() == "0") {
                ipv6_accept_ra = "0";
            }
            else if (vm["ipv6-accept-ra"].as<std::string>() == "1") {
                 ipv6_accept_ra = "1";
            }
            else if (vm["ipv6-accept-ra"].as<std::string>() == "2") {
                 ipv6_accept_ra = "2";
            }
            else {
                std::cerr<<std::endl<<"Error! IPv6 accept_ra! "<<vm["ipv6-accept-ra"].as<std::string>()<<std::endl<<std::endl;
                return 1;
            }
        }
         if (vm.count("ipv6-gateway")) {
            ipv6_gateway = vm["ipv6-gateway"].as<std::string>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
        return 1;
    }
    if (ipv4_gateway != "" and !is_ipv4_address(ipv4_gateway)) {
        std::cerr<<std::endl<<"Error! Invalid IPv4 gateway! "<<ipv4_gateway<<std::endl<<std::endl;
        return 1;
    }
     if (ipv6_gateway != "" and !is_ipv6_address(ipv6_gateway)) {
        std::cerr<<std::endl<<"Error! Invalid IPv6 gateway! "<<ipv6_gateway<<std::endl<<std::endl;
        return 1;
    }
     if (setuid(0)) { //I am now root!
             perror("setuid");
             return 1;
        }
    if (managed_by == "ifupdown") { //Interface is managed by ifupdown
        create_ifupdown_file (unconfigured, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else if (managed_by == "network_manager") { //Interface is managed by NetworkManager
        if(connection == "") {
            unconfigured = true;
        }
        create_nm_connection (unconfigured, interface, connection, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else if (managed_by == "networkd") {  //Interface is managed by systemd
        if (!std::filesystem::is_empty("/etc/systemd/network")) {
            std::string cmd = "/usr/bin/grep -q "+interface+" /etc/systemd/network/*.network";
            int check_configured = std::system(cmd.c_str());
            if(check_configured != 0) {
                unconfigured = true;
            }
            else {
                unconfigured = false;
            }
        }
        else {
            unconfigured = true;
        }
        create_networkd_file (unconfigured, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else if (managed_by == "netplan") {
//Debug start
            std::cout<<"Netplan is used"<<std::endl;
//Debug end
         if (!std::filesystem::is_empty("/etc/netplan")) {
//Debug start
             std::cout<<"Hallo"<<std::endl;
//Debug END
            std::string cmd = "/usr/bin/grep -q "+interface+" /etc/netplan/*.yaml";
            int check_configured = std::system(cmd.c_str());
            if(check_configured != 0) {
//Debug start
             std::cout<<"Hallo123"<<std::endl;
//Debug END
                unconfigured = true;
            }
            else {
//Debug start
             std::cout<<"Hallo456"<<std::endl;
//Debug END
                unconfigured = false;
            }
        }
        else {
//Debug start
             std::cout<<"Hallo789"<<std::endl;
//Debug END
            unconfigured = true;
        }
        create_netplan_file (unconfigured, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else { //interface is unconfigured
        bool unconfigured = true;
        remove_interface (interface);
        create_ifupdown_file (unconfigured, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    setuid(current_uid);
    return 0;
}
