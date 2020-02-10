#include "Helpers.h"
#include <fstream>

void writeToFile(const char* data) {
    std::ofstream file;
    file.open("output.txt");
    file << data;
    file.close();
}

long delay (SYSTEMTIME t1, SYSTEMTIME t2)
{
    long d;

    d = (t2.wSecond - t1.wSecond) * 1000;
    d += (t2.wMilliseconds - t1.wMilliseconds);
    return(d);
}
