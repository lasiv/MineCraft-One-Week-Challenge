#ifndef FILEUTIL_H_INCLUDED
#define FILEUTIL_H_INCLUDED

#include <string>

/**
 * @brief Reads the contents of a file into a string.
 * 
 * @param filePath The path to the file to be read.
 * 
 * @return A string containing the contents of the file.
 * 
 * @throws std::runtime_error If the file cannot be opened.
 * 
 * @details
 * This function opens the specified file and reads its contents into a
 * string. It uses an input file stream to read the file and a string
 * stream to store the contents. If the file cannot be opened, it throws
 * a runtime error with an appropriate message.
 */
std::string getFileContents(const std::string &filePath);

#endif // FILEUTIL_H_INCLUDED
