#include <iostream>
#include <fstream>
#include <regex>
#include <filesystem>
#include <boost/program_options.hpp>
#include <signal.h>

const std::string MODULE = "packetcapture";

int startcapture (int argc, char **argv) {
//DEBUG BEGIN
    std::cout<<"Stsart capture"<<std::endl;
    if( argc > 1 )
    {
        int i;
        std::cout << "Command line: "<<argv[0]<<" ";

        std::copy( argv+1, argv+argc, std::ostream_iterator<const char*>( std::cout, " " ) ) ;
    }
//DEBUG END
   /* Not full implemented yet
    if(!check_auth(MODULE)) {
         std::cerr<<std::endl<<"Error! Authentication failed! Did you run this program from the command line?"<<std::endl<<std::endl;
         return 1;
    }*/
    int current_uid = getuid();
    std::string arguments = "-Z root -U -n -e -tttt";
    std::string filter_on = "";
    bool filter = false;
    try {
        boost::program_options::options_description desc{"Options"};
        desc.add_options()
        ("help,h", "Help screen")
        ("count-bytes,c", boost::program_options::value<std::string>(), "Exit after receiving <arg> packets")
        ("maxsize,C", boost::program_options::value<std::string>(), "Maximum size of dump-file. If filesize is larger, a new file is created.")
        ("interface,i", boost::program_options::value<std::string>(), "Interface to listen on")
        ("maxfilecount,W", boost::program_options::value<std::string>(), "Filecount (in conjunction with the -C option")
        ("ip_version", boost::program_options::value<std::string>(), "Filter on IP version (ip or ip6)")
        ("protocol", boost::program_options::value<std::string>(), "Filter on protocol (tcp, udp, icmp, arp)")
        ("host_address", boost::program_options::value<std::string>(), "Filter on Host (source and destination)")
        ("port", boost::program_options::value<std::string>(), "Filter on port")
        ("path", boost::program_options::value<std::string>(), "Path to the dump-file");

        boost::program_options::variables_map vm;
        store(parse_command_line(argc, argv, desc), vm);

        notify(vm);

        if (vm.count("help")) {
            std::cout << desc << '\n';
        }
        if (vm.count("count-bytes")) {
            arguments = arguments+" -c "+vm["count-bytes"].as<std::string>();
        }
        if (vm.count("maxsize")) {
            arguments = arguments+" -C "+vm["maxsize"].as<std::string>();
        }
        if (vm.count("interface")) {
            arguments = arguments+" -i "+vm["interface"].as<std::string>();
        }
        if (vm.count("maxfilecount")) {
            arguments = arguments+" -W "+vm["maxfilecount"].as<std::string>();
        }
        if (vm.count("ip_version")) {
            if(filter == true) {
                filter_on = filter_on+" and "+vm["ip_version"].as<std::string>();
            }
            else {
                filter_on = filter_on+" "+vm["ip_version"].as<std::string>();
                filter = true;
            }
        }
        if (vm.count("protocol")) {
            if(filter == true) {
                filter_on = filter_on+" and "+vm["protocol"].as<std::string>();
            }
            else {
                filter_on = filter_on+" "+vm["protocol"].as<std::string>();
                filter = true;
            }
        }
        if (vm.count("host_address")) {
            if(filter == true) {
                filter_on = filter_on+" and host "+vm["host_address"].as<std::string>();
            }
            else {
                filter_on = filter_on+" host "+vm["host_address"].as<std::string>();
                filter = true;
            }
        }
        if (vm.count("port")) {
            if(filter == true) {
                filter_on = filter_on+" and port "+vm["port"].as<std::string>();
            }
            else {
                filter_on = filter_on+" port"+vm["port"].as<std::string>();
                filter = true;
            }
        }
        if (vm.count("path")) {
            arguments = arguments+" -w "+vm["path"].as<std::string>()+"/dump.pcap";
        }
    }
    catch (const boost::program_options::error &ex) {
        std::cerr << ex.what() << '\n';
    }
    std::string tcpdump = std::string("/usr/bin/tcpdump")+" "+arguments+" "+filter_on+" > /dev/null 2>&1&";
//DEBUG BEGIN
    std::cout<<tcpdump<<std::endl;
//DEBUG END
    if (setuid(0)) { //I am now root!
        perror("setuid");
        return 1;
    }
    std::system(tcpdump.c_str());
    setuid(current_uid); //return to previous user
    return 0;
}
int stopcapture (int pid) {
    std::regex processPattern(R"(^\/usr\/bin\/tcpdump)");
    std::string line;
    //Check if PID is a valid tcpdump process
     std::ifstream process;
     if (!std::filesystem::exists("/proc/"+std::to_string(pid)+"/cmdline")) {
          std::cerr<<std::endl<<"Error! Could not open process file! Is "<<pid<<" a valid PID?"<<std::endl<<std::endl;
          return 1;
     }
     process.open("/proc/"+std::to_string(pid)+"/cmdline");
     while(getline(process, line)) {
        std::smatch match;
        if (std::regex_search(line, match, processPattern)) { // PID is a valid tcpdump process
             kill(pid, SIGTERM);
        }
        else { //PID is not a tcpdump PID, possible misusage
            std::cerr<<std::endl<<"Error! PID " <<pid<<" is not a valid PID!"<<std::endl<<std::endl;
            return 1;
        }
     }
    return 0;
}
int main (int argc, char **argv) {
//DEBUG BEGIN
    std::cout<<"Warmup..."<<std::endl;
    if( argc > 1 )
    {
        int i;
        std::cout << "Command line: "<<argv[0]<<" ";

        std::copy( argv+1, argv+argc, std::ostream_iterator<const char*>( std::cout, " " ) ) ;
    }
    std::cout<<std::endl<<std::endl<<std::endl;
//DEBUG END
    if (argc < 2) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" startcapture || stopcapture"<<std::endl<<std::endl;
        return 1;
    }
    else if(argv[1] == std::string("stopcapture") && argc < 3) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" stopcapture PID"<<std::endl<<std::endl;
        return 1;
    }
    else if(argv[1] == std::string("deletecapture") && argc < 4) {
        std::cerr<<std::endl<<"Error! Usage: "<<argv[0]<<" deletecapture PID PATH"<<std::endl<<std::endl;
        return 1;
    }
    if (argv[1] == std::string("startcapture")) {
        startcapture(argc, argv);
    }
    else if (argv[1] == std::string("stopcapture")) {
        int pid = atoi(argv[2]);
        stopcapture(pid);
    }
    else if (argv[1] == std::string("deletecapture")) {
        int pid = atoi(argv[2]);
        if (pid != -1) {
             stopcapture(pid);
        }
        std::string path = std::string(argv[3]);
        std::filesystem::remove_all(path);
    }

    return 0;
}
