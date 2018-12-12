#ifndef GLOBALVARS
#define GLOBALVARS
extern enum PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };


extern struct playerMoveMessage {
	int messageType = 2;
	int stateMessage;
	float xPos;
	float yPos;
	int playerNum;
	bool dir;
};

extern struct  connectionMessage
{
	int messageType = 1;
	std::string clientIp;
	unsigned short clientPort;
	unsigned short InitialPort;
};

extern struct serverPositionPing
{
	int messageType = 3;
	float xPos1;
	float yPos1;
	int player1State;
	float xPos2;
	float yPos2;
	int player2State;
};


extern struct playerShootPing
{
	int messageType = 5;
	float posx;
	float posy;
	bool dir;
	int bulletNum;
	int playerNum;
};

extern struct serverBulletPing
{//maximum bullets in the scene will be 8 4 for each player
	int messageType = 4;
	float posx[4];
	float posy[4];
	bool awake[4];
	bool dir[4];
};

namespace GLOBALVARS
{

	extern enum  GameState { Menu, GameENUM, NoChange }; //enumerator for the current game state

	//enum for current playerstate


	
}
#endif // !GLOBALVARS
//file to store project wide data containers