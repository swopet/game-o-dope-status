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

//ERRORS
#define NO_SOLUTION_ERROR -1
sf::RenderWindow * window;

sf::Vector2u screen_size;

sf::Vector3f cross_product(sf::Vector3f v1, sf::Vector3f v2){
	sf::Vector3f cross_vec;
	cross_vec.x = v1.y*v2.z - v2.y*v1.z;
	cross_vec.y = v1.z*v2.x - v2.z*v1.x;
	cross_vec.z = v1.x*v2.y - v2.x*v1.y;
	float length = sqrt(cross_vec.x*cross_vec.x+cross_vec.y*cross_vec.y+cross_vec.z*cross_vec.z);
	cross_vec = cross_vec/length;
	return cross_vec;
}

#include "MapMaker.cpp"

MapMaker * map_maker;

void update(){
}

void display(){
#if WIREFRAME == true
	glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
#endif
	glClear(GL_COLOR_BUFFER_BIT|GL_DEPTH_BUFFER_BIT);
	glPushMatrix();
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
    glFrontFace(GL_CW);
	glEnable(GL_LIGHTING);
	glEnable(GL_LIGHT0);
	float light_position[] = {1.0,1.0,1.0,0.0};
	glLightfv(GL_LIGHT0,GL_POSITION,light_position);
	glEnable(GL_NORMALIZE);
	glEnable(GL_COLOR_MATERIAL);
	glMatrixMode(GL_PROJECTION);
    
	//glOrtho(-ORTHO_RADIUS*(float)screen_size.x/(float)screen_size.y,ORTHO_RADIUS*(float)screen_size.x/(float)screen_size.y,-ORTHO_RADIUS,ORTHO_RADIUS,-100.0,100.0);
    gluPerspective(45.0,(float)screen_size.x/(float)screen_size.y,0.001,1000.0);
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
	window = new sf::RenderWindow(sf::VideoMode(960,600),"One Mappy Boi",sf::Style::Default, settings);
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
                glViewport(0.0,0.0,screen_size.x,screen_size.y);
            }
        }
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Right)){
        	if (!right_pressed){
        		map_maker->load(RIGHT);
                right_pressed = 4;
        	}
            else right_pressed--;
        }
		else {
			right_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Left)){
        	if (!left_pressed){
        		map_maker->load(LEFT);
                left_pressed = 4;
        	}
            else left_pressed--;
        }
		else {
			left_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Up)){
        	if (!up_pressed){
        		map_maker->load(UP);
                up_pressed = 4;
        	}
            else up_pressed--;
        }
		else {
			up_pressed = 0;
		}
        if (sf::Keyboard::isKeyPressed(sf::Keyboard::Down)){
        	if (!down_pressed){
        		map_maker->load(DOWN);
                down_pressed = 4;
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
