#include "util/read_file.h"

#include <cstdio>

#include "util/error.h"

char *readFileToCString(string filename) {
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) error("File %s not found.\n", filename.c_str());
  fseek(file_pointer, 0, SEEK_END);
  long size = ftell(file_pointer);
  fseek(file_pointer, 0, SEEK_SET);
  char *text = new char[size + 1];
  long chars_read = fread(text, sizeof(char), size, file_pointer);
  text[chars_read] = '\0';
  fclose(file_pointer);
  return text;
}

json_value *readFileToJSON(string filename) {
  char *raw_text = readFileToCString(filename);
  json_settings settings;
  memset (&settings, 0, sizeof (json_settings));
  char err[512];
  json_value *json = json_parse_ex(&settings, raw_text, err);
  if (json == NULL) {
    error("Error parsing JSON in file %s.\n%s", filename.c_str(), err);
  }
  delete raw_text;
  return json;
}
