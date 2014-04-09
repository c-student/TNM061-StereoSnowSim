#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem(sgct::Engine* engine)
{
	mEngine = engine;
	mInitialized = false;
	mBillBoardVB = 0;
	mParticlePositionBuffer = 0;
}

void ParticleSystem::initialize()
{
	static const GLfloat vertexBufferData[] =
	{
		-0.5f, -0.5f, 0.0f,
		0.5f, -0.5f, 0.0f,
		-0.5f, 0.5f, 0.0f,
		0.5f, 0.5f, 0.0f
	};

	GLuint vertexArray = 0;

	glGenVertexArrays(1, &vertexArray);
	glBindVertexArray(vertexArray);

	glGenBuffers(1, &mBillBoardVB);
	glBindBuffer(GL_ARRAY_BUFFER, mBillBoardVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData),
				 vertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &mParticlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mParticlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES*4*sizeof(GLubyte),
				 NULL, GL_STREAM_DRAW);

	glBindVertexArray(0);

	mInitialized = true;
}

void ParticleSystem::draw()
{
	if(mInitialized)
	{
		// std::cout << "DRAWING! ";
	}
}

void ParticleSystem::destroy()
{
	if(mInitialized)
	{
		glDeleteBuffers(1, &mBillBoardVB);
		glDeleteBuffers(1, &mParticlePositionBuffer);
		mInitialized = false;
	}
}

void ParticleSystem::move()
{
	double lastTime = glfwGetTime();
	// do gravity and shit
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// keep track of time
		double currentTime = glfwGetTime();
		double delta = currentTime - lastTime;
		lastTime = currentTime;

		// get ref to current particle
		Particle& p = mParticles[i];

		// pull it down!
		p.mSpeed += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta;
		p.mPosition += p.mSpeed * (float)delta;

		// reduce life? Has it passed some sort of boundary?
		if(p.mPosition.y < 0)
		{
			p.mLife -= delta; // reduce life
		}
	}
}
