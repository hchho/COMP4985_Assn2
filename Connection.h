#ifndef CONNECTION_H
#define CONNECTION_H

class Connection {
public:
    virtual ~Connection() {}
    virtual bool send() = 0;
    virtual bool recv() = 0;
};

class TCPConnection : public Connection {
public:
    bool send() override;
    bool recv() override;
};

class UDPConnection : public Connection {
    bool send() override;
    bool recv() override;
};

#endif // CONNECTION_H
