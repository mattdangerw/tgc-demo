#include "render/shape_group.h"

#include <cstdio>
#include <sstream>
#include <stdlib.h>

#include "util/read_file.h"

ShapeGroup::ShapeGroup() {}

ShapeGroup::~ShapeGroup() {
  for (map<string, ShapeAndFill *>::iterator it = shapes_.begin(); it != shapes_.end(); ++it) {
    delete it->second;
  }
}

void ShapeGroup::init(string filename) {
  json_value &group_json = readFileToJSON("content/groups/" + filename);
  float priority = 0;
  const json_value &json_fills = group_json["fills"];
  const json_value &json_frames = group_json["frames"];
  const json_value &json_animations = group_json["animations"];
  const json_value &json_colors = group_json["colors"];
  bool multiple_frames = json_frames.getLength() > 1;
  bool has_animations = json_animations.type != json_none;
  bool has_colors = json_colors.type != json_none;
  int num_fills = json_fills.getLength();
  string start_frame;
  for (int fill_index = 0; fill_index < num_fills; fill_index++) {
    ShapeAndFill *shape_and_fill = new ShapeAndFill();
    Quad &fill = shape_and_fill->fill;
    PathShape &shape = shape_and_fill->shape;
    // Parse fill.
    string fill_name = json_fills.getNameAt(fill_index);
    const json_value &json_fill = json_fills.getValueAt(fill_index);
    string texture_file = json_fill["file"].getString();
    glm::vec2 texture_scale = json_fill["scale"].getVec2();
    fill.init("content/textures/" + texture_file, texture_scale);
    // Parse shape.
    if (multiple_frames) {
      vector<NamedFile> frames;
      for (int frame_index = 0; frame_index < json_frames.getLength(); frame_index++) {
        NamedFile frame;
        frame.name = json_frames.getNameAt(frame_index);
        if (frame_index == 0) start_frame = frame.name;
        const json_value &json_files = json_frames.getValueAt(frame_index);
        assert(json_files.getLength() == num_fills);
        frame.file = "content/paths/" + json_files[fill_name].getString();
        frames.push_back(frame);
      }
      shape.init(frames, &fill, &animator_);
    } else {
      const json_value &json_files = json_frames.getValueAt(0);
      string filename = "content/paths/" + json_files[fill_name].getString();
      shape.init(filename, &fill);
    }
    shape.setParent(this);
    shape.setDisplayPriority(priority);
    // Parse color.
    if (has_colors) {
      colors_[fill_name] = json_colors[fill_name].getVec4();
    } else {
      colors_[fill_name] = glm::vec4(0.0f);
    }
    // Add to group.
    shapes_[fill_name] = shape_and_fill;
    priority++;
  }
  if (has_animations) {
    animator_.init(json_animations, start_frame);
  } else {
    animator_.init(start_frame);
  }
  json_value_free(&group_json);
  lockChildren();
}

void ShapeGroup::colorize() {
  for (map<string, ShapeAndFill *>::iterator it = shapes_.begin(); it != shapes_.end(); ++it) {
    it->second->fill.setColorMultiplier(colors_[it->first]);
  }
}

void ShapeGroup::setColorMultipliers(glm::vec4 color) {
  for (map<string, ShapeAndFill *>::iterator it = shapes_.begin(); it != shapes_.end(); ++it) {
    it->second->fill.setColorMultiplier(color);
  }
}

void ShapeGroup::setColorAdditions(glm::vec4 color) {
  for (map<string, ShapeAndFill *>::iterator it = shapes_.begin(); it != shapes_.end(); ++it) {
    it->second->fill.setColorAddition(color);
  }
}
