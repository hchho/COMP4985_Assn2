#ifndef CONNECTION_H
#define CONNECTION_H

class Connection {
public:
    virtual bool send() = 0;
    virtual bool recv() = 0;
};

class TCPConnection : public Connection {

};

class UDPConnection : public Connection {

};

#endif // CONNECTION_H
