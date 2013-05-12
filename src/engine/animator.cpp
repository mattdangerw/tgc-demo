#include "engine/animator.h"

Animation::Animation()
  : repeat_(false),
    finished_(false),
    forcing_(false),
    stop_next_keyframe_(false),
    next_keyframe_(0),
    time_(0.0f),
    lerp_t_(0.0f) {}

Animation::~Animation() {}

void Animation::addKeyframe(Keyframe key) {
  assert(key.time > 0);
  //Added relative to last time, but we'll store global time as it make calculations easier.
  if (!keyframes_.empty()) key.time += keyframes_.back().time;
  keyframes_.push_back(key);
}

void Animation::start(string frame_name) {
  last_keyframe_.name = frame_name;
  last_keyframe_.time = 0.0f;
  next_keyframe_ = 0;
  time_ = 0.0f;
  lerp_t_ = 0.0f;
  finished_ = keyframes_.empty();
  stop_next_keyframe_ = false;
}

void Animation::startForced(string frame_name1, string frame_name2, float lerp_t) {
  forcing_ = true;
  last_keyframe_second_name_ = frame_name2;
  last_keyframe_lerp_t_ = lerp_t;
  start(frame_name1);
}

void Animation::update(float delta_time) {
  if (finished_) return;
  time_ += delta_time;
  while (time_ > keyframes_[next_keyframe_].time) {
    // Move on to a next keyframe target.
    last_keyframe_ = keyframes_[next_keyframe_];
    ++next_keyframe_;
    forcing_ = false;
    if (stop_next_keyframe_) {
      finished_ = true;
      return;
    }
    if (next_keyframe_ == keyframes_.size()) {
      // We just finished. Start over if repeat is true.
      if (repeat_) {
        float remaining_time = time_ - last_keyframe_.time;
        start(last_keyframe_.name);
        update(remaining_time);
      } else {
        finished_ = true;
      }
      return;
    }
  }
  lerp_t_ = (time_ - last_keyframe_.time) / (keyframes_[next_keyframe_].time - last_keyframe_.time);
}

void Animation::currentState(string frames[], float lerp_ts[]) {
  if (forcing_ == true) {
    // We forced this animation to start in middle of another. So interpolting three different keyframes.
    frames[0] = last_keyframe_.name;
    frames[1] = last_keyframe_second_name_;
    frames[2] = keyframes_[next_keyframe_].name;
    lerp_ts[0] = last_keyframe_lerp_t_;
    lerp_ts[1] = lerp_t_;
  } else if (finished_) {
    // Nothing needs interpoling here, we just resting on the final keyframe of animation.
    frames[0] = frames[1] = frames[2] = last_keyframe_.name;
    lerp_ts[0] = lerp_ts[1] = 0.0f;
  } else {
    // Interpolating between two keyframes in this animation.
    frames[0] = last_keyframe_.name;
    frames[1] = frames[2] = keyframes_[next_keyframe_].name;
    lerp_ts[0] = lerp_t_;
    lerp_ts[1] = 0.0f;
  }
}

Animator::Animator() {}

Animator::~Animator() {}

void Animator::init(map<string, Animation> animations, string start_frame) {
  init(start_frame);
  animations_ = animations;
}

void Animator::init(const json_value &json_animations, string start_frame) {
  init(start_frame);
  for (int i = 0; i < json_animations.getLength(); i++) {
    string name = json_animations.getNameAt(i);
    const json_value &json_animation = json_animations.getValueAt(i);
    Animation animation;
    animation.setRepeats(json_animation["repeats"].getBoolean());
    const json_value &json_keyframes = json_animation["keyframes"];
    for (int j = 0; j < json_keyframes.getLength(); j++) {
      const json_value &json_keyframe = json_keyframes[j];
      Keyframe keyframe;
      keyframe.name = json_keyframe["name"].getString();
      keyframe.time = json_keyframe["time"].getFloat();
      animation.addKeyframe(keyframe);
    }
    animations_[name] = animation;
  }
}

void Animator::queueAnimation(string name) {
  assert(animations_.count(name) != 0);
  Animation *next = &animations_[name];
  if (queued_.empty()) {
    // Nothings been animated yet. Start this one right away.
    next->start(start_keyframe_name_);
  } else if (queued_.front()->repeats()) {
    // This animation repeats forever so we need to ask it to stop.
    queued_.front()->stopAtNextKeyframe();
  }
  queued_.push(next);
}

void Animator::forceAnimation(string name) {
  Animation *next = &animations_[name];
  if (queued_.empty()) {
    // Nothings been animated yet. Start this one right away.
    next->start(start_keyframe_name_);
  } else {
    Animation *prev = queued_.front();
    // Clear queue.
    queued_ = queue<Animation *>();
    if (prev->forcing()) {
      // We can't force becuase the current animation is already using three keyframes to force.
      // Add current animation back to queue and tell it to finish soon.
      prev->stopAtNextKeyframe();
      queued_.push(prev);
    } else {
      // Force this animation to start from two interpolated keyframes.
      if (prev->finished()) {
        next->start(prev->lastKeyframeName());
      } else {
        next->startForced(prev->lastKeyframeName(), prev->nextKeyframeName(), prev->lerpT());
      }
    }
  }
  queued_.push(next);
}

void Animator::update(float delta_time) {
  if (!queued_.empty()) {
    if (queued_.size() > 1 && queued_.front()->finished()) {
      // Move on to next animation.
      Animation *last = queued_.front();
      queued_.pop();
      queued_.front()->start(last->lastKeyframeName());
    }
    queued_.front()->update(delta_time);
  }
}

void Animator::currentState(string frames[], float lerp_ts[]) {
  if (!queued_.empty()) {
    return queued_.front()->currentState(frames, lerp_ts);
  }
  frames[0] = frames[1] = frames[2] = start_keyframe_name_;
  lerp_ts[0] = lerp_ts[1] = 0.0f;
}
