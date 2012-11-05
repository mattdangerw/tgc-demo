#include "animator.h"

Animation::Animation()
  : repeat_(false),
    finished_(false),
    forcing_(false),
    stop_next_target_(false),
    next_keyframe_(0),
    time_(0.0f),
    lerp_t_(0.0f) {
  last_keyframe_.index = 0;
  last_keyframe_.time = 0.0f;  
}

Animation::~Animation() {}

void Animation::addKeyframe(Keyframe key) {
  assert(key.time > 0);
  for (vector<Keyframe>::iterator it = keyframes_.begin(); it != keyframes_.end(); ++it) {
    if ( key.time < it->time) {
      keyframes_.insert(it, key);
    }
  }
}

void Animation::start(int keyframe) {
  last_keyframe_.index = keyframe;
  last_keyframe_.time = 0.0f;
  next_keyframe_ = 0;
  time_ = 0.0f;
  lerp_t_ = 0.0f;
  finished_ = keyframes_.empty();
  stop_next_target_ = false;
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
    last_keyframe_ = keyframes_[next_keyframe_];
    ++next_keyframe_;
    forcing_ = false;
    if (stop_next_target_) {
      finished_ = true;
      return;
    }
    if (next_keyframe_ > keyframes_.size()) {
      if (repeat_) {
        start(last_keyframe_.index);
        update(delta_time - last_keyframe_.time);
      } else {
        finished_ = true;
      }
      return;
    }
  }
  lerp_t_ = (time_ - last_keyframe_.time) / (keyframes_[next_keyframe_].index - last_keyframe_.time);
}

void Animation::currentKeyState(int keyframes[], float lerp_ts[]) {
  if (forcing_ == true) {
    keyframes[0] = last_keyframe_.index;
    keyframes[1] = last_keyframe_second_index_;
    keyframes[2] = keyframes_[next_keyframe_].index;
    lerp_ts[0] = last_keyframe_lerp_t_;
    lerp_ts[1] = lerp_t_;
  } else if (finished_) {
    keyframes[0] = keyframes[1] = keyframes[2] = last_keyframe_.index;
    lerp_ts[0] = lerp_ts[1] = 0.0f;
  } else {
    keyframes[0] = last_keyframe_.index;
    keyframes[1] = keyframes[2] = keyframes_[next_keyframe_].index;
    lerp_ts[0] = lerp_t_;
    lerp_ts[1] = 0.0f;
  }
}

Animator::Animator()
  : current_(NULL),
    queued_(NULL),
    start_keyframe_index_(0) {}

Animator::~Animator() {}

void Animator::queueAnimation(string name) {
  if (current_ != NULL) {
    queued_ = &animations_[name];
  } else {
    current_ = &animations_[name];
    current_->start(start_keyframe_index_);
  }
}

void Animator::forceAnimation(string name) {
  if (current_ != NULL) {
    // TODO: erase rest of queue.
    if (current_->forcing()) {
      queueAnimation(name);
    } else {
      Animation *next = &animations_[name];
      next->startForced(current_->lastKeyframeIndex(), current_->nextKeyframeIndex(), current_->lerpT());
      current_ = next;
    }
  } else {
    current_ = &animations_[name];
    current_->start(start_keyframe_index_);
  }
}

void Animator::update(float delta_time) {
  if (current_ != NULL) {
    if (queued_ != NULL && current_->finished()) {
      queued_->start(current_->lastKeyframeIndex());
      current_ = queued_;
    }
    current_->update(delta_time);
  }
}

void Animator::currentKeyState(int keyframes[], float lerp_ts[]) {
  if (current_ != NULL) {
    return current_->currentKeyState(keyframes, lerp_ts);
  }
  keyframes[0] = keyframes[1] = keyframes[2] = start_keyframe_index_;
  lerp_ts[0] = lerp_ts[1] = 0.0f;
}
