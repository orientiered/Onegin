/// @file
/// @brief Input and output functions

#ifndef ONEGIN_IO_H
#define ONEGIN_IO_H

/*!
    @brief Reads strings from file

    @param fileName [in] Name of file
    @param text     [out] Array of strings
    @param length   [out] Number of read strings

    @return BAD_EXIT or FAIL if something went wrong, GOOD_EXIT otherwise
*/
enum error readFromFile(const char* fileName, char ***text, size_t *length);

/*!
    @brief Prints strings from array of strings

    @param index    [in] Array of strings
    @param length   [in] Number of strings to print
    @param file     [in] Stream to print strings
*/
void printText(char *index[], size_t length, FILE* file);

#endif
