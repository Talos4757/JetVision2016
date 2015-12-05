/*
 * JetServer.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#include "JetServer.h"

JetServer *JetServer::server = NULL;
pthread_t *JetServer::serverThread = NULL;

void JetServer::StartServer(vector<Target*> *targets, pthread_mutex_t *targetLocker)
{
	if(JetServer::server == NULL && JetServer::serverThread == NULL)
	{
		JetServer *server = new JetServer(targets, targetLocker);

		pthread_t *serverThread = new pthread_t();
		pthread_create(serverThread, NULL, JetServer::_ListenAsync, (void*)server);

		JetServer::server = server;
		JetServer::serverThread = serverThread;
	}
	else
	{
		cerr << "StartServer: Server already started..." << endl;
	}
}

void JetServer::CloseServer(void** ret)
{
	pthread_cancel(*JetServer::serverThread);
	pthread_join(*JetServer::serverThread, ret);

	delete JetServer::server;
	delete JetServer::serverThread;

	cerr << "Server closer: Success" << endl;
}

void* JetServer::_ListenAsync(void* arg)
{
	JetServer *server = (JetServer*)arg;

	try
	{
		return (void*)(server->Listen());
	}
	catch (...)
	{
		close(*server->serverSocket);
		cerr << "Async server wrapper: caught exception, closed socket. bye bye! (on " << pthread_self() << ")" << endl;
		throw;
	}

	return 0;
}

JetServer::JetServer(vector<Target*> *targetsref, pthread_mutex_t *targetLocker)
	:targets(*targetsref),
	 targetLocker(*targetLocker)
{
	this->serverSocket = new int(0);
	this->acpSocket = new int(0);
	this->Init();
}

JetServer::~JetServer()
{
	delete this->serverSocket;
	delete this->acpSocket;
}

int JetServer::Init()
{
	this->serverSocket = new int(socket(AF_INET, SOCK_STREAM, 0));
	if(*this->serverSocket < 0)
	{
		cerr << "Server: Could not create socket! Errno: " << errno << endl;
		return 1;
	}

	struct sockaddr_in sa;
	sa.sin_family = AF_INET;
	sa.sin_port = htons(SERVER_PORT);
	sa.sin_addr.s_addr = INADDR_ANY;

	if(bind(*this->serverSocket, (struct sockaddr *)&sa, sizeof(sa)) != 0)
	{
		cerr << "Server: Could not bind socket on port " << SERVER_PORT << ". Errno: " << errno << endl;
		return 1;
	}

	cout << "Server: Server active on port " << SERVER_PORT << endl;

	return 0;
}

int JetServer::Listen()
{
	cerr << "Server: Listening for incoming requests.." << endl;

	if(listen(*this->serverSocket, 6) < 0)
	{
		cerr << "Server: Listening failed!" << endl;
		return 1;
	}
	else
	{
		struct sockaddr sar;
		socklen_t st;

		this->acpSocket = new int(accept(*this->serverSocket, &sar, &st));
		cerr << "Server: Connected!" << endl;
	}

	return 0;
}
