#include "functions.hpp"

int delete_networkd_file (std::string interface, std::string type) {
    if (type == "bond") {
        //First, we execute networkctl delete to update the status of the bond member
        std::string update_cmd = "/usr/bin/networkctl delete "+interface;
        system(update_cmd.c_str());

        //Delete all .network-files from bond-member
        std::string find_member_files = "/usr/bin/grep -l Bond="+interface+" /etc/systemd/network/*.network";
        std::string temp_member_files = exec(find_member_files.c_str());
        std::vector<std::string> member_files = split(temp_member_files, "\n");
        std::vector<std::string> network_files;
        for(int j = 0; j < member_files.size(); j++) {
            if (member_files[j] == "") {
                continue;
            }
            std::string member_name_cmd = "/usr/bin/grep Name= "+member_files[j];
            std::string member_name = exec(member_name_cmd.c_str());
            std::filesystem::remove(member_files[j]);
        }
        //Delete .netdev file
        std::string netdev_file_cmd = "/usr/bin/grep -l "+interface+" /etc/systemd/network/*.netdev";
        std::string netdev_file_path = exec(netdev_file_cmd.c_str());
        netdev_file_path.erase(std::remove(netdev_file_path.begin(), netdev_file_path.end(), '\n'), netdev_file_path.cend());
        if (netdev_file_path == "") {
            netdev_file_path = "/etc/systemd/network/"+interface+".netdev";
        }
        if (netdev_file_path != "") {
            std::filesystem::remove(netdev_file_path);
        }
    }
    //Now we remove the interface file
    std::string cmd = "/usr/bin/grep -l Name="+interface+" /etc/systemd/network/*.network";
    std::string filename = exec(cmd.c_str());
    filename.pop_back();
    if (filename != "") {
        std::filesystem::remove(filename);
    }
    if (type == "bond") {
        //Delete all .network-files from bond-member
        std::string find_member_files = "/usr/bin/grep -l Bond="+interface+" /etc/systemd/network/*.network";
        std::string temp_member_files = exec(find_member_files.c_str());
        std::vector<std::string> member_files = split(temp_member_files, "\n");
        std::vector<std::string> network_files;
        for(int j = 0; j < member_files.size(); j++) {
            if (member_files[j] == "") {
                continue;
            }
            std::string member_name_cmd = "/usr/bin/grep Name= "+member_files[j];
            std::string member_name = exec(member_name_cmd.c_str());
            std::filesystem::remove(member_files[j]);
        }
        //Delete .netdev file
        std::string netdev_file_cmd = "/usr/bin/grep -l "+interface+" /etc/systemd/network/*.netdev";
        std::string netdev_file_path = exec(netdev_file_cmd.c_str());
        netdev_file_path.erase(std::remove(netdev_file_path.begin(), netdev_file_path.end(), '\n'), netdev_file_path.cend());
        if (netdev_file_path == "") {
            netdev_file_path = "/etc/systemd/network/"+interface+".netdev";
        }
        if (netdev_file_path != "") {
            std::filesystem::remove(netdev_file_path);
        }
    }
    return 0;
}
int create_networkd_file(bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary) {
    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    if (!unconfigured) {
        std::string cmd = "/usr/bin/grep -l Name="+interface+" /etc/systemd/network/*.network";
        filename = exec(cmd.c_str());
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
    if(ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured" && !create_new_interface) {
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
        file<<"DHCP=yes"<<std::endl;
    }
    else if (ipv4_assignment == "dhcp") {
        file<<"DHCP=ipv4"<<std::endl;
    }
    else if (ipv6_assignment == "auto" || ipv6_assignment == "dhcp") {
        file<<"DHCP=ipv6"<<std::endl;
    }
    if (ipv4_address != "" && ipv4_assignment == "static") {
        file<<"Address="<<ipv4_address<<"/"<<ipv4_netmask<<std::endl;
    }
    if (ipv4_gateway != "" && ipv4_assignment == "static") {
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
        file<<"Address="<<ipv6_address<<"/"<<ipv6_netmask<<std::endl;
    }
    if (ipv6_gateway != "" && ipv6_assignment == "static") {
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
    file.close();
    if (type == "bond") {

        //First, we need to delete the old .network files, since multiple bond members can be defined in a single .network file (and if we did not delete them, the write operation would be very complicated) and to remove the .network files of interfaces that are no longer members of the bond
        std::string find_member_files = "/usr/bin/grep -l Bond="+interface+" /etc/systemd/network/*.network";
        std::string temp_member_files = exec(find_member_files.c_str());
        std::vector<std::string> member_files = split(temp_member_files, "\n");
        std::vector<std::string> network_files;
        for(int j = 0; j < member_files.size(); j++) {
            if (member_files[j] == "") {
                continue;
            }
            std::string member_name_cmd = "/usr/bin/grep Name= "+member_files[j];
            std::string member_name = exec(member_name_cmd.c_str());
            std::filesystem::remove(member_files[j]);
        }
        //Write .network files of the bond members
        std::vector<std::string> new_bond_member = split(bond_member, ",");
        for(int i = 0; i < new_bond_member.size(); i++) {
            std::string network_file_path = "/etc/systemd/network/"+new_bond_member[i]+".network";
            std::ofstream network_file;
            network_file.open(network_file_path);
            if (!network_file.is_open()) {
                std::cerr<<std::endl<<"Error! Could not open file "+network_file_path<<std::endl<<std::endl;
                return 1;
            }
            network_file<<"[Match]"<<std::endl;
            network_file<<"Name="<<new_bond_member[i]<<std::endl<<std::endl;
            network_file<<"[Network]"<<std::endl;
            network_file<<"Bond="<<interface<<std::endl;
            if (primary == new_bond_member[i]) {
                network_file<<"PrimarySlave=true"<<std::endl;
            }
            network_file.close();
        }
        //Write the .netdev file
        //Check if a netdev file already exists
        std::string netdev_file_cmd = "/usr/bin/grep -l "+interface+" /etc/systemd/network/*.netdev";
        std::string netdev_file_path = exec(netdev_file_cmd.c_str());
        netdev_file_path.erase(std::remove(netdev_file_path.begin(), netdev_file_path.end(), '\n'), netdev_file_path.cend());
        if (netdev_file_path == "") {
            netdev_file_path = "/etc/systemd/network/"+interface+".netdev";
        }
        std::ofstream netdev_file;
        netdev_file.open(netdev_file_path);
        if (!netdev_file.is_open()) {
            std::cerr<<std::endl<<"Error! Could not open file "+netdev_file_path<<std::endl<<std::endl;
            return 1;
        }
        netdev_file<<"[NetDev]"<<std::endl;
        netdev_file<<"Name="<<interface<<std::endl;
        netdev_file<<"Kind=bond"<<std::endl<<std::endl;
        netdev_file<<"[Bond]"<<std::endl;
        netdev_file<<"Mode="<<mode<<std::endl;
        if(lacp_rate != "") {
            netdev_file<<"LACPTransmitRate="<<lacp_rate<<std::endl;
        }
        if(mii_monitor_interval != "") {
            int mii = stoi(mii_monitor_interval);
            mii = mii / 1000;
            netdev_file<<"MIIMonitorSec="<<mii<<"s"<<std::endl;
        }
        if(min_links != "") {
            netdev_file<<"MinLinks="<<min_links<<std::endl;
        }
        if(transmit_hash_policy != "") {
            netdev_file<<"TransmitHashPolicy="<<transmit_hash_policy<<std::endl;
        }
        if(ad_select != "") {
            netdev_file<<"AdSelect="<<ad_select<<std::endl;
        }
        if(all_members_active != "") {
            netdev_file<<"AllSlavesActive="<<all_members_active<<std::endl;
        }
        if(arp_interval != "") {
            int arp_int = std::stoi(arp_interval);
            arp_int = arp_int / 1000;
            netdev_file<<"ARPIntervalSec="<<arp_int<<"s"<<std::endl;
        }
        if(arp_ip_targets != "") {
            replaceAll(arp_ip_targets, ",", " ");
            netdev_file<<"ARPIPTargets="<<arp_ip_targets<<std::endl;
        }
        if(arp_validate != "") {
            netdev_file<<"ARPValidate="<<arp_validate<<std::endl;
        }
        if(arp_all_targets != "") {
            netdev_file<<"ARPAllTargets="<<arp_all_targets<<std::endl;
        }
        if(up_delay != "") {
            int up_delay_int = std::stoi(up_delay);
            up_delay_int = up_delay_int / 1000;
            netdev_file<<"UpDelaySec="<<up_delay_int<<"s"<<std::endl;
        }
        if(down_delay != "") {
            int down_delay_int = std::stoi(down_delay);
            down_delay_int = down_delay_int / 1000;
            netdev_file<<"DownDelaySec="<<down_delay_int<<"s"<<std::endl;
        }
        if(fail_over_mac_policy != "") {
            netdev_file<<"FailOverMACPolicy="<<fail_over_mac_policy<<std::endl;
        }
        if(gratuitous_arp != "") {
            netdev_file<<"GratuitousARP="<<gratuitous_arp<<std::endl;
        }
        if(packets_per_member != "") {
            netdev_file<<"PacketsPerSlave="<<packets_per_member<<std::endl;
        }
        if(primary_reselect_policy != "") {
            netdev_file<<"PrimaryReselectPolicy="<<primary_reselect_policy<<std::endl;
        }
        if(resend_igmp != "") {
            netdev_file<<"ResendIGMP="<<resend_igmp<<std::endl;
        }
        if(learn_packet_interval != "") {
            int learn_packet_interval_int = std::stoi(learn_packet_interval);
            learn_packet_interval_int = learn_packet_interval_int / 1000;
            netdev_file<<"LearnPacketIntervalSec="<<learn_packet_interval_int<<"s"<<std::endl;
        }
        netdev_file.close();
    }
    std::string restart_networkd_cmd = "/usr/bin/systemctl restart systemd-networkd";
    system(restart_networkd_cmd.c_str());
    return 0;
}
