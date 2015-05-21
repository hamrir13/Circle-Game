#include "circle.h"
#define PI 3.14159
using namespace std;
//------------------------------------------------------------------------
//constructor
/*Precondition: vec2 to determine location of the circle, 
 * vec2 to determine the direction(velocity) and GLfloat to determine radius
 * Postcondition: sets the loc (center of the circle), dir (velocity)
 *  and radius of the circle
*/
circleType::circleType(vec2 in_loc, vec2 in_dir, GLfloat in_rad)
{
   loc = in_loc;
   dir = in_dir;
   radius = in_rad;
}

//-----------------------------------------------------------------------
//Changes the location of the circle by moving it 1 unit
//Returns the new location of the circle
vec2 circleType::takeAstep(void)
{
   //circle location is updated every time this method is called.
   loc+=dir;
   return loc;
}

//------------------------------------------------------------------------
//Returns the location of the center of the circle
vec2 circleType::getLoc(void)
{
   return loc;
}

//----------------------------------------------------------------------
//Function that returns the scale of the circle. Function uses 
//the radius, previously determined from the constructor, as the scale.  
GLfloat circleType::getScale(void)
{
   //using the radius as the scale.
   return radius;
}

//------------------------------------------------------------------------
//Precondition: Two vec2's which are the locations of the two circles
//Postcondition: The distance between the two locations using the distance formula
GLfloat circleType::distance(vec2 a, vec2 b)
{
   //returns the distance between the loc of two circles
   return sqrt(pow(a.x-b.x,2)+pow(a.y-b.y,2));
}

//-------------------------------------------------------------------------
/*A void function that is used to determine when the user circle "captures"
  one of the random circles
  Precondition: The address of the circle the user circle is about to capture
  Postcondition: If the user circle is bigger then the user circle's area increases
  by the area of the circle it just captured. The captured circle then 
  disappears by setting its radius to 0.*/
bool circleType::updateRadius(circleType &a)
{
   GLfloat playerArea = PI*(radius*radius);
   GLfloat captureArea = PI*(a.radius*a.radius);
   if(distance(loc,a.loc) < (radius+a.radius)){
      if(playerArea > captureArea){
         playerArea += captureArea;
         radius = sqrt(playerArea/PI);
         a.loc = vec2(-2,-2);
         a.radius = 0;
         return true;
      }else{
         cout<<"You tried to capture a circle larger than you"<<endl; 
         cout<<"Try again."<<endl;
         exit(0);
      }   
   }else
      return false;
}

//----------------------------------------------------------------------
//Function is used to set the user circle to the location of mouse
/*Precondition: Two GLfloats that correspond to the mouse position
  in current world coordinates
  Postcondition: Sets that loc of the circle to the location of the mouse*/
void circleType::updateLoc(GLfloat x, GLfloat y)
{
   loc.x = x;
   loc.y = y;
}

//------------------------------------------------------------------------
/*A void function that changes the location of a circle if 
  it is generated on top of another one.
  Precondition: The circle that needs to move
  Postcondition: The new location of the circle */
void circleType::changeLoc(int i)
{
   srand(time(0));

   if(i == 0 || i == 4 || i == 8 || i == 12){
      loc.x = ((rand()%80)-200)/100.0;
      loc.y = ((rand()%120)-60)/100.0;
   }else if(i == 1 || i == 5 || i == 9 || i == 13){
      loc.x = ((rand()%120)-60)/100.0;
      loc.y = (rand()%80+120)/100.0;
   }else if(i == 2 || i ==6 || i == 10 || i == 14){
      loc.x = (rand()%80+120)/100.0;
      loc.y = ((rand()%120)-60)/100.0;
   }else{
      loc.x = ((rand()%120)-60)/100.0;
      loc.y = ((rand()%80)-200)/100.0;
   }
}

//---------------------------------------------------------------------
//A void function that changes the direction of the circle
//Circle checks to see if it takes a step, will it hit a wall
//if so then it changes direction
void circleType::checkWalls()
{
      if((loc.x+dir.x) + radius > 1.0)
         dir.x*=-1;
      if((loc.x+dir.x) - radius < -1.0)
         dir.x*=-1;
      if((loc.y+dir.y) + radius > 1.0)
         dir.y*=-1;
      if((loc.y+dir.y) - radius < -1.0)
         dir.y*=-1;

}

//----------------------------------------------------------------------
/*A void function that checks to see if two circles are going to collide
  Precondition: The address of the circle that is going to be compared
  Postcondition: The new direction vectors of both circles if they collide */
void circleType::checkCollision(circleType &a)
{
   if(distance(loc+dir, a.loc) <= (radius + a.radius)){
      vec2 unitNorm = normalize(a.loc-loc);
      vec2 unitTan = vec2(-unitNorm.y,unitNorm.x);
      GLfloat v1norm = dot(unitNorm,dir);
      GLfloat v1tan = dot(unitTan,dir);
      GLfloat v2norm = dot(unitNorm,a.dir);
      GLfloat v2tan = dot(unitTan,a.dir);
      GLfloat v1pn = (v1norm*(radius-a.radius)+2*a.radius*v2norm)/(radius+a.radius);
      GLfloat v2pn = (v2norm*(a.radius-radius)+2*radius*v1norm)/(radius+a.radius);
      vec2 postv1n = unitNorm*v1pn;
      vec2 postv1t = unitTan*v1tan;
      vec2 postv2n = unitNorm*v2pn;
      vec2 postv2t = unitTan*v2tan;
      if(onScreen())
         dir = postv1n+postv1t;
      if(a.onScreen())
         a.dir = postv2n+postv2t;
   }
}

//------------------------------------------------------------------------
//bool function that returns true if the circles are generated
//in positions that overlap one another
bool circleType::sameLoc(circleType &a)
{
   if(distance(loc+dir,a.loc+a.dir) < (radius+a.radius + 0.1))
      return true;
   else
      return false;
}

//-----------------------------------------------------------------------
//bool function that returns true once the circle is
//entirely on the visible area
bool circleType::onScreen()
{
   if(loc.x+radius < 1 && loc.x-radius > -1 && loc.y+radius < 1 && loc.y-radius > -1)
      return true;
   else
      return false;
}




