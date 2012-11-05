#include "animator.h"

Animation::Animation()
  : repeat_(false),
    finished_(false),
    forcing_(false),
    stop_next_keyframe_(false),
    next_keyframe_(0),
    time_(0.0f),
    lerp_t_(0.0f) {
  last_keyframe_.index = 0;
  last_keyframe_.time = 0.0f;  
}

Animation::~Animation() {}

void Animation::addKeyframe(Keyframe key) {
  assert(key.time > 0);
  // Add the keyframe in and keep things sorted.
  // Obviously inefficient for many many keyframes, but that's not in the usage plan.
  vector<Keyframe>::iterator it;
  for (it = keyframes_.begin(); it != keyframes_.end(); ++it) {
    if ( key.time < it->time) break;
  }
  keyframes_.insert(it, key);
}

void Animation::start(int keyframe_index) {
  last_keyframe_.index = keyframe_index;
  last_keyframe_.time = 0.0f;
  next_keyframe_ = 0;
  time_ = 0.0f;
  lerp_t_ = 0.0f;
  finished_ = keyframes_.empty();
  stop_next_keyframe_ = false;
}

void Animation::startForced(int keyframe1, int keyframe2, float lerp_t) {
  forcing_ = true;
  last_keyframe_second_index_ = keyframe2;
  last_keyframe_lerp_t_ = lerp_t;
  start(keyframe1);
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
        start(last_keyframe_.index);
        update(delta_time - last_keyframe_.time);
      } else {
        finished_ = true;
      }
      return;
    }
  }
  lerp_t_ = (time_ - last_keyframe_.time) / (keyframes_[next_keyframe_].time - last_keyframe_.time);
}

void Animation::currentKeyState(int keyframes[], float lerp_ts[]) {
  if (forcing_ == true) {
    // We forced this animation to start in middle of another. So interpolting three different keyframes.
    keyframes[0] = last_keyframe_.index;
    keyframes[1] = last_keyframe_second_index_;
    keyframes[2] = keyframes_[next_keyframe_].index;
    lerp_ts[0] = last_keyframe_lerp_t_;
    lerp_ts[1] = lerp_t_;
  } else if (finished_) {
    // Nothings really interpoling here, we just resting on the final keyframe of animation.
    keyframes[0] = keyframes[1] = keyframes[2] = last_keyframe_.index;
    lerp_ts[0] = lerp_ts[1] = 0.0f;
  } else {
    // Interpolating between two keyframes in this animation.
    keyframes[0] = last_keyframe_.index;
    keyframes[1] = keyframes[2] = keyframes_[next_keyframe_].index;
    lerp_ts[0] = lerp_t_;
    lerp_ts[1] = 0.0f;
  }
}

Animator::Animator() : start_keyframe_index_(0) {}

Animator::~Animator() {}

void Animator::queueAnimation(string name) {
  Animation *next = &animations_[name];
  if (queued_.empty()) {
    // Nothings been animated yet. Start this one right away.
    next->start(start_keyframe_index_);
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
    next->start(start_keyframe_index_);
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
        next->start(prev->lastKeyframeIndex());
      } else {
        next->startForced(prev->lastKeyframeIndex(), prev->nextKeyframeIndex(), prev->lerpT());
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
      queued_.front()->start(last->lastKeyframeIndex());
    }
    queued_.front()->update(delta_time);
  }
}

void Animator::currentKeyState(int keyframes[], float lerp_ts[]) {
  if (!queued_.empty()) {
    return queued_.front()->currentKeyState(keyframes, lerp_ts);
  }
  keyframes[0] = keyframes[1] = keyframes[2] = start_keyframe_index_;
  lerp_ts[0] = lerp_ts[1] = 0.0f;
}
