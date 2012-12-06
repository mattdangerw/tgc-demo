#include "render/shape_group.h"

#include <cstdio>
#include <sstream>
#include <stdlib.h>

#include "util/error.h"

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
  FILE *file_pointer = fopen(filename.c_str(), "r");
  if (file_pointer == NULL) error("PathGroup file %s not found.\n", filename.c_str());
  char line[128];
  float priority = 0.0f;
  while (fgets(line, 128, file_pointer) != NULL) {
    std::istringstream stream(line);
    string path_file, texture_file;
    glm::vec2 texture_scale;
    stream >> path_file >> texture_file >> texture_scale.x >> texture_scale.y;
    if (path_file.empty() || texture_file.empty()) continue;

    Quad *fill = new Quad();
    fill->init(texture_file, texture_scale);
    PathShape *shape = new PathShape();
    shape->init(path_file, fill);
    shape->setParent(this);
    shape->setDisplayPriority(priority);

    fills_.push_back(fill);
    shapes_.push_back(shape);
    priority++;
  }
  lockChildren();
}
