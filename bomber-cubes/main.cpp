#include "window.h"
#include "input.h"
#include "timer.h"
#include "ogl.h"
#include "load.h"
#include "camera.h"
#include "quaternion.h"
#include "maths_3d.h"
#include "map.h"
#include "player.h"
#include "bomb.h"


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//										Defining variables										//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////

// Setting up various global variables
float sizeSquareX = 50.0f;
float sizeSquareY = 30.0f;
float sizeSquareZ = 50.0f;

float colorSquareDestroyableR = 0.7f;
float colorSquareDestroyableG = 0.7f;
float colorSquareDestroyableB = 0.7f;

float colorSquareR = 0.3f;
float colorSquareG = 0.3f;
float colorSquareB = 0.3f;

int nbSquareX = 13;
int nbSquareZ = 11;


// Setting up the map
Map mapGame = Map(nbSquareX, nbSquareZ);


// Setting up players
Player playerOne = Player(0, 0);
float colorPlayerOneR = 1.0f;
float colorPlayerOneG = 0.0f;
float colorPlayerOneB = 1.0f;

Player playerTwo = Player(0, nbSquareZ - 1);
float colorPlayerTwoR = 0.0f;
float colorPlayerTwoG = 1.0f;
float colorPlayerTwoB = 0.0f;

Player playerThree = Player(nbSquareX - 1, 0);
float colorPlayerThreeR = 0.0f;
float colorPlayerThreeG = 0.0f;
float colorPlayerThreeB = 1.0f;

Player playerFour = Player(nbSquareX - 1, nbSquareZ - 1);
float colorPlayerFourR = 1.0f;
float colorPlayerFourG = 1.0f;
float colorPlayerFourB = 0.0f;

bool isPlayerOneAlive = true;
bool isPlayerTwoAlive = true;
bool isPlayerThreeAlive = true;
bool isPlayerFourAlive = true;

float sizePlayerX = sizeSquareX / 2;
float sizePlayerY = sizeSquareY * 2;
float sizePlayerZ = sizeSquareZ / 2;

float sizeBombX = sizeSquareX / 2;
float sizeBombY = sizeSquareY / 2 * 3;
float sizeBombZ = sizeSquareZ / 2;

float colorBombR = 0.8f;
float colorBombG = 0.2f;
float colorBombB = 0.2f;

float sizeBombExplosionX = sizeSquareX;
float sizeBombExplosionY = sizeSquareY / 3;
float sizeBombExplosionZ = sizeSquareZ;

float colorExplosionR = 1.0f;
float colorExplosionG = 0.0f;
float colorExplosionB = 0.0f;

float timeBeforeNextMove = 2.4f;

float timeBeforeNextMovePlayerOne = timeBeforeNextMove;
float timeBeforeNextMovePlayerTwo = timeBeforeNextMove;
float timeBeforeNextMovePlayerThree = timeBeforeNextMove;
float timeBeforeNextMovePlayerFour = timeBeforeNextMove;

bool isBombPlacedPlayerOne = false;
bool isBombPlacedPlayerTwo = false;
bool isBombPlacedPlayerThree = false;
bool isBombPlacedPlayerFour = false;

Bomb bombList = Bomb();
std::list<struct PosBombInMatrix> listPosBombExplodingThisIteration;
std::list<struct DataAboutExplodedBomb> listPosBombEffect;

float timeBeforeBombExplosion = 8.0f;
float timeDisplayAfterExplosion = 2.4f;


// Defining the size of the window
WINDOW *win = NULL;
float sizeScreenX = 1600;
float sizeScreenY = 900;

// Handling inputs and timer
MY_INPUT *inp = NULL;
TIMER *tim = NULL;

// Initializing the camera
CAMERA *cam;
float posXCam = nbSquareX * sizeSquareX / 2;
float posYCam = 500.0f;
float posZCam = -20.0f;
float targetPosX = nbSquareX * sizeSquareX / 2;
float targetPosY = -150.0f;
float targetPosZ = nbSquareZ * sizeSquareZ / 2;


// Handling sprites
Texture *textureArtemisLogo;


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//										Program initializer										//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////

// Starting the application, returning false if something goes wrong
bool start() {
	// Setting up the window
	win = new WINDOW();
	win->create(sizeScreenX, sizeScreenY, 32, 60, false);
	
	// Setting up the camera
	cam = new CAMERA();

	// Creating the timer
	tim = new TIMER();

	// Handling keyboard and mouse inputs
	inp = new MY_INPUT(win);


	// Finishing the basic setup
	create_context(*win);								// Creating OpenGL context in the window
	init_font(*win, "Courier");							// Initializing text management using OpenGL

  
	// OpenGL setup
	glClearColor(0.0f,0.0f,0.0f,1.0f);					// Set clear color for color buffer (RGBA, black)

	glViewport(0,0,win->Xres,win->Yres);				// Render zone (screen wide)

 	glShadeModel(GL_SMOOTH);							// Enable Smooth Shading
	glEnable(GL_DEPTH_TEST);							// Enables Depth Testing
	glDepthFunc(GL_LESS);								// The Type Of Depth Testing To Do
	glHint(GL_PERSPECTIVE_CORRECTION_HINT, GL_NICEST);	// Best mapping quality
	glFrontFace(GL_CCW);								// Front of face is defined counter clock wise
	glPolygonMode(GL_FRONT, GL_FILL);					// Front of a face is filled
	glPolygonMode(GL_BACK,  GL_FILL);					// Back of a face is filled
	glCullFace(GL_BACK);								// Cull back face only
	glDisable(GL_CULL_FACE);						    // Disable back face culling


	// Naming the window
	win->set_title("Bomber Cubes");


	// Loading textures
	textureArtemisLogo = new Texture;
	textureArtemisLogo->load_texture("artemis.tga", "artemis_mask.tga");
	glGenTextures(1, textureArtemisLogo->OpenGL_ID);						// Create a "name" for the texture (an id associated to this texture)

	// Texture parameters
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);			// Repeating texture in case U,V > 1.0
	glTexParameteri(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);			// or if < 0.0

	glTexEnvf(GL_TEXTURE_ENV, GL_TEXTURE_ENV_MODE, GL_MODULATE);			// Mixing up the texture and the color


	// Setting up the map
	mapGame.setupMapLevelOne();

	return true;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//									Defining basic functions									//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////

void displayLogoArtemis(float posX, float posY) {
	glMatrixMode(GL_PROJECTION);

	// Push current projection matrix on the matrix stack
	glPushMatrix();
	
	// Set an ortho projection based on window size
	glLoadIdentity();
	glOrtho(0, sizeScreenX, 0, sizeScreenY, 0, 1);

	// Switch back to model-view matrix
	glMatrixMode(GL_MODELVIEW);

	// Store current model-view matrix on the stack
	glPushMatrix();

	// Clear the model-view matrix
	glLoadIdentity();

	// You can specify this in window coordinates now
	glRasterPos2f(posX, posY);

	// Resizing the sprite
	float xfactor = 256.0 / textureArtemisLogo->img_color->lenx;
	float yfactor = 128.0 / textureArtemisLogo->img_color->leny;
	glPixelZoom(xfactor, yfactor);

	// Handling the sprite to display
	if (textureArtemisLogo->isRGBA) {
		glEnable(GL_ALPHA_TEST);
		glAlphaFunc(GL_GREATER, 0.99);
		glDrawPixels(textureArtemisLogo->img_color->lenx, textureArtemisLogo->img_color->leny, GL_RGBA, GL_UNSIGNED_BYTE, textureArtemisLogo->img_all);
		glDisable(GL_ALPHA_TEST);
	}
	else {
		glDrawPixels(textureArtemisLogo->img_color->lenx, textureArtemisLogo->img_color->leny, GL_RGB, GL_UNSIGNED_BYTE, textureArtemisLogo->img_color->data);
	}

	// Restore the model-view matrix
	glPopMatrix();

	// Switch to projection matrix and restore it
	glMatrixMode(GL_PROJECTION);
	glPopMatrix();

	// Switch back to model-view matrix
	glMatrixMode(GL_MODELVIEW);
}


void displaySquare(float posX, float posY, float posZ, float lengthX, float lengthZ, float colorR, float colorG, float colorB) {
	// Defining a quad in the asked size
	glBegin(GL_QUADS);

	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX, posY, posZ);

	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX + lengthX, posY, posZ);

	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX + lengthX, posY, posZ + lengthZ);

	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX, posY, posZ + lengthZ);

	glEnd();
}


void displayCube(float posX, float posY, float posZ, float lengthX, float lengthY, float lengthZ, float colorR, float colorG, float colorB) {
	// Front side
	glBegin(GL_QUADS);
	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX, posY + lengthY, posZ);
	glVertex3f(posX, posY, posZ);
	glVertex3f(posX + lengthX, posY, posZ);
	glVertex3f(posX + lengthX, posY + lengthY, posZ);
	glEnd();

	// Right side
	glBegin(GL_QUADS);
	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX + lengthX, posY, posZ);
	glVertex3f(posX + lengthX, posY + lengthY, posZ);
	glVertex3f(posX + lengthX, posY + lengthY, posZ + lengthZ);
	glVertex3f(posX + lengthX, posY, posZ + lengthZ);
	glEnd();

	// Left side
	glBegin(GL_QUADS);
	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX, posY, posZ + lengthZ);
	glVertex3f(posX, posY + lengthY, posZ + lengthZ);
	glVertex3f(posX, posY + lengthY, posZ);
	glVertex3f(posX, posY, posZ);
	glEnd();

	// Top side
	glBegin(GL_QUADS);
	glColor3f(colorR, colorG, colorB);
	glVertex3f(posX + lengthX, posY + lengthY, posZ + lengthZ);
	glVertex3f(posX + lengthX, posY + lengthY, posZ);
	glVertex3f(posX, posY + lengthY, posZ);
	glVertex3f(posX, posY + lengthY, posZ + lengthZ);
	glEnd();
}


void displayMap() {
	// Displaying the map ground
	for (int i = 0; i < nbSquareX; i++) {
		for (int j = 0; j < nbSquareZ; j++) {
			displaySquare(sizeSquareX * i, 0.0f, sizeSquareZ * j, sizeSquareX, sizeSquareZ, 1 - ((i + j) % 2 * 0.9f), 1 - ((i + j) % 2 * 0.9f), 1 - ((i + j) % 2 * 0.9f));
		}
	}

	// Looking for players in the map, if not found, they are considered dead
	bool isPlayerOneDrawn = false;
	bool isPlayerTwoDrawn = false;
	bool isPlayerThreeDrawn = false;
	bool isPlayerFourDrawn = false;

	// Displaying the map obstacles according to the matrix
	for (int i = 0; i < nbSquareX; i++) {
		for (int j = 0; j < nbSquareZ; j++) {
			float posXPlayer = 0.0f;
			float posZPlayer = 0.0f;

			switch (mapGame.getCase(i, j)) {
			case 10:
				posXPlayer = i * sizeSquareX + sizeSquareX / 4;
				posZPlayer = j * sizeSquareZ + sizeSquareZ / 4;

				displayCube(posXPlayer, 0.0f, posZPlayer, sizeBombX, sizeBombY, sizeBombZ, colorBombR, colorBombG, colorBombB);
				break;
			case 11:
				displayCube(i * sizeSquareX, 0.0f, j * sizeSquareZ, sizeSquareX, sizeSquareY, sizeSquareZ, colorSquareR, colorSquareG, colorSquareB);
				break;
			case 12:
				displayCube(i * sizeSquareX, 0.0f, j * sizeSquareZ, sizeSquareX, sizeSquareY, sizeSquareZ, colorSquareDestroyableR, colorSquareDestroyableG, colorSquareDestroyableB);
				break;
			case 1:
				posXPlayer = i * sizeSquareX + sizeSquareX / 4;
				posZPlayer = j * sizeSquareZ + sizeSquareZ / 4;

				displayCube(posXPlayer, 0.0f, posZPlayer, sizePlayerX, sizePlayerY, sizePlayerZ, colorPlayerOneR, colorPlayerOneG, colorPlayerOneB);

				isPlayerOneDrawn = true;
				break;
			case 2:
				posXPlayer = i * sizeSquareX + sizeSquareX / 4;
				posZPlayer = j * sizeSquareZ + sizeSquareZ / 4;

				displayCube(posXPlayer, 0.0f, posZPlayer, sizePlayerX, sizePlayerY, sizePlayerZ, colorPlayerTwoR, colorPlayerTwoG, colorPlayerTwoB);
				isPlayerTwoDrawn = true;
				break;
			case 3:
				posXPlayer = i * sizeSquareX + sizeSquareX / 4;
				posZPlayer = j * sizeSquareZ + sizeSquareZ / 4;

				displayCube(posXPlayer, 0.0f, posZPlayer, sizePlayerX, sizePlayerY, sizePlayerZ, colorPlayerThreeR, colorPlayerThreeG, colorPlayerThreeB);
				isPlayerThreeDrawn = true;
				break;
			case 4:
				posXPlayer = i * sizeSquareX + sizeSquareX / 4;
				posZPlayer = j * sizeSquareZ + sizeSquareZ / 4;

				displayCube(posXPlayer, 0.0f, posZPlayer, sizePlayerX, sizePlayerY, sizePlayerZ, colorPlayerFourR, colorPlayerFourG, colorPlayerFourB);
				isPlayerFourDrawn = true;
				break;
			default:
				break;
			}
		}
	}

	if (!isPlayerOneDrawn) {
		isPlayerOneAlive = false;
	}
	if (!isPlayerTwoDrawn) {
		isPlayerTwoAlive = false;
	}
	if (!isPlayerThreeDrawn) {
		isPlayerThreeAlive = false;
	}
	if (!isPlayerFourDrawn) {
		isPlayerFourAlive = false;
	}
}


void placeBombInQueue(int posX, int posZ) {
	bombList.addBombToQueue(posX, posZ, timeBeforeBombExplosion);
}


void movePlayer(int idPlayer, int newPosX, int newPosZ) {
	if (newPosX >= nbSquareX || newPosX < 0 || newPosZ >= nbSquareZ || newPosZ < 0) {
		return;
	}

	if (mapGame.getCase(newPosX, newPosZ) == 0) {

		switch (idPlayer) {
		case 1:
			if (timeBeforeNextMovePlayerOne < 0) {
				mapGame.updateMap(newPosX, newPosZ, idPlayer);

				if (isBombPlacedPlayerOne) {
					mapGame.placeBomb(playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ);
					mapGame.updateMap(playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ, 10);

					placeBombInQueue(playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ);

					isBombPlacedPlayerOne = false;
				}
				else {
					mapGame.updateMap(playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ, 0);
				}

				playerOne.updatePlayerPosition(newPosX, newPosZ);

				timeBeforeNextMovePlayerOne = timeBeforeNextMove;
			}
			break;
		case 2:
			if (timeBeforeNextMovePlayerTwo < 0) {
				mapGame.updateMap(newPosX, newPosZ, idPlayer);

				if (isBombPlacedPlayerTwo) {
					mapGame.placeBomb(playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ);
					mapGame.updateMap(playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ, 10);

					placeBombInQueue(playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ);

					isBombPlacedPlayerTwo = false;
				}
				else {
					mapGame.updateMap(playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ, 0);
				}

				playerTwo.updatePlayerPosition(newPosX, newPosZ);

				timeBeforeNextMovePlayerTwo = timeBeforeNextMove;
			}
			break;
		case 3:
			if (timeBeforeNextMovePlayerThree < 0) {
				mapGame.updateMap(newPosX, newPosZ, idPlayer);
				
				if (isBombPlacedPlayerThree) {
					mapGame.placeBomb(playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ);
					mapGame.updateMap(playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ, 10);

					placeBombInQueue(playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ);

					isBombPlacedPlayerThree = false;
				}
				else {
					mapGame.updateMap(playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ, 0);
				}

				playerThree.updatePlayerPosition(newPosX, newPosZ);

				timeBeforeNextMovePlayerThree = timeBeforeNextMove;
			}
			break;
		case 4:
			if (timeBeforeNextMovePlayerFour < 0) {
				mapGame.updateMap(newPosX, newPosZ, idPlayer);
				
				if (isBombPlacedPlayerFour) {
					mapGame.placeBomb(playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ);
					mapGame.updateMap(playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ, 10);

					placeBombInQueue(playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ);

					isBombPlacedPlayerFour = false;
				}
				else {
					mapGame.updateMap(playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ, 0);
				}

				playerFour.updatePlayerPosition(newPosX, newPosZ);

				timeBeforeNextMovePlayerFour = timeBeforeNextMove;
			}
			break;
		default:
			break;
		}
	}
}


void handleInput() {
	// Handling mouse movements
	if (inp->RBmouse) {
		inp->get_mouse_movement();
	}
	if (inp->keys[KEY_CODE_ESCAPE]) {
		// Stop the message flow by sending QUIT message
		PostMessage(win->handle, WM_CLOSE, 0, 0);
	}

	// Player one
	if (isPlayerOneAlive) {
		if (inp->keys[KEY_CODE_Z]) {
			movePlayer(1, playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ + 1);
		}
		if (inp->keys[KEY_CODE_Q]) {
			movePlayer(1, playerOne.getPlayerPosition().posX + 1, playerOne.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_S]) {
			movePlayer(1, playerOne.getPlayerPosition().posX, playerOne.getPlayerPosition().posZ - 1);
		}
		if (inp->keys[KEY_CODE_D]) {
			movePlayer(1, playerOne.getPlayerPosition().posX - 1, playerOne.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_E]) {
			isBombPlacedPlayerOne = true;
		}
	}

	// Player 2
	if (isPlayerTwoAlive) {
		if (inp->keys[KEY_CODE_T]) {
			movePlayer(2, playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ + 1);
		}
		if (inp->keys[KEY_CODE_F]) {
			movePlayer(2, playerTwo.getPlayerPosition().posX + 1, playerTwo.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_G]) {
			movePlayer(2, playerTwo.getPlayerPosition().posX, playerTwo.getPlayerPosition().posZ - 1);
		}
		if (inp->keys[KEY_CODE_H]) {
			movePlayer(2, playerTwo.getPlayerPosition().posX - 1, playerTwo.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_Y]) {
			isBombPlacedPlayerTwo = true;
		}
	}

	// Player 3
	if (isPlayerThreeAlive) {
		if (inp->keys[KEY_CODE_O]) {
			movePlayer(3, playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ + 1);
		}
		if (inp->keys[KEY_CODE_K]) {
			movePlayer(3, playerThree.getPlayerPosition().posX + 1, playerThree.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_L]) {
			movePlayer(3, playerThree.getPlayerPosition().posX, playerThree.getPlayerPosition().posZ - 1);
		}
		if (inp->keys[KEY_CODE_M]) {
			movePlayer(3, playerThree.getPlayerPosition().posX - 1, playerThree.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_P]) {
			isBombPlacedPlayerThree = true;
		}
	}

	// Player 4
	if (isPlayerFourAlive) {
		if (inp->keys[KEY_CODE_NUMPAD8]) {
			movePlayer(4, playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ + 1);
		}
		if (inp->keys[KEY_CODE_NUMPAD4]) {
			movePlayer(4, playerFour.getPlayerPosition().posX + 1, playerFour.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_NUMPAD5]) {
			movePlayer(4, playerFour.getPlayerPosition().posX, playerFour.getPlayerPosition().posZ - 1);
		}
		if (inp->keys[KEY_CODE_NUMPAD6]) {
			movePlayer(4, playerFour.getPlayerPosition().posX - 1, playerFour.getPlayerPosition().posZ);
		}
		if (inp->keys[KEY_CODE_NUMPAD9]) {
			isBombPlacedPlayerFour = true;
		}
	}
}


void refreshTimers() {
	if (timeBeforeNextMovePlayerOne >= 0.0f) {
		timeBeforeNextMovePlayerOne -= 0.02f;
	}
	if (timeBeforeNextMovePlayerTwo >= 0.0f) {
		timeBeforeNextMovePlayerTwo -= 0.02f;
	}
	if (timeBeforeNextMovePlayerThree >= 0.0f) {
		timeBeforeNextMovePlayerThree -= 0.02f;
	}
	if (timeBeforeNextMovePlayerFour >= 0.0f) {
		timeBeforeNextMovePlayerFour -= 0.02f;
	}
}


void handleBombExplosion(std::list<struct PosBombInMatrix> listPosBombExploding) {
	if (!listPosBombExploding.empty()) {
		for (std::list<struct PosBombInMatrix>::iterator it = listPosBombExploding.begin(); it != listPosBombExploding.end(); it++) {
			std::vector<int> rangeExplosionCreatedVector;
			struct DataAboutExplodedBomb rangeExplosionCreated;

			rangeExplosionCreatedVector = mapGame.updateMapBombOnExplosion(it->posX, it->posZ);

			rangeExplosionCreated.posX = it->posX;
			rangeExplosionCreated.posZ = it->posZ;
			rangeExplosionCreated.maxPosX = rangeExplosionCreatedVector[0];
			rangeExplosionCreated.minPosX = rangeExplosionCreatedVector[1];
			rangeExplosionCreated.maxPosZ = rangeExplosionCreatedVector[2];
			rangeExplosionCreated.minPosZ = rangeExplosionCreatedVector[3];
			rangeExplosionCreated.displayTimeAfterExplosion = timeDisplayAfterExplosion;

			listPosBombEffect.push_front(rangeExplosionCreated);
		}
	}
}


void displayBombEffectsCubes(int posX, int posZ, int maxPosX, int minPosX, int maxPosZ, int minPosZ) {
	for (int i = minPosX; i <= maxPosX; i++) {
		displayCube(i * sizeSquareX, 0.0f, posZ * sizeSquareZ, sizeBombExplosionX, sizeBombExplosionY, sizeBombExplosionZ, colorExplosionR, colorExplosionG, colorExplosionB);
	}
	for (int j = minPosZ; j <= maxPosZ; j++) {
		displayCube(posX * sizeSquareX, 0.0f, j * sizeSquareZ, sizeBombExplosionX, sizeBombExplosionY, sizeBombExplosionZ, colorExplosionR, colorExplosionG, colorExplosionB);
	}
}


void displayBombEffects() {
	std::list<struct DataAboutExplodedBomb> tempList;

	if (!listPosBombEffect.empty()) {
		while(!listPosBombEffect.empty()) {
			struct DataAboutExplodedBomb explosionDataToDisplay = listPosBombEffect.back();
			listPosBombEffect.pop_back();

			if (explosionDataToDisplay.displayTimeAfterExplosion >= 0.0f) {
				displayBombEffectsCubes(explosionDataToDisplay.posX, explosionDataToDisplay.posZ, explosionDataToDisplay.maxPosX, explosionDataToDisplay.minPosX, explosionDataToDisplay.maxPosZ, explosionDataToDisplay.minPosZ);

				explosionDataToDisplay.displayTimeAfterExplosion -= 0.02f;
				tempList.push_front(explosionDataToDisplay);
			}
		}
	}
	listPosBombEffect = tempList;
}


void isGameOver() {
	// TODO display game over screen
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//											Main loop											//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////

void main_loop() {
	// Time update
	tim->update_horloge();

	// Refreshing timers
	refreshTimers();

	// Input update
	inp->refresh();
	handleInput();

	// Handling bombs
	listPosBombExplodingThisIteration = bombList.updateTimersBombs();
	bombList.deleteBombsQueuedDone();


	// Clear The Screen And The Depth Buffer
	glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);


	// Handling the camera
	glMatrixMode(GL_PROJECTION);

	glLoadIdentity();

	// Setting up camera projection with a 60° view angle, near 10 and far 30000
	gluPerspective(60, (double)win->Xres / (double)win->Yres, 10, 30000);
	glMatrixMode(GL_MODELVIEW);

	glLoadIdentity();

	// Defining the point to look at with the camera
	// (arguments : posX, posY, posZ, targetPosX, targetPosY, targetPosZ, and the up vector)
	gluLookAt(posXCam, posYCam, posZCam, targetPosX, targetPosY, targetPosZ, 0.0f, 1.0f, 0.0f);


	//////////////////////////////////////////////////////////////////////////////////////////////////
	//									Entering main game loop										//
	//////////////////////////////////////////////////////////////////////////////////////////////////

	// Displaying Artemis logo
	displayLogoArtemis(10.0f, sizeScreenY - 128.0f);

	// Updating map according to the bomb explosion
	handleBombExplosion(listPosBombExplodingThisIteration);

	// Displaying the effects of explosing bombs
	displayBombEffects();

	// Displaying the map at its current state
	displayMap();


	// Displaying the created image to the screen
	swap_buffer(win);
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//									Stopping the application									//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////
void stop() {
	delete inp;
	delete tim;

	if (win) {
		kill_font();
		kill_context(*win);
		delete win;
	}
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//							Program entry point for Windows ("WIN32")							//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////
int WINAPI WinMain(HINSTANCE hInstance,HINSTANCE hPrevInstance,LPSTR szCmdLine,int iCmdShow) {
	MSG  msg;

	win	= NULL;
	inp	= NULL;
	tim = NULL;

	// Calling the scene setup function
	if (start() == false) {
		debug("Setting up of the scene encountered an error");
		stop();

		return 1;
	}

	// Initiating the loop
	while (true) {
		// When there is a message, calling WndProc() to handle it
		if (PeekMessage(&msg, NULL,0,0,PM_NOREMOVE)) {
			if (!GetMessage(&msg, NULL, 0, 0)) {
				break;
			}
			TranslateMessage(&msg);
			DispatchMessage (&msg);
		}
		// Else, continuing the main loop
		else {
			main_loop();
		}
	}

	// Stopping OpenGL and closing the window
	stop();

	return 0;
}


//////////////////////////////////////////////////////////////////////////////////////////////////
//																								//
//											Message loop										//
//																								//
//////////////////////////////////////////////////////////////////////////////////////////////////
LRESULT CALLBACK WndProc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam) {
	switch (msg) {
		case WM_MOVE:
			win->Xpos = (dword) LOWORD(lParam);
			win->Ypos = (dword) HIWORD(lParam);
			return 0;
	
		case WM_CLOSE:
			PostQuitMessage(0); // Tell to GetMessage() to send back 0
			return 0;
	
		case WM_SYSCOMMAND:  // Prevent energy saving screen
			switch (wParam) {
				case SC_SCREENSAVE:
		
				case SC_MONITORPOWER:
					return 0;
				}
				break;
	
		case WM_KEYDOWN:
			inp->set_key_down ((dword)wParam);
			return 0;
	
		case WM_KEYUP:
			inp->set_key_up((dword)wParam);
			return 0;

		case WM_LBUTTONDOWN:
			inp->set_mouse_left_down();
			return 0;

		case WM_LBUTTONUP:
			inp->set_mouse_left_up();
			return 0;
	
		case WM_RBUTTONDOWN: inp->set_mouse_right_down();
			return 0;
	
		case WM_RBUTTONUP:
			inp->set_mouse_right_up();
			return 0;
	
		case WM_MBUTTONDOWN: inp->set_mouse_middle_down();
			return 0;
	
		case WM_MBUTTONUP:
			inp->set_mouse_middle_up();
			return 0;
	}
	
	return DefWindowProc(hwnd,msg,wParam,lParam);
}
