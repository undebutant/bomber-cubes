#pragma once


struct PosInMatrix {
	int posX;
	int posZ;
};


class Player {
public:
	Player(int startingPosX, int startingPosZ);

	struct PosInMatrix getPlayerPosition();
	void updatePlayerPosition(int newPosX, int newPosZ);

private:
	struct PosInMatrix positionPlayer;
};
