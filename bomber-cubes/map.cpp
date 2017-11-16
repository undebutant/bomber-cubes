#include "map.h"


Map::Map(int creationSizeI, int creationSizeJ) : sizeI(creationSizeI), sizeJ(creationSizeJ), map(creationSizeI, std::vector<int>(creationSizeJ)) {

}


int Map::getCase(int i, int j) {
	return map[i][j];
}

void Map::updateMap(int i, int j, int newValue) {
	map[i][j] = newValue;
}

std::vector<int> Map::updateMapBombOnExplosion(int posBombX, int posBombZ) {
	std::vector<int> listExtremumsExplosion = std::vector<int>();

	int maxPosX = posBombX;
	int minPosX = posBombX;
	int maxPosZ = posBombZ;
	int minPosZ = posBombZ;

	if (posBombX + 1 < sizeI) {
		bool isCollisionFound = false;

		for (int i = posBombX + 1; i < sizeI; i++) {
			maxPosX++;

			switch (map[i][posBombZ]) {
			case 1:
				map[i][posBombZ] = 0;
				break;
			case 2:
				map[i][posBombZ] = 0;
				break;
			case 3:
				map[i][posBombZ] = 0;
				break;
			case 4:
				map[i][posBombZ] = 0;
				break;
			case 11:
				// Undestroyable block
				isCollisionFound = true;
				maxPosX--;
				break;
			case 12:
				// Destroying block
				map[i][posBombZ] = 0;
				isCollisionFound = true;
				break;
			default:
				break;
			}

			if (isCollisionFound) {
				break;
			}
		}
	}


	if (posBombX - 1 >= 0) {
		bool isCollisionFound = false;

		for (int i = posBombX - 1; i >= 0; i--) {
			minPosX--;

			switch (map[i][posBombZ]) {
			case 1:
				map[i][posBombZ] = 0;
				break;
			case 2:
				map[i][posBombZ] = 0;
				break;
			case 3:
				map[i][posBombZ] = 0;
				break;
			case 4:
				map[i][posBombZ] = 0;
				break;
			case 11:
				// Undestroyable block
				isCollisionFound = true;
				minPosX++;
				break;
			case 12:
				// Destroying block
				map[i][posBombZ] = 0;
				isCollisionFound = true;
				break;
			default:
				break;
			}

			if (isCollisionFound) {
				break;
			}
		}
	}


	if (posBombZ + 1 < sizeJ) {
		bool isCollisionFound = false;

		for (int j = posBombZ + 1; j < sizeJ; j++) {
			maxPosZ++;

			switch (map[posBombX][j]) {
			case 1:
				map[posBombX][j] = 0;
				break;
			case 2:
				map[posBombX][j] = 0;
				break;
			case 3:
				map[posBombX][j] = 0;
				break;
			case 4:
				map[posBombX][j] = 0;
				break;
			case 11:
				// Undestroyable block
				isCollisionFound = true;
				maxPosZ--;
				break;
			case 12:
				// Destroying block
				map[posBombX][j] = 0;
				isCollisionFound = true;
				break;
			default:
				break;
			}

			if (isCollisionFound) {
				break;
			}
		}
	}


	if (posBombZ - 1 >= 0) {
		bool isCollisionFound = false;

		for (int j = posBombZ - 1; j >= 0; j--) {
			minPosZ--;

			switch (map[posBombX][j]) {
			case 1:
				map[posBombX][j] = 0;
				break;
			case 2:
				map[posBombX][j] = 0;
				break;
			case 3:
				map[posBombX][j] = 0;
				break;
			case 4:
				map[posBombX][j] = 0;
				break;
			case 11:
				// Undestroyable block
				isCollisionFound = true;
				minPosZ++;
				break;
			case 12:
				// Destroying block
				map[posBombX][j] = 0;
				isCollisionFound = true;
				break;
			default:
				break;
			}

			if (isCollisionFound) {
				break;
			}
		}
	}
	
	map[posBombX][posBombZ] = 0;

	listExtremumsExplosion.push_back(maxPosX);
	listExtremumsExplosion.push_back(minPosX);
	listExtremumsExplosion.push_back(maxPosZ);
	listExtremumsExplosion.push_back(minPosZ);


	return listExtremumsExplosion;
}


// Bomb is with value 10
void Map::placeBomb(int i, int j) {
	map[i][j] = 10;
}


// Unbreakable blocks are with value 11, breakable blocks are with value 12
void Map::setupMapLevelOne() {
	for (int i = 1; i < sizeI - 1; i++) {
		for (int j = 1; j < sizeJ - 1; j++) {
			if (j % 2 == 1 && i % 2 == 1) {
				map[i][j] = 11;
			}
			else {
				map[i][j] = 12;
			}
		}
	}

	for (int i = 3; i < sizeI - 3; i++) {
		map[i][0] = 12;
		map[i][sizeJ - 1] = 12;
	}

	for (int j = 3; j < sizeJ - 3; j++) {
		map[0][j] = 12;
		map[sizeI - 1][j] = 12;
	}

	// Adding players to the map
	map[0][0] = 1;
	map[0][sizeJ - 1] = 2;
	map[sizeI - 1][0] = 3;
	map[sizeI - 1][sizeJ - 1] = 4;
}
