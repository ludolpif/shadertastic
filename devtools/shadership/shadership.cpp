#include <iostream>
#include <plugin-support.h>

int main() {
    std::cout << "Hello " << PLUGIN_NAME << " version " << PLUGIN_VERSION << std::endl;
    return 0;
}
