#include "Box2D/Box2D.h"
#include <SFML/Network.hpp>
#include <iostream>
#include "GlobalVars.h"
#include <list>
#include <chrono>


// The IP address of the server
#define SERVERIP "127.0.0.1"
using namespace std;

unsigned short PORT = 7777;
unsigned short CLIENTPORT = 7778;
sf::IpAddress serverip = sf::IpAddress::LocalHost;
sf::IpAddress anyIp = sf::IpAddress::Any;
sf::IpAddress clientIp;
sf::UdpSocket socket;


chrono::high_resolution_clock::time_point start;
chrono::high_resolution_clock::time_point stopPoint;

bool serverDone = false;

//list of current messages
list<connectionMessage> connectionMessageStack;
list<playerMoveMessage> moveMessageStack;

//player ip list
vector<sf::IpAddress> playerIps;
vector<unsigned short> playerPorts;

void messageHandler();
void messageCompute();

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
	

	socket.setBlocking(false);

	start = std::chrono::high_resolution_clock::now();


	while (!serverDone)
	{
		messageHandler();

		//if 1 ms has passed send out a new server ping
		stopPoint = std::chrono::high_resolution_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(stopPoint - start).count() >= 1)
		{
			//reset the timer
			start = std::chrono::high_resolution_clock::now();

			//cout << "Ping!" << endl;

			messageCompute();
		}
	}
}

void messageHandler()
{//one thread on the server will handle grabbing packets from the player and storing them into a container for the main thread
	//updating the simulation accordingly
	playerMoveMessage moveIn;
	connectionMessage connectIn;
	
	int messageType;
	sf::Packet packet;
	if (socket.receive(packet, anyIp, PORT) != sf::Socket::Done)
	{
		//cout << "Passed through without getting a new packet" << endl;
	}
	else
	{
		packet >> messageType;
		//figure out what kind of message we revieved
		switch (messageType)
		{
		case 0:
			//new player connection message
			packet >> connectIn.messageType >> connectIn.clientIp >> connectIn.clientPort;
			connectionMessageStack.push_back(connectIn);
			break;
		case 1:
			//new player move message
			packet >> moveIn.messageType >> moveIn.stateMessage >> moveIn.xPos >> moveIn.yPos;
			moveMessageStack.push_back(moveIn);
			break;

		default:
			break;
		}

		cout << "Recieved a new packet" << endl;
	}
}

//compute and send new ping update
void messageCompute()
{
	//go through each list of messages and figure out what to do with them, earliest first so we end up with the most up to date
	//positions
	if (!connectionMessageStack.empty())
	{
		list<connectionMessage>::iterator connectionIt;
		for (connectionIt = connectionMessageStack.begin(); connectionIt != connectionMessageStack.end(); connectionIt++)
		{
			playerIps.push_back(sf::IpAddress(connectionIt->clientIp.toWideString));
			playerPorts.push_back(connectionIt->clientPort);


			//grab new player ip and stuff then tell them they are connected
			playerMoveMessage newPlayer;
			sf::Packet packet;
			newPlayer.messageType = 1;
			newPlayer.stateMessage = PlayerStates::stationary;
			newPlayer.xPos = 600;
			newPlayer.yPos = 0;
			newPlayer.playerNum = playerIps.size();

			packet << newPlayer.messageType << newPlayer.stateMessage << newPlayer.xPos << newPlayer.yPos << newPlayer.playerNum;
			if (socket.send(packet, connectionIt->clientIp, connectionIt->clientPort) != sf::Socket::Done)
			{//send out initial message to the connecting player to get them into the game
				cout << "Player instansiate failed... Player ip: " << connectionIt->clientIp << endl;
			}
			else
			{
				cout << "New Client Connected" << endl;
			}
		}

		//clear message history
		connectionMessageStack.clear();
	}

	//figure out the most up to date position of everyone and ping out what they are doing
	if (!moveMessageStack.empty())
	{
		float playerPosArray[2][2];
		PlayerStates playerStateArray[2];

		list<playerMoveMessage>::iterator moveIt;
		for (moveIt = moveMessageStack.begin(); moveIt != moveMessageStack.end(); moveIt++)
		{
			playerStateArray[moveIt->playerNum - 1] = static_cast<PlayerStates>(moveIt->stateMessage);
			playerPosArray[moveIt->playerNum - 1][0] = moveIt->xPos;
			playerPosArray[moveIt->playerNum - 1][1] = moveIt->yPos;
		}

		//send out position ping
		sf::Packet packet;
		serverPositionPing movePing;
		movePing.player1State = playerStateArray[0];
		movePing.player2State = playerStateArray[1];
		movePing.xPos1 = playerPosArray[0][0];
		movePing.yPos1 = playerPosArray[0][1];
		movePing.xPos2 = playerPosArray[1][0];
		movePing.yPos2 = playerPosArray[1][1];

		packet << movePing.messageType << movePing.xPos1 << movePing.yPos1 << movePing.player1State << movePing.xPos2 << movePing.yPos2 << movePing.player2State;
		if (!playerIps.empty())
		{
			for (int it = 0; it < playerIps.size(); it++)
			{//cycle through connected clients and send the ping
				if (socket.send(packet, playerIps[it], playerPorts[it]) != sf::Socket::Done)
				{
					cout << "Error sending to player " << it + 1 << endl;
				}
			}
		}
	}
}