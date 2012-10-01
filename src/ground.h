#ifndef SRC_GROUND_H_
#define SRC_GROUND_H_

#include <glm.hpp>
#include <vector>

#include "path_shape.h"
#include "quad.h"
#include "tree.h"
#include "game_entity.h"

using std::vector;

class Ground : public GameEntity {
  public:
    Ground();
    ~Ground();
    void init();
    float width();
    float heightAt(float x);
    void update(float delta_time, GameState *state) {}
    void getTargets(vector<Target> &targets);
    void colorTarget(Target target);
  private:
    void initTrees();
    void initPathShape();
    vector<glm::vec2> points_;
    // Drawables.
    PathShape shape_;
    TiledTexturedQuad quad_;
    TiledTexturedQuad background_;
    vector<Tree> trees_;
    int ground_target_id_, background_target_id_, tree6_target_id_,
      tree7_target_id_, tree8_target_id_, tree9_target_id_;
};

#endif  // SRC_GROUND_H_
