#include <iostream>
#include <fstream>
#include "functions.hpp"
#include <boost/program_options.hpp>

std::string setup;
std::string myhostname;
std::string mydomain;
std::string myorigin;
std::string myorigin_path;
std::string server;
std::string username;
std::string password;
std::string use_auth;
std::string use_tls;
std::string port;
bool password_changed;
int current_uid = getuid();

int main (int argc, char **argv) {
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("setup", boost::program_options::value<std::string>()->required(), "use local oder remote server?")
        ("myhostname", boost::program_options::value<std::string>(), "Postfix myhostname parameter")
        ("mydomain", boost::program_options::value<std::string>(), "Postfix mydomain parameter")
        ("myorigin", boost::program_options::value<std::string>(), "Postfix myorigin parameter")
        ("myorigin_path", boost::program_options::value<std::string>(), "Path to the file (if myorigin in main.cf points to a filename)")
        ("server", boost::program_options::value<std::string>(), "address/hostname of the mailserver (if setup is set to 'remote'")
        ("use_auth", boost::program_options::value<std::string>(), "Use Authentication to send the mail (if setup is set to 'remote'")
        ("use_tls", boost::program_options::value<std::string>(), "Use TLS encryption to send the mail")
        ("port", boost::program_options::value<std::string>(), "which port to use for the encrypted connection (tls (587) or starttls (465)")
        ("username", boost::program_options::value<std::string>(), "username of the remote mailserver")
        ("password", boost::program_options::value<std::string>(), "password for the user")
        ("password_changed", boost::program_options::value<bool>(), "password changed in web ui");

        boost::program_options::variables_map vm;
        boost::program_options::command_line_style::allow_long_disguise;
        store(parse_command_line(argc, argv, desc), vm);
        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';

        }
        if (vm.count("setup")) {
            setup = vm["setup"].as<std::string>();
        }
        if (vm.count("myhostname")) {
            myhostname = vm["myhostname"].as<std::string>();
        }
        if (vm.count("mydomain")) {
            mydomain = vm["mydomain"].as<std::string>();
        }
        if (vm.count("myorigin")) {
            myorigin = vm["myorigin"].as<std::string>();
        }
        if (vm.count("myorigin_path")) {
            myorigin_path = vm["myorigin_path"].as<std::string>();
        }
        if (vm.count("server")) {
            server = vm["server"].as<std::string>();
        }
        if (vm.count("use_auth")) {
            use_auth = vm["use_auth"].as<std::string>();
        }
        if (vm.count("use_tls")) {
            use_tls = vm["use_tls"].as<std::string>();
        }
        if (vm.count("port")) {
            port = vm["port"].as<std::string>();
        }
        if (vm.count("username")) {
            username = vm["username"].as<std::string>();
        }
        if (vm.count("password")) {
            password = vm["password"].as<std::string>();
        }
        if (vm.count("password_changed")) {
            password_changed = vm["password_changed"].as<bool>();
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    if(setup == "local") {
        //Check if myorigin points to a file in main.cf
        if (myorigin_path != "") {
            try {
                std::ofstream myorigin_file;
                myorigin_file.open(myorigin_path);
                myorigin_file <<myorigin<<std::endl;
                myorigin_file.close();
            }
            catch (std::filesystem::filesystem_error& e) {
                std::cout << e.what() << '\n';
            }
            myorigin = myorigin_path;
        }
        std::string postconf_cmd = "/usr/sbin/postconf -e myhostname="+ myhostname +" mydomain="+ mydomain + " myorigin=" + myorigin + " smtp_use_tls=" + use_tls;
        if (use_tls == "yes") {
            postconf_cmd += " smtp_tls_security_level=dane smtp_tls_CAfile=/etc/ssl/certs/ca-certificates.crt";
        }
        system(postconf_cmd.c_str());
    }
    else {
        std::string postconf_cmd = "/usr/sbin/postconf -e smtp_use_tls=" + use_tls;
        if(use_tls == "yes") {
            postconf_cmd += " relayhost=" + server + ":" + port + " smtp_tls_security_level=dane smtp_tls_CAfile=/etc/ssl/certs/ca-certificates.crt smtp_sasl_tls_security_options=noanonymous";
        }
        else {
            postconf_cmd += " relayhost=" + server;
        }
        if(use_auth == "yes") {
            postconf_cmd += " smtp_sasl_auth_enable=yes smtp_sasl_password_maps=hash:/etc/postfix/sasl_passwd smtp_sasl_security_options=noanonymous";
            try {
                if(!password_changed) {
                    //password was not changed, so the *real* password was not trasmitted via web ui --> i need to read it from the file to reuse it
                    std::string getpasswd_cmd = "/usr/bin/grep -v '#' /etc/postfix/sasl_passwd | /usr/bin/awk -F ':' {'print $2'}";
                    password = exec(getpasswd_cmd.c_str());
                    password.erase(std::remove(password.begin(), password.end(), '\n'), password.end());
                }
                std::ofstream sasl_file;
                sasl_file.open("/etc/postfix/sasl_passwd");
                sasl_file <<server<<" "<<username<<":"<<password<<std::endl;
                sasl_file.close();
            }
            catch (std::filesystem::filesystem_error& e) {
                std::cout << e.what() << '\n';
            }
            std::string hash_password_cmd = "/usr/sbin/postmap hash:/etc/postfix/sasl_passwd";
            std::string chmod_cmd = "/usr/bin/chmod 600 /etc/postfix/sasl_passwd /etc/postfix/sasl_passwd.db";
            system(chmod_cmd.c_str());
            system(hash_password_cmd.c_str());
        }
        else {
           postconf_cmd += " smtp_sasl_auth_enable=no smtp_sasl_password_maps=\"\"";
           std::remove("/etc/postfix/sasl_passwd");
           std::remove("/etc/postfix/sasl_passwd.db");
        }
        system(postconf_cmd.c_str());
    }
    std::string reload_postfix_cmd = "/usr/bin/systemctl restart postfix";
    system(reload_postfix_cmd.c_str());
    setuid(current_uid); //return to previous user
    return 0;
}
