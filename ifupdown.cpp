#include <iostream>
#include "functions.hpp"

/**
 * Remove Interface from /etc/network/interfaces
 * @param Interface name
 */
int remove_interface (std::string interface) {
//Debug start
    std::cout<<"call of remove_interface ("<<interface<<") started"<<std::endl;

//Debug end

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
//Debug start
        std::cout<<line<<std::endl;

//Debug end
        if (line.find("allow-hotplug "+interface) != std::string::npos || line.find("iface "+interface) != std::string::npos) {
			outputLines = false;
//Debug start
            std::cout<<"Outputline aus "<<line<<std::endl;

//Debug end
			continue;
		}

		if (line.find("allow-hotplug") != std::string::npos || line.find("iface") != std::string::npos) {
			outputLines = true;
//Debug start
            std::cout<<"Outputline ein "<<line<<std::endl;

//Debug end
		}
		std::size_t found = line.find("#");
		if(outputLines || line == "" || found!=std::string::npos) {
			output.push_back({line});
		}
	}
	file.close();
//Debug start
    std::cout<<"Writing new /etc/network/interfaces. Content:"<<std::endl;

//Debug end

	std::ofstream outputfile("/etc/network/interfaces");
	if (outputfile.is_open())	{
		for (const auto& outputline : output) {
//DEbug start
            std::cout<<outputline<<std::endl;

//Debug end


			outputfile << outputline<<std::endl;
		}
		outputfile.close();
	}
	else {
		std::cerr<<std::endl<<"Error! Could not open file /etc/network/interfaces"<<std::endl<<std::endl;
        return 1;
	}
	 std::cout<<"call of remove_interface ("<<interface<<") ended"<<std::endl;
	return 0;
}

int create_ifupdown_file(bool unconfigured, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra) {
    std::string ifdown_cmd = "/usr/sbin/ifdown "+interface;
    system(ifdown_cmd.c_str());
//Debug start
    std::cout<<"Call of create_ifupdown_file started Values are: unconfigured: "<<unconfigured<<", interface: "<<interface<<", ipv4_address: "<<ipv4_address<<", ipv4_netmask: "<<ipv4_netmask<<", ipv4_gateway: "<<ipv4_gateway<<", ipv4_assignment: "<<ipv4_assignment<<", ipv6_address: "<<ipv6_address<<", ipv6_netmask: "<<ipv6_netmask<<", ipv6_gateway: "<<ipv6_gateway<<", ipv6_assignment: "<<ipv6_assignment<<", ipv6_autoconf: "<<ipv6_autoconf<<", ipv6_accept_ra: "<<ipv6_accept_ra<<std::endl;

//Debug end


    std::string interface_file;
    std::string nameservers_cmd;
    std::string dns_server;
    std::string search;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
//Debug start
            std::cout<<"Start checking if interface is unconfigured or not"<<std::endl;

//Debug end
    if (unconfigured) {
        std::string find_interface = "/usr/bin/grep -qr "+interface+" /etc/network/interfaces.d";
        int interface_found = system(find_interface.c_str());
        if(interface_found != 0) {
//Debug begin
            std::cout<<"Interface is unconfigured, and no file for this interface found in /etc/network/interfaces.d/. interface_file is /etc/network/interfaces.d/"<<interface<<std::endl;

//Debug end
            interface_file = "/etc/network/interfaces.d/"+interface;
        }
        else {
            std::string cmd = "/usr/bin/grep -rl "+interface+" /etc/network/interfaces.d";
            interface_file = exec(cmd.c_str());
            replace(interface_file.begin(), interface_file.end(), '\n', ' ');
            interface_file.pop_back();
//Debug start
            std::cout<<"Interface is unconfigured, but file for this interface found in /etc/network/interfaces.d/. interface_file is "<<interface_file<<std::endl;

//DEbug end
        }
    }
    else {
//Debug start
        std::cout<<"Interface is configured. Searching for config-file..."<<std::endl;

//Debug end
        std::string find_interface = "/usr/bin/grep -qr "+interface+" /etc/network/interfaces.d";
        int interface_found = system(find_interface.c_str());
        if(interface_found == 0) {
            std::string cmd = "/usr/bin/grep -rl "+interface+" /etc/network/interfaces.d";
            interface_file = exec(cmd.c_str());
            replace(interface_file.begin(), interface_file.end(), '\n', ' ');
            interface_file.pop_back();
//Debug start
            std::cout<<"Fond file: "<<interface_file<<std::endl;

//Debug end
        }
    }
//Debug start
            std::cout<<"Stop checking if interface is unconfigured or not interface_file is"<<interface_file<<std::endl;

//Debug end
    if (interface_file != "") { //file for this interface exists in /etc/network/interfaces.d
        if(!unconfigured) {
//Debug start
            std::cout<<"serching for nameservers and search-domains..."<<std::endl;

//Debug end
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
//Debug start
            std::cout<<"serch finished Nameservers IPv4: "<<ipv4_nameserver<<", Nameservers IPv6: "<<ipv6_nameserver<<", search-domains: "<<search<<std::endl;

//Debug end
        }

        else {
//Debug start
            std::cout<<"serching for nameservers and search-domains..."<<std::endl;

//Debug end
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
//Debug start
            std::cout<<"serch finished Nameservers IPv4: "<<ipv4_nameserver<<", Nameservers IPv6: "<<ipv6_nameserver<<", search-domains: "<<search<<std::endl;

//Debug end
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
//Debug start
        std::cout<<"Debug: "<<ipv4_nameserver<<" assignment: "<<ipv4_assignment<<std::endl;
//Debug end
        if (ipv4_nameserver != "" && ipv4_assignment != "unconfigured") {
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
        if (ipv6_nameserver != "" && ipv6_assignment != "unconfigured") {
            file<<"dns-nameservers "<<ipv6_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if(ipv6_autoconf != "" && (ipv6_assignment == "static" || ipv6_assignment == "dhcp")) {
            file<<"autoconf "<<ipv6_autoconf<<std::endl;
        }
        if (ipv6_accept_ra != "" && (ipv6_assignment == "static" || ipv6_assignment == "dhcp")) {
            file<<"accept_ra "<<ipv6_accept_ra<<std::endl;
        }
        file.close();
    }
    else {
//Debug start
            std::cout<<"serching for nameservers and search-domains..."<<std::endl;

//Debug end
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
//Debug start
            std::cout<<"serch finished Nameservers IPv4: "<<ipv4_nameserver<<", Nameservers IPv6: "<<ipv6_nameserver<<", search-domains: "<<search<<std::endl;

//Debug end
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
        if (ipv4_nameserver != "" && ipv4_assignment != "unconfigured") {
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
        if (ipv6_nameserver != "" && ipv6_assignment != "unconfigured") {
            file<<"dns-nameservers "<<ipv6_nameserver<<std::endl;
            if (search != "") {
                file<<"dns-search "<<search<<std::endl;
            }
        }
        if(ipv6_autoconf != "" && (ipv6_assignment == "static" || ipv6_assignment == "dhcp")) {
            file<<"autoconf "<<ipv6_autoconf<<std::endl;
        }
        if (ipv6_accept_ra != "" && (ipv6_assignment == "static" || ipv6_assignment == "dhcp")) {
            file<<"accept_ra "<<ipv6_accept_ra<<std::endl;
        }
        file.close();
    }
    std::string ifup_cmd = "/usr/sbin/ifup "+interface;
    system(ifup_cmd.c_str());
    return 0;
}
