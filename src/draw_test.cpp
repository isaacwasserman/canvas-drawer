/**
* Tests the drawing functions of the Canvas class
*
* @author: Isaac Wasserman
* @version: February 23, 2023
*/

#include <iostream>
#include "canvas.h"

using namespace agl;
using namespace std;

void test_line(Canvas& drawer, int ax, int ay, int bx, int by, const std::string& savename)
{
   drawer.background(0, 0, 0);
   drawer.begin(LINES);
   drawer.vertex(ax, ay);
   drawer.vertex(bx, by);
   drawer.end();
   drawer.save(savename);
}

int main(int argc, char** argv)
{
   Canvas drawer(100, 100);

   drawer.strokeColor(255, 255, 255);
   test_line(drawer, 0, 50, 99, 50, "horizontal-line.png");
   test_line(drawer, 50, 0, 50, 100, "vertical-line.png");
   test_line(drawer, 0, 0, 100, 100, "diagonal-line-1.png"); // slope = 1
   test_line(drawer, 25, 10, 75, 25,  "h-lessthan-w-line-1.png"); // slope H < W
   test_line(drawer, 25, 25, 75, 75,  "w-lessthan-h-line-1.png"); // slope W < H
   test_line(drawer, 0, 100, 100, 0, "diagonal-line-2.png"); // slope H < W
   test_line(drawer, 25, 90, 75, 75,  "h-lessthan-w-line-2.png"); // slope H < W
   test_line(drawer, 25, 90, 75, 25,  "w-lessthan-h-line-2.png"); // slope W < H

   // test line interpolation
   drawer.background(0, 0, 0);
   drawer.begin(LINES);
   drawer.strokeColor(255, 0, 255);
   drawer.vertex(0, 0);
   drawer.strokeColor(0, 255, 255);
   drawer.vertex(100, 100);
   drawer.end();
   drawer.save("line-color-interpolation.png");

   // test triangle with interpolation
   drawer.background(0, 0, 0);
   drawer.begin(TRIANGLES);
   drawer.strokeWidth(0);
   drawer.fillColor(255, 0, 255, 1);
   drawer.vertex(10, 0);
   drawer.fillColor(0, 255, 255, 1);
   drawer.vertex(90, 50);
   drawer.fillColor(255, 255, 0, 1);
   drawer.vertex(10, 90);
   drawer.end();
   drawer.save("triangle.png");

   // // test triangle with interpolation
   drawer.background(0, 0, 0);
   drawer.begin(TRIANGLES);
   drawer.fillColor(255, 0, 255);
   drawer.vertex(10, 10);
   drawer.vertex(10, 90);
   drawer.vertex(90, 90);

   drawer.fillColor(255, 255, 0);
   drawer.vertex(90, 90);
   drawer.vertex(90, 10);
   drawer.vertex(10, 10);
   drawer.end();
   drawer.save("quad.png");

   // // test quad with quad primitive
   drawer.background(0, 0, 0);
   drawer.begin(QUADS);
   drawer.fillColor(255, 0, 255);
   drawer.vertex(10, 10); // 1
   drawer.fillColor(255, 255, 0);
   drawer.vertex(10, 90); // 2
   drawer.fillColor(0, 255, 255);
   drawer.vertex(90, 90); // 3
   drawer.fillColor(0, 255, 0);
   drawer.vertex(90, 10); // 4
   drawer.end();
   drawer.save("quad-2.png");

   // // test fan
   drawer.background("../images/dog.jpg");
   drawer.begin(FANS);
   drawer.blendMode("subtract");
   drawer.strokeWidth(0);
   drawer.strokeColor(0, 0, 0, 0);
   drawer.fillColor(255, 255, 255, 1);
   drawer.vertex(500, 500); // center

   drawer.fillColor(255, 0, 0, 0.5);
   drawer.vertex(250, 750); // red

   drawer.fillColor(255, 128, 0, 0.5);
   drawer.vertex(750, 750); // orange

   drawer.fillColor(255, 255, 0, 0.5);
   drawer.vertex(990, 500); // yellow

   drawer.fillColor(0, 255, 0, 0.5);
   drawer.vertex(500, 0); // green

   drawer.fillColor(0, 0, 255, 0.5);
   drawer.vertex(0, 500); // blue

   drawer.end();
   drawer.save("fan.png");


   return 0;
}
