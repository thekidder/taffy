#ifndef  __PARTICLE_H__
#define  __PARTICLE_H__

#include "allgl.h"
#include "sore_util.h"

static const GLfloat defaultXGrav =  0.0f;
const GLfloat defaultYGrav = 0.0002f;
const GLfloat defaultZGrav =  0.0f;

const GLfloat defaultXVel = 0.0f;
const GLfloat defaultYVel = 0.001f;
const GLfloat defaultZVel = 0.0f;


const GLfloat defaultRStep = 0.004f;
const GLfloat defaultGStep = 0.004f;
const GLfloat defaultBStep = 0.004f;

const GLfloat defaultDecayRate = 0.01f;

const unsigned long defaultNumParticles = 800;


float getRandomMinMax( float fMin, float fMax );


struct Particle
{
	GLfloat xpos, ypos, zpos;
	GLfloat xvel, yvel, zvel;
	GLfloat r, g, b;
	GLfloat life; //or alpha
	bool active;
};

class ParticleEngine
{
	public:
		ParticleEngine(GLfloat* colorRange, GLfloat* gravityVector, unsigned long numParticles);
		ParticleEngine(GLfloat* gravityVector, unsigned long numParticles);
		ParticleEngine(unsigned long numParticles);
		ParticleEngine(GLuint tex);
		ParticleEngine();
		
		void Initialize();
		void Stop();
		void SetColorRange(GLfloat* colorRange); //array of 6 GLfloats representing RGB min-max
		void SetPosition(GLfloat xPos, GLfloat yPos, GLfloat zPos);
		void SetTexture(GLuint tex);
		int  UsePointSpriteMode(bool yes);
		
		void Display();
		void Update();
		
		
	protected:
		Particle* particles;
		unsigned long num;
		GLfloat x, y, z;
		GLfloat xGrav, yGrav, zGrav;
		GLfloat rMin, rMax, gMin, gMax, bMin, bMax; //color ranges
		GLfloat decayRate;
		GLuint texture;
		float  numActive, rate;
		unsigned long tick;
		bool usePointSprite;

};

bool operator<(Particle p1, Particle p2);

#endif //__PARTICLE_H__
