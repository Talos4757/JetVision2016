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
#define SERVER_PORT 4242
#define SERVER_IP "127.0.0.1"

class JetClient {
public:
	static void StartClient(int port);
	static void CloseClient(void* ret);
	static vector<Target>* GetTargets();
private:
	JetClient(int port);
	virtual ~JetClient();

	int Init();
	int Connect();
	void* Query(RequestType type);

	static JetClient *client;
	static pthread_t *clientThread;
	static void* _StartAsync(void* arg);

	int port;
	int clientSocket;

	bool isConnected;
	bool isErrored;
};

#endif /* CLIENT_JETCLIENT_H_ */
