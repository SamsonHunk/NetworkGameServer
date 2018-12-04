#include "Box2D/Box2D.h"
#include <SFML/Network.hpp>
#include <iostream>
#include <thread>
#include "GlobalVars.h"
#include <stack>

// The IP address of the server
#define SERVERIP "127.0.0.1"
unsigned short PORT = 7777;
sf::IpAddress serverip = sf::IpAddress::LocalHost;
sf::IpAddress anyIp = sf::IpAddress::Any;
sf::IpAddress clientIp;
sf::UdpSocket socket;

bool serverDone = false;

b2World* physicsWorld;

std::stack<connectionMessage> connectionMessageStack;
std::stack<playerMoveMessage> moveMessageStack;

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
	
	sf::Packet connectPacket;

	if (socket.receive(connectPacket, serverip, PORT) != sf::Socket::Done)
	{
		cout << "Error recieving packet" << endl;
	}
	connectionMessage packetIn;
	connectPacket >> packetIn.clientIp >> packetIn.clientPort;
	cout << "Packet Recieved" << endl;
	cout << packetIn.clientIp << endl;

}

void messageHandlerThread()
{//one thread on the server will handle grabbing packets from the player and storing them into a container for the main thread
	//updating the simulation accordingly
	playerMoveMessage moveIn;
	connectionMessage connectIn;
	while (!serverDone)
	{
		sf::Packet packet;
		socket.receive(packet, clientIp, PORT);

	}
}