#include "Helpers.h"
#include <fstream>

/*------------------------------------------------------------------------------------------------------------------
-- SOURCE FILE: Helpers.cpp - This file contains the implementation of the helper methods.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void writeToFile(const char* data)
-- long delay(SYSTEMTIME t1, SYSTEMTIME t2)
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- (N/A)
------------------------------------------------------------------------------------------------------------------*/

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: writeToFile
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: void writeToFile(const char* data)
--              const char* data - data to write to a file
--
-- RETURNS: void
--
-- NOTES:
-- Call this function to write data to a file named output.txt.
------------------------------------------------------------------------------------------------------------------*/
void writeToFile(const char* data) {
    std::ofstream file;
    file.open("output.txt", std::ios_base::app);
    file << data;
    file.close();
}

/*------------------------------------------------------------------------------------------------------------------
-- FUNCTION: delay
--
-- DATE: Feb 17, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- INTERFACE: long delay(SYSTEMTIME t1, SYSTEMTIME t2)
--              SYSTEMTIME t1 - system time value of the initial time
--              SYSTEMTIME t2 - system time value of the ending time
--
-- RETURNS: long
--
-- NOTES:
-- Call this function to calculate the delay in milliseconds between two system times.
------------------------------------------------------------------------------------------------------------------*/
long delay (SYSTEMTIME t1, SYSTEMTIME t2)
{
    long d;

    d = (t2.wSecond - t1.wSecond) * 1000;
    d += (t2.wMilliseconds - t1.wMilliseconds);
    return(d);
}
