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

   // Compute the min/max X and Y values for each line 
   double miny = (m_posY-(m_vHeight/2)); 
   double maxy = (m_posY+(m_vHeight/2)); 
   double minx = (m_posX-(m_vWidth/2)); 
   double maxx = (m_posX+(m_vWidth/2)); 
   //double xstep = (maxx - minx) / m_stepX;
   //double ystep = (maxy - miny) / m_stepY;
   // Draw lines in the Y direction 
   int steps = 0;
   for(double y = miny; y <= maxy; y+=m_stepY, steps++){ 
      glColor3ub(60,60,60); 
      if(steps%10 == 0){ 
         glEnd(); 
         glColor3ub(100,100,100); 
         glLineWidth(2.0f); 
         glBegin(GL_LINES); 
      } 
      if(steps%100 == 0) 
         glColor3ub(180,180,180); 
      glVertex3d(minx, (GLdouble)y, (GLdouble)m_posZ); 
      glVertex3d(maxx, (GLdouble)y, (GLdouble)m_posZ); 
      if(steps%10 == 0){ 
         glEnd(); 
         glLineWidth(0.5f); 
         glBegin(GL_LINES); 
      } 
   } 
   steps = 0;
   // Draw lines in the X direction 
   for(double x = minx; x <= maxx; x+=m_stepX, steps++){ 
      glColor3ub(60,60,60); 
      if(steps%10 == 0){ 
         glEnd(); 
         glColor3ub(100,100,100); 
         glLineWidth(2.0f); 
         glBegin(GL_LINES); 
      } 
      if(steps%100 == 0) 
         glColor3ub(180,180,180); 
      glVertex3d((GLdouble)x, miny, (GLdouble)m_posZ); 
      glVertex3d((GLdouble)x, maxy, (GLdouble)m_posZ); 
      if(steps%10 == 0){ 
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
   double miny = (m_posY-(m_vHeight/2)); 
   double maxy = (m_posY+(m_vHeight/2)); 
   double minx = (m_posX-(m_vWidth/2)); 
   double maxx = (m_posX+(m_vWidth/2)); 

   bbox.set(minx, miny, -0.1, maxx, maxy, 0.1); 

   return bbox; 
} 