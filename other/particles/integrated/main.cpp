



#include <cstdio>
#include <cstdlib>
#include <iostream>
#include <fstream>
#include <assert.h>
#include <cstring>
#include <map>
#include <vector>
#include "particle.h"

#define _USE_MATH_DEFINES
#include <cmath>

#include <GL/glut.h>

#define __STDC_LIMIT_MACROS
#include "stdint.h"

//#include "types.h"
//#include "gs_types.h"

using namespace std;

#define MIN(x,y) (x>y)?y:x
#define MAX(x,y) (x>y)?x:y

typedef struct materialStruct {
  GLfloat ambient[4];
  GLfloat diffuse[4];
  GLfloat specular[4];
  GLfloat shininess[1];
} materialStruct;

materialStruct Black = {
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0, 0.0, 0.0, 1.0},
  {0.0}
};

materialStruct Grey = {
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3, 0.3, 0.3, 1.0},
  {0.3}
};

//sets up a specific material
void materials(materialStruct materials) {
  glMaterialfv(GL_FRONT, GL_AMBIENT, materials.ambient);
  glMaterialfv(GL_FRONT, GL_DIFFUSE, materials.diffuse);
  glMaterialfv(GL_FRONT, GL_SPECULAR, materials.specular);
  glMaterialfv(GL_FRONT, GL_SHININESS, materials.shininess);
}


int light;
//globals for lighting - use a white light and apply materials
//light position
GLfloat light_pos[4] = {1.0, 5.0, 1.5, 1.0};
//light color (ambiant, diffuse and specular)
GLfloat light_amb[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_diff[4] = {0.6, 0.6, 0.6, 1.0};
GLfloat light_spec[4] = {0.8, 0.8, 0.8, 1.0};

int mat = 0;
//set up some materials



//other globals
int GW;
int GH;
float eyex, eyey, eyez;
float LAx, LAy, LAz;
float theta;
float angle;
float myX, myY, myZ;
bool flag = false;
bool rfire = false;
GLubyte * alpha;

fireball_s * fbsrc;
vector<fireball_p *> fbpar;
int fbtim;
explosion_s * exsrc;
vector<explosion_p *> expar;
bool explo;
vector<rapidfire *> rfpar;


float p2w_x(int x) {
  float x1;
  x1  = (x*(2/(float)GW)) + (((1/(float)GW) - 1));
  x1 = x1 * ((float)GW/(float)GH);

return x1;

}

float p2w_y(int y) {
  
  float y1;
  y1  = (y*(2/(float)GH)) + (((1/(float)GH) - 1));

return y1;
}

void genTex(){
	alpha = new GLubyte[16 * 16];
	int i,j;
	for(i=0;i<8;i++){
		for(j=0;j<8;j++) alpha[i+16*j] = 17*(MAX((i+j-5),0)); //17
		for(j=8;j<16;j++) alpha[i+16*j] = alpha[i+16*(15-j)]; //mirror
	}
	for(i=8;i<16;i++){
		for(j=0;j<16;j++) alpha[i+16*j] = alpha[(15-i)+16*j]; //mirror again
	}
}

void init_tex() {
	//glDisable(GL_DEPTH_TEST);
	glEnable(GL_BLEND);									// Enable Blending
	glBlendFunc(GL_SRC_ALPHA,GL_ONE_MINUS_SRC_ALPHA);

	genTex();
	glEnable(GL_TEXTURE_2D);
	//glTexEnvf(GL_TEXTURE_ENV,GL_TEXTURE_ENV_MODE,GL_DECAL);
	glBindTexture(GL_TEXTURE_2D, 0);
	glTexImage2D(GL_TEXTURE_2D, 0, GL_ALPHA, 16, 16, 0, GL_ALPHA, GL_UNSIGNED_BYTE, alpha);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_S, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_WRAP_T, GL_REPEAT);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MAG_FILTER, GL_NEAREST);
	//glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_NEAREST);
	glTexParameterf(GL_TEXTURE_2D, GL_TEXTURE_MIN_FILTER, GL_LINEAR);
	glDisable(GL_TEXTURE_2D);

}

void init_particle(){
	glNewList(PARTLIST,GL_COMPILE);
	glDisable(GL_LIGHTING);
	glEnable(GL_TEXTURE_2D);
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0.1f,0);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0.1f,0);
	glTexCoord2d(1,0); glVertex3f(0.1f,-0.1f,0);
	glTexCoord2d(0,0); glVertex3f(-0.1f,-0.1f,0);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0.1f,0,0.1f);
	glTexCoord2d(0,1); glVertex3f(-0.1f,0,0.1f);
	glTexCoord2d(1,0); glVertex3f(0.1f,0,-0.1f);
	glTexCoord2d(0,0); glVertex3f(-0.1f,0,-0.1f);
	glEnd();
	glBegin(GL_TRIANGLE_STRIP);
	glTexCoord2d(1,1); glVertex3f(0,0.1f,0.1f);
	glTexCoord2d(0,1); glVertex3f(0,-0.1f,0.1f);
	glTexCoord2d(1,0); glVertex3f(0,0.1f,-0.1f);
	glTexCoord2d(0,0); glVertex3f(0,-0.1f,-0.1f);
	glEnd();
	glEnable(GL_LIGHTING);
	glDisable(GL_TEXTURE_2D);
	glEndList();
}

//initialization calls for opengl for static light
//note that we still need to enable lighting in order for it to work
void init_lighting() {
  //turn on light0
  glEnable(GL_LIGHT0);
  //set up the diffuse, ambient and specular components for the light
  glLightfv(GL_LIGHT0, GL_DIFFUSE, light_diff);
  glLightfv(GL_LIGHT0, GL_AMBIENT, light_amb);
  glLightfv(GL_LIGHT0, GL_SPECULAR, light_spec);
  //specify our lighting model as 1 normal per face
}

void pos_light() {
  //set the light's position
  glMatrixMode(GL_MODELVIEW);
  glLightfv(GL_LIGHT0, GL_POSITION, light_pos);
}

void spawnFireball(){
	float fbx = -sin(theta);
	float fbz = -cos(theta);
	fbsrc = new fireball_s(myX/10.0f,-myZ/10.0f,fbx/5.0f,fbz/5.0f);
	for(int i=0;i<200;i++){
		fbpar.push_back(new fireball_p(fbsrc));
	}
}

void detonate(fireball_s * fbs){
	exsrc = new explosion_s(fbs->x,fbs->z);
	for(int i=0;i<400;i++){
		expar.push_back(new explosion_p(exsrc));
	}
}

void rapid(){
	if(rfpar.size()<100)
		rfpar.push_back(new rapidfire(myX/10.0f,-myZ/10.0f,theta,angle));
}

void reshape(int w, int h) {
  GW = w;
  GH = h;


  glMatrixMode(GL_PROJECTION);
  glLoadIdentity();

  if (h!=0){
      gluPerspective(80, w/h, .5, 24);
  }

  glMatrixMode(GL_MODELVIEW);
  glViewport(0, 0, w, h);
  
  glutPostRedisplay();
}



void drawGrid() {
  int rows = 50;
  int columns = 50;

  glBegin(GL_LINES);
    glColor3f(0.0, 0.0, 0.0);
    materials(Black);
    /* Horizontal lines. */
    for (int i=-rows; i<=rows; i++) {
      glVertex3f(columns, 0, i);
      glVertex3f(-columns, 0, i);
    }
    /* Vertical lines. */
    for (int i=-columns; i<=columns; i++) {
      glVertex3f(i, 0, -rows);
      glVertex3f(i, 0, rows);
    }
  glEnd();

}


void drawPlayer() {
  materials(Grey);
  glPushMatrix();
  
    glTranslatef(0.0, 0.25, 0.0);
    glTranslatef(0.0, 0.0, -0.5);
    gluCylinder(gluNewQuadric(), .05, .2, 1, 12, 36);
  glPopMatrix();
}

void drawFireball() {
	fbsrc->draw();
	for(int i=0;i<fbpar.size();i++){
		fbpar[i]->draw();
	}
}

void drawExplosion() {
	//exsrc->draw();
	for(int i=0;i<expar.size();i++){
		expar[i]->draw();
	}
}

void drawRapid() {
	for(int i=0;i<rfpar.size();i++){
		rfpar[i]->draw();
	}
}

void display() {

  
  glClear(GL_COLOR_BUFFER_BIT | GL_DEPTH_BUFFER_BIT);
 
  glMatrixMode(GL_MODELVIEW);
    
  glPushMatrix();
  //set up the camera
    gluLookAt(eyex + (myX/10.0), eyey, eyez - (myZ/10.0), LAx + (myX/10.0), LAy, LAz - (myZ/10.0), 0, 0, -1);
    glPushMatrix();

      glPushMatrix();
	  if (flag){
	  }
		glTranslatef((myX/10.0), 0, (-myZ/10.0));
        glRotatef(angle, 0, 1, 0);
        drawPlayer();
      glPopMatrix();

      glPushMatrix();
        glTranslatef(0.0, 0.01, 0.0);
        drawGrid();
		glTranslatef(1.0,0,1.0);
		glutSolidSphere(1.0,10,10);
      glPopMatrix();
	  if(fbtim>-1) drawFireball();
	  if(explo) drawExplosion();
	  drawRapid();
    glPopMatrix();
  glPopMatrix();
  
  glutSwapBuffers();
    
}



void mouse(int button, int state, int x, int y) {
  if (button == GLUT_RIGHT_BUTTON) {
    if (state == GLUT_DOWN) { 
		flag = true;
    }
	else {
		flag = false;
	}
  }
	if(button == GLUT_LEFT_BUTTON) {
		if (state == GLUT_DOWN) {
			if(rfire) { rapid(); }
			else if(fbtim<0) {spawnFireball(); fbtim = 0;}
		}
	}
}

void processMousePassiveMotion(int x, int y) {


	//float theta = 0;
	x -= GW/2;
	y -= GH/2;
	
	//        0  dir
	//        | /
	//        |/ 
	// 270----------90
	//        |
	//        |
	//       180
	
	if (y==0 && x<0) // handle div by zero case.
		theta = M_PI/2.0f;
	else if (y==0 && x>0) // handle div by zero case.
		theta = 3.0f*M_PI/2.0f;
	else if (y<0 && x<0)
		theta = atan((float)x/(float)y);
	else if (y>0 && x<=0)
		theta = atan((float)x/(float)y)+M_PI;
	else if (y<0 && x>=0)
		theta = atan((float)x/(float)y)+2*M_PI;
	else if (y>0 && x>0)
		theta = atan((float)x/(float)y)+M_PI;
		
	angle=theta*(180.0f / M_PI);
	
	
  glutPostRedisplay();

}

void processMouseActiveMotion(int x, int y) {
	
	//float theta = 0;
	x -= GW/2;
	y -= GH/2;
	
	//        0  dir
	//        | /
	//        |/ 
	// 270----------90
	//        |
	//        |
	//       180
	
	if (y==0 && x<0) // handle div by zero case.
		theta = M_PI/2.0f;
	else if (y==0 && x>0) // handle div by zero case.
		theta = 3.0f*M_PI/2.0f;
	else if (y<0 && x<0)
		theta = atan((float)x/(float)y);
	else if (y>0 && x<=0)
		theta = atan((float)x/(float)y)+M_PI;
	else if (y<0 && x>=0)
		theta = atan((float)x/(float)y)+2*M_PI;
	else if (y>0 && x>0)
		theta = atan((float)x/(float)y)+M_PI;
		
	angle=theta*(180.0f / M_PI);
	//myX += -sin(theta);
	//myZ += cos(theta);
	//if(rfire) { rapid(); }
  glutPostRedisplay();

}

void keyboard(unsigned char key, int x, int y ){
  switch( key ) {
    case 'q': case 'Q' :
      exit( EXIT_SUCCESS );
      break;
	case ' ':
		rfire = !rfire;
		break;
  }
}

void tick(int state) {

	if (flag){
		myX += -sin(theta);
		myZ += cos(theta);
	}
	if (fbtim>-1){
		fbtim++;
		fbsrc->move();
		for(int i=0;i<fbpar.size();i++){
			fbpar[i]->move();
			if(fbpar[i]->life<0.0f){
				fbpar[i] = new fireball_p(fbsrc);
			}
		}
	}
	if(fbtim>50||(fbtim>-1&&fbsrc->collide(1.0,1.0,1.0))){
		fbtim=-1;
		fbsrc->active = false;
		fbpar.clear();
		detonate(fbsrc);
		explo = true;
	}
	if (explo){
		exsrc->move();
		for(int i=expar.size()-1;i>-1;i--){
			expar[i]->move();
			if(expar[i]->life<0.0f){
				expar.erase(expar.begin()+i);
			}
		}
		if(expar.empty()){
			explo = false;
		}
	}
	for(int i=rfpar.size()-1;i>-1;i--){
		rfpar[i]->move();
		if(!rfpar[i]->boom&&rfpar[i]->collide(1.0,1.0,1.0)){
			rfpar[i]->boom = true;
			rfpar[i]->life = 0.0;
		}
		if(!rfpar[i]->active){
			rfpar.erase(rfpar.begin()+i);
		}
	}
	glutPostRedisplay();
	glutTimerFunc(30, &tick, 0);
}

int main( int argc, char** argv ) {
  

  //set up my window
  glutInit(&argc, argv);
  glutInitDisplayMode(GLUT_RGB | GLUT_DOUBLE | GLUT_DEPTH);
  //glutInitWindowSize(800, 600); 
  GW = 800;
  GH = 600;
  //glutInitWindowPosition(0, 0);
  //glutCreateWindow("Mesh display");
  glutGameModeString("800x600:32");
  if (glutGameModeGet(GLUT_GAME_MODE_POSSIBLE)){
	glutEnterGameMode();
  }
  else{
	  exit(1);
  }
  //glClearColor(0.0, 0.0, 0.0, 1.0);
  glClearColor(1.0, 1.0, 1.0, 1.0);

  myX = 0;
  myY = 0;
  myZ = 0;
  angle = 0;
  theta = 0;
  eyex = 0;
  eyey = 4.33;//4.33;
  eyez = 5;//5;
  LAx = 0;
  LAy = 0;
  LAz = 0;
  fbtim = -1;
  explo = false;


  
  //register glut callback functions
  glutDisplayFunc( display );
  glutReshapeFunc( reshape );
  glutMouseFunc(mouse);
  glutKeyboardFunc( keyboard );
  glutPassiveMotionFunc(processMousePassiveMotion);
  glutMotionFunc(processMouseActiveMotion);
  glutTimerFunc(30,&tick,0);
  glEnable(GL_DEPTH_TEST);

  init_lighting();
  glEnable(GL_LIGHTING);
	init_tex();
	init_particle();
  
  glutMainLoop();
}
