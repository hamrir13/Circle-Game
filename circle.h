#ifndef CIRCLE_H
#define CIRCLE_H	

class circleType {
   private:
      vec2 loc;
      vec2 dir;  //direction, length is speed.
      GLfloat radius;
   public:
      circleType(vec2 in_loc, vec2 in_dir, GLfloat in_rad);
      vec2 takeAstep();
      vec2 getLoc(void);
      GLfloat getScale(void);
      GLfloat distance(vec2 a, vec2 b);
      bool updateRadius(circleType &a);
      void updateLoc(GLfloat x, GLfloat y);
      void changeLoc(int i);
      void checkWalls();
      void checkCollision(circleType &a);
      bool sameLoc(circleType &a);
      bool onScreen();
};
#endif
