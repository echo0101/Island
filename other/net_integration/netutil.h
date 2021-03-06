#pragma once

#include <vector>

#include "stdint.h"
#include "types.h"

using namespace std;

class gamestate_t;
class playerstate_t;
class objectstate_t;
class wepfirestate_t;

struct gDelta_data;

#define SYNCTYPE_SYNC 1
#define SYNCTYPE_DELTA 2
struct psSync_data;

#define GSSTATE_INACTIVE 0
#define GSSTATE_ACTIVE 1
#define GSSTATE_INVALID 99

#ifndef GS_MAX_MAP_NAME_LEN
#define GS_MAX_MAP_NAME_LEN 50
#endif
using namespace std;

class netobj{

   public:
      virtual int serialize_delta(char* buf, int sz) = 0;
	  virtual int serialize_sync(char* buf, int sz) = 0;
      virtual int sync(char* buf, int sz) =0;
};


class gamestate_t: public netobj{
   public:
      uint32_t _tick;
      uint8_t _state;
      char _map[GS_MAX_MAP_NAME_LEN];

      vector<playerstate_t*> _players;
      vector<objectstate_t*> _objects;
      vector<wepfirestate_t*> _wepfire;

      gamestate_t(uint32_t time, char* map);
	  ~gamestate_t();

      void tick(uint32_t time);

      void addPlayer(playerstate_t *player);
      void removePlayer(uint16_t id);

      void addObject(objectstate_t *object);
      void removeObject(uint16_t id);

      void addWepfire(wepfirestate_t *wepfire);

      void updateState(uint8_t state);

	  vector<gDelta_data> _deltas;

      //network functions:
      int serialize_delta(char* buf, int sz) {return 0;};
	  int serialize_sync(char* buf, int sz) {return 0;};
      int sync(char* buf, int sz) {return 0;};
};

#define PSSTATE_INIT 0
#define PSSTATE_ALIVE 1
#define PSSTATE_DEAD 2
#define PSSTATE_WAITSYNC 3

#define PLAYERSTATE_MAXABILITY 5

class playerstate_t: public netobj{
   public:
	  uint32_t _tick; //4
      uint16_t _id; //2
      uint8_t _hp; //1
      uint8_t _mp; //1
      uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
      uint8_t _weapon; //1
      coord2d_t _pos; //8
      coord2d_t _vel; //8
      uint8_t _state; //1
      uint16_t _score; //2

	  vector<gDelta_data> _deltas;

	  playerstate_t(uint32_t time);
	  playerstate_t(const playerstate_t &player);
	  playerstate_t(psSync_data* data);

	  void tick(uint32_t time);
	  void draw() {};

	  void change_velocity(coord2d_t nV);
	  void change_velocity(double nVx, double nVy);

	  //network functions:
      int serialize_delta(char* buf, int sz){cerr << "TODO" << endl; return 0;};
	  int serialize_sync(char* buf, int sz) ;
      int sync(char* buf, int sz);
};

#define OBJECTSTATE_CRATE 0
#define OBJECTSTATE_CRATE_DIM 1

class objectstate_t: public netobj{
   public:

      uint16_t _id;
      uint8_t _hp;
      uint8_t _type;
      coord2d_t _pos;

	  vector<gDelta_data> _deltas;

     objectstate_t(): _id(0), _hp(0), _type(0) {};
	 objectstate_t(uint16_t id, uint8_t hp, uint8_t type, coord2d_t pos): _id(id), _hp(hp), _type(type), _pos(pos) {};
	 objectstate_t(uint16_t id, uint8_t hp, uint8_t type, double x, double y): _id(id), _hp(hp), _type(type), _pos(x,y) {};
	 objectstate_t(const objectstate_t& other): _id(other._id), _hp(other._hp),
                           _type(other._type), _pos(other._pos) {};


	  void setType(uint8_t type);

	  void draw() {};

	  //network functions:
      int serialize_delta(char* buf, int sz) {return 0;};
	  int serialize_sync(char* buf, int sz) {return 0;};
      int sync(char* buf, int sz) {return 0;};
};

class wepfirestate_t{
   public:

      coord2d_t _pos;
      coord2d_t _vel;
      uint32_t _ttl;
      uint8_t _type;

	  vector<gDelta_data> _deltas;

	  void draw() {};

	  //network functions:
      int serialize_delta(char* buf, int sz) {return 0;};
	  int serialize_sync(char* buf, int sz) {return 0;};
      int sync(char* buf, int sz) {return 0;};
};

#if 0
class gDelta_t{
	uint32_t _tick;

public:

	gDelta_t():_tick(0){};
	gDelta_t(const gDelta_t &o):_tick(o._tick){};

	gDelta_t operator=(const gDelta_t &);

};
#endif

class client_t{
	uint16_t _clientid;
	string _name;
};

class gameServer{
	uint16_t _serverid;
	vector<client_t> _clients;

};

//packets
#define MAX_PKTSZ 1024

//pkt types
#define PKT_ACK 1

#define PKT_SYNC_GAMESTATE 11
#define PKT_SYNC_PLAYERSTATE 12
#define PKT_SYNC_OBJECTSTATE 13
#define PKT_SYNC_WEPFIRESTATE 14
#define PKT_DELTA_GAMESTATE 21
#define PKT_DELTA_PLAYERSTATE 22
#define PKT_DELTA_OBJECTSTATE 23
#define PKT_DELTA_WEPFIRESTATE 24
#define PKT_REQSYNC_GAMESTATE 31
#define PKT_REQSYNC_PLAYERSTATE 32
#define PKT_REQSYNC_OBJECTSTATE 33
#define PKT_REQSYNC_WEPFIRESTATE 34

#define PKT_PLAYER_MOVE 50
#define PKT_PLAYER_ATTACK 51

#define PKT_TXT 90

#define SEQ_INVALID_PKT 0

struct pkt_header{
	uint8_t start; //1 #
	uint8_t type; //1
	uint16_t clientid; //2
	uint16_t serverid; //2
	uint8_t checksum; //1
	uint8_t length; // 1 -- only used in lists (otherwise = 0)
	uint32_t seq; //4
};



#define NET_ACK 1
#define NET_NACK 0
struct gAck_data{
	uint8_t ack_value;
};
int make_ack(char * buf, int bufsz, 
			 uint8_t value, uint32_t seq);



struct gHello_data{
	uint32_t challengeVersion; //ensures that client and server have same version
};

//header for delta list
struct gDeltaHdr_data{
	//deprecated
	uint32_t list_len;
};

#define GDELTA_DATA_DEFINED
struct gDelta_data{ //13
   uint32_t tick; //4
   uint8_t field; //1
   varies oValue; //4
   varies nValue; //4
};

//HACK
#ifndef GS_MAX_MAP_NAME_LEN
#define GS_MAX_MAP_NAME_LEN 50
#endif

struct gsSync_data{
      uint32_t _tick;
      uint8_t _state;
      char _map[GS_MAX_MAP_NAME_LEN];
	  // send syncs for all 
	  // player,object,wepfire state
	  // variables independently
};

#ifndef PLAYERSTATE_MAXABILITY
#define PLAYERSTATE_MAXABILITY 5
#endif

struct psSync_data{ //34
	uint8_t _synctype; //=SYNC 1
	uint32_t _tick; //4
	uint16_t _id; //2
	uint8_t _hp; //1
	uint8_t _mp; //1
	uint8_t _ability[PLAYERSTATE_MAXABILITY]; //1*5
	uint8_t _weapon; //1
	double _pos_x; //4
	double _pos_y; //4
	double _vel_x; //4
	double _vel_y; //4
	uint8_t _state; //1
	uint16_t _score; //2
};

struct osSync_data{
	uint16_t _id;
	uint8_t _hp;
	uint8_t _type;
	double _pos_x;
	double _pos_y;
};


struct wsSync_data{
	double _pos_x;
	double _pos_y;
	double _vel_x;
	double _vel_y;
	uint32_t _ttl;
	uint8_t _type;
};

struct playerChangeMove_data{
	uint16_t _id; //2
	double _vel_x;
	double _vel_y;
};

struct playerAttack_data{
	uint16_t _id; //2
	uint8_t _weapon;
	double _pos_x; //4
	double _pos_y; //4
	double _vel_x; //4
	double _vel_y; //4
	uint16_t _ttl;
};

uint8_t calcAddSum(const char* buf, int size);
bool verify_checksum(const char* buf, int size);
