/// @file
/// @brief Input and output functions

#ifndef ONEGIN_IO_H
#define ONEGIN_IO_H

/*!
    @brief Reads strings from file

    @param fileName [in] Name of file

    @return ERROR or FAIL if something went wrong, SUCCESS otherwise
*/
enum status readTextFromFile(const char* fileName, text_t *textInfo);

/*!
    @brief Prints strings from array of strings

    @param file     [in] Stream to print strings
*/
void writeTextToFile(string_t *lines, size_t size, FILE *file);

size_t getFileSize(const char *fileName);

void splittedStringToArray(text_t *textInfo);

void splitString(text_t *textInfo);

#endif
