#include "Grid.h"

void Grid::drawImplementation(osg::RenderInfo & renderInfo) const 
{ 
   osg::State* currentState = renderInfo.getState();
   currentState->applyMode(GL_BLEND, true);
   currentState->applyMode(GL_LINE_SMOOTH, true);
   currentState->applyMode(GL_LIGHTING, false); 
   glColor3ub(60,60,60); 

   //render floor grid 
   glLineWidth(0.5f); 
   glBegin(GL_LINES); 

   // Compute the Z location of the grid 
   int zg = (int)-10; 
   int ystep = 100; 
   int xstep = 100; 

   // Compute the Y-Step depending on the current zoom factor 
   if(m_zoom < 1.5) 
      ystep = xstep = 10; 
   if(m_zoom < 0.8) 
      ystep = xstep = 5; 
   if(m_zoom < 0.4) 
      ystep = xstep = 2; 
   if(m_zoom < 0.2) 
      ystep = xstep = 1; 

   // Compute the min/max X and Y values for each line 
   float miny = (m_posY-(m_vHeight/2*m_zoom)); 
   float maxy = (m_posY+(m_vHeight/2*m_zoom)); 
   float minx = (m_posX-(m_vWidth/2*m_zoom)); 
   float maxx = (m_posX+(m_vWidth/2*m_zoom)); 

   // Draw lines in the Y direction 
   for(int yg = (int)(miny - ((int)miny%ystep)); yg <= (int)(maxy - ((int)maxy%ystep)); yg+=ystep){ 
      glColor3ub(60,60,60); 
      if(yg%10 == 0){ 
         glEnd(); 
         glColor3ub(100,100,100); 
         glLineWidth(2.0f); 
         glBegin(GL_LINES); 
      } 
      if(yg%100 == 0) 
         glColor3ub(180,180,180); 
      glVertex3f(minx, (GLfloat)yg, (GLfloat)zg); 
      glVertex3f(maxx, (GLfloat)yg, (GLfloat)zg); 
      if(yg%10 == 0){ 
         glEnd(); 
         glLineWidth(0.5f); 
         glBegin(GL_LINES); 
      } 
   } 

   // Draw lines in the X direction 
   for(int xg = (int)(minx - ((int)minx%xstep)); xg <= (int)(maxx - ((int)maxx%xstep)); xg+=xstep){ 
      glColor3ub(60,60,60); 
      if(xg%10 == 0){ 
         glEnd(); 
         glColor3ub(100,100,100); 
         glLineWidth(2.0f); 
         glBegin(GL_LINES); 
      } 
      if(xg%100 == 0) 
         glColor3ub(180,180,180); 
      glVertex3f((GLfloat)xg, miny, (GLfloat)zg); 
      glVertex3f((GLfloat)xg, maxy, (GLfloat)zg); 
      if(xg%10 == 0){ 
         glEnd(); 
         glLineWidth(0.5f); 
         glBegin(GL_LINES); 
      } 
   } 

   glEnd(); 

   // Re-enable lightning 
   currentState->applyMode(GL_LIGHTING, true); 
} 

osg::BoundingBox Grid::computeBound() const 
{ 
   osg::BoundingBox bbox; 

   // Compute the min/max X and Y values for the grid 
   float miny = (m_posY-(m_vHeight/2*m_zoom)); 
   float maxy = (m_posY+(m_vHeight/2*m_zoom)); 
   float minx = (m_posX-(m_vWidth/2*m_zoom)); 
   float maxx = (m_posX+(m_vWidth/2*m_zoom)); 

   bbox.set(minx, miny, -0.1, maxx, maxy, 0.1); 

   return bbox; 
} 