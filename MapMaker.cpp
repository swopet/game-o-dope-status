#ifndef MAP_MAKER
#define MAP_MAKER
#define DRAW_ALL true

#include "Tile.cpp"
const GLfloat cube_coords[] = {-1,1,1,1,1,1,1,1,-1,-1,1,-1, //TOP
                          1,1,-1,1,1,1,1,-1,1,1,-1,-1, //RIGHT
                          1,-1,1,-1,-1,1,-1,-1,-1,1,-1,-1, //BOTTOM
                          -1,1,1,-1,1,-1,-1,-1,-1,-1,-1,1, //LEFT
                          -1,1,-1,1,1,-1,1,-1,-1,-1,-1,-1, //FRONT
                          1,1,1,-1,1,1,-1,-1,1,1,-1,1}; //BACK
typedef std::map<int, Tile*> tile_map;

class MapMaker {
private:
    tile_map map;
    tile_map loaded;
    int center;
public:
    MapMaker(){
        for (int i = 0; i < 6; i++){
            GLfloat * temp_vertices = (GLfloat *)&cube_coords[i*12];
            GLfloat vertices[24];
            for (int j = 0; j < 4; j++){
                vertices[j*6] = temp_vertices[j*3];
                vertices[j*6+3] = temp_vertices[j*3];
                vertices[j*6+1] = temp_vertices[j*3+1];
                vertices[j*6+4] = temp_vertices[j*3+1];
                vertices[j*6+2] = temp_vertices[j*3+2];
                vertices[j*6+5] = temp_vertices[j*3+2];
            }
            map[i] = new Tile();
            map[i]->setID(i);
            map[i]->setVertices(vertices);
            map[i]->setType(LAND);
        }
        map[0]->setNeighbor(3,LEFT);
        map[0]->setNeighbor(1,RIGHT);
        map[0]->setNeighbor(5,UP);
        map[0]->setNeighbor(4,DOWN);
        map[1]->setNeighbor(4,LEFT);
        map[1]->setNeighbor(5,RIGHT);
        map[1]->setNeighbor(0,UP);
        map[1]->setNeighbor(2,DOWN);
        map[2]->setNeighbor(1,LEFT);
        map[2]->setNeighbor(3,RIGHT);
        map[2]->setNeighbor(5,UP);
        map[2]->setNeighbor(4,DOWN);
        map[3]->setNeighbor(5,LEFT);
        map[3]->setNeighbor(4,RIGHT);
        map[3]->setNeighbor(0,UP);
        map[3]->setNeighbor(2,DOWN);
        map[4]->setNeighbor(3,LEFT);
        map[4]->setNeighbor(1,RIGHT);
        map[4]->setNeighbor(0,UP);
        map[4]->setNeighbor(2,DOWN);
        map[5]->setNeighbor(1,LEFT);
        map[5]->setNeighbor(3,RIGHT);
        map[5]->setNeighbor(0,UP);
        map[5]->setNeighbor(2,DOWN);
        for (int i = 0; i < 4; i++){
            subdivide();
        }
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
            it->second->correct_and_normalize();
            it->second->printNeighbors();
        }
        
        center = 0;
        loaded[0] = map[0];
        load(DOWN);
    }
    void draw(){
    #if DRAW_ALL == true
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
    #else
        for (tile_map::iterator it = loaded.begin(); it != loaded.end(); ++it){
    #endif
            if (it->first == center){
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else {
                glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            it->second->draw();
        }
    }
    void load(int direction){
        center = loaded[center]->neighborAt(direction);
        loaded.clear();
        loaded[center] = map[center];
        for (int i = 0; i < 4; i++){
            loaded[loaded[center]->neighborAt(i)]=map[loaded[center]->neighborAt(i)];
        }
        std::map<int,bool> to_load;
        for (tile_map::iterator it = loaded.begin(); it != loaded.end(); ++it){
            for (int i = 0; i < 4; i++){
                try{
                    loaded.at(it->second->neighborAt(i))->getID();
                }
                catch (const std::out_of_range &err){
                    to_load[it->second->neighborAt(i)] = true;
                }
            }
        }
        for (std::map<int,bool>::iterator it = to_load.begin(); it!= to_load.end(); ++it){
            loaded[it->first] = map[it->first];
        }
    }
    void subdivide(){
        tile_map new_map;
        Tile ** sub_tiles;
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
            sub_tiles = it->second->subdivide(map);
            for (int i = 0; i < 4; i++){
                new_map[sub_tiles[i]->getID()] = sub_tiles[i];
            }
        }
        map = new_map;
    }
};


#endif