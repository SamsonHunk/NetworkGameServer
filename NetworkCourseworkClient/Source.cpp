#include "Box2D/Box2D.h"
#include <SFML/Network.hpp>
#include <iostream>

// The IP address of the server
#define SERVERIP "127.0.0.1"
#define PORT 7777

sf::UdpSocket socket;
extern 	enum class PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };


extern struct playerMoveMessage {
	PlayerStates stateMessage;
	b2Vec2 position;
};

sf::Packet movePacket;

using namespace std;

int main()
{
	if (socket.bind(7777, sf::IpAddress("127.0.0.1")) != socket.Done)
	{//error making socket port
		cout << "Error Binding port, error code 1" << endl;
		return 1;
	}

	if (socket.receive(movePacket, sf::IpAddress(SERVERIP), PORT);
}