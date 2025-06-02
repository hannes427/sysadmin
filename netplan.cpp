#include "functions.hpp"

int delete_netplan_file(const std::string& interface, const std::string& type) {
    std::string cmd = "/usr/bin/grep -l "+interface+" /etc/netplan/*.yaml";
    std::string filename = exec(cmd.c_str());
    filename.pop_back();
    if (filename != "") {
        std::filesystem::remove(filename);
    }
    int test = std::system("/usr/sbin/netplan apply");
    if (type == "bond") {
        std::string remove_virtual_interface = "/usr/bin/ip link delete dev "+interface;
        system(remove_virtual_interface.c_str());
    }
    return 0;
}
int create_netplan_file(bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary) {
    if (type == "ethernet") {
        type = "ethernets";
    }
    else if (type == "bond") {
        type = "bonds";
    }

    std::string dns_server;
    std::string ipv4_nameserver;
    std::string ipv6_nameserver;
    std::string search;
    std::string filename;
    std::string member;
    if (!unconfigured) {
        std::string cmd = "/usr/bin/grep -l "+interface+" /etc/netplan/*.yaml";
        filename = exec(cmd.c_str());
        filename.pop_back();
        filename = std::filesystem::path(filename).stem();
        std::string dns_cmd = "/usr/sbin/netplan get network."+type+"."+interface+".nameservers.addresses | /usr/bin/grep -v null";
        dns_server = exec(dns_cmd.c_str());
        replace(dns_server.begin(), dns_server.end(), '\n', ' ');
        replace(dns_server.begin(), dns_server.end(), '-', ' ');
        std::vector<std::string> nameservers = split(dns_server, " ");
        for(int i = 0; i < nameservers.size(); i++) {
            if(is_ipv4_address((nameservers[i]))) {
                ipv4_nameserver += nameservers[i]+", ";
            }
            else if (is_ipv6_address(nameservers[i])) {
                ipv6_nameserver += nameservers[i]+", ";
            }
        }
        //Remove last space
        if (!ipv4_nameserver.empty() && ipv4_nameserver[ipv4_nameserver.length()-1] == ' ') {
            ipv4_nameserver.erase(ipv4_nameserver.length()-1);
        }
        if (!ipv6_nameserver.empty() && ipv6_nameserver[ipv6_nameserver.length()-1] == ' ') {
            ipv6_nameserver.erase(ipv6_nameserver.length()-1);
        }
        std::string search_cmd = "/usr/sbin/netplan get network."+type+"."+interface+".nameservers.search | /usr/bin/grep -v null";
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
    //remove old config
    if (type == "ethernets") {
        std::string remove_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+interface+"=\"{addresses: null, dhcp4: false, dhcp6: false, routes: null}\"";
        std::system(remove_cmd.c_str());
        std::string remove_nameserver_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.ethernets."+interface+".nameservers={addresses: null, search: null}\"";
        std::system(remove_nameserver_cmd.c_str());

    }
    else if (type == "bonds") {
        std::string remove_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.bonds."+interface+"=\"{addresses: null, dhcp4: false, dhcp6: false, routes: null, nameservers.addresse: null, nameservers.search: null, parameters: null}\"";
        std::string remove_nameserver_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.bonds."+interface+".nameservers={addresses: null, search: null}\"";
        std::system(remove_nameserver_cmd.c_str());
        std::system(remove_cmd.c_str());
    }
    std::system("/usr/sbin/netplan apply >> /dev/null 2>&1");
    if (ipv4_assignment == "unconfigured" && ipv6_assignment == "unconfigured" && !create_new_interface) {
        int test = std::system("/usr/sbin/netplan apply");
        return 0;
    }
    if (ipv4_assignment == "dhcp" || ipv6_assignment == "dhcp" || ipv6_assignment == "auto") {
        std::string dhcp = "/usr/sbin/netplan set --origin-hint "+filename+" network."+type+"."+interface+"=\"{";
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
        std::system(dhcp.c_str());
    }
    if (ipv6_accept_ra != "0") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network."+type+"."+interface+".accept-ra=true";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_assignment == "static") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network."+type+"."+interface+".addresses=["+ipv4_address+"/"+ipv4_netmask+"]";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" "+type+"."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv4_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv6_assignment == "static") {
       std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network."+type+"."+interface+".addresses=["+ipv6_address+"/"+ipv6_netmask+"]";
       std::system(netplan_cmd.c_str());
    }
    if (ipv6_gateway != "") {
        std::string netplan_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" "+type+"."+interface+".routes='[{\"to\":\"default\", \"via\": \""+ipv6_gateway+"\"}]'";
        std::system(netplan_cmd.c_str());
    }
    if (ipv4_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set --origin-hint "+filename+" \"network."+type+"."+interface+".nameservers.addresses=["+ipv4_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
    if (ipv6_nameserver != "") {
        std::string netplan_add_server = "/usr/sbin/netplan set --origin-hint "+filename+" \"network."+type+"."+interface+".nameservers.addresses=["+ipv6_nameserver+"\"]";
        std::system(netplan_add_server.c_str());
    }
     if (search != "") {
        std::string netplan_add_search = "/usr/sbin/netplan set --origin-hint "+filename+" \"network."+type+"."+interface+".nameservers.search=["+search+"\"]";
        std::system(netplan_add_search.c_str());
    }
    //Bond Configuration only
    if (type == "bonds") {
        std::string bond_parameter;
        bond_parameter += "mode: "+mode+", ";
        if (bond_member != "") {
            std::vector<std::string> bond_member_vector = split(bond_member, ",");
            member ="[";
            for(int i = 0; i < bond_member_vector.size(); i++) {
                member += bond_member_vector[i]+", ";
                std::string add_member_cmd = "/usr/sbin/netplan set --origin-hint "+filename+" network.ethernets."+bond_member_vector[i]+"=\"{}\"";
                system(add_member_cmd.c_str());
            }
            member += "]";
        }
        if (lacp_rate != "") {
            bond_parameter += "lacp-rate: "+lacp_rate+", ";
        }
        if (mii_monitor_interval != "") {
            bond_parameter += "mii-monitor-interval: "+mii_monitor_interval+", ";
        }
        if (min_links != "") {
            bond_parameter += "min-links: "+min_links+", ";
        }
        if (transmit_hash_policy != "") {
            bond_parameter += "transmit-hash-policy: "+transmit_hash_policy+", ";
        }
        if (ad_select != "") {
            bond_parameter += "ad-select: "+ad_select+", ";
        }
        if (all_members_active != "") {
            bond_parameter += "all-members-active: "+all_members_active+", ";
        }
        if (arp_interval != "") {
            bond_parameter += "arp-interval: "+arp_interval+", ";
        }
        if (arp_ip_targets != "") {
            std::vector<std::string> hosts = split(arp_ip_targets, ",");
            std::string ip_targets = "[";
            for(int i = 0; i < hosts.size(); i++) {
                if (hosts[i] != "") {
                    ip_targets += hosts[i]+", ";
                }
            }
            //Remove last space and comma from ip_targets
            ip_targets.pop_back();
            ip_targets.pop_back();
            ip_targets += "]";
            bond_parameter += "arp-ip-targets: "+ip_targets+", ";
        }
        if (arp_validate != "") {
            bond_parameter += "arp-validate: "+arp_validate+", ";
        }
        if (arp_all_targets != "") {
            bond_parameter += "arp-all-targets: "+arp_all_targets+", ";
        }
        if (up_delay != "") {
            bond_parameter += "up-delay: "+up_delay+", ";
        }
        if (down_delay != "") {
            bond_parameter += "down-delay: "+down_delay+", ";
        }
        if (fail_over_mac_policy != "") {
            bond_parameter += "fail-over-mac-policy: "+fail_over_mac_policy+", ";
        }
        if (gratuitous_arp != "") {
            bond_parameter += "gratuitous-arp: "+gratuitous_arp+", ";
        }
        if (packets_per_member != "") {
            bond_parameter += "packets-per-member: "+packets_per_member+", ";
        }
        if (primary_reselect_policy != "") {
            bond_parameter += "primary-reselect-policy: "+primary_reselect_policy+", ";
        }
        if (resend_igmp != "") {
            bond_parameter += "resend-igmp: "+resend_igmp+", ";
        }
        if (learn_packet_interval != "") {
            bond_parameter += "learn-packet-interval: "+learn_packet_interval+", ";
        }
        if (primary != "") {
            bond_parameter += "primary: "+primary+", ";
        }
        std::string add_bond_member = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.bonds."+interface+"={interfaces: "+member+"}\"";
        system(add_bond_member.c_str());
        std::string netplan_add_bond_parameter = "/usr/sbin/netplan set --origin-hint "+filename+" \"network.bonds."+interface+".parameters={"+bond_parameter+"}\"";
        std::system(netplan_add_bond_parameter.c_str());
    }
    int test = std::system("/usr/sbin/netplan apply");
    return 0;
}
