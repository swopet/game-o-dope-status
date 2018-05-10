# game-o-dope-status

run make_and_run.bat to make and run the project in its current form. Make sure you have the appropriate libraries (SFML, OpenGL, etc.)

current status:

generates a world composed of 6\*4^7 = 98304 almost-square tiles. does simple self-similar selection of land, ocean, and ice tiles. generates two flat triangles for each tile, which will represent the xz-plane when more complicated terrain is generated. Camera is 1.5 meters above the ground at the "origin" or the current triangle.

Next changes:

change arrow key movement from 'tile-based' to 'camera-based': to do this:
  
  create a 'Player' class with simple movement functionality with the following:
  
  private:
    
    a position struct composed of:
      
      current tile ID
      
      current tile triangle (triangle_up_right or triangle_down_left)
      position (xyz) relative to the origin of the current tile triangle
    
    a current direction indicating which direction the player is looking; rotate the perspective matrix appropriately
  
  public:
    
    keyboard input handler that moves in the direction of the camera on arrow presses. On reaching a border of the current triangle, the world-loader (to-be-implemented) must update the player's position struct accordingly. This will be tricky
    
    mouse input handler that rotates the direction the player is looking. This will be simple. Clamp down movement to (0,-1,0) direction, upward movement to (0,1,0) direction, but allow free rotation around the y axis.
