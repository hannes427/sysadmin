#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP
#include <iostream>
#include <array>
#include <filesystem>
#include <fstream>
#include <cstdio>
#include <memory>
#include <stdexcept>
#include <string>
#include <boost/program_options.hpp>
#include <mariadb/mysql.h>
#include <mariadb/mysqld_error.h>
#include <tuple>
#include <ctime>
#include <sstream>
#include <vector>
#include <regex>

/**
 * Split String by delimiter
 *
 * @param string "string_to_split", string "delimiter"
 * @return vector of strings
 */
std::vector<std::string> split(std::string s, std::string delimiter) {
    size_t pos_start = 0, pos_end, delim_len = delimiter.length();
    std::string token;
    std::vector<std::string> res;

    while ((pos_end = s.find(delimiter, pos_start)) != std::string::npos) {
        token = s.substr (pos_start, pos_end - pos_start);
        pos_start = pos_end + delim_len;
        res.push_back (token);
    }

    res.push_back (s.substr (pos_start));
    return res;
}

/**
 * Execute system command and returns output
 *
 * @param Command
 * @return string output
 */
std::string exec(const char* cmd) {
    std::array<char, 128> buffer;
    std::string result;
    std::unique_ptr<FILE, decltype(&pclose)> pipe(popen(cmd, "r"), pclose);
    if (!pipe) {
        throw std::runtime_error("popen() failed!");
    }
    while (fgets(buffer.data(), static_cast<int>(buffer.size()), pipe.get()) != nullptr) {
        result += buffer.data();
    }
    return result;
}

struct SQLConnection {
    std::string server, user, password, database;

    SQLConnection(std::string server, std::string user, std::string password, std::string database)
    {
        this->server = server;
        this->user = user;
        this->password = password;
        this->database = database;
    }
};

/**
 * @brief
 * Will try to connect to SQL, return a tuple, first value will be true if connection was a success
 *
 * @param mysql_details Details to connect ot SQL db
 * @return std::tuple<bool, MYSQL *>
 */
std::tuple<bool, MYSQL *> sqlConnectionSetup(struct SQLConnection mysql_details) {
    // there are multiple ways to return multiple values, here we use a tuple
    MYSQL *connection = mysql_init(NULL); // mysql instance
    bool success = true;

    // connect database
    // c_str -> converts std::string to char
    if (!mysql_real_connect(connection, mysql_details.server.c_str(), mysql_details.user.c_str(), mysql_details.password.c_str(), mysql_details.database.c_str(), 0, NULL, 0)) {
        success = false;
        std::cout << "Connection Error: " << mysql_error(connection) << std::endl;
    }
    return std::make_tuple(success, connection);
}

/**
 * @brief
 * Execute a SQL query, will return a tuple, first value will be true if success
 *
 * @param connection SQL connection
 * @param query SQL query
 * @return result struct with success and res (result)
 */
auto execSQLQuery(MYSQL *connection, std::string query) {
    // instead of returning a tuple, you could return a struct instead
    struct result {
        bool success;
        MYSQL_RES *res;
    };
    bool success = true;

    // send query to db
    if (mysql_query(connection, query.c_str())) {
        std::cout << "MySQL Query Error: " << mysql_error(connection) << std::endl;
        success = false;
    }

    return result{success, mysql_use_result(connection)};
}

/**
 * read each line from a file and save the line into a variable if the line matches dns-nameservers
 * @param file
 * @return std::string nameserver
 */
std::string get_nameserver (std::string file, std::string regex, std::string delimiter = "false") {
    std::string nameserver;
    if (!std::filesystem::exists(file)) {
        std::cerr<<std::endl<<"Error! "<<file<<" does not exists!" <<std::endl<<std::endl;
        return "Error";
        }
    std::string line;
    std::ifstream dnsfile (file);
    if (!dnsfile.is_open()) {
        std::cerr<<std::endl<<"Error! Could not open "<<file<<std::endl<<std::endl;
        return "Error";
    }
    std::regex dnsRegex("(^"+regex+"(.*)$)");
    while (getline(dnsfile,line)) {
        if (std::regex_match(line, dnsRegex)) {
            std::vector<std::string> server = split(line, delimiter);
            for(int i = 0; i < server.size(); i++) {
                int already_listed = nameserver.find(server[i]);
                if (already_listed == std::string::npos and (!regex_match(server[i], dnsRegex) and server[i] != " ")) {
                    nameserver += server[i]+" ";
                }
            }
        }
    }
    return nameserver;
}

/**
 * read ech line from a file and save the line into a variable if the line matches dns-search
 * @param file
 * @return std::string dns_domains
 */
std::string get_dns_domains (std::string file, std::string regex, std::string delimiter = "false") {
    std::string dns_domains;
    if (!std::filesystem::exists(file)) {
        std::cerr<<std::endl<<"Error! "<<file<<" does not exists!" <<std::endl<<std::endl;
        return "Error";
        }
    std::string line;
    std::ifstream dnsfile (file);
    if (!dnsfile.is_open()) {
        std::cerr<<std::endl<<"Error! Could not open "<<file<<std::endl<<std::endl;
        return "Error";
    }
    std::regex domainRegex("(^"+regex+"(.*)$)");
    while (getline(dnsfile,line)) {
        if (std::regex_match(line, domainRegex)) {
            std::vector<std::string> domain = split(line, delimiter);
            for(int i = 0; i < domain.size(); i++) {
                int already_listed = dns_domains.find(domain[i]);
                if (already_listed == std::string::npos and (!regex_match(domain[i], domainRegex) and domain[i] != " ")) {
                    dns_domains += domain[i]+" ";
                }
            }
        }
    }
    return dns_domains;
}

/**
 * Test if program is executed manualy by query the database for a valid entry.
 * The entry is made by the admin-interface when you press the "Submit" button
 *
 * @param Module-Name (hostname, packet_capture etc)
 * @return bool true/false
 */
bool check_auth (const std::string module) {
    int timestamp_diff;
    bool auth_ok = false;
    try {
        boost::program_options::options_description configfile{"File"};
        configfile.add_options()
        ("use_timestamp", boost::program_options::value<std::string>(), "Use timestamp")
        ("timestamp_diff", boost::program_options::value<int>(), "timestamp_diff")
        ("hostname", boost::program_options::value<std::string>(), "Hostname")
        ("port", boost::program_options::value<int>(), "Port")
        ("dbname", boost::program_options::value<std::string>(), "Database Name")
        ("username", boost::program_options::value<std::string>(), "Username")
        ("password", boost::program_options::value<std::string>(), "Password");

        boost::program_options::variables_map chk_auth;

        if (!std::filesystem::exists("/home/hannes/Downloads/c++/freepbx.conf")) {
            std::cerr<<std::endl<<"Error! Could not open file /home/hannes/Downloads/c++/freepbx.conf"<<std::endl<<std::endl;
            return false;
        }
        std::ifstream conffile ("/home/hannes/Downloads/c++/freepbx.conf");
        if (conffile) {
            store(parse_config_file(conffile, configfile), chk_auth);
        }

        notify(chk_auth);

        if (chk_auth["use_timestamp"].as<std::string>() == "yes") {
            int timestamp_diff = chk_auth["timestamp_diff"].as<int>();
            std::string hostname = chk_auth["hostname"].as<std::string>();
            int port = chk_auth["port"].as<int>();
            std::string dbname = chk_auth["dbname"].as<std::string>();
            std::string username = chk_auth["username"].as<std::string>();
            std::string password = chk_auth["password"].as<std::string>();

            bool success;

            MYSQL *con; // the connection
            // MYSQL_RES *res; // the results
            MYSQL_ROW row; // the results rows (array)
            struct SQLConnection sqlDetails(hostname, username, password, dbname);

            // connect to the mysql database
            std::tie(success, con) = sqlConnectionSetup(sqlDetails);
            if (!success) {
                std::cerr<<std::endl<<"Error! Could not connect to database!"<<std::endl<<std::endl;
                return false;
            }

            // get the results from executing commands
            auto result = execSQLQuery(con, "SELECT timestamp from sysadmin WHERE module='"+module+"';");

            if (!result.success) {
                // handle any errors
                std::cerr<<std::endl<<"Error! Could not query the database!"<<std::endl<<std::endl;
                return false;
            }
            auto t = std::time(nullptr);
            auto tm = *std::localtime(&t);
            auto time_now = std::put_time(&tm, "%Y-%m-%d %H-%M-%S");
            int timestamp_now = std::mktime(std::localtime(&t));

            while ((row = mysql_fetch_row(result.res)) != NULL) {
                int timestamp_query = atoi(row[0]);
                if (timestamp_now - timestamp_query <= timestamp_diff) {
                    auth_ok = true;
                }
            }
        }
        else {
            auth_ok = true;
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    return auth_ok;
}

/**
 * Validate ipv4/ipv6 addresses
 *
 * @param ipv4/ipv6 addresses, delimiter
 * @return bool true/false
 */
bool validate_ip_address (std::string addresses, std::string delimiter) {
    bool valid_address = true;
    std::regex ipv4Regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$)");
    std::regex ipv6Regex(R"(^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))$)");
    std::vector<std::string> address = split(addresses, delimiter);
    for(int i = 0; i < address.size(); i++) {
        if (!std::regex_match(address[i], ipv4Regex) and !std::regex_match(address[i], ipv6Regex)) {
            valid_address = false;
        }
    }
    return valid_address;
}

/**
 * Check if adress is a ipv4 address
 *
 * @param ipv4 address
 * @return bool true/false
 */
bool is_ipv4_address (std::string address) {
    bool ipv4_address = false;
    std::regex ipv4Regex(R"(^((25[0-5]|(2[0-4]|1\d|[1-9]|)\d)(\.(?!$)|$)){4}$)");
    if (std::regex_match(address, ipv4Regex)) {
        ipv4_address = true;
    }
    return ipv4_address;
}

/**
 * Check if adress is a ipv6 address
 *
 * @param ipv6 address
 * @return bool true/false
 */
bool is_ipv6_address (std::string address) {
    bool ipv6_address = false;
    std::regex ipv6Regex(R"(^(([0-9a-fA-F]{1,4}:){7,7}[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,7}:|([0-9a-fA-F]{1,4}:){1,6}:[0-9a-fA-F]{1,4}|([0-9a-fA-F]{1,4}:){1,5}(:[0-9a-fA-F]{1,4}){1,2}|([0-9a-fA-F]{1,4}:){1,4}(:[0-9a-fA-F]{1,4}){1,3}|([0-9a-fA-F]{1,4}:){1,3}(:[0-9a-fA-F]{1,4}){1,4}|([0-9a-fA-F]{1,4}:){1,2}(:[0-9a-fA-F]{1,4}){1,5}|[0-9a-fA-F]{1,4}:((:[0-9a-fA-F]{1,4}){1,6})|:((:[0-9a-fA-F]{1,4}){1,7}|:)|fe80:(:[0-9a-fA-F]{0,4}){0,4}%[0-9a-zA-Z]{1,}|::(ffff(:0{1,4}){0,1}:){0,1}((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])|([0-9a-fA-F]{1,4}:){1,4}:((25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9])\.){3,3}(25[0-5]|(2[0-4]|1{0,1}[0-9]){0,1}[0-9]))$)");
    if (std::regex_match(address, ipv6Regex)) {
        ipv6_address = true;
    }
    return ipv6_address;
}

/**
 * Checks whether interface is managed by ifupdown
 *
 * @param Name of the interface
 * @return bool true/false
 */
bool check_ifupdown (const std::string interface) {
    bool ifupdown = false;
    if(!std::filesystem::exists("/sys/class/net/"+interface)) {
        std::cerr<<std::endl<<"Error! Could not open interface file! Is "<<interface<<" a valid network interface?"<<std::endl<<std::endl;
        std::abort();
    }
    else if (std::filesystem::exists("/run/network/ifstate") && !(std::filesystem::file_size("/run/network/ifstate") == 0)) {
        std::ifstream ifstate;
        ifstate.open("/run/network/ifstate");
        if(ifstate.good()) {
            std::string line_ifstate;
            while(getline(ifstate, line_ifstate)) {
                std::size_t found = line_ifstate.find(interface);
                if (found!=std::string::npos) {
                    ifupdown = true;
                    break;
                }
            }
        }
    }
    return ifupdown;
}

/**
 * Function checks whether the interface is managed by netplan.io
 *
 * @param Name of the interface
 * @return bool true/false
 */
bool check_netplan (const std::string interface) {
    bool netplan = false;
    if(!std::filesystem::exists("/sys/class/net/"+interface)) {
        std::cerr<<std::endl<<"Error! Could not open interface file! Is "<<interface<<" a valid network interface?"<<std::endl<<std::endl;
        std::abort();
    }
    else if (std::filesystem::exists("/etc/netplan")) {
        netplan = true;
    }
    return netplan;
}

/**
 * Function checks whether the interface is managed by NetworkManager
 *
 * @param Name of the interface
 * @return bool true/false
 */
bool check_networkManager (const std::string interface) {
    bool nm = false;
    if(!std::filesystem::exists("/sys/class/net/"+interface)) {
        std::cerr<<std::endl<<"Error! Could not open interface file! Is "<<interface<<" a valid network interface?"<<std::endl<<std::endl;
        std::abort();
    }
    else {
        int check_nm_process = system("/usr/bin/systemctl status NetworkManager >> /dev/null 2>&1");
        if (check_nm_process == 0) {
            if (check_ifupdown(interface) || check_netplan(interface)) {
                nm =  false;
            }
            else {
                nm =  true;
            }
        }
        else {
            nm = false;
        }
    }
    return nm;
}

/**
 * Function checks whether the interface is managed by systemd
 *
 * @param Name of the interface
 * @return bool true/false
 */
bool check_systemd_networkd (const std::string interface) {
    bool systemd = false;
    if(!std::filesystem::exists("/sys/class/net/"+interface)) {
        std::cerr<<std::endl<<"Error! Could not open interface file! Is "<<interface<<" a valid network interface?"<<std::endl<<std::endl;
        std::abort;
    }
    else {
        if (check_netplan(interface)) {
            systemd = false;
        }
        else {
            int check_systemd_process = system("/usr/bin/systemctl status systemd-networkd >> /dev/null 2>&1");
            if (check_systemd_process == 0) {
                systemd = true;
                }
            else {
                systemd = false;
            }
        }
    }
    return systemd;
}

bool check_resolvconf () {
    int check_resolvconf = std::system("/usr/bin/dpkg-query -W --showformat='${Status}\n' resolvconf | grep \"install ok installed\" >> /dev/null 2>&1");
    if(check_resolvconf == 0) {
        return true;
    }
    else {
        return false;
    }
}

#endif // FUNCTIONS_HPP

