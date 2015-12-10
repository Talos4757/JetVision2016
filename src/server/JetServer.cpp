/*
 * JetServer.cpp
 *
 *  Created on: Dec 5, 2015
 *      Author: ubuntu
 */

#include "JetServer.h"

JetServer *JetServer::server = NULL;
pthread_t *JetServer::serverThread = NULL;
bool JetServer::started = false;

void JetServer::StartServer(vector<Target> &targets, pthread_mutex_t &targetLocker, int port)
{
    if(!JetServer::started)
    {
        JetServer *aserver = new JetServer(targets, targetLocker, port);

        if(aserver->isInited)
        {
            JetServer::server = aserver;
            JetServer::serverThread = new pthread_t();
            pthread_create(serverThread, NULL, JetServer::_ListenAsync, (void*) JetServer::server);

            JetServer::started = true;
        }
        else
        {
            cerr << "StartServer(): Server failed to init!" << endl;
            delete aserver;
        }
    }
    else
    {
        cerr << "StartServer(): Server already started..." << endl;
    }
}

void JetServer::CloseServer()
{
    JetServer::CloseServer(NULL);
}

void JetServer::CloseServer(void** ret)
{
    if(JetServer::started)
    {
        pthread_cancel(*JetServer::serverThread);
        pthread_join(*JetServer::serverThread, ret);

        delete JetServer::server;
        delete JetServer::serverThread;

        JetServer::started = false;
    }

    cerr << "Server closer: Success" << endl;
}

void* JetServer::_ListenAsync(void* arg)
{
    JetServer *server = (JetServer*)arg;

    try
    {
        server->Listen();
    }
    catch (abi::__forced_unwind&)
    {
        cerr <<  "Server: Thread terminated, closing..." << endl;
        throw;
    }

    return 0;
}

JetServer::JetServer(vector<Target> &targets, pthread_mutex_t &targetLocker, int port)
    :targets(targets),
     targetLocker(targetLocker)
{
    this->port = port;
    this->targets = targets;
    this->targetLocker = targetLocker;
    this->isInited = false;
    this->serverSocket = 0;
    this->rioSocket = 0;
    this->Init();
}

JetServer::~JetServer()
{
    close(this->serverSocket);
    close(this->rioSocket);
}

int JetServer::Init()
{
    this->serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    int optval = 1;
    setsockopt(this->serverSocket, SOL_SOCKET, SO_REUSEADDR, &optval, sizeof(optval));

    if(this->serverSocket < 0)
    {
        cerr << "Server: Could not create socket! Errno: " << errno << endl;
        return 1;
    }

    struct sockaddr_in sa;
    sa.sin_family = AF_INET;
    sa.sin_port = htons(this->port);
    sa.sin_addr.s_addr = INADDR_ANY;

    if(bind(this->serverSocket, (struct sockaddr *)&sa, sizeof(sa)) != 0)
    {
        cerr << "Server: Could not bind socket on port " << this->port << ". Errno: " << errno << endl;
        return 1;
    }

    cout << "Server: Server active on port " << this->port << endl;
    this->isInited = true;

    return 0;
}

int JetServer::Listen()
{
    if(this->isInited)
    {
        while(true)
        {
            cerr << "Server: Listening.." << endl;

            if(listen(this->serverSocket, 2) < 0)
            {
                cerr << "Server: Listening failed!" << endl;
                return 1;
            }
            else
            {
                struct sockaddr sar;
                memset(&sar, 0, sizeof(sar));
                socklen_t st;
                memset(&st, 0, sizeof(st));

                cerr << "Server: Waiting for connection..." << endl;
                this->rioSocket = accept(this->serverSocket, &sar, &st);

                if(this->rioSocket < 0)
                {
                    cerr << "Server: Error connecting" << endl;
                }
                else
                {
                    cerr << "Server: Connected!" << endl;
                    this->HandleRequests();
                    cerr << "Server: Back to listen loop" << endl;
                }
            }
        }
    }
    else
    {
        cerr << "Server: Not initialized, cannot listen" << endl;
        return 1;
    }
}

void JetServer::HandleRequests()
{
    cerr << "Server: Connected and waiting for client request" << endl;

    char buffer[sizeof(RequestType)];

    while(true)
    {
        int len = recv(this->rioSocket, buffer, sizeof(RequestType), 0);
        if(len > 0)
        {
            RequestType rt = *(RequestType*)buffer;

            switch(rt)
            {
                case TargetReq:
                    if(this->SendTargets())
                    {
                        cerr << "Server: Targets sent successfully" << endl;
                    }
                    else
                    {
                        cerr << "Server: Error sending targets" << endl;
                    }
                    break;
                /*
                 * Add other cases here...
                 */
                default:
                    cerr << "Server: Unrecognized request number:" <<  rt << endl;
                    this->SendInvalidRequestResponse();
                    break;
            }
        }
        else
        {
            cerr << "Server: Client disconnected" << endl;
            break;
        }
    }
}

bool JetServer::SendInvalidRequestResponse()
{
    int value = -1;
    if(send(this->rioSocket,(void*)&value, sizeof(int), 0) == -1)
    {
         cout <<  "Server: Failed to send invalid request response. Errno: " << errno << endl;
         return false;
    }

    cout <<  "Server: Sent invalid request response" << endl;

    return true;
}

bool JetServer::SendTargets()
{
    cerr << "Server: Attempting to send targets..." << endl;

    vector<Target> targetsCopy;

    pthread_mutex_lock(&this->targetLocker);
    targetsCopy = vector<Target>(this->targets);
    pthread_mutex_unlock(&this->targetLocker);

    int targetCount = targetsCopy.size();

     if(send(this->rioSocket,(void*)&targetCount, sizeof(int), 0) == -1)
     {
         cout <<  "Server: Failed to send targets count header. Errno: " << errno << endl;
         return false;
     }

    for(int i = 0; i < targetCount; i++)
    {
        char* encodedTarget = targetsCopy[i].Serialize();

        if(send(this->rioSocket, encodedTarget, TARGET_SIZE, 0) == -1)
        {
            return false;
        }

        delete[] encodedTarget;
    }

    cerr << "Server: Sent " << targetCount << " targets" << endl;
    return true;
}


