#ifndef GLOBALVARS
#define GLOBALVARS
extern enum class PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };

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
		std::string clientIp;
		unsigned short clientPort;
	};
}
#endif // !GLOBALVARS
//file to store project wide data containers