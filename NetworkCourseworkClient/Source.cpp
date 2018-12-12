#include "Box2D/Box2D.h"
#include <SFML/Network.hpp>
#include <iostream>
#include "GlobalVars.h"
#include <list>
#include <chrono>
#include <vector>
#include <thread>
#include <mutex>

// The IP address of the server
#define SERVERIP "127.0.0.1"
using namespace std;

unsigned short PORT = 7777;
sf::IpAddress serverip = sf::IpAddress::LocalHost;
sf::IpAddress anyIp = sf::IpAddress::Any;
sf::IpAddress clientIp;
sf::UdpSocket socket;

int maxBullets = 4;

chrono::high_resolution_clock::time_point start;
chrono::high_resolution_clock::time_point stopPoint;

bool serverDone = false;

//list of current messages
mutex messageLock; //mutex lock to protect the shared storage data
list<connectionMessage> connectionMessageStack;
list<playerMoveMessage> moveMessageStack;
list<playerShootPing> shootStack;

//player ip list
vector<sf::IpAddress> playerIps;
vector<unsigned short> playerPorts;

void messageHandler();
void messageCompute();
sf::Packet bulletPingPack(serverBulletPing pingIn);

int main()
{
	int input;
	socket.setBlocking(true);
	cout << "press 1 for local host, 2 for LAN" << endl;
	cin >> input;

	switch (input)
	{
	case 1:
		serverip = sf::IpAddress::LocalHost;
		break;
	case 2:
		serverip = sf::IpAddress::getLocalAddress();
		break;
	}
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
	cout << "ServerIp: " << serverip << endl;

	start = std::chrono::high_resolution_clock::now();

	thread messageFork(messageHandler);

	while (!serverDone)
	{
		cout.flush();
		//if 1 ms has passed send out a new server ping
		stopPoint = std::chrono::high_resolution_clock::now();
		if (chrono::duration_cast<chrono::milliseconds>(stopPoint - start).count() >= 100)
		{
			messageLock.lock();
			//reset the timer
			start = std::chrono::high_resolution_clock::now();
			messageCompute();
			messageLock.unlock();
		}
	}
	//when the server is done, wait for the thread to join then exit
	messageFork.join();
}

void messageHandler()
{//one thread on the server will handle grabbing packets from the player and storing them into a container for the main thread
	int messageType = 0;
	sf::Packet packet;
	sf::IpAddress connectionIp;
	unsigned short connectionPort;

	while (!serverDone)
	{
			playerMoveMessage moveIn;
			connectionMessage connectIn;
			playerShootPing bulletIn;
			messageType = 0;
			packet.clear();

			//ask the selector if there is any data for the socket to collect
			if (socket.receive(packet, connectionIp, connectionPort) != sf::Socket::Done)
			{
				//cout << "Packet error" << endl;
			}
			else
			{
				messageLock.lock();
				packet >> messageType;
				//figure out what kind of message we revieved
				switch (messageType)
				{
				case 1:
					//grab information from connection packet
					packet >> connectIn.clientIp >> connectIn.clientPort;
					connectIn.InitialPort = connectionPort;
					connectionMessageStack.push_back(connectIn);
					cout << "Client packet recieved" << endl;
					break;
				case 2:
					//new player move message
					packet >> moveIn.stateMessage >> moveIn.xPos >> moveIn.yPos >> moveIn.playerNum >> moveIn.dir >> moveIn.yVel;
					moveMessageStack.push_back(moveIn);
					//cout << "Move packet recieved" << endl;
					break;
				case 5:
					//new bullet ping
					packet >> bulletIn.posx >> bulletIn.posy >> bulletIn.dir >> bulletIn.bulletNum >> bulletIn.playerNum;
					shootStack.push_back(bulletIn);
					cout << "Bullet packet recieved" << endl;
					break;
				default:
					cout << "Bad Input" << endl;
					break;
				}
				messageLock.unlock();
				//cout << "packet recieved" << endl;
			}
		}
	}


//compute and send new ping update
void messageCompute()
{
	//go through each list of messages and figure out what to do with them, earliest first so we end up with the most up to date
	//positions
	if (connectionMessageStack.size() != 0)
	{
		list<connectionMessage>::iterator connectionIt;
		for (connectionIt = connectionMessageStack.begin(); connectionIt != connectionMessageStack.end(); connectionIt++)
		{


			//grab new player ip and stuff then tell them they are connected
			playerMoveMessage newPlayer;
			sf::Packet packet;
			newPlayer.messageType = 2;
			newPlayer.stateMessage = PlayerStates::stationary;
			newPlayer.xPos = 600;
			newPlayer.yPos = 0;
			newPlayer.playerNum = playerIps.size() + 1;
			switch (newPlayer.playerNum)
			{
			case 1:
				newPlayer.xPos = 200;
				break;
			case 2:
				newPlayer.xPos = 500;
			}
			

			packet << newPlayer.messageType << newPlayer.stateMessage << newPlayer.xPos << newPlayer.yPos << newPlayer.playerNum;
			sf::IpAddress ipOut(connectionIt->clientIp);
			if (socket.send(packet, ipOut, connectionIt->InitialPort) != sf::Socket::Done)
			{//send out initial message to the connecting player to get them into the game
				cout << "Player instansiate failed... Player ip: " << connectionIt->clientIp << " client port: " << 5428 << endl;
			}
			else
			{
				switch (newPlayer.playerNum)
				{
				case 1:
					cout << "New Client Connected: " << connectionIt->clientIp << " port: " << connectionIt->clientPort << endl;
					playerIps.push_back(sf::IpAddress(connectionIt->clientIp));
					playerPorts.push_back(connectionIt->clientPort);
					break;
				case 2:
					cout << "New Client Connected: " << connectionIt->clientIp << " port: " << connectionIt->clientPort << endl;
					playerIps.push_back(sf::IpAddress(connectionIt->clientIp));
					playerPorts.push_back(connectionIt->clientPort);
					break;
				default:
					cout << "Refused connection, too many players..." << endl;
					break;
				}
				
			}
		}

		//clear message history
		connectionMessageStack.clear();
	}

	//figure out the most up to date position of everyone and ping out what they are doing
	if (moveMessageStack.size() != 0 && playerIps.size() > 1)
	{
		float playerPosArray[2][2];
		PlayerStates playerStateArray[2];
		float playerYVelArray[2];

		list<playerMoveMessage>::iterator moveIt;
		for (moveIt = moveMessageStack.begin(); moveIt != moveMessageStack.end(); moveIt++)
		{
			playerStateArray[moveIt->playerNum - 1] = static_cast<PlayerStates>(moveIt->stateMessage);
			playerPosArray[moveIt->playerNum - 1][0] = moveIt->xPos;
			playerPosArray[moveIt->playerNum - 1][1] = moveIt->yPos;
			playerYVelArray[moveIt->playerNum - 1] = moveIt->yVel;
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
		movePing.yVel1 = playerYVelArray[0];
		movePing.yVel2 = playerYVelArray[1];


		//cout << movePing.xPos2 << movePing.yPos2 << endl;

		packet << movePing.messageType << movePing.xPos1 << movePing.yPos1 << movePing.player1State << movePing.xPos2 << movePing.yPos2 << movePing.player2State << movePing.yVel1 << movePing.yVel2;
		if (!playerIps.empty())
		{
			for (int it = 0; it < playerIps.size(); it++)
			{//cycle through connected clients and send the ping
				if (socket.send(packet, playerIps[it], playerPorts[it]) != sf::Socket::Done)
				{
					cout << "Error sending to player " << it << endl;
				}
				else
				{
					//cout << "Ping" << endl;
				}
			}
		}
		moveMessageStack.clear();
	}

	//send out a ping of all the currently calculated bullet positions 
	if (shootStack.size() != 0)
	{
		serverBulletPing bulletsOut1;
		serverBulletPing bulletsOut2;
		//initilise the packet
		for (int count = 0; count < maxBullets; count++)
		{
			bulletsOut1.messageType = 4;
			bulletsOut1.posx[count] = 0;
			bulletsOut1.posy[count] = 0;
			bulletsOut1.dir[count] = false;
			bulletsOut1.awake[count] = false;

			bulletsOut2.messageType = 4;
			bulletsOut2.posx[count] = 0;
			bulletsOut2.posy[count] = 0;
			bulletsOut2.dir[count] = false;
			bulletsOut2.awake[count] = false;
		}
		int count = 0;
		list<playerShootPing>::iterator it;
		for (it = shootStack.begin(); it != shootStack.end(); it++)
		{//go throught the list of bullet pings and find all the bullet pings and broadcast the bullet positions
			switch (it->playerNum)
			{
			case 1:
				bulletsOut1.posx[it->bulletNum] = it->posx;
				bulletsOut1.posy[it->bulletNum] = it->posy;
				bulletsOut1.dir[it->bulletNum] = it->dir;
				bulletsOut1.awake[it->bulletNum] = true;
				count++;
				break;
			case 2:
				bulletsOut2.posx[it->bulletNum] = it->posx;
				bulletsOut2.posy[it->bulletNum] = it->posy;
				bulletsOut2.dir[it->bulletNum] = it->dir;
				bulletsOut2.awake[it->bulletNum] = true;
				count++;
				break;
			}
		}
		if (count > 3)
		{
			int debug;
		}
	
		//shove the data into the packet
		sf::Packet packet1 = bulletPingPack(bulletsOut2);
		sf::Packet packet2 = bulletPingPack(bulletsOut1);

		if (!playerIps.empty())
		{
			for (int count = 0; count < playerIps.size(); count++)
			{//send bullet ping to clients
				switch (count)
				{
				case 0:
					if (socket.send(packet1, playerIps[count], playerPorts[count]) != sf::Socket::Done)
					{
						cout << "Error sending to player 1" << count << endl;
					}
					else
					{
						//cout << "Ping" << endl;
					}
					break;
				case 1:
					if (socket.send(packet2, playerIps[count], playerPorts[count]) != sf::Socket::Done)
					{
						cout << "Error sending to player 2" << count << endl;
					}
					else
					{
						//cout << "Ping" << endl;
					}
					break;
				}

			}
		}
		shootStack.clear();
	}
}

sf::Packet bulletPingPack(serverBulletPing pingIn)
{//function to pack bullet packet into the port
	sf::Packet output;
	output << pingIn.messageType;
	for (int it = 0; it < maxBullets; it++)
	{
		output << pingIn.posx[it];
	}
	for (int it = 0; it < maxBullets; it++)
	{
		output << pingIn.posy[it];
	}
	for (int it = 0; it < maxBullets; it++)
	{
		output << pingIn.awake[it];
	}
	for (int it = 0; it < maxBullets; it++)
	{
		output << pingIn.dir[it];
	}

	return output;
}
