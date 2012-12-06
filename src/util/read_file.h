#ifndef READ_FILE_H_
#define READ_FILE_H_

#include <string>

#include "util/json.h"

using std::string;

// Reads entire file into a char star. Your responsible for freeing the result.
char *readFileToCString(string filename);

// Parses a file into a json object. Again you need to free this with json_value_free.
json_value *readFileToJSON(string filename);

#endif  // READ_FILE_H_
