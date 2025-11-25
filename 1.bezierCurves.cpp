#include <iostream>
#include <vector>
#include <cmath>
#include <algorithm>
#include <string>
#include <stdexcept>

struct Point{
  double x, y;
  Point(double x = 0, double y = 0) :  x(x), y(y) {}
};

Point interpolation(Point p1, Point p2, double t){
  double A = (1-t) * p1.x + t * p2.x;
  double B = (1-t) * p1.y + t * p2.y;
  return Point(A, B);
}

std::vector<Point> getIntermediatePoints(std::vector<Point>& points, double t){
  std::vector<Point> intermediatePoints;
  if(points.size() == 1){
    return intermediatePoints;
  }
  for(size_t  i = 0; i < points.size() - 1; i++){
    intermediatePoints.push_back(interpolation(points[i], points[i+1], t));
  }
  return intermediatePoints;
}

Point getSinglePointOnCurve(std::vector<Point>& controlPoints, double t){
  std::vector<Point> currentLevel = controlPoints;
  if(controlPoints.empty()){
    throw std::invalid_argument("Error");
  }
  if(controlPoints.size() == 1){
    return controlPoints[0];
  }
  while(currentLevel.size() > 1){
    currentLevel = getIntermediatePoints(currentLevel, t);
  }
  return currentLevel[0];
}

int main(){
  Point A = {0.0, 0.0};
  Point B = {10.0,0.0};
  auto interp = interpolation(A, B, 0.4);
  std::cout << interp.x << "," << interp.y << "\n";

  std::vector<Point> intermediate = {Point(0,0), Point(10,0), Point(20,0)};
  auto result = getIntermediatePoints(intermediate,  0.5);
  std::cout << "intermediate 1 : " << result[0].x << "," << result[0].y << "\n";

  auto curvePoint = getSinglePointOnCurve(intermediate, 0.4);
  std::cout << "curvePoint : " << curvePoint.x << ", " << curvePoint.y << "\n";


  std::vector<Point> cubicCurvePoints = {
    {0.0, 10.0},
    {10.0, 20.0},
    {20.0, 10.0},
    {30.0, 0.0}

  };

  int width = 31, height = 31, steps = 10;
  std::vector<std::vector<char>> canvas(height, std::vector<char>(width, ' '));

  for(int y = 0; y < height; y++){
    canvas[y][0] = '.';
  }

  for(int i = 0; i < steps; i++){
    double t = (double)i / (double)(steps - 1);
    auto singlePoint = getSinglePointOnCurve(cubicCurvePoints, t);
    int x = (int)std::round(singlePoint.x);
    int y = (int)std::round(singlePoint.y);
    if(x >= 0 && x < width && y >= 0 && y < height){
      canvas[y][x] = '#';
    }
  }

  for(int y = height - 1; y >= 0 ; y--){
    int lastNonSpace = -1;
    std::string line = "";

    for(int x = 0; x < width; x++){
      if(canvas[y][x] != ' '){
        lastNonSpace = x;
      }
    }
    for(int x = 0; x <= lastNonSpace;x++){
      line += canvas[y][x];
    }
    std::cout << line << std::endl;
  }
  return 0;
  
}
