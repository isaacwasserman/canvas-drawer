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
   else if(_type == TRIANGLES){
      struct Triangle triangle;
      while(_vertices.size() > 2){
         triangle = popTriangle();
         drawTriangle(triangle);
      }
   }
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

   return line;
}

struct Triangle Canvas::popTriangle(){
   struct Triangle triangle;

   struct Vertex v1 = _vertices[0];
   struct Vertex v2 = _vertices[1];
   struct Vertex v3 = _vertices[2];

   _vertices.pop_front();
   _vertices.pop_front();
   _vertices.pop_front();

   triangle.v1 = v1;
   triangle.v2 = v2;
   triangle.v3 = v3;

   return triangle;
}

void Canvas::drawLine(Line line){
   int W = line.v2.x - line.v1.x;
   int H = line.v2.y - line.v1.y;

   if(abs(H) < abs(W)){
      if(abs(line.v1.x) > abs(line.v2.x)){
         line = Line{line.v2, line.v1};
      }
      drawLineLow(line);
   }
   else {
      if(abs(line.v1.y) > abs(line.v2.y)){
         line = Line{line.v2, line.v1};
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
   Pixel color;
   for(int x = line.v1.x; x < line.v2.x; x++){
      float progress = (float)(x - line.v1.x) / (float)(line.v2.x - line.v1.x);
      color = (line.v1.color * (1 - progress)) + (line.v2.color * progress);
      _canvas.set(y, x, color);
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
   Pixel color;
   for(int y = line.v1.y; y < line.v2.y; y++){
      float progress = (float)(y - line.v1.y) / (float)(line.v2.y - line.v1.y);
      color = (line.v1.color * (1 - progress)) + (line.v2.color * progress);
      _canvas.set(y, x, color);
      if(F > 0){
         x += dx;
         F += 2 * (W - H);
      }
      else {
         F += 2 * W;
      }
   }
}

float implicitLineEquation(struct Vertex a, struct Vertex b, struct Vertex p){
   return p.x*(a.y - b.y) + p.y*(b.x - a.x) + a.x*b.y - b.x*a.y;
}

void Canvas::drawTriangle(Triangle triangle){
   int xMin = min(min(triangle.v1.x, triangle.v2.x), triangle.v3.x);
   int xMax = max(max(triangle.v1.x, triangle.v2.x), triangle.v3.x);
   int yMin = min(min(triangle.v1.y, triangle.v2.y), triangle.v3.y);
   int yMax = max(max(triangle.v1.y, triangle.v2.y), triangle.v3.y);

   struct Vertex p;
   float alpha, beta, gamma;
   bool in_triangle;
   Pixel color;
   for(int x = xMin; x <= xMax; x++){
      for(int y = yMin; y <= yMax; y++){
         p = Vertex{x, y, currentColor};
         alpha = implicitLineEquation(triangle.v2, triangle.v3, p) / implicitLineEquation(triangle.v2, triangle.v3, triangle.v1);
         beta = implicitLineEquation(triangle.v3, triangle.v1, p) / implicitLineEquation(triangle.v3, triangle.v1, triangle.v2);
         gamma = (1 - alpha) - beta;
         in_triangle = (alpha >= 0 && alpha <= 1) && (beta >= 0 && beta <= 1) && (gamma >= 0 && gamma <= 1);
         if(in_triangle){
            color = (triangle.v1.color * alpha) + (triangle.v2.color * beta) + (triangle.v3.color * gamma);
            _canvas.set(y, x, color);
         }
      }
   }
   drawLine(Line{triangle.v1, triangle.v2});
   drawLine(Line{triangle.v2, triangle.v3});
   drawLine(Line{triangle.v3, triangle.v1});
}

void Canvas::vertex(int x, int y) {
   struct Vertex v = {x, y, currentColor};
   _vertices.push_back(v);
}

void Canvas::color(unsigned char r, unsigned char g, unsigned char b) {
   currentColor = Pixel{r, g, b};
}

void Canvas::background(unsigned char r, unsigned char g, unsigned char b) {
   for(int i = 0; i < _canvas.height(); i++){
      for(int j = 0; j < _canvas.width(); j++){
         _canvas.set(i, j, Pixel{r, g, b});
      }
   }
}
