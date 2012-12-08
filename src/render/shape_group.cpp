#include "render/shape_group.h"

#include <cstdio>
#include <sstream>
#include <stdlib.h>

#include "util/read_file.h"

ShapeGroup::ShapeGroup() {}

ShapeGroup::~ShapeGroup() {
  for (vector<Quad *>::const_iterator it = fills_.begin(); it != fills_.end(); ++it) {
    delete *it;
  }
  for (vector<PathShape *>::const_iterator it = shapes_.begin(); it != shapes_.end(); ++it) {
    delete *it;
  }
}

void ShapeGroup::init(string filename) {
  json_value &group_json = readFileToJSON(filename);
  float priority = 0;
  const json_value &json_fills = group_json["fills"];
  const json_value &json_frames = group_json["frames"];
  const json_value &json_animations = group_json["animations"];
  int num_fills = json_fills.getLength();
  for (int fill_index = 0; fill_index < num_fills; fill_index++) {
    const json_value &json_fill = json_fills[fill_index];
    string texture_file = json_fill["file"].getString();
    glm::vec2 texture_scale;
    texture_scale.x = json_fill["scale"]["x"].getFloat();
    texture_scale.y = json_fill["scale"]["y"].getFloat();

    Quad *fill = new Quad();
    PathShape *shape = new PathShape();
    fill->init("content/textures/" + texture_file, texture_scale);
    vector<NamedShape> keyframes;
    string start_frame;
    for (int frame_index = 0; frame_index < json_frames.getLength(); frame_index++) {
      NamedShape keyframe;
      keyframe.name = json_frames.getNameAt(frame_index);
      if (frame_index == 0) start_frame = keyframe.name;
      const json_value &json_files = json_frames.getValueAt(frame_index);
      assert(json_files.u.array.length == num_fills);
      keyframe.file = "content/paths/" + json_files[fill_index].getString();
      keyframes.push_back(keyframe);
    }
    animator_.init(json_animations, start_frame);
    shape->init(keyframes, fill, &animator_);
    shape->setParent(this);
    shape->setDisplayPriority(priority);
    fills_.push_back(fill);
    shapes_.push_back(shape);
    priority++;
  }
  json_value_free(&group_json);
  lockChildren();
}
