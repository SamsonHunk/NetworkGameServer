#include "Box2D/Box2D.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include "GlobalVars.h"

// The IP address of the server
#define SERVERIP "127.0.0.1"
unsigned short PORT = 7777;
sf::IpAddress serverip = sf::IpAddress::LocalHost;
sf::IpAddress anyIp = sf::IpAddress::Any;
sf::IpAddress clientIp;
sf::UdpSocket socket;

extern 	enum class PlayerStates { movingLeft, movingRight, stationary, movingUp, movingDown };

b2World* physicsWorld;

sf::Packet movePacket;

using namespace std;
void messageHandlerThread();

int main()
{
	//server initial setup
	cout << "Starting Server" << endl;
	if (socket.bind(PORT, serverip) != sf::Socket::Done)
	{
		cout << "Server bind failed" << endl;
	}
	else
	{
		cout << "Bind success" << endl;
	}
	//initialise the physics simulation
	physicsWorld = new b2World(b2Vec2(0, .5));
	cout << "Physics world initialised" << endl;

	socket.setBlocking(true);
	cout << "Waiting for client connection..." << endl;
	
	if (socket.receive(movePacket, serverip, PORT) != sf::Socket::Done)
	{
		cout << "Error recieving packet" << endl;
	}
	connectionMessage packetIn;
	movePacket >> packetIn.clientIp >> packetIn.clientPort;
	cout << "Packet Recieved" << endl;
	cout << packetIn.clientIp << endl;

}

void messageHandlerThread()
{//one thread on the server will handle grabbing packets from the player and
	//updating the simulation accordingly
	playerMoveMessage packetIn;
	sf::Packet packet;
	socket.receive(packet, clientIp, PORT);
	packet >> 
}