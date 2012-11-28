#ifndef SRC_UTIL_BEZIER_H_
#define SRC_UTIL_BEZIER_H_

template <typename genType>
class BezierCurve {
  public:
    BezierCurve();
    BezierCurve(genType start, genType control, genType end);
    BezierCurve(genType start, genType control1, genType control2, genType end);
    genType interpolate(float t);
  private:
    bool cubic;
    genType start, control1, control2, end;
};


template <typename genType>
class BezierTrack {
  public:
    BezierTrack() : segment_(0), current_time_(0.0f), start_(glm::vec3(0.0f)) {}
    ~BezierTrack() {}
    void setStart(glm::vec3 start) { start_ = start; }
    void addCurve(BezierCurve<genType> curve, float arrival_time);
    glm::vec3 step(float delta_time);
    bool done() { return segment_ == controls_.size(); }
  private:
    int segment_;
    float current_time_;
    genType start_;
    vector<BezierCurve<genType> > controls_;
    vector<float> times_;
};

#endif  // SRC_UTIL_BEZIER_H_
