#include "canvas.h"

#include <cassert>

using namespace std;
using namespace agl;

Canvas::Canvas(int w, int h) : _canvas(w, h) {}

Canvas::~Canvas() {}

void Canvas::save(const std::string& filename) { _canvas.save(filename); }

void Canvas::begin(PrimitiveType type) {
   _type = type;
}

void Canvas::end() {
   if(_type == LINES){
      struct Line line;
      while(_vertices.size() > 1){
         line = popLine();
         drawLine(line);
      }
   }
   // else if(_type == TRIANGLES){
   //    struct Triangle triangle;
   //    while(_vertices.size() > 2){
   //       triangle = popTriangle();
   //       drawTriangle(triangle);
   //    }
   // }
   _vertices.clear();
}

struct Line Canvas::popLine(){
   struct Line line;

   struct Vertex v1 = _vertices[0];
   struct Vertex v2 = _vertices[1];

   _vertices.pop_front();
   _vertices.pop_front();

   line.v1 = v1;
   line.v2 = v2;

   line.color = v1.color;

   return line;
}

void Canvas::drawLine(Line line){
   int W = line.v2.x - line.v1.x;
   int H = line.v2.y - line.v1.y;

   if(abs(H) < abs(W)){
      if(abs(line.v1.x) > abs(line.v2.x)){
         line = Line{line.v2, line.v1, line.color};
      }
      drawLineLow(line);
   }
   else {
      if(abs(line.v1.y) > abs(line.v2.y)){
         line = Line{line.v2, line.v1, line.color};
      }
      drawLineHigh(line);
   }
}

void Canvas::drawLineLow(Line line){
   int y = line.v1.y;
   int W = line.v2.x - line.v1.x;
   int H = line.v2.y - line.v1.y;
   int dy = 1;
   if(H < 0){
      dy = -1;
      H = -H;
   }
   int F = 2 * H - W;
   for(int x = line.v1.x; x < line.v2.x; x++){
      _canvas.set(y, x, line.color);
      if(F > 0){
         y += dy;
         F += 2 * (H - W);
      }
      else {
         F += 2 * H;
      }
   }
}

void Canvas::drawLineHigh(Line line){
   int x = line.v1.x;
   int W = line.v2.x - line.v1.x;
   int H = line.v2.y - line.v1.y;
   int dx = 1;
   if(W < 0){
      dx = -1;
      W = -W;
   }
   int F = 2 * W - H;
   for(int y = line.v1.y; y < line.v2.y; y++){
      _canvas.set(y, x, line.color);
      if(F > 0){
         x += dx;
         F += 2 * (W - H);
      }
      else {
         F += 2 * W;
      }
   }
}

void Canvas::drawTriangle(Triangle triangle){

}

void Canvas::vertex(int x, int y) {
   struct Vertex v = {x, y, currentColor};
   _vertices.push_back(v);
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b) {
   currentColor = Pixel{r, g, b};
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b) {}
