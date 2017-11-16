#include "bomb.h"


void Bomb::addBombToQueue(int posBombX, int posBombZ, float timeBeforeExploding) {
	struct DataAboutBomb newBombToAdd = DataAboutBomb();

	newBombToAdd.posX = posBombX;
	newBombToAdd.posZ = posBombZ;

	newBombToAdd.isExploded = false;

	newBombToAdd.timeLeftBeforeExplosion = timeBeforeExploding;

	bombQueue.push_front(newBombToAdd);
}

void Bomb::deleteBombsQueuedDone() {
	std::list<struct DataAboutBomb> tempListDataBombs;

	if (!bombQueue.empty()) {
		struct DataAboutBomb lastBombQueued;

		while (!bombQueue.empty()) {
			lastBombQueued = bombQueue.back();
			bombQueue.pop_back();

			if (!lastBombQueued.isExploded) {
				tempListDataBombs.push_front(lastBombQueued);
			}
		}
		bombQueue = tempListDataBombs;
	}
}


std::list<struct PosBombInMatrix> Bomb::updateTimersBombs() {
	std::list<struct PosBombInMatrix> listBombExploding;

	for (std::list<struct DataAboutBomb>::iterator it = bombQueue.begin(); it != bombQueue.end(); it++) {
		if (it->timeLeftBeforeExplosion >= 0) {
			it->timeLeftBeforeExplosion -= 0.02f;
		}
		if (it->timeLeftBeforeExplosion < 0) {
			if (!it->isExploded) {
				struct PosBombInMatrix posBombExploded = PosBombInMatrix();

				posBombExploded.posX = it->posX;
				posBombExploded.posZ = it->posZ;

				listBombExploding.push_front(posBombExploded);

				it->isExploded = true;
			}
		}
	}
	return listBombExploding;
}
