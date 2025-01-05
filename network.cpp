#include <iostream>
#include <filesystem>
#include <fstream>
#include <string>
#include "functions.hpp"
#include <boost/program_options.hpp>

std::string interface;
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

/**
 * Remove Interface from /etc/network/interfaces
 * @param Interface name
 */
int remove_interface (std::string interface) {
    std::vector<std::string> output;
    std::string filepath = std::string("/etc/network/interfaces");
	std::string line;
	std::ifstream file(filepath);
	if (!file.is_open()) {
		std::cerr<<std::endl<<"Error! Could not open file /etc/network/interfaces"<<std::endl<<std::endl;
        return 1;
    }
	bool outputLines = true;
	while(std::getline(file, line)) {
        if (line.find("allow-hotplug "+interface) != std::string::npos || line.find("iface "+interface) != std::string::npos) {
			outputLines = false;
			continue;
		}

		if (line.find("allow-hotplug") != std::string::npos || line.find("iface") != std::string::npos) {
			outputLines = true;
		}
		std::size_t found = line.find("#");
		if(outputLines || line == "" || found!=std::string::npos) {
			output.push_back({line});
		}
	}
	file.close();
	std::ofstream outputfile("/etc/network/interfaces");
	if (outputfile.is_open())	{
		for (const auto& outputline : output) {
			outputfile << outputline<<std::endl;
		}
		outputfile.close();
	}
	else {
		std::cerr<<std::endl<<"Error! Could not open file /etc/network/interfaces"<<std::endl<<std::endl;
        return 1;
	}
	return 0;
}

int create_ifupdown_file(bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    std::string interface_file;
    std::string nameservers_cmd;
    std::string dns_server;
    std::string search;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    if (unconfigured) {
        std::string find_interface = "/usr/bin/grep -qr "+interface+" /etc/network/interfaces.d";
        int interface_found = system(find_interface.c_str());
        if(interface_found != 0) {
            interface_file = "/etc/network/interfaces.d/"+interface;
        }
        else {
            std::string cmd = "/usr/bin/grep -rl "+interface+" /etc/network/interfaces.d";
            interface_file = exec(cmd.c_str());
            replace(interface_file.begin(), interface_file.end(), '\n', ' ');
            interface_file.pop_back();
        }
    }
    else {
        std::string find_interface = "/usr/bin/grep -qr "+interface+" /etc/network/interfaces.d";
        int interface_found = system(find_interface.c_str());
        if(interface_found == 0) {
            std::string cmd = "/usr/bin/grep -rl "+interface+" /etc/network/interfaces.d";
            interface_file = exec(cmd.c_str());
            replace(interface_file.begin(), interface_file.end(), '\n', ' ');
            interface_file.pop_back();
        }
    }
    if (interface_file != "") { //file for this interface exists in /etc/network/interfaces.d
        if(!unconfigured) {
            dns_server = get_nameserver(interface_file, "dns-nameservers", " ");
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
            search = get_dns_domains(interface_file, "dns-search", " ");
            //Remove last space
            if (!search.empty() && search[search.length()-1] == ' ') {
                search.erase(search.length()-1);
            }
        }
        else {
            nameservers_cmd = "/usr/bin/grep dns-nameservers /etc/network/interfaces";
            dns_server = get_nameserver("/etc/network/interfaces", "dns-nameservers", " ");
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
            search = get_dns_domains("/etc/network/interfaces", "dns-search", " ");
            //Remove last space
            if (!search.empty() && search[search.length()-1] == ' ') {
                search.erase(search.length()-1);
            }
        }
        std::ofstream file;
        file.open(interface_file);
        if (!file.is_open()) {
            std::cerr<<std::endl<<"Error! Could not open file "<<interface_file<<std::endl<<std::endl;
            return 1;
        }
        file<<"allow-hotplug "<<interface<<std::endl;
        if (ipv4_assignment == "dhcp") {
            file<<"iface "<<interface<<" inet dhcp"<<std::endl;
        }
        else if (ipv4_address != "") {
            file<<"iface "<<interface<<" inet static"<<std::endl;
            file<<"address "<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
        }
        if (ipv4_gateway != "") {
            file<<"gateway "<<ipv4_gateway<<std::endl;
        }
        if (ipv4_nameserver != "" and ipv4_address != "") {
            file<<"dns-nameservers "<<ipv4_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if (ipv6_assignment == "dhcp") {
            file<<"iface "<<interface<<" inet6 dhcp"<<std::endl;
        }
        else if (ipv6_assignment == "auto") {
            file<<"iface "<<interface<<" inet6 auto"<<std::endl;
        }
        else if (ipv6_address != "") {
            file<<"iface "<<interface<<" inet6 static"<<std::endl;
            file<<"address "<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
        }
        if (ipv6_gateway != "") {
            file<<"gateway "<<ipv6_gateway<<std::endl;
        }
        if (ipv6_nameserver != "" and ipv6_address != "") {
            file<<"dns-nameservers "<<ipv6_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if(ipv6_autoconf != "") {
            file<<"autoconf "<<ipv6_autoconf<<std::endl;
        }
        if (ipv6_accept_ra != "") {
            file<<"accept_ra "<<ipv6_accept_ra<<std::endl;
        }
        file.close();
    }
    else {
        dns_server = get_nameserver("/etc/network/interfaces", "dns-nameservers", " ");
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
        search = get_dns_domains("/etc/network/interfaces", "dns-search", " ");
        //Remove last space
        if (!search.empty() && search[search.length()-1] == ' ') {
            search.erase(search.length()-1);
        }
        remove_interface (interface);
        interface_file = "/etc/network/interfaces.d/"+interface;
        std::ofstream file;
        file.open(interface_file);
        if (!file.is_open()) {
            std::cerr<<std::endl<<"Error! Could not open file "<<interface_file<<std::endl<<std::endl;
            return 1;
        }
        file<<"allow-hotplug "<<interface<<std::endl;
        if (ipv4_assignment == "dhcp") {
            file<<"iface "<<interface<<" inet dhcp"<<std::endl;
        }
        else if (ipv4_address != "") {
            file<<"iface "<<interface<<" inet static"<<std::endl;
            file<<"address "<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
        }
        if (ipv4_gateway != "") {
            file<<"gateway "<<ipv4_gateway<<std::endl;
        }
        if (ipv4_nameserver != "" and ipv4_address != "") {
            file<<"dns-nameservers "<<ipv4_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if (ipv6_assignment == "dhcp") {
            file<<"iface "<<interface<<" inet6 dhcp"<<std::endl;
        }
        else if (ipv6_assignment == "auto") {
            file<<"iface "<<interface<<" inet6 auto"<<std::endl;
        }
        else if (ipv6_address != "") {
            file<<"iface "<<interface<<" inet6 static"<<std::endl;
            file<<"address "<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
        }
        if (ipv6_gateway != "") {
            file<<"gateway "<<ipv6_gateway<<std::endl;
        }
        if (ipv6_nameserver != "" and ipv6_address != "") {
            file<<"dns-nameservers "<<ipv6_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if(ipv6_autoconf != "") {
            file<<"autoconf "<<ipv6_autoconf<<std::endl;
        }
        if (ipv6_accept_ra != "") {
            file<<"accept_ra "<<ipv6_accept_ra<<std::endl;
        }
        file.close();
    }
    return 0;
}

int create_nm_connection(bool unconfigured, std::string interface, std::string connection, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    if (ipv4_assignment == "unconfigured" and ipv6_assignment == "unconfigured") {
        std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" ipv4.method disabled ipv6.method disabled ipv4.address \"\" ipv4.gateway \"\" ipv6.address \"\" ipv6.gateway \"\"";
        std::system(nmcli_command.c_str());
    }
    else {
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
        if (ipv6_assignment == "auto") {
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
        if (unconfigured) {
            std::string nmcli_command = "/usr/bin/nmcli connection add type ethernet ifname "+interface+" con-name "+interface+" "+argument;
            std::system(nmcli_command.c_str());
        }
        else {
            std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" "+argument;
            std::system(nmcli_command.c_str());
        }
    }
    return 0;
}

int create_networkd_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    if (!unconfigured) {
        std::string cmd = "/usr/bin/grep -l "+interface+" /etc/systemd/network/*.network";
        std::string filename = exec(cmd.c_str());
        filename.pop_back();
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
    if (ipv4_assignment == "dhcp" and (ipv6_assignment == "auto" || ipv6_assignment == "dhcp")) {
        file<<"DHCP=yes"<<std::endl;
    }
    else if (ipv4_assignment == "dhcp") {
        file<<"DHCP=ipv4"<<std::endl;
    }
    else if (ipv6_assignment == "auto" || ipv6_assignment == "dhcp") {
        file<<"DHCP=ipv6"<<std::endl;
    }
    else {
        if (ipv4_address != "") {
            file<<"Address="<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
        }
        if (ipv4_gateway != "") {
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
        if (ipv6_address != "") {
            file<<"Address="<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
        }
        if (ipv6_gateway != "") {
            file<<"Gateway="<<ipv6_gateway<<std::endl;
        }
        if (ipv6_nameserver != "") {
            std::vector<std::string> nameserver = split(ipv6_nameserver, " ");
            for(int i = 0; i < nameserver.size(); i++) {
                file<<"DNS="<<nameserver[i]<<std::endl;
            }
        }
    }
    return 0;
}

int create_netplan_file (bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    if (!unconfigured) {
        std::string cmd = "/usr/bin/grep -l "+interface+" /etc/netplan/*.yaml";
        std::string filename = exec(cmd.c_str());
        filename.pop_back();
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
        filename = "/etc/netplan/"+interface+".yaml";
    }
    //remove old config
    std::remove(filename.c_str());
    std::system("/usr/sbin/netplan apply >> /dev/null 2>&1");
    if (ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured") {
        return 0;
    }
    if (ipv4_assignment == "dhcp" || ipv6_assignment == "dhcp" || ipv6_assignment == "auto") {
        std::string dhcp;
        if (ipv4_assignment == "dhcp") {
            dhcp += " network.ethernets."+interface+".dhcp4=true";
        }
        if (ipv6_assignment == "dhcp" || ipv6_assignment == "auto") {
            dhcp += " network.ethernets."+interface+".dhcp6=true";
        }
        std::system(dhcp.c_str());
        return 0;
    }
    if (ipv6_accept_ra != "0") {
        std::string netplan_cmd = "/usr/sbin/netplan set network.ethernets."+interface+".accept-ra=true";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_assignment == "static") {
        std::string netplan_cmd = "/usr/sbin/netplan set network.ethernets."+interface+".addresses=["+ipv4_address+"/"+ipv4_netmask+"]";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set ethernets."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv4_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv6_assignment == "static") {
       std::string netplan_cmd = "/usr/sbin/netplan set network.ethernets."+interface+".addresses=["+ipv6_address+"/"+ipv6_netmask+"]";
       std::system(netplan_cmd.c_str());
    }
    if (ipv6_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set ethernets."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv6_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set \"network.ethernets."+interface+".nameservers.addresses=["+ipv4_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
    if (ipv6_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set \"network.ethernets."+interface+".nameservers.addresses=["+ipv6_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
     if (search != "") {
        std::string netplan_add_search = "/usr/sbin/netplan set \"network.ethernets."+interface+".nameservers.search=["+search+"\"]";
        std::system(netplan_add_search.c_str());
    }
    std::system("/usr/sbin/netplan apply");
    return 0;
}

int main (int argc, char **argv) {
    int current_uid = getuid();
    bool unconfigured = false;
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("interface", boost::program_options::value<std::string>()->required(), "name of the interface")
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
        if (vm.count("connection")) {
            connection = vm["connection"].as<std::string>();
        }
        if (vm.count("ipv4-address")) {
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
         if (vm.count("ipv4-gateway")) {
            ipv4_gateway = vm["ipv4-gateway"].as<std::string>();
        }
        if (vm.count("ipv6-address")) {
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
    if (check_ifupdown(interface)) { //Interface is managed by ifupdown
        create_ifupdown_file (unconfigured, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else if (check_networkManager(interface)) { //Interface is managed by NetworkManager
        if(connection == "") {
            unconfigured = true;
        }
        create_nm_connection (unconfigured, interface, connection, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra);
    }
    else if (check_systemd_networkd(interface)) {  //Interface is managed by systemd
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
    else if (check_netplan(interface)) {
         if (!std::filesystem::is_empty("/etc/netplan")) {
            std::string cmd = "/usr/bin/grep -q "+interface+" /etc/netplan/*.yaml";
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
