
#include "particle.h"
#include "sore_texture.h"

ParticleEngine::ParticleEngine(GLuint tex)
{
}

ParticleEngine::ParticleEngine()
{
	num   = defaultNumParticles;
	xGrav = defaultXGrav;
	yGrav = defaultYGrav;
	zGrav = defaultZGrav;
	x = y = z = 0.0f;
	particles = new Particle[num];
	
	rMin = gMin = bMin = 0.0f;
	rMax = gMax = bMax = 1.0f;
	decayRate = defaultDecayRate;
	
	rate = 1.0f/decayRate;
	
	rate = (num / rate);
				
	numActive = rate;
	usePointSprite = false;
	
	tick = 0;
}

ParticleEngine::ParticleEngine(GLfloat* colorRange, GLfloat* gravityVector, unsigned long numParticles)
{
}

ParticleEngine::ParticleEngine(GLfloat* gravityVector, unsigned long numParticles)
{
}

ParticleEngine::ParticleEngine(unsigned long numParticles)
{
}

void ParticleEngine::Initialize()
{
	for(unsigned int i=0;i<num;i++)
	{
		particles[i].xpos = x + getRandomMinMax(-0.1f, 0.1f);
		particles[i].ypos = y + getRandomMinMax(-0.1f, 0.1f);
		particles[i].zpos = z + getRandomMinMax(-0.1f, 0.1f);
		particles[i].life = getRandomMinMax(0.0f, 1.0f);
		
		particles[i].xvel = defaultXVel + getRandomMinMax(-0.003f, 0.003f)*10;
		particles[i].yvel = defaultYVel + getRandomMinMax(-0.003f, 0.003f)*10;
		particles[i].zvel = defaultZVel + getRandomMinMax(-0.003f, 0.003f)*10;
		
		particles[i].r = getRandomMinMax(rMin, rMax);
		particles[i].g = getRandomMinMax(gMin, gMax);
		particles[i].b = getRandomMinMax(bMin, bMax);
		particles[i].active = true;
	}
}

int ParticleEngine::UsePointSpriteMode(bool yes)
{
	if(yes)
	{
		if(CheckExtension("GL_ARB_point_sprite") && glPointParameterf && glPointParameterfv)
		{
			usePointSprite = true;
			return 0;
		}
		else
		{
			usePointSprite = false;
			return -1;
		}
	}
	else
	{
		usePointSprite = false;
		return 0;
	}
}

void ParticleEngine::SetColorRange(GLfloat* colorRange) //array of 6 GLfloats representing RGB min-max
{
	rMin = colorRange[0];
	rMax = colorRange[1];
	gMin = colorRange[2];
	gMax = colorRange[3];
	bMin = colorRange[4];
	bMax = colorRange[5];
}

void ParticleEngine::SetPosition(GLfloat xPos, GLfloat yPos, GLfloat zPos)
{
	x = xPos;
	y = yPos;
	z = zPos;
}

void ParticleEngine::Display()
{
	SORE_Resource::ResourceManager* rm = SORE_Resource::ResourceManager::GetManager();
	SORE_Resource::Resource* r;
	SORE_Resource::ResourceHandle* rd;
	sort(particles, particles + (num/sizeof(Particle)));
	GLfloat maxSize;
	GLfloat atten[3] = {1.0f, 1.0f, 1.0f};
	glGetFloatv( GL_POINT_SIZE_MAX_ARB, &maxSize );
	glPointSize( 64.0f );
	r = rm->GetPtr("data/Textures/sprite.tga");
	rd = dynamic_cast<SORE_Resource::ResourceHandle*>(r);
	glBindTexture( GL_TEXTURE_2D, rd->GetHandle());
	
	glEnable ( GL_POINT_SPRITE_ARB );
	
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);
	glEnable(GL_BLEND);
	glDepthMask (GL_FALSE);
	//glDisable( GL_DEPTH_TEST );
	
	if(usePointSprite)
	{
		glPointParameterf( GL_POINT_SIZE_MAX_ARB, maxSize );
		glPointParameterf( GL_POINT_SIZE_MIN_ARB, 1.0f );
		glPointParameterfv(GL_POINT_DISTANCE_ATTENUATION_ARB, atten);
		glTexEnvf( GL_POINT_SPRITE_ARB, GL_COORD_REPLACE_ARB, GL_TRUE  );
		
		glBegin( GL_POINTS );
		{
			for(unsigned int i=0;i<num;i++)
			{
				if(particles[i].active)
				{
					glColor4f(particles[i].r,particles[i].g,particles[i].b,particles[i].life);
					glVertex3f(particles[i].xpos, particles[i].ypos, particles[i].zpos);
				}
			}
		}
		glEnd();
	}
	else
	{
		glPushMatrix();
		//glMatrixMode(GL_MODELVIEW);
		//glLoadIdentity();
	
		//camera.TransformView();
		
		GLfloat* rotation = camera.GetRotation();
		
		//glRotatef(rotation[0], 1.0f, 0.0f, 0.0f);
		//glRotatef(rotation[1], 0.0f, 1.0f, 0.0f);
		//glRotatef(rotation[2], 0.0f, 0.0f, 1.0f);
		
			
		for(unsigned int i=0;i<num;i++)
		{
			if(particles[i].active)
			{
				glBegin( GL_TRIANGLE_STRIP);
				glColor4f(particles[i].r,particles[i].g,particles[i].b,particles[i].life);
				glTexCoord2d(1.0f, 1.0f); glVertex3f(particles[i].xpos+0.02f, particles[i].ypos+0.02f, particles[i].zpos);
				glTexCoord2d(0.0f, 1.0f); glVertex3f(particles[i].xpos-0.02f, particles[i].ypos+0.02f, particles[i].zpos);
				glTexCoord2d(1.0f, 0.0f); glVertex3f(particles[i].xpos+0.02f, particles[i].ypos-0.02f, particles[i].zpos);
				glTexCoord2d(0.0f, 0.0f); glVertex3f(particles[i].xpos-0.02f, particles[i].ypos-0.02f, particles[i].zpos);
				glEnd();
			}
		}
	}
	
	glPopMatrix();
	
	glDisable( GL_POINT_SPRITE_ARB );
	//glEnable( GL_DEPTH_TEST );
	glDisable(GL_BLEND);
	glDepthMask (GL_TRUE);
}

void ParticleEngine::Update()
{
	for(unsigned int i=0;i<num;i++)
	{
		if(particles[i].active)
		{
			particles[i].life -= decayRate;
			particles[i].xpos += particles[i].xvel;
			particles[i].ypos += particles[i].yvel;
			particles[i].zpos += particles[i].zvel;
			
			particles[i].xvel += xGrav;
			particles[i].yvel += yGrav;
			particles[i].zvel += zGrav;
			
			particles[i].r += defaultRStep;
			particles[i].g += defaultGStep;
			particles[i].b += defaultBStep;
			
			if(particles[i].life <= 0.0f)
			{
				particles[i].life = 1.0f;
				particles[i].xpos = x + getRandomMinMax(-0.1f, 0.1f);
				particles[i].ypos = y + getRandomMinMax(-0.1f, 0.1f);
				particles[i].zpos = z + getRandomMinMax(-0.1f, 0.1f);
				
				particles[i].xvel = defaultXVel + getRandomMinMax(-0.003f, 0.003f);
				particles[i].yvel = defaultYVel + getRandomMinMax(-0.003f, 0.003f);
				particles[i].zvel = defaultZVel + getRandomMinMax(-0.003f, 0.003f);
				particles[i].r = getRandomMinMax(rMin, rMax);
				particles[i].g = getRandomMinMax(gMin, gMax);
				particles[i].b = getRandomMinMax(bMin, bMax);
			}
		}
	}
	
	//std::cout << "numActive: " << numActive << std::endl;
	
	if(numActive < num)
		numActive += rate;
	
	/*if(tick%20==0)
	{
		std::cout << "tick: " << tick << " numActive: " << numActive << std::endl;
	}
	
	tick++;*/
}

void ParticleEngine::SetTexture(GLuint tex)
{
	texture = tex;
}

bool operator<(Particle p1, Particle p2)
{
	GLfloat* camPos = camera.GetPosition();
	GLfloat x = camPos[0];
	GLfloat y = camPos[1];
	GLfloat z = camPos[2];
	delete[] camPos;
	
	return Distance(x,y,z,p1.xpos,p1.ypos,p1.zpos)<Distance(x,y,z,p2.xpos,p2.ypos,p2.zpos);
}

