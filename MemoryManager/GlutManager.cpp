#include "GlutManager.hpp"
#include "GlutIncludes.hpp"
#include "RenderFunctions.hpp"
#include <iostream>
#include "RenderFunctions.hpp"

void renderScene()
{
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);

	// Create faux input
	int* renderedBarCount = new int(0);
	std::vector<std::vector<bool>> vectors = createFauxTestData();

	// Render vectors 
	GLfloat stackEntryColor[] = { 0.0f, 1.0f, 0.0f };
	renderVector(vectors, stackEntryColor, renderedBarCount);

	GLfloat poolEntryColor[] = { 0.0f, 0.0f, 1.0f };
	renderVector(vectors, poolEntryColor, renderedBarCount);

	delete renderedBarCount;
	glutSwapBuffers();
}

GlutManager::GlutManager()
{
	// Fake input so that glut doesn't REEEEEEEEEEE
	int argc = 1; 
	char *temp = new char('a');
	char **argv = &temp;

	// Initialize GLUT & vectors
	this->initialize(argc, argv);

	// Clean
	delete temp;
}

GlutManager::~GlutManager()
{
}

void GlutManager::EnterMainLoop()
{
	// Enter the mainloop
	glutMainLoop();
}

void GlutManager::updateVectors(std::vector<std::vector<bool>> stacks, std::vector<std::vector<bool>> pools)
{
	m_stacks = stacks;
	m_pools = pools;
}

void GlutManager::initialize(int argc, char **argv)
{
	// Init GLUT and create the window
	glutInit(&argc, argv);		// - Initializes GLUT itself
	glutInitWindowPosition(-1, -1);	//
	glutInitWindowSize(600, 600);	//
	glutInitDisplayMode(GLUT_RGBA | GLUT_DOUBLE);	//
	glutCreateWindow("Render Window");

	// register callbacks
	glutDisplayFunc(renderScene);

	// Fill vectors with empty data
	m_stacks = createFauxTestData();
	m_pools = createFauxTestData();
}

std::vector<std::vector<bool>> GlutManager::createFauxTestData()
{
	std::vector<std::vector<bool>> vectors;
	std::vector<bool> inputEveryOther;
	for (int i = 0; i < 150; i++) {
		if (i % 2 == 0) {
			inputEveryOther.push_back(true);
		}
		else {
			inputEveryOther.push_back(false);
		}
	}
	vectors.push_back(inputEveryOther);

	std::vector<bool> inputHalf;
	for (int i = 0; i < 100; i++) {
		if (i < 50) {
			inputHalf.push_back(true);
		}
		else {
			inputHalf.push_back(false);
		}
	}
	vectors.push_back(inputHalf);

	std::vector<bool> inputOtherHalf;
	for (int i = 0; i < 100; i++) {
		if (i < 50) {
			inputOtherHalf.push_back(false);
		}
		else {
			inputOtherHalf.push_back(true);
		}
	}
	vectors.push_back(inputOtherHalf);

	return vectors;
}

