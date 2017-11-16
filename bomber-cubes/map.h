#pragma once

#include <vector>


class Map {
public:
	Map(int creationSizeI, int creationSizeJ);

	int getCase(int i, int j);

	void updateMap(int i, int j, int newValue);
	std::vector<int> updateMapBombOnExplosion(int i, int j);

	void placeBomb(int i, int j);

	void setupMapLevelOne();

private:
	int sizeI;
	int sizeJ;

	std::vector<std::vector<int>> map;
};
