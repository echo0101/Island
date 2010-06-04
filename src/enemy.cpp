#include "enemy.h"
#include "types.h"
#include <math.h>
#include "collision.h"
#include "Bin.h"
#include "mdmodel.h"
#include "particle.h"

#define MELEE_AWARE_DIST 7.0
#define RANGED_AWARE_DIST 8.0
#define RANGED_ATTACK_DIST 6.0
#define RANGED_FLEE_DIST 3.0

#define PSTATE_FLEEING 20
#define PSTATE_NOTFLEEING 19
extern mdmodel *enemyMod;
extern gamestate *gs;

void meleeAI::tick(uint32_t time){
	if(_pos.distanceTo((*gs->player)._pos) > MELEE_AWARE_DIST){ //unaware
		_vel.x() += (double)(rand()%10-5)/5.0;
		_vel.y() = .004;
	}
	else { //aware
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+ecc;
		if(collide(front,gs->player->body)){
			_vel.y() = 0;
			//attack (blood particles?)
		}else{
			_vel.y() = .004;
		}
	}
	if(gs->SmaPlCollision(this)&&!collide(front,gs->player->body)){
		_vel.y() = -.002;
		ecc = (double)(rand()%5)/3.0-.75;
	}
	for(; _tick<time; ++_tick){
		if (_vel.y() > DBL_EPSILON){
			_pos.x() += (-sin(_vel.x()) * _vel.y());
			_pos.y() += (cos(_vel.x()) * _vel.y());
		}
	}
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
	if(_vel.y()>0) Animate(&enemyMod->md5anim[1],&walAni,WORLD_TIME_RESOLUTION);
	else Animate(&enemyMod->md5anim[0],&idlAni,WORLD_TIME_RESOLUTION);
}

void meleeAI::draw(){
	glPushMatrix();
	glTranslatef(_pos.x(), .6, -_pos.y());
    glRotatef(_vel.x()*180.0/3.1415, 0, 1, 0);
    glRotatef(180,0,1,0);
	//if(_state == PLYRSTATATT)
	//	enemyMod->draw(attAni);
	//else
		enemyMod->draw((_vel.y()>0)?walAni:idlAni);
	glPopMatrix();
}

void rangedAI::tick(uint32_t time){
	double distto = _pos.distanceTo(gs->player->_pos);
	if(distto > RANGED_AWARE_DIST){ //unaware
		_vel.x() += (double)(rand()%10-5)/5.0;
		_vel.y() = .004;
		cooldown = 0;
	}
	else if(distto > RANGED_ATTACK_DIST){ //aware but out of range
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+ecc;
		_vel.y() = .004;
		_state = PSTATE_NOTFLEEING;
		cooldown--;
	}
	else if(distto > RANGED_FLEE_DIST && _state!=PSTATE_FLEEING){
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y());
		_vel.y() = .000;
		if(cooldown==0){
			if(checkLOA()){
				gs->rapid(*this);
				cooldown = 5;
			}
			else{
				_vel.x() += (ecc>0)?1.57:-1.57;
				_vel.y() = .002;
			}
		}
		else cooldown--;
	}
	else{
		_vel.x() = atan2(_pos.x()-gs->player->_pos.x(),gs->player->_pos.y()-_pos.y())+3.1415;
		_vel.y() = .004;
		_state = PSTATE_FLEEING;
		cooldown--;
	}
	if(gs->SmaPlCollision(this)) {_vel.y() = -.002; ecc = (double)(rand()%5)/3.0-.75;}
	if(_vel.y()>0) Animate(&enemyMod->md5anim[1],&walAni,WORLD_TIME_RESOLUTION);//30.0*(time-_tick));
	else Animate(&enemyMod->md5anim[0],&idlAni,WORLD_TIME_RESOLUTION);
	
	for(; _tick<time; ++_tick){
		if (_vel.y() > DBL_EPSILON){
			_pos.x() += (-sin(_vel.x()) * _vel.y());
			_pos.y() += (cos(_vel.x()) * _vel.y());
		}
	}
	if(cooldown<0) cooldown = 0;
	body = bbody(_pos.x(),-_pos.y(),.5,0,BB_CIRC);
	coord2d_t dummy;
	front = bbody(this->calcHotSpot(dummy,.6),.1,BB_CIRC);
	gs->updatBinLists(this,UPDAT);
	
}

void rangedAI::draw(){
	glPushMatrix();
	glTranslatef(_pos.x(), .6, -_pos.y());
    glRotatef(_vel.x()*180.0/3.1415, 0, 1, 0);
    glRotatef(180,0,1,0);
	//if(_state == PLYRSTATATT)
	//	enemyMod->draw(attAni);
	//else
		enemyMod->draw((_vel.y()>0)?walAni:idlAni);
	glPopMatrix();
}

bool rangedAI::checkLOA(){
	lineOfAtt *loa = new lineOfAtt(_pos,gs->player->_pos,_id);
	int px = _pos.x()+50; int pz = -_pos.y()+50;
	gs->LarPaCollision(loa,px-6,px+6,pz-6,pz+6); //already know it can't be more than 6 from this
	return !(loa->active); //if loa->active == true, something's in the way
}