#include "Helpers.h"
#include <fstream>

void writeToFile(const char* data) {
    std::ofstream file;
    file.open("output.txt");
    file << data;
    file.close();
}
