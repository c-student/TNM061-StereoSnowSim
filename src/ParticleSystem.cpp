#include "ParticleSystem.h"
#include <iostream>

ParticleSystem::ParticleSystem(sgct::Engine* engine)
{
	mEngine = engine;
	mInitialized = false;

	mVertexArray = 0;

	mLastUsedParticle = 0;

	mBillBoardVB = 0;
	mParticlePositionBuffer = 0;

	g_particule_position_size_data = new GLfloat[MAX_PARTICLES * 4];
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




	glGenVertexArrays(1, &mVertexArray);
	glBindVertexArray(mVertexArray);

	glGenBuffers(1, &mBillBoardVB);
	glBindBuffer(GL_ARRAY_BUFFER, mBillBoardVB);
	glBufferData(GL_ARRAY_BUFFER, sizeof(vertexBufferData),
				 vertexBufferData, GL_STATIC_DRAW);

	glGenBuffers(1, &mParticlePositionBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mParticlePositionBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES*4*sizeof(GLubyte),
				 NULL, GL_STREAM_DRAW);

	glGenBuffers(1, &mParticleColorBuffer);
	glBindBuffer(GL_ARRAY_BUFFER, mParticleColorBuffer);
	glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES*4*sizeof(GLubyte),
				 NULL, GL_STREAM_DRAW);

	glBindVertexArray(0);

	mInitialized = true;

	sgct::ShaderManager::instance()->addShaderProgram("particle", "particle.vert", "particle.frag");

	sgct::ShaderManager::instance()->bindShaderProgram( "particle" );

	mMatrixLoc = sgct::ShaderManager::instance()->getShaderProgram( "particle").getUniformLocation( "VP" );

	sgct::ShaderManager::instance()->unBindShaderProgram();

}

int ParticleSystem::findLastParticle()
{
	for (int i = mLastUsedParticle; i < MAX_PARTICLES; ++i)
	{
		if(mParticles[i].mLife < 0)
		{
			mLastUsedParticle = i;
			return i;
		}
	}

	for (int i = 0; i < MAX_PARTICLES; ++i)
	{
		if(mParticles[i].mLife < 0)
		{
			mLastUsedParticle = i;
			return i;
		}
	}

	return 0;
}

void ParticleSystem::draw(double delta)
{
	if(mInitialized)
	{

		int newParticles = (int)(delta*10000.0);
		if(newParticles > (int)(0.016f*10000.0))
		{
			newParticles = (int)(0.016f*10000.0);
		}


		for (int i = 0; i < newParticles; ++i)
		{
			Particle &p = mParticles[i];

			int particleIndex = findLastParticle();
			mParticles[particleIndex].mLife = 5.0f;
			mParticles[particleIndex].mPosition = glm::vec3(0,0,-10);
			mParticles[particleIndex].mVelocity = glm::vec3(0,10,0);

			//FIXME
            mParticles[particleIndex].mSize = (rand()%1000)/2000.0f + 0.1f;
		}


		int particleCount = 0;
		for(int i=0; i< MAX_PARTICLES; i++)
		{
			Particle &p = mParticles[i];

			if(p.mLife > 0.0f)
			{
				g_particule_position_size_data[4*particleCount+0] = p.mPosition.x;
                g_particule_position_size_data[4*particleCount+1] = p.mPosition.y;
                g_particule_position_size_data[4*particleCount+2] = p.mPosition.z;

                g_particule_position_size_data[4*particleCount+3] = p.mSize;
			}

		}

		glBindBuffer(GL_ARRAY_BUFFER, mParticlePositionBuffer);
        glBufferData(GL_ARRAY_BUFFER, MAX_PARTICLES * 4 * sizeof(GLfloat), NULL, GL_STREAM_DRAW); // Buffer orphaning, a common way to improve streaming perf. See above link for details.
        glBufferSubData(GL_ARRAY_BUFFER, 0, particleCount * sizeof(GLfloat) * 4, g_particule_position_size_data);


		glEnable(GL_BLEND);
        glBlendFunc(GL_SRC_ALPHA, GL_ONE_MINUS_SRC_ALPHA);

		sgct::ShaderManager::instance()->bindShaderProgram( "particle" );

		glm::mat4 MVP = mEngine->getActiveModelViewProjectionMatrix();
		glUniformMatrix4fv(mMatrixLoc, 1, GL_FALSE, &MVP[0][0]);

		glBindVertexArray(mVertexArray);

        glEnableVertexAttribArray(0);
		glBindBuffer(GL_ARRAY_BUFFER, mBillBoardVB);
		glVertexAttribPointer(
                    0,
                    3,
                    GL_FLOAT,
                    GL_FALSE,
                    0,
                    reinterpret_cast<void*>(0) // array buffer offset
            );
       	glEnableVertexAttribArray(1);
		glBindBuffer(GL_ARRAY_BUFFER, mParticlePositionBuffer);

		glVertexAttribPointer(
                        1,
                        4,
                        GL_FLOAT,
                        GL_FALSE,
                        0,
						reinterpret_cast<void*>(0) // array buffer offset
                );
        // Draw the particules !
        // This draws many times a small triangle_strip (which looks like a quad).
        // This is equivalent to :
        // for(i in ParticlesCount) : glDrawArrays(GL_TRIANGLE_STRIP, 0, 4),
        // but faster.

        //FIXME
		glVertexAttribDivisor(0, 0); // particles vertices : always reuse the same 4 vertices -> 0
        glVertexAttribDivisor(1, 1); // positions : one per quad (its center)                 -> 1

        glDrawArraysInstanced(GL_TRIANGLE_STRIP, 0, 4, particleCount);

        glDisableVertexAttribArray(0);
        glDisableVertexAttribArray(1);

		glBindVertexArray(0);

		sgct::ShaderManager::instance()->unBindShaderProgram();
		//std::cout << "Drawing all them pritty particles\n";
	}
}

void ParticleSystem::destroy()
{
	if(mInitialized)
	{
		glDeleteBuffers(1, &mBillBoardVB);
		glDeleteBuffers(1, &mParticlePositionBuffer);
		glDeleteBuffers(1, &mParticleColorBuffer);

		glDeleteVertexArrays(1, &mVertexArray);

		mInitialized = false;

		delete[] g_particule_position_size_data;
	}
}

void ParticleSystem::move(double delta)
{

	// do gravity and shit
	for (int i = 0; i < MAX_PARTICLES; i++)
	{
		// get ref to current particle
		Particle& p = mParticles[i];

		if(p.mLife < 0.0f)
		{
			// pull it down!
			p.mVelocity += glm::vec3(0.0f, -9.81f, 0.0f) * (float)delta;
			p.mPosition += p.mVelocity;
			p.mLife -= 0.01f; // reduce life

			// reduce life? Has it passed some sort of boundary?
			if(p.mPosition.y < 0)
			{
				p.mLife = 0.0f; // reduce life
			}
		}
	}
}
