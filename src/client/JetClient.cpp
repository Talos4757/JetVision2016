/*
 * JetClient.cpp
 *
 *  Created on: Dec 8, 2015
 *      Author: matan
 */

#include "JetClient.h"

JetClient *JetClient::client = NULL;
pthread_t *JetClient::clientThread = NULL;

void JetClient::StartClient(int port)
{
	if(JetClient::client == NULL && JetClient::clientThread == NULL)
	{
		cerr << "Client: Starting..." << endl;
		JetClient::client = new JetClient(port);
		JetClient::clientThread = new pthread_t();

		pthread_create(JetClient::clientThread, NULL, _StartAsync, (void*)JetClient::client);
	}
	else
	{
		cerr << "Client: Already started" << endl;
	}
}

void JetClient::CloseClient(void* ret)
{
	pthread_cancel(*JetClient::clientThread);
	pthread_join(*JetClient::clientThread, &ret);

	delete JetClient::client;
	delete JetClient::clientThread;

	cerr << "Client closer: Success" << endl;
}

void* JetClient::_StartAsync(void* arg)
{
	JetClient *jclient = (JetClient*)arg;

	try
	{
		return ((void*)jclient->Connect());
	}
	catch(abi::__forced_unwind&)
	{
		cerr << "Client: Thread terminated, closing..." << endl;
		throw;
	}

	return NULL;
}

JetClient::JetClient(int port)
{
	this->port = port;
	this->clientSocket = 0;
	this->isConnected = false;
	this->isErrored = false;
	this->Init();
}

JetClient::~JetClient()
{
	close(this->clientSocket);
}

int JetClient::Init()
{
	this->clientSocket = socket(AF_INET, SOCK_STREAM, 0);
	if(this->clientSocket < 0)
	{
		cerr << "Client: Could not create socket! Errno: " << errno << endl;
		return 1;
	}

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(this->port);
	sa.sin_addr.s_addr = INADDR_ANY;

	if(bind(this->clientSocket, (struct sockaddr *)&sa, sizeof(sa)) != 0)
	{
		cerr << "Client: Could not bind socket on port " << this->port << ". Errno: " << errno << endl;
		return 1;
	}

	cerr << "Client: Client active on port " << this->port << endl;

	return this->Connect();
}

int JetClient::Connect()
{
	cout << "Client: Connecting to server..." << endl;

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SERVER_PORT);
	sa.sin_addr.s_addr = inet_addr(SERVER_IP);

	bool notified = false;
	while(connect(this->clientSocket,(struct sockaddr*)&sa, sizeof(sa)) != 0)
	{
		if(!notified)
		{
			cerr << "Client: failed to connect to the server. Errno: " << errno << endl;
			notified = true;
		}
	}

	cerr << "Client: connected to server!" << endl;

	this->isConnected = true;
	this->isErrored = false;
	return 0;
}

vector<Target>* JetClient::GetTargets()
{
	return (vector<Target>*)JetClient::client->Query(TargetReq);
}

void* JetClient::Query(RequestType type)
{
	if(this->isConnected && !this->isErrored)
	{
		if(send(this->clientSocket, &type, sizeof(type), 0) < 0)
		{
			cerr << "Client: failed to query server" << endl;
			return (void*)NULL;
		}

		cerr << "Client: Queried server, awaiting response" << endl;

		char buffer[sizeof(int)];
		recv(this->clientSocket, buffer, sizeof(RequestType), 0);
		int count = *(int*)buffer;

		vector<Target> *targets = new vector<Target>();
		char targetBuffer[TARGET_SIZE];
		for(int i = 0; i < count; i++)
		{
			recv(this->clientSocket, targetBuffer, TARGET_SIZE, 0);
			targets->push_back(*(Target*)targetBuffer);
		}

		return (void*)targets;
	}

	return (void*)NULL;
}
