/**
* Creates the art for the assignment.
*
* @author: Isaac Wasserman
* @version: February 23, 2023
*/

#include <iostream>
#include <math.h>
#include "canvas.h"
using namespace std;
using namespace agl;

int main(int argc, char** argv)
{
   Canvas drawer(1000, 1000);
   // your code here

   // test variable width line
   int triangleWidth = 256;
   int triangleHeight = 144;
   drawer.begin(TRIANGLES);
   drawer.strokeWidth(0);
   drawer.fillColor(255, 255, 255);
   int colors[3][3] = {{255, 255, 255}, {255, 255, 255}, {255, 255, 255}};
   int j = 0;
   for(int i = 0; i <= drawer.getHeight() / triangleHeight; i++){
      for(int k = 0; k <= drawer.getWidth() / (triangleWidth / 2) + 1; k++){
         int xOffset = (triangleWidth / 2) * k;
         drawer.fillColor(colors[(j + 0) % 3][0], colors[(j + 0) % 3][1], colors[(j + 0) % 3][2], 0.5 + 0.5*((k+1) % 2));
         drawer.vertex(-(triangleWidth / 2) + xOffset, (i * triangleHeight) + (triangleHeight * (k % 2)));
         drawer.fillColor(colors[(j + 1) % 3][0], colors[(j + 1) % 3][1], colors[(j + 1) % 3][2], 0.5 + 0.5*((k+0) % 2));
         drawer.vertex(0 + xOffset, (i * triangleHeight) + triangleHeight * ((k + 1) % 2));
         drawer.fillColor(colors[(j + 2) % 3][0], colors[(j + 2) % 3][1], colors[(j + 2) % 3][2], 0.5 + 0.5*((k+0) % 2));
         drawer.vertex((triangleWidth / 2) + xOffset, (i * triangleHeight) + triangleHeight * (k % 2));
         j++;
      }
   }
   drawer.end();

   drawer.begin(FANS);
   drawer.strokeWidth(0);
   drawer.strokeColor(0, 0, 0, 0);
   drawer.fillColor(255, 255, 255, 1);

   drawer.vertex(500, 500); // center

   drawer.fillColor(255, 0, 0, 0.5);
   drawer.vertex(0, 0); // red

   drawer.fillColor(255, 128, 0, 0.5);
   drawer.vertex(1000, 0); // orange

   drawer.fillColor(255, 255, 0, 0.5);
   drawer.vertex(1000, 500); // yellow

   drawer.fillColor(0, 255, 0, 0.5);
   drawer.vertex(1000, 1000); // green

   drawer.fillColor(0, 0, 255, 0.5);
   drawer.vertex(0, 1000); // blue
   drawer.end();

   drawer.begin(QUADS);
   drawer.blendMode("subtract");
   drawer.strokeWidth(0);
   drawer.fillColor(100,100,255,0.5);
   int n = 12;
   int start = -100;
   for(int i = 0; i < n; i ++){
      drawer.vertex(start + 0 + i * (drawer.getWidth() / n), start + 0 + i * (drawer.getWidth() / n));
      drawer.vertex(start + 300 + i * (drawer.getWidth() / n), start + 0 + i * (drawer.getWidth() / n));
      drawer.vertex(start + 300 + i * (drawer.getWidth() / n), start + 240 + i * (drawer.getWidth() / n));
      drawer.vertex(start + 0 + i * (drawer.getWidth() / n), start + 240 + i * (drawer.getWidth() / n));
   }
   drawer.end();

   drawer.begin(FANS);
   drawer.blendMode("normal");
   drawer.strokeWidth(0);
   drawer.fillColor(255,255,255,1);
   drawer.vertex(500, 500);
   for(int i = 0; i < 360; i += 5){
      int shade = std::min(255, (255 - 255 * abs(180 - i) / 180) + 75);
      drawer.fillColor(shade,shade,shade,1);
      drawer.vertex(500 + 200 * cos(((i + 140) % 360) * M_PI / 180), 500 + 200 * sin(((i + 140) % 360) * M_PI / 180));
   }
   drawer.end();

   drawer.save("tesselate.png");
}

