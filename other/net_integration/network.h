#pragma once

#ifndef WIN32_LEAN_AND_MEAN
#define WIN32_LEAN_AND_MEAN
#endif

#pragma comment(lib, "ws2_32.lib")
#pragma comment(lib, "wsock32.lib")

#include <windows.h>
#include <winsock2.h>
#include <ws2tcpip.h>
#include <iphlpapi.h>
#include <cstdio>

#include <iostream>
#include <vector>

#include "netutil.h"


using namespace std;

class connection{
protected:
public:
	connection(){};
	
	virtual int sendBuf(char*, int) = 0;

};

#define CLIENT_DISCONNECT 0
#define CLIENT_ACTIVE 1

class client: public connection{
	WSADATA _wsaData;
	SOCKET _cSocket;
	int _state;
	gamestate_t* _gObj;
	int _me;
	bool _flagMoved;

public:
	client();
	~client();
	
	void connectTo(char* addr, char* port);

	void setGameObj(gamestate_t* gObj) {_gObj = gObj;};

	void change_velocity(double nVx, double nVy);
	void change_velocity(coord2d_t nV);

	void flagPlayerMove(){
		_flagMoved = true;
	}

	void setPlayerMove(coord2d_t v){
		char buf[1024];
		playerChangeMove_data* ptr;
		((pkt_header*)(buf))->start = '#';
		((pkt_header*)(buf))->type = PKT_PLAYER_MOVE;
		((pkt_header*)(buf))->clientid = 0;
		((pkt_header*)(buf))->serverid = 0; //TODO
		((pkt_header*)(buf))->checksum = 0;
		((pkt_header*)(buf))->length = sizeof(pkt_header)+sizeof(playerChangeMove_data);
		((pkt_header*)(buf))->seq = 0; //TODO

		ptr = (playerChangeMove_data*) (buf+sizeof(pkt_header));
		ptr->_id = 0;
		ptr->_vel_x = v.x();
		ptr->_vel_y = v.y();

		((pkt_header*)(buf))->checksum = calcAddSum(buf, sizeof(pkt_header)+sizeof(playerChangeMove_data));
		sendBuf(buf,sizeof(pkt_header)+sizeof(playerChangeMove_data));
	}

	void addPlayer(playerstate_t *player, bool isMe=false){
		char buf[1024];
		
		psSync_data* ptr;
		((pkt_header*)(buf))->start = '#';
		((pkt_header*)(buf))->type = PKT_SYNC_PLAYERSTATE;
		((pkt_header*)(buf))->clientid = 0;
		((pkt_header*)(buf))->serverid = 0; //TODO
		((pkt_header*)(buf))->checksum = 0;
		((pkt_header*)(buf))->length = sizeof(pkt_header)+sizeof(psSync_data);
		((pkt_header*)(buf))->seq = 0; //TODO
		player->serialize_sync(buf+sizeof(pkt_header),1024-sizeof(pkt_header));

		ptr = (psSync_data*) (buf+sizeof(pkt_header));
		cerr << "DEBUG: sending " << ptr->_id << " " << (int) ptr->_state << endl;

		((pkt_header*)(buf))->checksum = calcAddSum(buf, sizeof(pkt_header)+sizeof(psSync_data));
		sendBuf(buf,sizeof(pkt_header)+sizeof(psSync_data));
		if (isMe) _me = player->_id;
	}

	void tickRcv();

	int sendBuf(char*, int);
	int recvBuf(char*, int);
};

#define SERVER_DISCONNECT 0
#define SERVER_ACTIVE 1
#define SERVER_W4PLAYERS 2

class server: public connection{
	WSADATA _wsaData;
	SOCKET _lSocket; // listen socket

	struct cInfo{
		SOCKET cSocket;
		int state;
	};
	vector<cInfo> _clients;
	gamestate_t* _gObj;

	int _state;

public:
	server();
	~server();

	void setup(char* listen_port);
	void setGameObj(gamestate_t* gObj) {_gObj = gObj;};
	void setState(int state) {_state = state;};
	void startGame();

	int getState(){return _state;};

	int sendBuf(char*, int);

	void tickSnd();
	void tickRcv();

protected:
	
	void acceptClient();

	void disconnectAll();

};


