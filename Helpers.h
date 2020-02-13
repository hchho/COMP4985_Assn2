#ifndef HELPERS_H
#define HELPERS_H

#include <WinSock2.h>

/*------------------------------------------------------------------------------------------------------------------
-- HEADER FILE: Helpers.h - This file contains generic helper functions unrelated to TCP/UDP.
--
-- PROGRAM: Knekt
--
-- FUNCTIONS:
-- void writeToFile(const char*)
-- long delay(SYSTEMTIME t1, SYSTEMTIME t2)
--
-- DATE: Feb 12, 2020
--
-- REVISIONS: N/A
--
-- DESIGNER: Henry Ho
--
-- PROGRAMMER: Henry Ho
--
-- NOTES:
-- This file should only contain generic helper methods unrelated to TCP/UDP connections.
------------------------------------------------------------------------------------------------------------------*/

void writeToFile(const char*);

long delay (SYSTEMTIME t1, SYSTEMTIME t2);

#endif // HELPERS_H
