#include "character.h"
#include "GL\glfw.h"

Character::Character() {
  renderer = NULL;
}

Character::~Character() {}

void Character::init(Renderer *renderer, Ground *ground) {
  this->renderer = renderer;
  this->ground = ground;
  renderer->addDrawable(this);
}

void Character::draw() {
}

string Character::shadingGrounp() {
  return "default";
}
