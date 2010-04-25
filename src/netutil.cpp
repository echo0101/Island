#include "netutil.h"

#include "types.h"
#include <cstring>

#define __STDC_LIMIT_MACROS
#include "stdint.h"

using namespace std;
// gamestate_t

gamestate_t::gamestate_t(uint32_t time, char *map)
			: _tick(time), _state(GSSTATE_INACTIVE){
				
	if(strlen(map) > GS_MAX_MAP_NAME_LEN){
		strcpy(_map,"");
		_state = GSSTATE_INVALID;
		throw gException("ERROR: Map name length exceeds max len.");
	}
	strcpy(_map, map);
}

void gamestate_t::tick(uint32_t time){
	// TODO: implement method	
}

void gamestate_t::addPlayer(const playerstate_t &player){
	// TODO: implement method	
}

void gamestate_t::removePlayer(uint16_t id){
	// TODO: implement method	
}

void gamestate_t::addObject(const objectstate_t &object){
	// TODO: implement method	
}

void gamestate_t::removeObject(uint16_t id){
	// TODO: implement method	
}

void gamestate_t::addWepfire(const wepfirestate_t &wepfire){
	// TODO: implement method	
}

void gamestate_t::updateState(uint8_t state){
	// TODO: implement method	
}


// playerstate_t


playerstate_t::playerstate_t(uint32_t time): _tick(time), _hp(0), _mp(0), 
					_weapon(0), _state(PSSTATE_INIT),
					_score(0){
	_id = (uint16_t) ( 
		( (double) rand() / ( (double) (RAND_MAX) + 1.0 ) )
		* UINT16_MAX ) + 1;
	
	for (int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		_ability[i]=0;
}

playerstate_t::playerstate_t(const playerstate_t &player){
	_tick = player._tick;
	_id = player._id;
	_hp = player._hp;
	_mp = player._mp;

	for (int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		_ability[i]=player._ability[i];

	_weapon = player._weapon;
	_pos = player._pos;
	_vel = player._vel;
	_state = player._state;
	_score = player._score;

	for( vector<gDelta_data>::const_iterator it = player._deltas.begin();
		it != player._deltas.end();
		++it )
		_deltas.push_back((*it));
}

void playerstate_t::tick(uint32_t time){
	for(; _tick<time; ++_tick){
		if (_vel.y() > DBL_EPSILON){
			_pos.x() += (-sin(_vel.x()) * _vel.y());
			_pos.y() += (cos(_vel.x()) * _vel.y());
		}
	}
}

void playerstate_t::change_velocity(coord2d_t nV){
	/*
	playerChangeMove_delta netdata;
	netdata.id = 0; // TODO
	netdata._vel_x = nV.x();
	netdata._vel_y = nV.y();
	*/
	//TODO: instantly tell server that you've moved.

	_vel = nV;
}
void playerstate_t::change_velocity(double nVx, double nVy){
	_vel.x() = nVx;
	_vel.y() = nVy;
}

int playerstate_t::serialize_delta(char* buf, int sz) {
	psSync_data* ptr = (psSync_data*) (buf);
	ptr->_tick = _tick;
	ptr->_id = _id;
	ptr->_hp = _hp;
	ptr->_mp = _mp;
	for(int i=0; i<PLAYERSTATE_MAXABILITY; ++i)
		ptr->_ability[i] = _ability[i];
	ptr->weapon = _weapon;
	ptr->_pos_x = _pos.x();
	ptr->_pos_y = _pos.y();
	ptr->_vel_x = _vel.x();
	ptr->_vel_y = _vel.y();
	ptr->_state = _state;
	ptr-._score = _score;

	return 0;
};

int make_ack(char * buf, int bufsz, 
			 uint8_t value, uint32_t seq){
	if (bufsz < sizeof(pkt_header) + sizeof(gAck_data))
		return 0;
	
	((pkt_header*)buf)->start = '#';
	((pkt_header*)buf)->type = PKT_ACK;
	((pkt_header*)buf)->clientid = 0; //TODO
	((pkt_header*)buf)->serverid = 0; //TODO
	((pkt_header*)buf)->checksum = 0;
	((pkt_header*)buf)->length = 0;
	((pkt_header*)buf)->seq = 0; //TODO

	((gAck_data*)(buf+sizeof(pkt_header)))->ack_value = value;

	return sizeof(pkt_header)+sizeof(pkt_header);

}

bool verify_checksum(const char* buf, int size){
	bool retval = false;

	char* temp;

	if (size >= sizeof(pkt_header)){
		temp = (char*) malloc(sizeof(size));
		memcpy(temp, buf, size);
		((pkt_header*)temp)->checksum = 0;

		retval = (((pkt_header*)buf)->checksum == 
			calcAddSum(temp, size));

		free(temp);
	}
	return retval;
}

uint8_t calcAddSum(const char* buf, int size){
	uint8_t sum = 0;

	while(size-- > 0)
		sum += *(buf++);
	return (~sum);
}
