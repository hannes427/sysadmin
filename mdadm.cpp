#include <iostream>
#include "functions.hpp"

std::string fetch_md_raid (std::string device) {
    std::string mdadm_cmd = "/usr/sbin/mdadm --detail "+device+" | /usr/bin/jc --mdadm";
    std::string output = exec(mdadm_cmd.c_str());
    return output;
}
