#include <iostream>
#include "sgct.h"
#include "ParticleSystem.h"
#include "World.h"
#include "HelperFunctions.h"
#include "Field.h"
#include <iostream>

sgct::Engine* gEngine;
ParticleSystem* gParticles;
World* gWorld;

using namespace std;

void initialize();
void draw();

int main(int argc, char *argv[])
{
	gEngine = new sgct::Engine(argc, argv);

	gEngine->setInitOGLFunction(initialize);
	gEngine->setDrawFunction(draw);

	gParticles = new ParticleSystem(gEngine);
	gWorld = new World(gEngine);

	if(!gEngine->init(sgct::Engine::OpenGL_3_3_Core_Profile))
	{
		delete gEngine;
		return EXIT_FAILURE;
	}

	gEngine->render();
	gParticles->destroy();
	delete gEngine;
	delete gParticles;
	exit(EXIT_SUCCESS);
}

void initialize()
{
	gParticles->initialize();
	gWorld->initializeWorld();
}

void draw()
{
	double delta = gEngine->getDt();

	gWorld->drawWorld();

	gParticles->move(delta);
	gParticles->draw(delta);
}
