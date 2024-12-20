#include <iostream>
#include <regex>
int main (int argc, char **argv) {
    std::regex hostnameRegex(R"(^[a-zA-Z0-9][a-zA-Z0-9\-]*$)");
    if (std::regex_match(argv[1], hostnameRegex)) {
        std::cout<<"Passt"<<std::endl;
    }
    else {
        std::cout<<"Passt nicht"<<std::endl;
    }
return 0;
}
