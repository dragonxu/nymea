#ifndef JSONRPCSERVER_H
#define JSONRPCSERVER_H

class JsonHandler;

class JsonRPCServer
{
public:
    explicit JsonRPCServer() = default;
    virtual ~JsonRPCServer() = default;

    virtual bool registerExperienceHandler(JsonHandler *handler, int majorVersion, int minorVersion) = 0;

};

#endif // JSONRPCSERVER_H
