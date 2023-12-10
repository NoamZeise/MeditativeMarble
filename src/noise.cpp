#include "noise.h"
#include <math.h>
#include <cstdint>
#include <graphics/logger.h>

namespace noise {

  // https://mrl.cs.nyu.edu/~perlin/noise/
  // 1-255 randomly distributed
  static int p[256] = {
      151, 160, 137, 91,  90,  15,  131, 13,  201, 95,  96,  53,  194, 233, 7,
      225, 140, 36,  103, 30,  69,  142, 8,   99,  37,  240, 21,  10,  23,  190,
      6,   148, 247, 120, 234, 75,  0,   26,  197, 62,  94,  252, 219, 203, 117,
      35,  11,  32,  57,  177, 33,  88,  237, 149, 56,  87,  174, 20,  125, 136,
      171, 168, 68,  175, 74,  165, 71,  134, 139, 48,  27,  166, 77,  146, 158,
      231, 83,  111, 229, 122, 60,  211, 133, 230, 220, 105, 92,  41,  55,  46,
      245, 40,  244, 102, 143, 54,  65,  25,  63,  161, 1,   216, 80,  73,  209,
      76,  132, 187, 208, 89,  18,  169, 200, 196, 135, 130, 116, 188, 159, 86,
      164, 100, 109, 198, 173, 186, 3,   64,  52,  217, 226, 250, 124, 123, 5,
      202, 38,  147, 118, 126, 255, 82,  85,  212, 207, 206, 59,  227, 47,  16,
      58,  17,  182, 189, 28,  42,  223, 183, 170, 213, 119, 248, 152, 2,   44,
      154, 163, 70,  221, 153, 101, 155, 167, 43,  172, 9,   129, 22,  39,  253,
      19,  98,  108, 110, 79,  113, 224, 232, 178, 185, 112, 104, 218, 246, 97,
      228, 251, 34,  242, 193, 238, 210, 144, 12,  191, 179, 162, 241, 81,  51,
      145, 235, 249, 14,  239, 107, 49,  192, 214, 31,  181, 199, 106, 157, 184,
      84,  204, 176, 115, 121, 50,  45,  127, 4,   150, 254, 138, 236, 205, 93,
      222, 114, 67,  29,  24,  72,  243, 141, 128, 195, 78,  66,  215, 61,  156,
      180};
  
  // grad based on grad in simplexnoise1234.cpp by Stefan Gustavson
  //https://web.archive.org/web/20221112031018/https://weber.itn.liu.se/~stegu/aqsis/aqsis-newnoise/
  float grad(int hash, float* input, int dim) {
      int h = hash & (1 << (dim+1)) - 1;
      float result = 0;
      for(int i = 0; i < dim - 1; i++) {
	  float val = 0;
	  float alt = (dim == 3 && i == 2) ?
	      ((h == 12 || h == 14) ? input[0] : input[i+1])
	      : input[i + 1];
	  val = (h < (1 << (dim - (i + 1)))) ? input[i] : alt;
	  result += (h&i) ? -val : val;
      }

      return result;
  }

  /// source of algorithm design:
  /// Course Notes: Chapter 2 - Noise Hardware, By Ken Perlin
  /// https://www.csee.umbc.edu/~olano/s2002c36/ch02.pdf
  float simplex(float x, float y, float z, float w) {
      const int dim = 3;
      float input[] = {x, y, z};

      //calc skew amount
      float skew = 0;
      for(int i = 0; i < dim; i++)
	  skew += input[i];
      float skewFactor = (sqrt(dim + 1) - 1) / dim;
      skew*=skewFactor;
      // calc coord of skewed cube + unskew amount
      int cube[dim];  //skewed cube
      float unskew = 0;
      for(int i = 0; i < dim; i++) {
	  float skewedIn = input[i] + skew;
	  cube[i] = floor(skewedIn);
	  unskew += cube[i];
      }
      float unskewFactor = (1 - (1/sqrt(dim+1)))/dim;
      unskew *= unskewFactor;

      // coord of input relative to unskewed cube
      float inputUnskew[dim];
      int order[dim];
      for(int i = 0; i < dim; i++) {  // in - unskewed cube
	  inputUnskew[i] = input[i] - cube[i] + unskew;
	  order[i] = i;
      }
      //sort unskewd input - highest to lowest
      for(int i = 1; i < dim; i++) {
	  for(int j = i; j > 0 && inputUnskew[order[j-1]] < inputUnskew[order[j]]; j--) {
	      int temp = order[j];
	      order[j] = order[j-1];
	      order[j-1] = temp;
	  }
      }
      // calc coords of the simplex that the unskewed input is in
      // always a traversal from
      // (0, 0, ...) -> (1, 1, ...)
      // a simplex has dim + 1 coords in dim dimensions
      int simplexPos[dim + 1][dim];
      for(int i = 0; i < dim; i++) {
	  simplexPos[0][i] = 0;
	  for(int j = 1; j < dim + 1; j++) {
	      simplexPos[j][i] =		  
		  simplexPos[j-1][i] +
		  (order[j-1] == i ? 1 : 0);
	  }
      }
      float unskewSimplex[dim + 1];
      for(int i = 0; i < dim + 1; i++) {
	  // unskew factor * sum of coords of simplex verts
	  unskewSimplex[i] = unskewFactor * i;
      }
      float simplexOffsets[dim+1][dim];
      for(int i = 0; i < dim; i++) {
	  for(int j = 0; j < dim + 1; j++) {
	      simplexOffsets[j][i] = inputUnskew[i] - simplexPos[j][i] +
		  unskewSimplex[j];
	  }
      }
      
      float result = 0;
      int hashedInds[dim + 1];
      for(int i = 0; i < dim + 1; i++) {
	  hashedInds[i] = 0;
	  float t = 0.6;
	  for(int j = dim - 1; j >= 0; j--) {
	      hashedInds[i] =
		  p[(cube[j] + hashedInds[i] + simplexPos[i][j]) & 0xff];
	      t -= pow(simplexOffsets[i][j], 2);
	  }
	  result += t <= 0 ? 0 :
	      pow(t, 4) * grad(hashedInds[i], simplexOffsets[i], dim);
      }      
      return 32 * result; //scale to range [-1,1]
  }

};
