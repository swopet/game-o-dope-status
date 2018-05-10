#ifndef TILE
#define TILE

static const GLuint triangles_array[] = {0,1,2,2,3,0};

enum TileType {
    LAND,
    OCEAN,
    ICE
};

struct flat_triangle {
    GLfloat vertices[18];
    GLfloat transform_matrix[16];
};

void multiply_vector(GLfloat * vec, GLfloat * matrix){
	sf::Vector3f buffer;
	buffer.x = vec[0]*matrix[0] + vec[1]*matrix[4] + vec[2]*matrix[8] + 1*matrix[12];
	buffer.y = vec[0]*matrix[1] + vec[1]*matrix[5] + vec[2]*matrix[9] + 1*matrix[13];
	buffer.z = vec[0]*matrix[2] + vec[1]*matrix[6] + vec[2]*matrix[10] + 1*matrix[14];
	vec[0]=buffer.x;
	vec[1]=buffer.y;
	vec[2]=buffer.z;
}

sf::Vector2f solve_triangle_translation(sf::Vector2f right_corner, sf::Vector2f top_corner, sf::Vector2f position){
    double a = right_corner.x, b = top_corner.x, c = position.x, d = right_corner.y, e = top_corner.y, f = position.y;
    sf::Vector2f return_vec;
    do {
        if (a == 0){
            if (b != 0)
                return sf::Vector2f((f-e*c/b)/d,c/b);
            else
                break;
        }
        if (b == 0){
            if (a != 0)
                return sf::Vector2f(c/a,(f-d*c/a)/e);
            else
                break;
        }
        if (d == 0){
            if (e != 0)
                return sf::Vector2f((c-b*f/e)/a,f/e);
            else
                break;
        }
        if (e == 0){
            if (d != 0)
                return sf::Vector2f(f/d,(c-a*f/d)/b);
            else
                break;
        }
        double ratio = -d/a;
        d = 0;
        e = e + ratio * b;
        f = f + ratio * c;
    }while (true);
    throw NO_SOLUTION_ERROR;
}



class Tile {
private:
    int id;
    TileType type;
    int neighbors[4]; //LEFT, RIGHT, UP, DOWN ids
    GLfloat vertices[24]; //it's a quad yo, (3 per vertex + 3 per vertex normal)*4 vertices = 24
    GLfloat avg_normal[3];
    flat_triangle triangle_up_right;
    flat_triangle triangle_down_left;
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
        GLfloat current_matrix[16];
        GLfloat origin_1[3] = {vertices[9],vertices[10],vertices[11]};
        GLfloat x_vec_1[3] = {vertices[3],vertices[4],vertices[5]};
        GLfloat y_vec_1[3] = {vertices[15],vertices[16],vertices[17]};
        printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)->",origin_1[0],origin_1[1],origin_1[2],x_vec_1[0],x_vec_1[1],x_vec_1[2],y_vec_1[0],y_vec_1[1],y_vec_1[2]);
        glPushMatrix();
        glLoadIdentity();
        
        glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
        //origin_1[0] = 0.0; origin_1[1] = 0.0; origin_1[2] = 0.0;
        //multiply_vector(x_vec_1,current_matrix);
        //multiply_vector(y_vec_1,current_matrix);
        sf::Vector3f cross = cross_product(sf::Vector3f(y_vec_1[0]-origin_1[0],y_vec_1[1]-origin_1[1],y_vec_1[2]-origin_1[2]),sf::Vector3f(x_vec_1[0]-origin_1[0],x_vec_1[1]-origin_1[1],x_vec_1[2]-origin_1[2]));
        sf::Vector3f cross_2 = cross_product(sf::Vector3f(0,-1.0,0.0),cross);
        double angle = (acos((cross.y)/(sqrt(pow(cross.x,2.0)+pow(cross.y,2.0)+pow(cross.z,2.0)))))*180.0/M_PI;
        //glLoadIdentity();
        glRotatef(angle,cross_2.x,cross_2.y,cross_2.z);
        glTranslatef(-origin_1[0],-origin_1[1],-origin_1[2]);
        glGetFloatv(GL_MODELVIEW_MATRIX, current_matrix);
        multiply_vector(origin_1,current_matrix);
        multiply_vector(x_vec_1,current_matrix);
        multiply_vector(y_vec_1,current_matrix);
        x_vec_1[1] = 0.0;
        y_vec_1[1] = 0.0;
        printf("(%f,%f,%f),(%f,%f,%f),(%f,%f,%f)\n",origin_1[0],origin_1[1],origin_1[2],x_vec_1[0],x_vec_1[1],x_vec_1[2],y_vec_1[0],y_vec_1[1],y_vec_1[2]);
        glPopMatrix();
        triangle_up_right.vertices[0] = 0.0;
        triangle_up_right.vertices[1] = 1.0;
        triangle_up_right.vertices[2] = 0.0;
        triangle_up_right.vertices[3] = x_vec_1[0];
        triangle_up_right.vertices[4] = x_vec_1[1];
        triangle_up_right.vertices[5] = x_vec_1[2];
        triangle_up_right.vertices[6] = 0.0;
        triangle_up_right.vertices[7] = 1.0;
        triangle_up_right.vertices[8] = 0.0;
        triangle_up_right.vertices[9] = origin_1[0];
        triangle_up_right.vertices[10] = origin_1[1];
        triangle_up_right.vertices[11] = origin_1[2];
        triangle_up_right.vertices[12] = 0.0;
        triangle_up_right.vertices[13] = 1.0;
        triangle_up_right.vertices[14] = 0.0;
        triangle_up_right.vertices[15] = y_vec_1[0];
        triangle_up_right.vertices[16] = y_vec_1[1];
        triangle_up_right.vertices[17] = y_vec_1[2];
        for (int i = 0; i < 16; i++)
            triangle_up_right.transform_matrix[i] = current_matrix[i];
        GLfloat origin_2[3] = {vertices[21],vertices[22],vertices[23]};
        GLfloat x_vec_2[3] = {vertices[15],vertices[16],vertices[17]};
        GLfloat y_vec_2[3] = {vertices[3],vertices[4],vertices[5]};
    }
    void setVertices(GLfloat * new_vertices){
        for (int i = 0; i < 24; i++){
            vertices[i] = new_vertices[i];
        }
        for (int i = 0; i < 3; i++){
            avg_normal[i] = (vertices[i]+vertices[6+i]+vertices[12+i]+vertices[18+i])/4.0;
        }
    }
    void draw(bool center = false){
        if (!center) glInterleavedArrays(GL_N3F_V3F,0,vertices);
        else glInterleavedArrays(GL_N3F_V3F,0,triangle_up_right.vertices);
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
        if (!center) glDrawElements(GL_TRIANGLES,6,GL_UNSIGNED_INT,triangles_array);
        else glDrawElements(GL_TRIANGLES,3,GL_UNSIGNED_INT,triangles_array);
    }
    void transform(){
        glMultMatrixf(triangle_up_right.transform_matrix);
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