/*
 * JetServer.h
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#ifndef JETSERVER_H_
#define JETSERVER_H_

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <vector>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>

#include "Target.h"

#define TCP_SOCKET 0
#define SERVER_PORT 4242

using namespace std;

class JetServer
{
public:
	static void StartServer(vector<Target*> *targets, pthread_mutex_t *targetLocker);
	static void CloseServer(void** ret);
private:
	JetServer(vector<Target*> *targets, pthread_mutex_t *targetLocker);
	virtual ~JetServer();

	int Init();
	int Listen();

	static JetServer *server;
	static pthread_t *serverThread;
	static void* _ListenAsync(void* arg);

	vector<Target*> &targets;
	pthread_mutex_t &targetLocker;

	int *serverSocket;
	int *acpSocket;
};

#endif /* JETSERVER_H_ */
