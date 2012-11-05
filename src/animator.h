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
    // Repeat animation forever till asked to stop.
    bool repeats() { return repeat_; }
    void setRepeats(bool repeat) { repeat_ = repeat; }

    // Functions for during use (used by Animator).
    // Start animation from the given keyframe.
    void start(int keyframe_index);
    // Start animation in the middle of another animation. So will briefly be interpolating between three keyframes.
    void startForced(int keyframe1, int keyframe2, float lerp_t);
    void stopAtNextKeyframe() { stop_next_keyframe_ = true; }
    void update(float delta_time);
    // Is currently interpolating with three keyframes.
    bool forcing() { return forcing_; }
    bool finished() { return finished_; }
    // The last keyframe used in animation. If forcing there is no one keyframe, use currentKeyState.
    int lastKeyframeIndex() { return last_keyframe_.index; }
    int nextKeyframeIndex() { return keyframes_[next_keyframe_].index; }
    float lerpT() { return lerp_t_; }
    // Gets the current keyframe state for use in rendering.
    // Always gives three keyframes and two lerp values, no matter how many keyframes are actually used.
    void currentKeyState(int *keyframes, float *lerp_ts);

  private:
    vector<Keyframe> keyframes_;
    bool repeat_, finished_, forcing_, stop_next_keyframe_;
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
    // Queue up an animation. This will cause any repeating animations ahead in queue to stop.
    void queueAnimation(string name);
    // Force start an animation. This will delete the queue and move this to front.
    // If the current animation is between keyframes, this will treat that lerped target as the starting keyframe.
    // Forcint the new animation to temporarily lerp between three keyframes.
    void forceAnimation(string name);
    void update(float delta_time);
    // Gets the current keyframe state for use in rendering.
    // Always gives three keyframes and two lerp values, no matter how many keyframes are actually used.
    void currentKeyState(int keyframes[], float lerp_ts[]);

  private:
    map<string, Animation> animations_;
    queue<Animation *> queued_;
    int start_keyframe_index_;
};

#endif  // SRC_ANIMATOR_H_
