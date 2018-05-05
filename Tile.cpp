#ifndef TILE
#define TILE

static const GLuint quad_array[] = {0,1,2,3};

enum TileType {
    LAND,
    OCEAN,
    ICE
};

class Tile {
private:
    int id;
    TileType type;
    int neighbors[4]; //LEFT, RIGHT, UP, DOWN ids
    GLfloat vertices[24]; //it's a quad yo, (3 per vertex + 3 per vertex normal)*4 vertices = 24
    GLfloat avg_normal[3];
public:
    int getID(){
        return id;
    }
    TileType getType(){
        return type;
    }
    void setID(int new_id){
        id = new_id;
    }
    void setType(TileType new_type){
        type = new_type;
    }
    void setNeighbor(int neighbor, int which){
        neighbors[which] = neighbor;
    }
    int getNeighbor(int neighbor){
        for (int i = 0; i < 4; i++){
            if (neighbors[i] == neighbor){
                return i;
            }
        }
        return 4;
    }
    int neighborAt(int which){
        return neighbors[which];
    }
    void correct_and_normalize(GLfloat radius = 1.0){
        for (int i = 0; i < 4; i++){
            GLfloat norm_length = sqrt(pow(vertices[i*6],2)+pow(vertices[i*6+1],2)+pow(vertices[i*6+2],2));
            if (norm_length==0){
                return;
            }
            GLfloat vert_length = sqrt(pow(vertices[i*6+3],2)+pow(vertices[i*6+4],2)+pow(vertices[i*6+5],2));
            vertices[i*6] = vertices[i*6]/norm_length;
            vertices[i*6+1] = vertices[i*6+1]/norm_length;
            vertices[i*6+2] = vertices[i*6+2]/norm_length;
            vertices[i*6+3] = radius*vertices[i*6+3]/vert_length;
            vertices[i*6+4] = radius*vertices[i*6+4]/vert_length;
            vertices[i*6+5] = radius*vertices[i*6+5]/vert_length;
        }
    }
    void setVertices(GLfloat * new_vertices){
        for (int i = 0; i < 24; i++){
            vertices[i] = new_vertices[i];
        }
        for (int i = 0; i < 3; i++){
            avg_normal[i] = (vertices[i]+vertices[6+i]+vertices[12+i]+vertices[18+i])/4.0;
        }
    }
    void draw(){
        glInterleavedArrays(GL_N3F_V3F,0,vertices);
        switch(type){
            case LAND:
                glColor3f(0.1,0.7,0.1);
            break;
            case OCEAN:
                glColor3f(0.1,0.1,0.7);
            break;
            case ICE:
                glColor3f(0.9,0.9,0.9);
            break;
        }
        glDrawElements(GL_QUADS,4,GL_UNSIGNED_INT,quad_array);
    }
    void swap_LR(){
        int right = neighbors[RIGHT];
        neighbors[RIGHT] = neighbors[LEFT];
        neighbors[LEFT] = right;
    }
    sf::Vector3f getNormalVector3f(){
        return sf::Vector3f(avg_normal[0],avg_normal[1],avg_normal[2]);
    }
    Tile ** subdivide(std::map<int, Tile*> tile_map){ //assume that this will be called on every other tile at the same level
        Tile ** new_tiles = (Tile **)malloc(4*sizeof(Tile *));
        for (int i = 0; i < 4; i++){
            new_tiles[i] = new Tile();
            new_tiles[i]->setID(id*4+i);
        }
        switch(tile_map[neighbors[LEFT]]->getNeighbor(id)){
            case RIGHT:
                new_tiles[0]->setNeighbor(neighbors[LEFT]*4+1,LEFT);
                new_tiles[3]->setNeighbor(neighbors[LEFT]*4+2,LEFT);
            break;
            case UP:
                new_tiles[0]->setNeighbor(neighbors[LEFT]*4+0,LEFT);
                new_tiles[3]->setNeighbor(neighbors[LEFT]*4+1,LEFT);
            break;
            case LEFT:
                new_tiles[0]->setNeighbor(neighbors[LEFT]*4+3,LEFT);
                new_tiles[3]->setNeighbor(neighbors[LEFT]*4+0,LEFT);
            break;
            case DOWN:
                new_tiles[0]->setNeighbor(neighbors[LEFT]*4+2,LEFT);
                new_tiles[3]->setNeighbor(neighbors[LEFT]*4+3,LEFT);
            break;
        }
        switch(tile_map[neighbors[UP]]->getNeighbor(id)){
            case RIGHT:
                new_tiles[0]->setNeighbor(neighbors[UP]*4+2,UP);
                new_tiles[1]->setNeighbor(neighbors[UP]*4+1,UP);
            break;
            case UP:
                new_tiles[0]->setNeighbor(neighbors[UP]*4+1,UP);
                new_tiles[1]->setNeighbor(neighbors[UP]*4+0,UP);
            break;
            case LEFT:
                new_tiles[0]->setNeighbor(neighbors[UP]*4+0,UP);
                new_tiles[1]->setNeighbor(neighbors[UP]*4+3,UP);
            break;
            case DOWN:
                new_tiles[0]->setNeighbor(neighbors[UP]*4+3,UP);
                new_tiles[1]->setNeighbor(neighbors[UP]*4+2,UP);
            break;
        }
        switch(tile_map[neighbors[DOWN]]->getNeighbor(id)){
            case RIGHT:
                new_tiles[3]->setNeighbor(neighbors[DOWN]*4+1,DOWN);
                new_tiles[2]->setNeighbor(neighbors[DOWN]*4+2,DOWN);
            break;
            case UP:
                new_tiles[3]->setNeighbor(neighbors[DOWN]*4+0,DOWN);
                new_tiles[2]->setNeighbor(neighbors[DOWN]*4+1,DOWN);
            break;
            case LEFT:
                new_tiles[3]->setNeighbor(neighbors[DOWN]*4+3,DOWN);
                new_tiles[2]->setNeighbor(neighbors[DOWN]*4+0,DOWN);
            break;
            case DOWN:
                new_tiles[3]->setNeighbor(neighbors[DOWN]*4+2,DOWN);
                new_tiles[2]->setNeighbor(neighbors[DOWN]*4+3,DOWN);
            break;
        }
        switch(tile_map[neighbors[RIGHT]]->getNeighbor(id)){
            case RIGHT:
                new_tiles[1]->setNeighbor(neighbors[RIGHT]*4+2,RIGHT);
                new_tiles[2]->setNeighbor(neighbors[RIGHT]*4+1,RIGHT);
            break;
            case UP:
                new_tiles[1]->setNeighbor(neighbors[RIGHT]*4+1,RIGHT);
                new_tiles[2]->setNeighbor(neighbors[RIGHT]*4+0,RIGHT);
            break;
            case LEFT:
                new_tiles[1]->setNeighbor(neighbors[RIGHT]*4+0,RIGHT);
                new_tiles[2]->setNeighbor(neighbors[RIGHT]*4+3,RIGHT);
            break;
            case DOWN:
                new_tiles[1]->setNeighbor(neighbors[RIGHT]*4+3,RIGHT);
                new_tiles[2]->setNeighbor(neighbors[RIGHT]*4+2,RIGHT);
            break;
        }
        GLfloat new_vertices[96];
        for (int i = 0; i < 4; i++){
            for (int j = 0; j < 24; j++){
                if (vertices[i*6+j%6] == vertices[j]) new_vertices[i*24+j] = vertices[j];
                
                else new_vertices[i*24+j] = (vertices[i*6+j%6]+vertices[j])/2.0;
            }
        }
        for (int i = 0; i < 4; i++){
            
            new_tiles[i]->setVertices(&new_vertices[i*24]);
        }
        new_tiles[0]->setNeighbor(id*4+1,RIGHT);
        new_tiles[0]->setNeighbor(id*4+3,DOWN);
        new_tiles[1]->setNeighbor(id*4+0,LEFT);
        new_tiles[1]->setNeighbor(id*4+2,DOWN);
        new_tiles[2]->setNeighbor(id*4+1,UP);
        new_tiles[2]->setNeighbor(id*4+3,LEFT);
        new_tiles[3]->setNeighbor(id*4+0,UP);
        new_tiles[3]->setNeighbor(id*4+2,RIGHT);
        return new_tiles;
    }
    void printNeighbors(){
        printf("Tile %d:\nLEFT: %d\nUP: %d\nRIGHT: %d\nDOWN: %d\n",id,neighbors[LEFT],neighbors[UP],neighbors[RIGHT],neighbors[DOWN]);
    }
};

#endif