#ifndef MAP_MAKER
#define MAP_MAKER
#define DRAW_ALL false

#define WORLD_RADIUS 1000.0 //in kilometers

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
        for (int i = 0; i < 3; i++){
            map = subdivide();
        }
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
            float y = it->second->getNormalVector3f().y;
            if (y == 1.0 || y == -1.0){
                it->second->setType(ICE);
            }
            else {
                int thresh = 10+(int) 60.0*cos(y*M_PI);
                int choice = rand()%100;
                if (choice < thresh){
                    it->second->setType(LAND);
                }
                else{
                    it->second->setType(OCEAN);
                }
            }
            
        }
        for (int i = 0; i < 1; i++){
            printf("randomly generating land (step %d)...\n",i);
            tile_map old_map = map;
            map = subdivide();
            map = subdivide();
            int count = 0;
            for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
                count++;
                if (count%100 == 0){
                    printf("Got through %d tiles\n",count);
                }
                int choice = rand()%100;
                if (choice < 20){ //CHOOSE LEFT
                    it->second->setType(old_map[it->second->neighborAt(LEFT)/16]->getType());
                }
                else if (choice < 40){ //CHOOSE UP
                    it->second->setType(old_map[it->second->neighborAt(UP)/16]->getType());
                }
                else if (choice < 60){ //CHOOSE RIGHT
                    it->second->setType(old_map[it->second->neighborAt(RIGHT)/16]->getType());
                }
                else if (choice < 80){ //CHOOSE DOWN
                    it->second->setType(old_map[it->second->neighborAt(DOWN)/16]->getType());
                }
                else { //CHOOSE SELF
                    it->second->setType(old_map[it->first/16]->getType());
                }
            }
        }
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
            it->second->correct_and_normalize(WORLD_RADIUS);
            it->second->swap_LR();
        }
        
        center = 0;
        loaded[0] = map[0];
        load(DOWN);
    }
    void draw(){
        glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
        glPushMatrix();
        glTranslatef(0.0,-1.6,-1.0);
        map[center]->draw(true);
        glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
        map[center]->transform();
        for (tile_map::iterator it = loaded.begin(); it != loaded.end(); ++it){
            if (it->first != center){
                it->second->draw();
            }
        }
        glPopMatrix();
        
        /*sf::Vector3f target(0.0,1.0,0.0);
        sf::Vector3f center_vec = loaded[center]->getNormalVector3f();
        sf::Vector3f rot_axis = cross_product(center_vec,target);
        double angle = (acos((center_vec.y)/(sqrt(pow(center_vec.x,2.0)+pow(center_vec.y,2.0)+pow(center_vec.z,2.0)))))*180.0/M_PI;
        glPushMatrix();
        
        glTranslatef(0.0,-12.0,-10.0);
        //glRotatef(10.0,1.0,0.0,0.0);
        glScalef(10.0,10.0,10.0);
        glRotatef(angle,rot_axis.x,rot_axis.y,rot_axis.z);
        
    #if DRAW_ALL == true
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
    #else
        for (tile_map::iterator it = loaded.begin(); it != loaded.end(); ++it){
    #endif
            if (it->first == center){
                glPolygonMode(GL_FRONT_AND_BACK, GL_FILL);
            }
            else {
                //glPolygonMode(GL_FRONT_AND_BACK, GL_LINE);
            }
            it->second->draw();
        }
        glPopMatrix();
        */
    }
    void load(int direction){
        center = loaded[center]->neighborAt(direction);
        loaded.clear();
        loaded[center] = map[center];
        for (int i = 0; i < 6; i++){
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
            
    }
    tile_map subdivide(){
        printf("subdividing map of size %d\n",map.size());
        tile_map new_map;
        Tile ** sub_tiles;
        int count = 0;
        for (tile_map::iterator it = map.begin(); it != map.end(); ++it){
            count++;
            if (count%100 == 0){
                printf("subdivided %d tiles\n",count);
            }
            sub_tiles = it->second->subdivide(map);
            for (int i = 0; i < 4; i++){
                new_map[sub_tiles[i]->getID()] = sub_tiles[i];
            }
        }
        return new_map;
    }
};


#endif