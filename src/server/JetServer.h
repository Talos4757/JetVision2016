#ifndef JETSERVER_H_
#define JETSERVER_H_

#include <iostream>
#include <cstring>
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

using namespace std;

class JetServer
{
public:
    static void StartServer(vector<Target> &targets, pthread_mutex_t &targetLocker, int port);
    static void CloseServer();
    static void CloseServer(void** ret);
private:
    JetServer(vector<Target> &targets, pthread_mutex_t &targetLocker, int port);
    virtual ~JetServer();

    bool Init();

    void Listen();
    void HandleRequests();

    bool SendTargets();
    bool SendInvalidRequestResponse();

    static JetServer *server;
    static pthread_t *serverThread;
    static bool started;

    static void* _ListenAsync(void* arg);

    vector<Target> &targets;
    pthread_mutex_t &targetLocker;

    int serverSocket;
    int rioSocket;

    int port;

    bool isInited;
};

#endif /* JETSERVER_H_ */
