#include <iostream>

#include "implementations/color2gray.h"

int main(int argc, char* argv[])
{
    int err = 0;
    std::string dev_type = parseArgs(argc, argv, err);
    if (err) {
        std::cerr << dev_type << std::endl;
        return err;
    }
    std::string res = measureExecTime(color2gray, dev_type);
    std::cout << "OpenCLRunner, Exec time: " << res << std::endl;
    return 0;
}
