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
#include <iostream>
#include <fstream>
#include <cstring>
#define WIREFRAME false
#define ORTHO_RADIUS 1.0
#define TILE_WIDTH 0.02
#define LEFT 0
#define UP 1
#define RIGHT 2
#define DOWN 3
sf::RenderWindow * window;

sf::Vector2u screen_size;

#include "MapMaker.cpp"

MapMaker * map_maker;

float ctr = 30.0;
void update(){
	//ctr += 1.0;
}

void display(){
#if WIREFRAME == true
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
    glScalef(0.5,0.5,0.5);
    glRotatef(ctr,1.0,1.0,1.0);
    map_maker->draw();
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
    map_maker = new MapMaker();
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
    int up_pressed = 0;
    int down_pressed = 0;
    int right_pressed = 0;
    int left_pressed = 0;
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
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        	if (!right_pressed){
        		map_maker->load(RIGHT);
                right_pressed = 10;
        	}
            else right_pressed--;
        }
		else {
			right_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        	if (!left_pressed){
        		map_maker->load(LEFT);
                left_pressed = 10;
        	}
            else left_pressed--;
        }
		else {
			left_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        	if (!up_pressed){
        		map_maker->load(UP);
                up_pressed = 10;
        	}
            else up_pressed--;
        }
		else {
			up_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        	if (!down_pressed){
        		map_maker->load(DOWN);
                down_pressed = 10;
        	}
            else down_pressed--;
        }
		else {
			down_pressed = 0;
		}
        if (sf::Mouse::isButtonPressed(sf::Mouse::Left)){

        }
        update();
        display();

    }

	return 1;
}
