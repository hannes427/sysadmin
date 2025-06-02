#include "functions.hpp"

int delete_nm_file (std::string interface, std::string type) {
    if (type == "bond") {
        //First we get the filenames of the bond-member
        std::string find_member_cmd = "/usr/bin/grep -l master="+interface+" /etc/NetworkManager/system-connections/*.nmconnection";
        std::string find_member = exec(find_member_cmd.c_str());
        find_member.pop_back();
        std::vector<std::string>member_filename = split(find_member, "\n");
        for(int i = 0; i < member_filename.size(); i++) {
            //Now we get the connection name of the member
            std::string find_connection_name = "/usr/bin/grep ^id= "+member_filename[i]+" | awk -F'=' {'print $2'}";
            std::string connection_name = exec(find_connection_name.c_str());
            std::string delete_member_connection = "/usr/bin/nmcli connection delete "+connection_name;
            system(delete_member_connection.c_str());
        }
    }
    //Get the name of the interface connection
    std::string find_connection_cmd = "/usr/bin/grep -h ^id="+interface+" /etc/NetworkManager/system-connections/*.nmconnection | awk -F'=' {'print $2'}";
    std::string connection = exec(find_connection_cmd.c_str());
    std::string delete_connection = "/usr/bin/nmcli connection delete "+connection;
    system(delete_connection.c_str());
    return 0;
}
int create_nm_connection (bool unconfigured, bool create_new_interface, std::string interface, std::string connection, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary) {
    if (ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured" && !create_new_interface) {
        std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" ipv4.method disabled ipv6.method disabled ipv4.address \"\" ipv4.gateway \"\" ipv6.address \"\" ipv6.gateway \"\"";
        std::system(nmcli_command.c_str());
    }
    else {
        std::vector<std::string> bond_member_vector = split(bond_member, ",");
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
            if  (std::filesystem::exists("/proc/sys/net/ipv6/conf/"+interface)) {
                std::string accept_ra_cmd = "echo 0 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
                system(accept_ra_cmd.c_str());
            }
        }
        else if (ipv6_accept_ra == "1") {
            if (std::filesystem::exists("/proc/sys/net/ipv6/conf/"+interface)) {
                std::string accept_ra_cmd = "echo 1 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
                system(accept_ra_cmd.c_str());
            }
        }
        else {
            if (std::filesystem::exists("/proc/sys/net/ipv6/conf/"+interface)) {
                std::string accept_ra_cmd = "echo 2 > /proc/sys/net/ipv6/conf/"+interface+"/accept_ra";
                system(accept_ra_cmd.c_str());
            }
        }
        if (type == "bond") {
            argument += " bond.options mode="+mode;
            if (lacp_rate != "") {
                argument += ",lacp_rate="+lacp_rate;
            }
            if (mii_monitor_interval != "") {
                argument += ",miimon="+mii_monitor_interval;
            }
            if (min_links != "") {
                argument += ",min_links="+min_links;
            }
            if (transmit_hash_policy != "") {
                argument += ",xmit_hash_policy="+transmit_hash_policy;
            }
            if (ad_select != "") {
                argument += ",ad_select="+ad_select;
            }
            if (all_members_active != "") {
                if (all_members_active == "false") {
                    argument += ",all_slaves_active=0";
                }
                else {
                     argument += ",all_slaves_active=1";
                }
            }
            if (arp_interval != "") {
                argument += ",arp_interval="+arp_interval;
            }
            if (arp_ip_targets != "") {
                std::vector<std::string> hosts = split(arp_ip_targets, ",");
                std::string ip_targets = "'";
                for(int i = 0; i < hosts.size(); i++) {
                    if (hosts[i] != "") {
                        ip_targets += hosts[i]+" ";
                    }
                }
                ip_targets += "'";
                argument += ",arp_ip_targets="+ip_targets;
            }
            if (arp_validate != "") {
                argument += ",arp_validate="+arp_validate;
            }
            if (arp_all_targets != "") {
                argument += ",arp_all_targets="+arp_all_targets;
            }
            if (up_delay != "") {
                argument += ",updelay="+up_delay;
            }
            if (down_delay != "") {
                argument += ",downdelay="+down_delay;
            }
            if (fail_over_mac_policy != "") {
                argument += ",fail_over_mac="+fail_over_mac_policy;
            }
            if (gratuitous_arp != "") {
                argument += ",num_grat_arp="+gratuitous_arp;
            }
            if (packets_per_member != "") {
                argument += ",packets_per_slave="+packets_per_member;
            }
            if (primary_reselect_policy != "") {
                argument += ",primary_reselect="+primary_reselect_policy;
            }
            if (resend_igmp != "") {
                argument += ",resend_igmp="+resend_igmp;
            }
            if (learn_packet_interval != "") {
                argument += ",lp_interval="+learn_packet_interval;
            }
            if (primary != "") {
                argument += ",primary="+primary;
            }
        }
        if (unconfigured) {
            connection = interface;
            std::string nmcli_command = "/usr/bin/nmcli connection add type "+type+" ifname "+interface+" con-name "+interface+" "+argument;
            std::system(nmcli_command.c_str());
            if (type == "bond") {
                for(int i = 0; i < bond_member_vector.size(); i++) {
                    std::string add_bond_member_con = "/usr/bin/nmcli connection add type ethernet ifname "+bond_member_vector[i]+" master "+interface+" con-name "+bond_member_vector[i];
                    system(add_bond_member_con.c_str());
                    std::string activate_bond_member_con = "/usr/bin/nmcli connection up "+bond_member_vector[i];
                    system(activate_bond_member_con.c_str());
                }
            }
        }
        else {
            std::string nmcli_command = "/usr/bin/nmcli connection modify "+connection+" "+argument;
            std::system(nmcli_command.c_str());
            if (type == "bond") {
                //Configure bond member

                //First we get rid of old bond member
                std::string get_old_member_cmd = "/usr/bin/nmcli -t -f all device show "+interface+" | grep -i bond.slaves | /usr/bin/awk -F ':' {'print$2'}";
                std::string temp_old_member  = exec(get_old_member_cmd.c_str());
                if (!temp_old_member.empty() && temp_old_member[temp_old_member.length()-1] == '\n') {
                    temp_old_member.erase(temp_old_member.length()-1);
                }
                std::vector<std::string> old_member = split(temp_old_member, " ");
                for(int i = 0; i < old_member.size(); i++) {
                    if (bond_member.find(old_member[i]) == std::string::npos) { //We found an old bond member, now we need to know the name of the connection
                        std::string get_old_member_connection_name = "/usr/bin/nmcli -t -f all device show "+old_member[i]+" | grep GENERAL.CONNECTION | /usr/bin/awk -F':' {'print $2'}";
                        std::string old_member_connection_name = exec(get_old_member_connection_name.c_str());
                        std::string delete_old_member_connection = "/usr/bin/nmcli connection delete "+old_member_connection_name;
                        system(delete_old_member_connection.c_str());
                    }
                }

                // Now we add connections for new bond member
                for (int i = 0; i < bond_member_vector.size(); i++) {
                    if (temp_old_member.find(bond_member_vector[i]) == std::string::npos) {
                        std::string add_bond_member_con = "/usr/bin/nmcli connection add type ethernet ifname "+bond_member_vector[i]+" master "+connection+" con-name "+bond_member_vector[i];
                        system(add_bond_member_con.c_str());
                        std::string activate_bond_member_con = "/usr/bin/nmcli connection up "+bond_member_vector[i];
                        system(activate_bond_member_con.c_str());
                    }
                }
            }

        }
    }
    std::string activate_conn_cmd = "/usr/bin/nmcli connection up "+connection;
    std::system(activate_conn_cmd.c_str());
    //We need to sleep for some time (otherwise not all bond members are shown in the webui if the bond-interface was just created
    std::string sleep_cmd = "/usr/bin/sleep 4";
    system(sleep_cmd.c_str());
    return 0;
}
