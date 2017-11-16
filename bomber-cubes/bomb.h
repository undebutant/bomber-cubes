#pragma once

#include <list>


struct DataAboutBomb {
	int posX;
	int posZ;
	bool isExploded;
	float timeLeftBeforeExplosion;
};

struct DataAboutExplodedBomb {
	int posX;
	int posZ;
	int maxPosX;
	int minPosX;
	int maxPosZ;
	int minPosZ;
	float displayTimeAfterExplosion;
};


struct PosBombInMatrix {
	int posX;
	int posZ;
};


class Bomb {
public:
	void addBombToQueue(int posBombX, int posBombZ, float timeBeforeExploding);
	void deleteBombsQueuedDone();

	std::list<struct PosBombInMatrix> updateTimersBombs();

private:
	std::list<struct DataAboutBomb> bombQueue;

};
