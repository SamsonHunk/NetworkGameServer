#ifndef GLOBALVARS
#define GLOBALVARS
namespace GLOBALVARS
{
	extern enum class GameState { Menu, Game, NoChange }; //enumerator for the current game state
	extern 	enum class PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };
	//enum for current playerstate

	extern struct playerMoveMessage {
		int stateMessage;
		float xPos;
		float yPos;
		int playerNum;
	};

	extern struct connectionMessage {
		std::string clientIp;
		unsigned short clientPort;
	};
}
#endif // !GLOBALVARS
//file to store project wide data containers