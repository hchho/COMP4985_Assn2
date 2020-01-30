#ifndef CONNECTION_H
#define CONNECTION_H

class Connection {
public:
    virtual ~Connection() {}
    virtual int start() = 0;
    virtual void stop() = 0;
};

class TCPConnection : public Connection {
public:
    int start() override {
        return 0;
    }
    void stop() override {}
};

class UDPConnection : public Connection {
    int start() override {
        return 0;
    }
    void stop() override {}
};

#endif // CONNECTION_H
