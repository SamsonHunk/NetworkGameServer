#ifndef GLOBALVARS
#define GLOBALVARS
extern enum PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };

namespace GLOBALVARS
{

	extern enum  GameState { Menu, GameENUM, NoChange }; //enumerator for the current game state

	//enum for current playerstate


	extern struct playerMoveMessage {
		int messageType = 1;
		int stateMessage;
		float xPos;
		float yPos;
		int playerNum;
	};

	extern struct  connectionMessage
	{
		int messageType = 0;
		sf::String clientIp;
		unsigned short clientPort;
	};

	extern struct serverPositionPing
	{
		int messageType = 2;
		float xPos1;
		float yPos1;
		int player1State;
		float xPos2;
		float yPos2;
		int player2State;
	};
}
#endif // !GLOBALVARS
//file to store project wide data containers