#ifndef SETTINGS_H_
#define SETTINGS_H_

#include <string>
#include <map>

#include "util/json.h"

using std::string;
using std::map;

void loadSettings(string filename);

const json_value &getSetting(string name);

#endif  // SETTINGS_H_