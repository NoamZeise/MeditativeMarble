#ifndef NOISE_H
#define NOISE_H

namespace noise {
  /// Simplex Noise returns number in range [-1, 1]
  /// set dim to size of input
  /// (a given dim must be used in noise.cpp to be defined)
  /// defined for: 2, 3, 4, 5, 6
  template <unsigned int dim>
  float simplex(float *input);
  volatile float createTemplateDefsForSimplex();
};
#endif /* NOISE_H */
