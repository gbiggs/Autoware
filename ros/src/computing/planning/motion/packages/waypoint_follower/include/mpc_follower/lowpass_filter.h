#pragma once
#include <cmath>
#include <vector>

class Butterworth2d {
private:
  double y1_, y2_, u1_, u2_, a0_, a1_, a2_, b0_, b1_, b2_;

public:
  Butterworth2d(double dt = 0.1, double f_cutoff_hz = 10.0) {
    initialize(dt, f_cutoff_hz);
  };
  ~Butterworth2d(){};


  void initialize(const double &dt, const double &f_cutoff_hz) {
    y1_ = 0.0;
    y2_ = 0.0;
    u2_ = 0.0;
    u1_ = 0.0;

    /* 2d butterworth lowpass filter with bi-linear transformation */
    double wc = 2.0 * M_PI * f_cutoff_hz;
    double n = 2 / dt;
    a0_ = n * n + sqrt(2) * wc * n + wc * wc;
    a1_ = 2 * wc * wc - 2 * n * n;
    a2_ = n * n - sqrt(2) * wc * n + wc * wc;
    b0_ = wc * wc;
    b1_ = 2 * b0_;
    b2_ = b0_;
  }

  double filter(double &u0) {
    double y0 = (b2_ * u2_ + b1_ * u1_ + b0_ * u0 - a2_ * y2_ - a1_ * y1_) / a0_;
    y2_ = y1_;
    y1_ = y0;
    u2_ = u1_;
    u1_ = u0;
    return y0;
  }

  void filt_vector(const std::vector<double> &t, std::vector<double> &u) {
    double y1 = u.at(0);
    double y2 = u.at(0);
    double u2 = u.at(0);
    double u1 = u.at(0);
    double y0 = 0.0;
    double u0 = 0.0;
    for (uint i = 0; i < u.size(); ++i) {
      u0 = u.at(i);
      y0 = (b2_ * u2 + b1_ * u1 + b0_ * u0 - a2_ * y2 - a1_ * y1) / a0_;
      y2 = y1;
      y1 = y0;
      u2 = u1;
      u1 = u0;
      u.at(i) = y0;
    }
  }

  // filtering forward and backward direction
  void filtfilt_vector(const std::vector<double> &t, std::vector<double> &u) {
    std::vector<double> u_rev(u);

    // forward filtering
    filt_vector(t, u);

    // backward filtering
    std::reverse(u_rev.begin(), u_rev.end());
    filt_vector(t, u_rev);
    std::reverse(u_rev.begin(), u_rev.end());

    // merge
    for (uint i = 0; i < u.size(); ++i) {
      u[i] = (u[i] + u_rev[i]) * 0.5;
    }
  }
};