#include <SFML/Graphics.hpp>
#include <SFML/OpenGL.hpp>

#include <GL/glu.h>
#include <cstring>
#include <math.h>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <map>
#include <sstream>
#include <cstring>

//#define SCREENCAP
#define ROTATION_SPEED 0.05
#define WIREFRAME false
#define ORTHO_RADIUS 1.0
#define TILE_WIDTH 1.0
sf::RenderWindow * window;

sf::Vector2u screen_size;

enum TileType {
	OCEAN = 0,
	LAND = 1
};

class Tile {
private:
	TileType type;
	int id;
public:
	void set_id(int new_id){
		id = new_id;
	}
	void set_type(TileType new_type){
		type = new_type;
	}
	void draw(){
		switch (type){
		case OCEAN:
			glColor3f(0.1,0.1,0.7);
		break;
		case LAND:
			glColor3f(0.1,0.7,0.1);
		break;
		}
		glBegin(GL_QUADS);
			glVertex2f(-TILE_WIDTH/2.0,-TILE_WIDTH/2.0);
			glVertex2f(-TILE_WIDTH/2.0,TILE_WIDTH/2.0);
			glVertex2f(TILE_WIDTH/2.0,TILE_WIDTH/2.0);
			glVertex2f(TILE_WIDTH/2.0,-TILE_WIDTH/2.0);
		glEnd();
	}
};

class QuadMap {
private:
	Tile *** tiles;
	int width;
	int height;
public:
	QuadMap(int new_width, int new_height, int div){
		width = new_width*div*div;
		height = new_height*div*div;
		tiles = (Tile ***)malloc(width*sizeof(Tile**));
		int id = 0;
		for (int x = 0; x < width; x++){
			tiles[x] = (Tile **)malloc(height*sizeof(Tile*));
			for (int y = 0; y < height; y++){
				tiles[x][y] = new Tile();
				tiles[x][y]->set_id(id);
				id++;
			}
		}
		TileType land_map[new_width][new_height];
		for (int x = 0; x < new_width; x++){
			for (int y = 0; y < new_height; y++){
				if (y==0 || y == new_height-1){
					land_map[x][y] = OCEAN;
					continue;
				}
				int land_thresh = 5+40*sin((float)y*M_PI/(float)(new_height));
				int choice = rand()%100;
				if (choice < land_thresh){
					land_map[x][y] = LAND;
				}
				else {
					land_map[x][y] = OCEAN;
				}
			}
		}
		TileType land_map_2[new_width*div][new_height*div];
		int choice;
		int coord_x;
		int coord_y;
		for (int x = 0; x < new_width*div; x++){
			for (int y = 0; y < new_height*div; y++){
				choice = rand()%100;
				if (x%div == 0){ //this is on the left border of a divxdiv
					if (y%div == 0){ //this is on the top border of a divxdiv --> this is a top left corner
						if (choice < 20){ //pick (-1,-1)
							coord_x = ((x/div)+(new_width-1))%new_width;
							coord_y = ((y/div)+(new_height-1))%new_height;
						}
						else if (choice < 40){ //pick (-1,0)
							coord_x = ((x/div)+(new_width-1))%new_width;
							coord_y = y/div;
						}
						else if (choice < 60){ //pick (0,-1)
							coord_x = x/div;
							coord_y = ((y/div)+(new_height-1))%new_height;
						}
						else { //pick (0,0)
							coord_x = x/div;
							coord_y = y/div;
						}
					}
					else if (y%div == div-1) { //this is on the right border of a divxdiv --> this is a bottom rightt corner
						if (choice < 20){ //pick (-1,1)
							coord_x = ((x/div)+(new_width-1))%new_width;
							coord_y = ((y/div)+1)%new_height;
						}
						else if (choice < 40){ //pick (-1,0)
							coord_x = ((x/div)+(new_width-1))%new_width;
							coord_y = y/div;
						}
						else if (choice < 60){ //pick (0,1)
							coord_x = x/div;
							coord_y = ((y/div)+1)%new_height;
						}
						else { //pick (0,0)
							coord_x = x/div;
							coord_y = y/div;
						}
					}
					else {
						coord_y = y/div;
						if (choice < 40){ //this is a non-corner left border of a divxdiv
							coord_x = ((x/div)+(new_width-1))%new_width;
						}
						else{
							coord_x = x/div;
						}
					}
				}
				else if (x%div == div-1){
					if (y%div == 0){ //this is on the top border of a divxdiv --> this is a top right corner
						if (choice < 20){ //pick (1,-1)
							coord_x = ((x/div)+1)%new_width;
							coord_y = ((y/div)+(new_height-1))%new_height;
						}
						else if (choice < 40){ //pick (1,0)
							coord_x = ((x/div)+1)%new_width;
							coord_y = y/div;
						}
						else if (choice < 60){ //pick (0,-1)
							coord_x = x/div;
							coord_y = ((y/div)+(new_height-1))%new_height;
						}
						else { //pick (0,0)
							coord_x = x/div;
							coord_y = y/div;
						}
					}
					else if (y%div == div-1) { //this is on the right border of a divxdiv --> this is a bottom right corner
						if (choice < 20){ //pick (1,1)
							coord_x = ((x/div)+1)%new_width;
							coord_y = ((y/div)+1)%new_height;
						}
						else if (choice < 40){ //pick (1,0)
							coord_x = ((x/div)+1)%new_width;
							coord_y = y/div;
						}
						else if (choice < 60){ //pick (0,1)
							coord_x = x/div;
							coord_y = ((y/div)+1)%new_height;
						}
						else { //pick (0,0)
							coord_x = x/div;
							coord_y = y/div;
						}
					}
					else {
						coord_y = y/div;
						if (choice < 40){ //this is a non-corner right border of a 4x4
							coord_x = ((x/div)+1)%new_width;
						}
						else{
							coord_x = x/div;
						}
					}
				}
				else {
					coord_x = x/div;
					if (y%div == 0){
						if (choice < 40){
							coord_y = ((y/div)+(new_height-1))%new_height;
						}
						else {
							coord_y = y/div;
						}
					}
					else if (y%div == div-1){
						if (choice < 40){
							coord_y = (y/div+1)%new_height;
						}
						else {
							coord_y = y/div;
						}
					}
					else {
						coord_y = y/div;
					}
				}
				land_map_2[x][y]=land_map[coord_x][coord_y];
			}
			for (int x = 0; x < new_width*div*div; x++){
				for (int y = 0; y < new_height*div*div; y++){
					choice = rand()%100;
					if (x%div == 0){ //this is on the left border of a divxdiv
						if (y%div == 0){ //this is on the top border of a divxdiv --> this is a top left corner
							if (choice < 20){ //pick (-1,-1)
								coord_x = ((x/div)+(new_width*div-1))%(new_width*div);
								coord_y = ((y/div)+(new_height*div-1))%(new_height*div);
							}
							else if (choice < 40){ //pick (-1,0)
								coord_x = ((x/div)+(new_width*div-1))%(new_width*div);
								coord_y = y/div;
							}
							else if (choice < 60){ //pick (0,-1)
								coord_x = x/div;
								coord_y = ((y/div)+(new_height*div-1))%(new_height*div);
							}
							else { //pick (0,0)
								coord_x = x/div;
								coord_y = y/div;
							}
						}
						else if (y%div == div-1) { //this is on the right border of a divxdiv --> this is a bottom rightt corner
							if (choice < 20){ //pick (-1,1)
								coord_x = ((x/div)+(new_width*div-1))%(new_width*div);
								coord_y = ((y/div)+1)%(new_height*div);
							}
							else if (choice < 40){ //pick (-1,0)
								coord_x = ((x/div)+(new_width*div-1))%(new_width*div);
								coord_y = y/div;
							}
							else if (choice < 60){ //pick (0,1)
								coord_x = x/div;
								coord_y = ((y/div)+1)%(new_height*div);
							}
							else { //pick (0,0)
								coord_x = x/div;
								coord_y = y/div;
							}
						}
						else {
							coord_y = y/div;
							if (choice < 40){ //this is a non-corner left border of a divxdiv
								coord_x = ((x/div)+(new_width*div-1))%(new_width*div);
							}
							else{
								coord_x = x/div;
							}
						}
					}
					else if (x%div == div-1){
						if (y%div == 0){ //this is on the top border of a divxdiv --> this is a top right corner
							if (choice < 20){ //pick (1,-1)
								coord_x = ((x/div)+1)%(new_width*div);
								coord_y = ((y/div)+(new_height*div-1))%(new_height*div);
							}
							else if (choice < 40){ //pick (1,0)
								coord_x = ((x/div)+1)%(new_width*div);
								coord_y = y/div;
							}
							else if (choice < 60){ //pick (0,-1)
								coord_x = x/div;
								coord_y = ((y/div)+(new_height*div-1))%(new_height*div);
							}
							else { //pick (0,0)
								coord_x = x/div;
								coord_y = y/div;
							}
						}
						else if (y%div == div-1) { //this is on the right border of a divxdiv --> this is a bottom right corner
							if (choice < 20){ //pick (1,1)
								coord_x = ((x/div)+1)%(new_width*div);
								coord_y = ((y/div)+1)%(new_height*div);
							}
							else if (choice < 40){ //pick (1,0)
								coord_x = ((x/div)+1)%(new_width*div);
								coord_y = y/div;
							}
							else if (choice < 60){ //pick (0,1)
								coord_x = x/div;
								coord_y = ((y/div)+1)%(new_height*div);
							}
							else { //pick (0,0)
								coord_x = x/div;
								coord_y = y/div;
							}
						}
						else {
							coord_y = y/div;
							if (choice < 40){ //this is a non-corner right border of a divxdiv
								coord_x = ((x/div)+1)%(new_width*div);
							}
							else{
								coord_x = x/div;
							}
						}
					}
					else {
						coord_x = x/div;
						if (y%div == 0){
							if (choice < 40){
								coord_y = ((y/div)+(new_height*div-1))%(new_height*div);
							}
							else {
								coord_y = y/div;
							}
						}
						else if (y%div == div-1){
							if (choice < 40){
								coord_y = (y/div+1)%(new_height*div);
							}
							else {
								coord_y = y/div;
							}
						}
						else {
							coord_y = y/div;
						}
					}
					tiles[x][y]->set_type(land_map_2[coord_x][coord_y]);
				}
			}
		}
	}
	void draw(int off_ctr, float marg_off){
		for (int x = 0; x < width; x++){
			for (int y = 0; y < height; y++){
				glPushMatrix();
					glTranslatef((float)(-width/2+(x+off_ctr)%width + marg_off)*TILE_WIDTH,(float)(-height/2+y)*TILE_WIDTH,0.0);
					tiles[x][y]->draw();
				glPopMatrix();
			}
		}
	}
};

QuadMap *map;
int ctr = 0;
float marginal_ctr = 0.0;
void update(){
	marginal_ctr += 0.1;
	if (marginal_ctr >= 1.0){
		marginal_ctr = 0.0;
		ctr++;
	}
}

void display(){
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
	glScalef(0.0125,0.0125,0.0125);
	map->draw(ctr,marginal_ctr);
	glPopMatrix();
	window->display();
}

void init(){
	screen_size = window->getSize();
	glEnable(GL_DEPTH_TEST);
	glClearColor(0.0,0.0,0.0,1.0);
	glViewport(0.0,0.0,screen_size.x,screen_size.y);
	glShadeModel (GL_SMOOTH);

	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float light_position[] = {1.0,1.0,1.0,0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_PROJECTION);
	glOrtho(-ORTHO_RADIUS*(float)screen_size.x/(float)screen_size.y,ORTHO_RADIUS*(float)screen_size.x/(float)screen_size.y,-ORTHO_RADIUS,ORTHO_RADIUS,-100.0,100.0);
	glMatrixMode(GL_MODELVIEW);
	map = new QuadMap(32,16,3);
}

int main(int argc, char **argv) {
	sf::ContextSettings settings;
	settings.depthBits = 24;
	settings.stencilBits = 0;
	settings.antialiasingLevel = 0;
	settings.majorVersion = 3;
	settings.minorVersion = 0;
	window = new sf::RenderWindow(sf::VideoMode(800,400),"One Mappy Boi",sf::Style::Default, settings);
	//printf("depth bits: %d\n",window->getSettings().depthBits);
    window->setVerticalSyncEnabled(true);
    srand(time(NULL));
    // activate the window
    window->setActive(true);
    init();

    bool running = true;
    while (running){
        // handle events
        sf::Event event;
        while (window->pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
            {
            	running = false;
                exit(0);
            }
            else if (event.type == sf::Event::Resized)
            {
                // adjust the viewport when the window is resized
                screen_size.x = event.size.width;
                screen_size.y = event.size.height;
            }
        }
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){

        }
        update();
        display();

    }

	return 1;
}
