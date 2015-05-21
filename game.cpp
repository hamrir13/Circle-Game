/* Name: Bobby Hamrick
   Date: 4.14.2015
   Class: CSC-3360

   This code randomly draws circles outside of the visible area. Those circles then
   float onto the screen. Once they are on the screen they stay within the visible
   area and bounce off one another. The user controls one circle with their mosue 
   and the objective, for the user, is to capture other circles that are smaller than it.
   Each time the user circle captures another circle, it's area grows by the area
   of the circle it just captured, thus, allowing the user circle to become larger and larger
   until it can capture all of the circles on the screen. 

*/


#include <vector>
#include "../include/Angel.h"
#include "circle.cpp"

using namespace std;

//shader stuff
GLint projLoc;
GLint scaleLoc;
GLint positionLoc; 
GLint vLoc;


int winwidth=512, winheight=512;//global variables to set window width and height
				//also used to calculate between world and screen coords.

int level = 1; //starts the game at level 1
int numCircles = pow(2,(level+1));
typedef vec2 pt_type;

//holds the points for one circle at the origin with radius 1
int NumPoints;  //number of iterations to generate the points
pt_type *circle_pts;


//world coordinates
const GLfloat wld_bot=-1.0, wld_top=1.0;
const GLfloat wld_left=-1.0, wld_right=1.0;
const GLfloat wld_near=-1.0, wld_far=1.0;
GLfloat cwld_bot=-1.0, cwld_top=1.0;
GLfloat cwld_left=-1.0, cwld_right=1.0;
GLfloat cwld_near=-1.0, cwld_far=1.0;
  

vector<circleType> myCircle;//vector used to store the circles on the screen


//--------------------------End Global Variables------------------------------------

/*void function that checks to see if the randomly created circles were created
  on top of one another. If so, then this function calls the changeLoc function 
  in the circleType class, which chooses another random location.*/
void inSameLocation()
{
   for(int i=0;i<numCircles;i++){
      for(int j=0;j<numCircles;j++){
         if(i!=j){
            while(myCircle.at(i).sameLoc(myCircle.at(j))){
               myCircle.at(i).changeLoc(i);
            }
         }
      }
   }
   
}

//==============================================================================
/*A void function that randomonly creates the cirlces 
  based on the global variable numCircles
  pushes the circles onto myCircle and calls inSameLocation()*/
void circles_and_level()
{
   srand(time(0));
   while((int)myCircle.size() != numCircles){
     circleType circle1(vec2(((rand()%80)-200)/100.0,((rand()%120)-60)/100.0),
                         vec2((rand()%60+30)/10000.0,((rand()%20)-10)/10000.0),
                         (rand()%5+4)/100.0);
      circleType circle2(vec2(((rand()%120)-60)/100.0,(rand()%80+120)/100.0),
                         vec2(((rand()%20)-10)/10000.0,((rand()%60)-90)/10000.0),
                         0.1);
      circleType circle3(vec2((rand()%80+120)/100.0,((rand()%120)-60)/100.0),
                         vec2(((rand()%60)-90)/10000.0,((rand()%20)-10)/10000.0),
                         (rand()%3+12)/100.0);
      circleType circle4(vec2(((rand()%120)-60)/100.0,((rand()%80)-200)/100.0),
                         vec2(((rand()%20)-10)/10000.0,(rand()%60+30)/10000.0),
                         (rand()%5+15)/100.0);
      myCircle.push_back(circle1);
      myCircle.push_back(circle2);
      myCircle.push_back(circle3);
      myCircle.push_back(circle4);
   }
   if(level > 1)
      inSameLocation();
   circleType userCircle(vec2(0,0),vec2(0,0),0.1);
   myCircle.push_back(userCircle);
}

//-------------------------------------------------------------------------------
/*A recursive function to find the points in the UNIT circle
  Precondition: two points, the points array, adress to location in array, and the
  number of iterations
  Postcondition: The points in the unit circle are stored in the points array*/
void makeCirclePoints(pt_type a, pt_type c, pt_type points[], int &n, int iterations)
{
   if(iterations == 0) {
      points[n++] = a;
   } else {
      pt_type b; 
      //vector overloaded opeartions in next two lines
      b = (a+c)/2; 
      b = normalize(b);

      makeCirclePoints(a, b, points, n, iterations-1); 
      makeCirclePoints(b, c, points, n, iterations-1); 
   }
}

//----------------------------------------------------------------------------
/*Generates the original circle that is used to scale and translate the modified circles
  Precondition: iterations, which is 3, found in the main
  PostCondition: the points array which holds the points for the original circle*/
pt_type *generateCirclePoints(int iterations)
{
   //determine the size of the array based on the number of iterations of 
   //the recursive function.  NOTE: for each line segment that is cut,
   //you will end up 4 smaller line segments.
   NumPoints = (int)(pow(2,iterations+2));

   pt_type *points = new pt_type[NumPoints];
   int n=0;

   // Specifiy the vertices for a triangle
   pt_type vertices[4] = {
      pt_type( 0.0, 1.0), 
      pt_type( -1.0, 0.0 ), 
      pt_type( 0.0, -1.0 ), 
      pt_type( 1.0, 0.0)
   };

   makeCirclePoints(vertices[0], vertices[1], points, n, iterations); 
   makeCirclePoints(vertices[1], vertices[2], points, n, iterations); 
   makeCirclePoints(vertices[2], vertices[3], points, n, iterations); 
   makeCirclePoints(vertices[3], vertices[0], points, n, iterations); 

   return points;
}

//----------------------------------------------------------------------------
/*projLoc, scaleLoc, positionLoc and vLoc are define as global 
  variables since multiple callback functions will need to access them.*/
void initGLBuffer_and_shaders(pt_type points[], int NumPoints)
{
   GLuint buffer;
   GLuint vao;
   GLuint program;
   mat4 p;
   // Create a vertex array object
#ifdef __APPLE__
   glGenVertexArraysAPPLE( 1, &vao );
   glBindVertexArrayAPPLE( vao );
#else
   glGenVertexArrays( 1, &vao );
   glBindVertexArray( vao );
#endif

   circles_and_level();
   //inSameLocation();    
   // Create and initialize a buffer object
   glGenBuffers( 1, &buffer );
   glBindBuffer( GL_ARRAY_BUFFER, buffer );
   glBufferData( GL_ARRAY_BUFFER, NumPoints * sizeof(pt_type), points , GL_STATIC_DRAW );

   // Load shaders and use the resulting shader program
   program = InitShader( "vshader.glsl", "fshader.glsl" );
   glUseProgram( program );

   //Get the locations of transformation matrices from the vshader
   projLoc = glGetUniformLocation(program, "p");
   //Get the locations of the scale and position from the vshader
   //These will be filled as the program runs, the vshader will
   //use these to values to correctly size and position the circle.
   scaleLoc = glGetUniformLocation(program, "scale");
   positionLoc = glGetUniformLocation(program, "pos");


   // Initialize the vertex position attribute from the vertex shader
   vLoc = glGetAttribLocation( program, "vPosition" );
   glEnableVertexAttribArray( vLoc );
   glVertexAttribPointer( vLoc, 2, GL_FLOAT, GL_FALSE, 0, BUFFER_OFFSET(0) );

   glClearColor( 1.0, 1.0, 1.0, 1.0 ); // white background
}

//----------------------------------------------------------------------------
/*Used by the callback function glutPassiveMotionFunc to track the location of the mouse
  Precondition: two variables correlating to the x and y coords of the mouse's current location
  Poscondition: the center of the user circle is moved to the cursor location on the screen*/
void passive(int x, int y)
{
   GLfloat mouseX,mouseY;
   mouseX = (GLfloat) x/(winwidth/2.0)-1.0;
   mouseY = (GLfloat) (winheight-y)/(winheight/2.0)-1.0;
   glutSetCursor(GLUT_CURSOR_NONE); 
   myCircle.at(numCircles).updateLoc(mouseX,mouseY);

   glutPostRedisplay();
}

//---------------------------------------------------------------------------
/*Draws text on the openGL screen.
  Precondition: Two GLfloats which represent where the text should be positioned,
  a pointer to the font to be used, and a pointer to the text to be displayed
  Postcondition: The text is displayed in the predetermined location with the 
  predeterined font.*/
void displayText(GLfloat x, GLfloat y, void *font, const char *string)
{
   const char *c;
   glWindowPos2f(x,y);
   for(c = string; *c!= '\0'; c++)
      glutBitmapCharacter(font,*c);
}

//----------------------------------------------------------------------------
/*Void function used by the callback function glutDisplayFunc
  this funciton clears the screen, displays the circles that need to be drawn
  and also displays any text that needs to be displayed by calling displayText.*/
void display( void )
{
   glClear( GL_COLOR_BUFFER_BIT );     // clear the window
   
   for(int i=0;i<numCircles+1;i++){
      glUniform1f(scaleLoc,myCircle.at(i).getScale());
      glUniform2fv(positionLoc,1,myCircle.at(i).getLoc());
      glDrawArrays( GL_LINE_LOOP, 0, NumPoints );    // draw the points
   }
   displayText(0,1,GLUT_BITMAP_TIMES_ROMAN_24,"LEVEL: ");
   if(level == 1)
      displayText(90,1,GLUT_BITMAP_TIMES_ROMAN_24,"1");
   else if(level == 2)
      displayText(90,1,GLUT_BITMAP_TIMES_ROMAN_24,"2");
   else
      displayText(90,1,GLUT_BITMAP_TIMES_ROMAN_24,"3");
   if((level==1 || level ==2) && myCircle.size()==1)
      displayText(100,450,GLUT_BITMAP_TIMES_ROMAN_24,"Well Done! Try the next Level.");
   if(level == 3 && myCircle.size() == 1)
      displayText(200,450,GLUT_BITMAP_TIMES_ROMAN_24,"YOU WIN!");
   
      
 
   glutSwapBuffers();
}

//----------------------------------------------------------------------------
//Used by the callback function glutKeyboardFunc to exit out of the screen when
//either 'Q' or 'q' is hit
void keyboard( unsigned char key, int x, int y )
{
   switch ( key ) {
      case 'Q':
      case 'q':
          exit( EXIT_SUCCESS );
          break;
   }
}

//------------------------------------------------------------------------------
/*Used by the callback function glutIdleFunc. Moves the circles onto the screen
  then checks for collisions with of circles and collisions with the wall. Also,
  checks to see if the user can capture other circles or not. */
void myIdle()
{   
   for(int i=0;i<numCircles;i++){
      if(!myCircle.at(i).onScreen()){
         myCircle.at(i).takeAstep();
       }else{
         for(int j=0;j<numCircles;j++){
            if(i!=j)
               myCircle.at(i).checkCollision(myCircle.at(j));
         }
         myCircle.at(i).checkWalls();
         myCircle.at(i).takeAstep();
         if(myCircle.at(numCircles).updateRadius(myCircle.at(i))){
            myCircle.erase(myCircle.begin()+i);
            numCircles--;
         }
      }
   }
         

   glutPostRedisplay();
}
     
//----------------------------------------------------------------------------
/* myReshape is called when the window is resized.
 * w and h are the new pixel width and height of the window
 */
void myReshape(GLint w, GLint h)
{
   mat4 p;
   glViewport(0,0,w,h);
   winwidth = w;
   winheight = h;

   if(w<=h) {
      cwld_bot = wld_bot*(GLfloat)h/w;
      cwld_top = wld_top*(GLfloat)h/w;
   } else {
      cwld_left = wld_left*(GLfloat)w/h;
      cwld_right = wld_right*(GLfloat)w/h;
   }
   p = Ortho(cwld_left, cwld_right, cwld_bot, cwld_top, cwld_near, cwld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);

}

//-----------------------------------------------------------------------------
//Sets up the window based on the global variables that refer to 
//the world coordinates using the Ortho funciton
void myinit()
{
   mat4 p;
   glClearColor(1.0,1.0,1.0,0.0);  //clear color is white
   glColor3f(0.0,0.0,0.0);         //fill color set to black

   /* set up standard orthogonal view with clipping box
      as cube of side 2 centered at origin
      This is default view and these statements could be removed. */
   p = Ortho(wld_left, wld_right, wld_bot, wld_top, wld_near, wld_far);
   glUniformMatrix4fv(projLoc, 1, GL_TRUE, p);
}

   
//------------------------------------------------------------------------
/*Called when the user selects this option through the menu. 
  Clears the entire vector and resets the level back to 1 and then
  draws new circles. */
void restartGame()
{
   myCircle.clear();
   level = 1;
   numCircles = pow(2,(level+1));
   circles_and_level();
}
//-----------------------------------------------------------------------
/* Called when the user selects this option through the menu. 
   Clears the entire vector and then increases the level then draws new circles
   based on the new level.*/
void nextLevel()
{
   if(level == 3)
      restartGame();
   else{
      myCircle.clear();
      level+=1;
      numCircles = pow(2,(level+1));
      circles_and_level();
   }
}

//-------------------------------------------------------------------------
/* Performs the task that is associated with each case. Will perform the task
   based on the case that the user selects. */
void myMenu(int item)
{
   switch(item){
      case 1: restartGame(); 
              break;
      case 2: nextLevel();
              break;
      case 3: exit(0);
   }
}

//----------------------------------------------------------------------------
/* Sets up the menu with three different entries: new game, next level, quit.
   Also attaches the menu to the right button. So when the user right clicks the mouse,
   the menu will appear. */
void set_up_menu(void)
{
   glutCreateMenu(myMenu);
   glutAddMenuEntry("New Game",1);
   glutAddMenuEntry("Next Level",2);
   glutAddMenuEntry("Quit",3);
   glutAttachMenu(GLUT_RIGHT_BUTTON);
}

//-----------------------------------------------------------------------------
int main( int argc, char **argv )
{
   if(argc != 1) {
      std::cerr<<"Error: Enter the executable, 'game'.\n";
      exit(1);
   }

   glutInit( &argc, argv );
   glutInitDisplayMode( GLUT_RGBA | GLUT_DOUBLE);
   glutInitWindowSize( winwidth, winheight );

   glutCreateWindow( "Circle Game" );

#ifndef __APPLE__
   glewExperimental = GL_TRUE;
   glewInit();
#endif

   circle_pts = generateCirclePoints(3);
   initGLBuffer_and_shaders(circle_pts, NumPoints);
   
   myinit();
   
   glutDisplayFunc(display);
   glutReshapeFunc(myReshape);
   glutKeyboardFunc(keyboard);
   glutIdleFunc(myIdle);
   glutPassiveMotionFunc(passive);

   cout << endl;
   cout << "===========================" << endl;
   cout << "Use Q|q to quit the program" << endl;
   cout << "===========================" << endl;
   
   set_up_menu();

   glutMainLoop();
   return 0;
}
# Circle-Game
