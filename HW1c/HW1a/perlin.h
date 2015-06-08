//added the function that lets you actually get 3 dimensional noise - Cory Boatright
#ifndef PERLIN_H_

#define PERLIN_H_

#include <stdlib.h>
#include "Vector.h"

#define SAMPLE_SIZE 1024

class Perlin {
public:

  Perlin(int octaves,float freq,float amp,int seed);

  float Get(float x,float y)  {
    float vec[2];
    vec[0] = x;
    vec[1] = y;
    return perlin_noise_2D(vec);
  };

  //Taadaa, 3D noise.  Thank me later - Cory Boatright
  float Get(float x, float y, float z) {
	  float vec[3];
	  vec[0] = x;
	  vec[1] = y;
	  vec[2] = z;
	  return perlin_noise_3D(vec);
  }

  float Get(Vector &v) {
	  float vec[3];
	  vec[0] = v[0];
	  vec[1] = v[1];
	  vec[2] = v[2];
	  return perlin_noise_3D(vec);
  }

private:
  void init_perlin(int n,float p);
  float perlin_noise_2D(float vec[2]);
  float perlin_noise_3D(float vec[3]);

  float noise1(float arg);
  float noise2(float vec[2]);
  float noise3(float vec[3]);
  void normalize2(float v[2]);
  void normalize3(float v[3]);
  void init(void);

  int mOctaves;
  float mFrequency;
  float mAmplitude;
  int mSeed;

  int p[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  float g3[SAMPLE_SIZE + SAMPLE_SIZE + 2][3];
  float g2[SAMPLE_SIZE + SAMPLE_SIZE + 2][2];
  float g1[SAMPLE_SIZE + SAMPLE_SIZE + 2];
  bool mStart;
};

#endif