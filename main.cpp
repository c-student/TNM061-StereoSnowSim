#include <iostream>
#include "sgct.h"
#include "ParticleSystem.h"
#include "World.h"
#include "HelperFunctions.h"
#include "Field.h"
#include "Gravity.h"
#include "Wind.h"
#include "SoapBubble.h"
#include <iostream>

sgct::Engine* gEngine;
ParticleSystem* gParticles;
World* gWorld;
SoapBubble* gBubble;


void initialize();
void draw();

int main(int argc, char *argv[])
{
	initRandom();

	gEngine = new sgct::Engine(argc, argv);

	gEngine->setInitOGLFunction(initialize);
	gEngine->setDrawFunction(draw);

	gParticles = new ParticleSystem(gEngine);
	gWorld = new World(gEngine);
	gBubble = new SoapBubble(gEngine);

	Gravity* grav = new Gravity();
	grav->init(-0.1f);
	gParticles->addField(grav);

	Wind* wind = new Wind();
	wind->init(getRandom(-0.5, 0.5), 0.0f, getRandom(-0.5, 0.5));
	gParticles->addField(wind);

	cout << "Wind direction: " << wind->getAcceleration() << endl;

	if(!gEngine->init(sgct::Engine::OpenGL_3_3_Core_Profile))
	{
		delete gEngine;
		return EXIT_FAILURE;
	}
	sgct::SGCTSettings::instance()->setSwapInterval(1);
	gEngine->render();
	gParticles->destroy();

	delete gEngine;
	delete gParticles;
	delete gWorld;
	delete gBubble;

	exit(EXIT_SUCCESS);
}

void initialize()
{
	gParticles->initialize();
	gWorld->initializeWorld();
	gBubble->createSphere(1.5f, 100);
}

void draw()
{
	double delta = gEngine->getDt();

	gWorld->drawWorld();
	gBubble->drawBubble();

	//gParticles->move(delta);
	//gParticles->draw(delta);
}
