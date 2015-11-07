#include <iostream>
#include "variant.h"


int main(int argc, char *argv[]) {

    using variant_t = Variant<int, std::string>;
    variant_t variant;

    variant = 7;
    std::cout << variant << std::endl;

    variant = "Test";
    std:: cout << variant << std::endl;

    return 0;
}
