#include <iostream>
#include "functions.hpp"

int main (int argc, char **argv) {
    if (!validate_ip_address(argv[1], ","))
        std::cout<<"Falsch"<<std::endl;
    else
        std::cout<<"Korrekt"<<std::endl;
}
