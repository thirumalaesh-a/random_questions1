#include <iostream>
#include <vector>
#include <string>
#include <algorithm>
#include <cmath>

// Point: Represents a 2D coordinate
struct Point{
    double x, y;
    Point(double x = 0, double y = 0) : x(x), y(y) {}
};

Point interpolation(Point p1, Point p2, double t){
    double A = (1 - t) * p1.x + t * p2.x;  // Weighted average of x
    double B = (1 - t) * p1.y + t * p2.y;  // Weighted average of y
    return Point(A, B);
}

std::vector<Point> getIntermediatePoints(std::vector<Point>& points, double t){
    std::vector<Point> intermediatePoints;
    if(points.empty()) {
        std::cerr << "Warning: Empty points vector" << std::endl;
        return intermediatePoints;
    }
    
    if(points.size() == 1) {
        return intermediatePoints;  // Return empty
    }
    // Create n-1 interpolated points from n input points
    for(size_t i = 0; i < points.size() - 1; i++){
        intermediatePoints.push_back(interpolation(points[i], points[i + 1], t));
    }
    return intermediatePoints;
}

/*
Recursively reduce control points until one point remains (point on curve)

ALGORITHM (De Casteljau):
1. Start with n control points
2. Create n-1 interpolated points at parameter t
3. Repeat until 1 point remains
4. This final point lies on the Bézier curve at parameter t
*/
Point getSinglePointOnCurve(std::vector<Point>& controlPoints, double t){
    // Edge case: empty or single point
    if(controlPoints.empty()) {
        throw std::invalid_argument("Error: Empty control points vector");
    }
    
    // Edge case: Single control point (degenerate curve)
    if(controlPoints.size() == 1) {
        return controlPoints[0];
    }
    
    std::vector<Point> currentLevel = controlPoints;
    
    // Reduce points level by level until one remains
    // n → n-1 → n-2 → ... → 1
    while(currentLevel.size() > 1){
        currentLevel = getIntermediatePoints(currentLevel, t);
    }
    
    return currentLevel[0];  // Final point on Bézier curve
}

int main(){
    std::cout << "========== TESTING FUNCTIONS ==========\n\n";
    
    // Test 1: Interpolation
    Point p1(0, 0);
    Point p2(10, 10);
    Point interp = interpolation(p1, p2, 0.0);
    std::cout << "Test 1 - Interpolation t=0.0: (" << interp.x << ", " << interp.y << ")\n\n";

    // Test 2: Intermediate Points
    std::vector<Point> intermediate = {Point(0, 0), Point(10, 10), Point(20, 0)};
    auto result2 = getIntermediatePoints(intermediate, 0.5);
    std::cout << "Test 2 - Three points: size=" << result2.size() << "\n";
    std::cout << "  Intermediate 1: (" << result2[0].x << ", " << result2[0].y << ")\n";
    std::cout << "  Intermediate 2: (" << result2[1].x << ", " << result2[1].y << ")\n\n";

    // Test 3: Control Points
    std::vector<Point> cubic = {Point(0, 0), Point(0, 10), Point(10, 10), Point(10, 0)};
    Point result3_start = getSinglePointOnCurve(cubic, 0.0);
    Point result3_mid = getSinglePointOnCurve(cubic, 0.5);
    Point result3_end = getSinglePointOnCurve(cubic, 1.0);
    std::cout << "Test 3 - Cubic (4 points):\n";
    std::cout << "  t=0.0: (" << result3_start.x << ", " << result3_start.y << ")\n";
    std::cout << "  t=0.5: (" << result3_mid.x << ", " << result3_mid.y << ")\n";
    std::cout << "  t=1.0: (" << result3_end.x << ", " << result3_end.y << ")\n\n";

    std::cout << "========== GENERATING CURVE ==========\n\n";

    // Generate Bézier curve
    std::vector<Point> controlPoints = {
        {0.0, 10.0},   // P0: Start point
        {10.0, 20.0},  // P1: Control point (pulls curve up)
        {20.0, 10.0},  // P2: Control point (pulls curve down)
        {30.0, 0.0}    // P3: End point
    };
    
    int width = 31, height = 31;
    int steps = 10;  // Number of sample points on curve
    std::vector<std::vector<char>> canvas(height, std::vector<char>(width, ' '));
    
    // Draw y-axis reference line
    for(int y = 0; y < height; y++){
        canvas[y][0] = '.';
    }
    
    for(int i = 0; i < steps; i++){
        double t = (double)i / (double)(steps - 1);  // Normalize to [0, 1]
        
        Point point = getSinglePointOnCurve(controlPoints, t);
        
        // Convert to integer coordinates for canvas
        int x = (int)std::round(point.x);
        int y = (int)std::round(point.y);
        
        // Plot point if within bounds (edge case handling)
        if(x >= 0 && x < width && y >= 0 && y < height){
            canvas[y][x] = '#';
        }
    }
    
    /*
    RENDER CANVAS (flip y-axis for proper orientation)
    Print from top to bottom: y=height-1 down to y=0
    This makes y-axis increase upward (mathematical convention)
    */
    for(int y = height - 1; y >= 0; y--){
        std::string line = "";
        
        // Find rightmost non-space character to avoid trailing spaces
        int lastNonSpace = -1;
        for(int x = 0; x < width; x++){
            if(canvas[y][x] != ' '){
                lastNonSpace = x;
            }
        }
        
        // Build line up to last non-space character
        for(int x = 0; x <= lastNonSpace; x++){
            line += canvas[y][x];
        }
        
        std::cout << line << std::endl;
    }
    
    /*    
    Edge Cases Handled:
    1. Empty control points → throws exception
    2. Single control point → returns that point
    3. Out-of-bounds coordinates → skipped
    4. t < 0 or t > 1 → curve extrapolation (works but unusual)
    5. Duplicate control points → curve passes through/near duplicates
    */
    
    return 0;
}