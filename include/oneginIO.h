/// @file
/// @brief Input and output functions

#ifndef ONEGIN_IO_H
#define ONEGIN_IO_H

/*!
    @brief Open file

    If file can't be opened, returns ERROR
*/
enum status openFile(FILE **file, const char *fileName, const char *mode);

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

enum status getFileSize(const char *fileName, size_t *size);

void splittedStringToArray(text_t *textInfo);

void splitString(text_t *textInfo);

#endif
