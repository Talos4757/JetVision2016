/*
 * JetClient.h
 *
 *  Created on: Dec 8, 2015
 *      Author: matan
 */

#ifndef CLIENT_JETCLIENT_H_
#define CLIENT_JETCLIENT_H_

#include <iostream>
#include <sys/socket.h>
#include <sys/types.h>
#include <netinet/in.h>
#include <unistd.h>
#include <errno.h>
#include <pthread.h>
#include <netinet/in.h>
#include <arpa/inet.h>
#include <cxxabi.h>
#include <vector>

#include "../types/RequestType.h"
#include "../types/Target.h"
#include "../types/TargetType.h"

#define TCP_SOCKET 0

#define MAX_CONNECT_RETRIES 5

class JetClient
{
public:
	static void StartClient(int port, int serverPort, string serverAddress);
	static void CloseClient();
	static void CloseClient(void* ret);
	static vector<Target>* GetTargets();

private:
	JetClient(int port, int serverPort, string serverAddress);
	virtual ~JetClient();

	int Init();
	int Connect();
	int Query(RequestType type);

	static bool isStarted;
	static JetClient *client;
	static pthread_t *clientThread;
	static void* _StartAsync(void* arg);

	int port;
	int serverPort;
	string serverAddress;

	int clientSocket;
	int connectRetryCount;

	bool isInited;
	bool isConnected;
};

#endif /* CLIENT_JETCLIENT_H_ */
