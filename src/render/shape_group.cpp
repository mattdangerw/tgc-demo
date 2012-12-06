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
  json_value *group_json = readFileToJSON(filename);
  float priority = 0;
  for (size_t i = 0; i < group_json->u.object.length; i++) {
    json_value elem = (*group_json)[i];
    string path_file = elem["path_file"];
    string texture_file = elem["texture_file"];
    glm::vec2 texture_scale;
    texture_scale.x = static_cast<float>(elem["texture_scale"]["x"].u.dbl);
    texture_scale.y = static_cast<float>(elem["texture_scale"]["y"].u.dbl);

    Quad *fill = new Quad();
    fill->init("content/textures/" + texture_file, texture_scale);
    PathShape *shape = new PathShape();
    shape->init("content/paths/" + path_file, fill);
    shape->setParent(this);
    shape->setDisplayPriority(priority);

    fills_.push_back(fill);
    shapes_.push_back(shape);
    priority++;
  }
  lockChildren();
}
