#include "util/settings.h"

#include "util/read_file.h"
#include "util/json.h"
#include "error.h"

class Settings{
  public:
    Settings() : loaded_(false) {}
    ~Settings() {
      if (loaded_) json_value_free(json_);
    }
    void load(string filename) {
      if (loaded_) {
        json_value_free(json_);
        settings_.clear();
      }
      json_ = &readFileToJSON(filename);
      for (int i = 0; i < json_->getLength(); i++) {
        settings_[json_->getNameAt(i)] = &json_->getValueAt(i);
      }
      loaded_ = true;
    }
    const json_value &byName(string name) {
      if (settings_.count(name) == 0) error("No such setting!");
      return *settings_[name];
    }
  private:
    bool loaded_;
    json_value *json_;
    map<string, const json_value *> settings_;
};

static Settings the_settings;

void loadSettings(string filename) {
  the_settings.load(filename);
}

const json_value &getSetting(string name) {
  return the_settings.byName(name);
}
