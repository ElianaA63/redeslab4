#ifndef NET
#define NET

#include <string.h>
#include <omnetpp.h>
#include <packet_m.h>
#include <list>
#include <algorithm>
#include <iostream>

using namespace omnetpp;
using namespace std;

class Net: public cSimpleModule {
private:
    int secuencia;
    int outs;
    list <int> table[57];
    int hopTable[57];
    HelloPacket *hello;
    cMessage *sendHello = new cMessage("sendHello");
public:
    Net();
    virtual ~Net();
protected:
    virtual void initialize();
    virtual void finish();
    virtual void handleMessage(cMessage *msg);
    virtual bool isIn(int sec, int source);
    virtual void sendAll(int e, cMessage *msg);
    virtual void sendAll(cMessage *msg);
};

Define_Module(Net);

#endif /* NET */

Net::Net() {
}

Net::~Net() {
}

void Net::initialize() {

    //table pre-filled with max length
    for(int i = 0; i < 56; i++){
        if(this->getParentModule()->getIndex() == i) hopTable[i] = 0;
        else hopTable[i] = 20;
    }

    secuencia = 0;
    outs = 4;

    scheduleAt(0, sendHello);
}

void Net::finish() {
    cancelAndDelete(hello);
    cancelAndDelete(sendHello);
}

bool Net::isIn(int sec, int source){
    list<int>::iterator it;
    for (it = table[source].begin(); it != table[source].end(); it++){
        int n = (*it);
        if(n == sec) return true;
    }
    return false;
}

void Net::sendAll(cMessage *msg){
    sendAll(-1, msg);
}

void Net::sendAll(int e, cMessage *msg){
    Packet *pkt = (Packet *)msg;
    for(int i = 0; i < outs; i++){
        if(i != e){
            if(this->gate("toLnk$o", i)->isPathOK()){
                Packet *copy = pkt->dup();
                send(copy, "toLnk$o", i);
            }
        }
    }
}

void Net::handleMessage(cMessage *msg) {

    if(msg == sendHello){
        int index = this->getParentModule()->getIndex();
        hello = new HelloPacket("hello", this->getParentModule()->getIndex());
        hello->setHops(0);
        sendAll(hello);
    }
    else if(strcmp(msg->getName(), "hello") == 0){
        HelloPacket *h = (HelloPacket *)msg;
        h->setHops(h->getHops() + 1);
        int hops = h->getHops();
        int node = h->getKind();
        if(hopTable[node] > hops){
            hopTable[h->getKind()] = hops;
            sendAll(msg->getArrivalGate()->getIndex(), msg);
        }
        else{
            delete(msg);
        }
    }
    else{
        // All msg (events) on net are packets
        Packet *pkt = (Packet *) msg;

        int destination = pkt->getDestination();
        int currentIndex = this->getParentModule()->getIndex();
        //packet from App
        if(msg->arrivedOn("toApp$i")){
            // set sec number
            pkt->setSec(secuencia);
            pkt->setMaxHops(hopTable[pkt->getDestination()]);
            // save sec number to next msg
            secuencia = secuencia + 1;
            // send it for all out
            sendAll(msg);
        }
        //packet from Lnk
        else{
            // if sec number already exits, delete
            if(isIn(pkt->getSec(), pkt->getSource()) || pkt->getMaxHops() == 0){
                delete(msg);
            }
            // if sec number dont exits
            else{
                // add par (source, sec) to table
                table[pkt->getSource()].push_front(pkt->getSec());
                pkt->setMaxHops(pkt->getMaxHops() -1);
                // If this node is the final destination, send to App
                if (destination == currentIndex) {
                    send(msg, "toApp$o");
                }
                // If not, forward the packet to some else... to who?
                else{
                    pkt->setHopCount(pkt->getHopCount() + 1);
                    // send it for all out except wich arrived on
                    sendAll(msg->getArrivalGate()->getIndex(), msg);
                }
            }
        }
    }
}
