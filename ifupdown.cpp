#include "functions.hpp"

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
        if (line.find("allow-hotplug "+interface) != std::string::npos || line.find("iface "+interface) != std::string::npos || line.find("auto "+interface) != std::string::npos) {
			outputLines = false;
			continue;
		}

		if (line.find("allow-hotplug") != std::string::npos || line.find("iface") != std::string::npos || line.find("auto") != std::string::npos) {
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

int delete_ifupdown_file (const std::string& interface) {
    if (std::filesystem::exists("/etc/network/interfaces.d/"+interface)) {
        std::string ifdown_cmd = "/usr/sbin/ifdown "+interface;
        system(ifdown_cmd.c_str());
        std::filesystem::remove("/etc/network/interfaces.d/"+interface);
    }
return 0;
}

int write_interface_file (const bool& create_new_interface, const std::string& filepath, const std::string& interface, const std::string& ipv4_address, const std::string& ipv4_netmask, const std::string& ipv4_gateway, const std::string& ipv4_assignment, const std::string& ipv6_address, const std::string& ipv6_netmask, const std::string& ipv6_gateway, const std::string& ipv6_assignment, const std::string& ipv6_autoconf, const std::string& ipv6_accept_ra, const std::string& type, const std::string& ipv4_nameserver, const std::string& search, const std::string& ipv6_nameserver,  std::string& mode, const std::string& bond_member, const std::string& lacp_rate, const std::string& mii_monitor_interval, const std::string& min_links, const std::string& transmit_hash_policy, const std::string& ad_select, const std::string& all_members_active, const std::string& arp_interval, std::string& arp_ip_targets, const std::string& arp_validate, const std::string& arp_all_targets, const std::string& up_delay, const std::string& down_delay, const std::string& fail_over_mac_policy, const std::string& gratuitous_arp, const std::string& packets_per_member, const std::string& primary_reselect_policy, const std::string& resend_igmp, const std::string& learn_packet_interval, const std::string& primary) {
    std::ofstream file;
    file.open(filepath);
    if (!file.is_open()) {
        std::cerr<<std::endl<<"Error! Could not open file "<<filepath<<std::endl<<std::endl;
        return 1;
    }
    file<<"auto "<<interface<<std::endl;
    if (create_new_interface) {
        file<<"iface "<<interface<<" inet manual"<<std::endl;
    }
    else if (ipv4_assignment == "dhcp") {
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
    if (type == "bond") {
        file<<"bond-mode "<<mode<<std::endl;
        file<<"bond-slaves "<<bond_member<<std::endl;
        if(lacp_rate != "") {
            file<<"bond-lacp-rate "<<lacp_rate<<std::endl;
        }
        if(mii_monitor_interval != "") {
            file<<"bond-miimon "<<mii_monitor_interval<<std::endl;
        }
        if(min_links != "") {
            file<<"bond-min-links "<<min_links<<std::endl;
        }
        if(transmit_hash_policy != "") {
            file<<"bond-xmit-hash-policy "<<transmit_hash_policy<<std::endl;
        }
        if(ad_select != "") {
            file<<"bond-ad-select "<<ad_select<<std::endl;
        }
        if(all_members_active != "") {
            file<<"bond-all-slaves-active "<<all_members_active<<std::endl;
        }
        if(arp_interval != "") {
            file<<"bond-arp-interval "<<arp_interval<<std::endl;
        }
        if(arp_ip_targets != "") {
            replaceAll(arp_ip_targets, ",", " ");
            file<<"bond-arp-ip-target "<<arp_ip_targets<<std::endl;
        }
        if(arp_validate != "") {
            file<<"bond-arp-validate "<<arp_validate<<std::endl;
        }
        if(arp_all_targets != "") {
            file<<"bond-arp-all-targets "<<arp_all_targets<<std::endl;
        }
        if(up_delay != "") {
            file<<"bond-updelay "<<up_delay<<std::endl;
        }
        if(down_delay != "") {
            file<<"bond-downdelay "<<down_delay<<std::endl;
        }
        if(fail_over_mac_policy != "") {
            file<<"fail-over-mac-policy "<<fail_over_mac_policy<<std::endl;
        }
        if(gratuitous_arp != "") {
            file<<"bond-num-grat-arp "<<gratuitous_arp<<std::endl;
        }
        if(packets_per_member != "") {
            file<<"bond-packets-per-slave "<<packets_per_member<<std::endl;
        }
        if(primary_reselect_policy != "") {
            file<<"bond-primary-reselect "<<primary_reselect_policy<<std::endl;
        }
        if(resend_igmp != "") {
            file<<"bond-resend-igmp "<<resend_igmp<<std::endl;
        }
        if(learn_packet_interval != "") {
            file<<"bond-lp-interval "<<learn_packet_interval<<std::endl;
        }
        if(primary != "") {
            file<<"bond-primary "<<primary<<std::endl;
        }
    }
    file.close();
    return 0;
}

int create_ifupdown_file(bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary) {
    replaceAll(bond_member, ",", " ");
    std::string ifdown_cmd = "/usr/sbin/ifdown "+interface;
    system(ifdown_cmd.c_str());
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
        if (interface_file == "") {
            interface_file = "/etc/network/interfaces.d/"+interface;
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
        write_interface_file (create_new_interface, interface_file, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, ipv4_nameserver, search, ipv6_nameserver, mode, bond_member, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
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
        write_interface_file (create_new_interface, interface_file, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, ipv4_nameserver, search, ipv6_nameserver, mode, bond_member, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
    }
    std::string ifup_cmd = "/usr/sbin/ifup "+interface;
    system(ifup_cmd.c_str());
    return 0;
}
