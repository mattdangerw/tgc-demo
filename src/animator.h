#ifndef SRC_ANIMATOR_H_
#define SRC_ANIMATOR_H_

#include <glm/glm.hpp>
#include <string>
#include <vector>
#include <map>
#include <queue>

using std::string;
using std::vector;
using std::map;
using std::queue;

struct Keyframe {
  int index;
  float time;
};

class Animation {
  public:
    Animation();
    ~Animation();
    // Set up functions.
    void addKeyframe(Keyframe key);
    void setRepeat(bool repeat) { repeat_ = repeat; }
    // Functions for during use (used by Animator).
    void start(int keyframe);
    void startForced(int keyframe1, int keyframe2, float lerp_t);
    bool forcing() { return forcing_; }
    int lastKeyframeIndex() { return last_keyframe_.index; }
    int nextKeyframeIndex() { return keyframes_[next_keyframe_].index; }
    float lerpT() { return lerp_t_; }
    void update(float delta_time);
    void stopAtNextTarget() { stop_next_target_ = true; }
    bool finished() { return finished_; }
    void currentKeyState(int *keyframes, float *lerp_ts);

  private:
    vector<Keyframe> keyframes_;
    bool repeat_, finished_, forcing_, stop_next_target_;
    Keyframe last_keyframe_;
    int next_keyframe_, last_keyframe_second_index_;
    float time_, lerp_t_, last_keyframe_lerp_t_;
};

class Animator {
  public:
    Animator();
    ~Animator();
    // Set up function.
    void setStartKeyframe(int index) { start_keyframe_index_ = index; }
    void addAnimation(string name, const Animation &animation) { animations_[name] = animation; }
    // Functions for during use.
    void queueAnimation(string name);
    void forceAnimation(string name);
    void update(float delta_time);
    void currentKeyState(int keyframes[], float lerp_ts[]);

  private:
    map<string, Animation> animations_;
    Animation *current_;
    Animation *queued_;
    int start_keyframe_index_;
};

#endif  // SRC_ANIMATOR_H_
