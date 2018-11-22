#pragma once
#include <vector>

/*
Initializing the GlutManager causes it to run a separate window displaying the
graphics of the internal vectors, 'stacks' and 'pools'


*/
class GlutManager {
private:
	void initialize(int argc, char **argv);
	std::vector<std::vector<bool>> createFauxTestData();
	std::vector<std::vector<bool>> m_stacks;
	std::vector<std::vector<bool>> m_pools;

public:
	GlutManager();
	~GlutManager();
	//static GlutManager& getInstance()
	//{
	//	static GlutManager instance;

	//	// Instance is returned
	//	return instance;
	//}
	void EnterMainLoop();

	void updateVectors(std::vector<std::vector<bool>> stacks, std::vector<std::vector<bool>> pools);
};
