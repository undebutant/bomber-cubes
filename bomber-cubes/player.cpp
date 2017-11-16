#include "player.h"


Player::Player(int startingPosX, int startingPosZ) {
	struct PosInMatrix startingPositionPlayer = PosInMatrix();

	startingPositionPlayer.posX = startingPosX;
	startingPositionPlayer.posZ = startingPosZ;

	positionPlayer = startingPositionPlayer;
}


struct PosInMatrix Player::getPlayerPosition() {
	return positionPlayer;
}

void Player::updatePlayerPosition(int newPosX, int newPosZ) {
	positionPlayer.posX = newPosX;
	positionPlayer.posZ = newPosZ;
}
