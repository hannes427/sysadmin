#include "functions.hpp"
bool delete_interface = false;
bool create_new_interface = false;
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
std::string type;
std::string bond_member;
std::string mode;
std::string lacp_rate;
std::string mii_monitor_interval;
std::string min_links;
std::string transmit_hash_policy;
std::string ad_select;
std::string all_members_active;
std::string arp_interval;
std::string arp_ip_targets;
std::string arp_validate;
std::string arp_all_targets;
std::string up_delay;
std::string down_delay;
std::string fail_over_mac_policy;
std::string gratuitous_arp;
std::string packets_per_member;
std::string primary_reselect_policy;
std::string resend_igmp;
std::string learn_packet_interval;
std::string primary;

int remove_interface (std::string interface);

int delete_ifupdown_file (const std::string& interface);

int create_ifupdown_file(bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary);

int delete_netplan_file(const std::string& interface, const std::string& type);

int create_netplan_file (bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary);

int delete_networkd_file (std::string interface, std::string type);

int create_networkd_file (bool unconfigured, bool create_new_interface, std::string interface, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary);

int delete_nm_file (std::string interface, std::string type);

int create_nm_connection (bool unconfigured, bool create_new_interface, std::string interface, std::string connection, std::string ipv4_address, std::string ipv4_netmask, std::string ipv4_gateway, std::string ipv4_assignment, std::string ipv6_address, std::string ipv6_netmask, std::string ipv6_gateway, std::string ipv6_assignment, std::string ipv6_autoconf, std::string ipv6_accept_ra, std::string type, std::string bond_member, std::string mode, std::string lacp_rate, std::string mii_monitor_interval, std::string min_links, std::string transmit_hash_policy, std::string ad_select, std::string all_members_active, std::string arp_interval, std::string arp_ip_targets, std::string arp_validate, std::string arp_all_targets, std::string up_delay, std::string down_delay, std::string fail_over_mac_policy, std::string gratuitous_arp, std::string packets_per_member, std::string primary_reselect_policy, std::string resend_igmp, std::string learn_packet_interval, std::string primary);


int main (int argc, char **argv) {
    int current_uid = getuid();
    bool unconfigured = false;
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("delete-interface", "Delete interface config")
        ("create-new-interface", "Create new interface config (e.g. bond)")
        ("interface", boost::program_options::value<std::string>()->required(), "name of the interface")
        ("managed-by", boost::program_options::value<std::string>(), "which way is used to configure this interface (ifupdown, netplan, networkd, network-manager)")
        ("connection", boost::program_options::value<std::string>(), "NetworkManager connection (if NetworkManager is used")
        ("ipv4-address", boost::program_options::value<std::string>(), "IPv4 address/netmask (IP/MASK")
        ("ipv4-gateway", boost::program_options::value<std::string>(), "IPv4 default gateway")
        ("ipv4-assignment", boost::program_options::value<std::string>(), "Method to set IPv4 addres (valid input: static, dhcp or unconfigured")
        ("ipv6-address", boost::program_options::value<std::string>(), "IPv6 address")
        ("ipv6-gateway", boost::program_options::value<std::string>(), "IPv6 default gateway")
        ("ipv6-assignment", boost::program_options::value<std::string>(), "Method to set IPv6 addres (valid input: static, dhcp, auto or unconfigured")
        ("ipv6-autoconf", boost::program_options::value<std::string>(), "Use ipv6 autoconf (0=off, 1=on)")
        ("ipv6-accept-ra", boost::program_options::value<std::string>(), "Accept router advertisements (0=off, 1=on, 2=on+forwarding)")
        ("type", boost::program_options::value<std::string>()->required(), "type of the interface (ethernet || bond)")
        ("bond_member", boost::program_options::value<std::string>(), "Bonding member")
        ("mode", boost::program_options::value<std::string>(), "Bonding mode")
        ("lacp-rate", boost::program_options::value<std::string>(), "LACP rate")
        ("mii-monitor-interval", boost::program_options::value<std::string>(), "MII Interval")
        ("min-links", boost::program_options::value<std::string>(), "minimum number of links up to consider the bond interface to be up")
        ("transmit-hash-policy", boost::program_options::value<std::string>(), "Transmit hash policy for the selection of ports")
        ("ad-select", boost::program_options::value<std::string>(), "aggregation selection mode")
        ("all-members-active", boost::program_options::value<std::string>(), "If set to true, received on inactive ports are delivered")
        ("arp-interval", boost::program_options::value<std::string>(), "interval value in milliseconds for how frequently ARP link monitoring should happen")
        ("arp-ip-targets", boost::program_options::value<std::string>(), "List of IP-addresses to which ARP-request should be send to in order to validate that a port is up (only ipV4, max 16 addresses.")
        ("arp-validate", boost::program_options::value<std::string>(), "how ARP replies are to be validated when using ARP link monitoring")
        ("arp-all-targets", boost::program_options::value<std::string>(), "Should all targets be up for a port to be considered up")
        ("up-delay", boost::program_options::value<std::string>(), "Specify the delay before enabling a link once the link is physically up")
        ("down-delay", boost::program_options::value<std::string>(), "Specify the delay before disabling a link once the link has been lost")
        ("fail-over-mac-policy", boost::program_options::value<std::string>(), "Set whether to set all ports to the same MAC address when adding them to the bond")
        ("gratuitous-arp", boost::program_options::value<std::string>(), "Specify how many ARP packets to send after failover")
        ("packets-per-member", boost::program_options::value<std::string>(), "In balance-rr mode, specifies the number of packets to transmit on a port before switching to the next")
        ("primary-reselect-policy", boost::program_options::value<std::string>(), "Set the reselection policy for the primary port")
        ("resend-igmp", boost::program_options::value<std::string>(), "This parameter specifies how many IGMP membership reports are issued on a failover event")
        ("learn-packet-interval", boost::program_options::value<std::string>(), "Specify the interval between sending learning packets to each port")
        ("primary", boost::program_options::value<std::string>(), "Specify a device to be used as a primary port, or preferred device to use as a port for the bond");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
            return 0;
        }

        notify(vm);

        if (!vm.count("delete-interface")) {
            if (!vm.count("ipv4-assignment")) {
                std::cerr << "Error: the option '--ipv4-assignment' is required but missing." << std::endl;
                return 1; // Fehlercode
            }
            if (!vm.count("ipv6-assignment")) {
                std::cerr << "Error: the option '--ipv6-assignment' is required but missing." << std::endl;
                return 1; // Fehlercode
            }
        }
        else {
            delete_interface = true;
        }
        if (vm.count("create-new-interface")) {
            create_new_interface = true;
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
        if (vm.count("type")) {
            type = vm["type"].as<std::string>();
        }
        if (vm.count("bond_member")) {
            bond_member = vm["bond_member"].as<std::string>();
        }
        if (vm.count("mode")) {
            mode = vm["mode"].as<std::string>();
        }
        if (vm.count("lacp-rate")) {
            lacp_rate = vm["lacp-rate"].as<std::string>();
        }
        if (vm.count("mii-monitor-interval")) {
            mii_monitor_interval = vm["mii-monitor-interval"].as<std::string>();
        }
        if (vm.count("min-links")) {
            min_links = vm["min-links"].as<std::string>();
        }
        if (vm.count("transmit-hash-policy")) {
            transmit_hash_policy = vm["transmit-hash-policy"].as<std::string>();
        }
        if (vm.count("ad-select")) {
            ad_select = vm["ad-select"].as<std::string>();
        }
        if (vm.count("all-members-active")) {
            all_members_active = vm["all-members-active"].as<std::string>();
        }
        if (vm.count("arp-interval")) {
            arp_interval = vm["arp-interval"].as<std::string>();
        }
        if (vm.count("arp-ip-targets")) {
            arp_ip_targets = vm["arp-ip-targets"].as<std::string>();
        }
        if (vm.count("arp-validate")) {
            arp_validate = vm["arp-validate"].as<std::string>();
        }
        if (vm.count("arp-all-targets")) {
            arp_all_targets = vm["arp-all-targets"].as<std::string>();
        }
        if (vm.count("up-delay")) {
            up_delay = vm["up-delay"].as<std::string>();
        }
        if (vm.count("down-delay")) {
            down_delay = vm["down-delay"].as<std::string>();
        }
        if (vm.count("fail-over-mac-policy")) {
            fail_over_mac_policy = vm["fail-over-mac-policy"].as<std::string>();
        }
        if (vm.count("gratuitous-arp")) {
            gratuitous_arp = vm["gratuitous-arp"].as<std::string>();
        }
        if (vm.count("packets-per-member")) {
            packets_per_member = vm["packets-per-member"].as<std::string>();
        }
        if (vm.count("primary-reselect-policy")) {
            primary_reselect_policy = vm["primary-reselect-policy"].as<std::string>();
        }
        if (vm.count("resend-igmp")) {
            resend_igmp = vm["resend-igmp"].as<std::string>();
        }
        if (vm.count("learn-packet-interval")) {
            learn_packet_interval = vm["learn-packet-interval"].as<std::string>();
        }
        if (vm.count("primary")) {
            primary = vm["primary"].as<std::string>();
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
        if (delete_interface) {
            delete_ifupdown_file (interface);
        }
        else {
            create_ifupdown_file(unconfigured, create_new_interface, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, bond_member, mode, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
        }
    }
    else if (managed_by == "network_manager") { //Interface is managed by NetworkManager
        if (delete_interface) {
            delete_nm_file (interface, type);
        }
        else {
            if(connection == "") {
                unconfigured = true;
            }
            create_nm_connection (unconfigured, create_new_interface, interface, connection, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, bond_member, mode, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
        }
    }
    else if (managed_by == "networkd") {  //Interface is managed by systemd
        if (delete_interface) {
            delete_networkd_file (interface, type);
        }
        else {
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
            create_networkd_file (unconfigured, create_new_interface, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, bond_member, mode, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
        }
    }
    else if (managed_by == "netplan") {
        if (delete_interface) {
            delete_netplan_file (interface, type);
        }
        else {
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
            create_netplan_file (unconfigured, create_new_interface, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, bond_member, mode, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
        }
    }
    else { //interface is unconfigured
        bool unconfigured = true;
        remove_interface (interface);
        create_ifupdown_file (unconfigured, create_new_interface, interface, ipv4_address, ipv4_netmask, ipv4_gateway, ipv4_assignment, ipv6_address, ipv6_netmask, ipv6_gateway, ipv6_assignment, ipv6_autoconf, ipv6_accept_ra, type, bond_member, mode, lacp_rate, mii_monitor_interval, min_links, transmit_hash_policy, ad_select, all_members_active, arp_interval, arp_ip_targets, arp_validate, arp_all_targets, up_delay, down_delay, fail_over_mac_policy, gratuitous_arp, packets_per_member, primary_reselect_policy, resend_igmp, learn_packet_interval, primary);
    }
    setuid(current_uid);
    return 0;
}
