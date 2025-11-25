#include <iostream>
#include <string>
#include <cmath>
#include <algorithm>
#include <vector>

struct Point{
    double x, y;  // x: horizontal position, y: vertical position
    Point(double x = 0.0, double y = 0.0) : x(x), y(y) {}
};

struct Polygon{
    std::vector<Point> vertices;  // Ordered list of corner points
};

/*
═══════════════════════════════════════════════════════════════════════════
CROSS PRODUCT IN 2D
═══════════════════════════════════════════════════════════════════════════
Formula: cross(a, b) = a.x * b.y - a.y * b.x

GEOMETRIC MEANING:
- Result > 0: vector 'b' is on the LEFT side of vector 'a'
- Result < 0: vector 'b' is on the RIGHT side of vector 'a'
- Result = 0: vectors are PARALLEL (same or opposite direction)

*/
double cross2D(Point a, Point b) {
    return a.x * b.y - a.y * b.x;
}

/*
VISUAL SETUP:
                    p2
                    *
                   /|
      ray_dir     / |
         ↑       /  | seg_dir = p2 - p1
         |      /   |
    origin *---/----+--→
         |    /p1   |
         |   /      |
         +--*-------+--→
            |
      origin_to_p1 = p1 - origin

KEY PARAMETERS:
- pos_ray (t): Distance along ray to intersection point
               t = 0 → at origin
               t = 1 → one ray_dir length away
               t < 0 → behind the ray (no hit)

- pos_seg (s): Position along segment [p1, p2]
               s = 0 → at p1
               s = 1 → at p2
               s ∈ [0,1] → on the segment
               s < 0 or s > 1 → outside segment (no hit)
*/
bool raySegmentIntersect(Point origin, Point ray_dir, Point p1, Point p2){
    
    // seg_dir: Direction vector from p1 to p2
    Point seg_dir(p2.x - p1.x, p2.y - p1.y);
    
    // origin_to_p1: Vector from ray origin to segment start point p1
    Point origin_to_p1(p1.x - origin.x, p1.y - origin.y);
    
    // cross_dirs: Check if ray and segment are parallel
    // If cross(ray_dir, seg_dir) ≈ 0, they're parallel → no intersection
    double cross_dirs = cross2D(ray_dir, seg_dir);
    if(std::abs(cross_dirs) < 1e-10) return false;  // Parallel or anti-parallel
    
    // pos_ray (t): How far along the ray is the intersection?
    double pos_ray = cross2D(origin_to_p1, seg_dir) / cross_dirs;
    if (pos_ray < 1e-10) return false;  // Intersection is behind the ray origin
    
    // pos_seg (s): Where along the segment [p1, p2] is the intersection?
    double pos_seg = cross2D(origin_to_p1, ray_dir) / cross_dirs;
    if (pos_seg < 0 || pos_seg > 1) return false;  // Intersection outside segment endpoints
    
    return true;  // Ray hits the segment!
}

bool rayPolygonIntersect(Point origin, Point ray_dir, Polygon& poly){
    // Iterate through all edges of the polygon
    for(int i = 0; i < poly.vertices.size(); i++){
        Point p1 = poly.vertices[i];
        Point p2 = poly.vertices[(i+1) % poly.vertices.size()];  // Wrap around to first vertex
        
        if(raySegmentIntersect(origin, ray_dir, p1, p2)){
            return true;  // Ray hits this edge
        }
    }
    return false;  // Ray doesn't hit any edge
}

/*
Shoot rays in all directions from robot and count how many hit obstacles
ALGORITHM:
 Sample 'samples' equally-spaced angles around 360°
 For each angle, cast a ray
 Probability = (rays that hit) / (total rays)
*/
double calculateProbability(Point robot, std::vector<Polygon>& obstacles){
    int hits = 0;           // Counter for rays that hit obstacles
    int samples = 360;      // Number of ray directions to test (1° resolution)
    
    // Cast rays in all directions
    for(int i = 0; i < samples; i++){
        double angle = 2 * M_PI * i / samples;
        // ray_direction: Unit vector pointing in 'angle' direction
        // VISUAL:
        //        (0, 1)
        //          ↑
        //          | 90°
        //          |
        // 180° ←---*---→ 0° (1, 0)
        //          |
        //          | 270°
        //          ↓
        //       (0, -1)
        Point ray_direction(std::cos(angle), std::sin(angle));
        
        // Check if this ray hits any obstacle
        for(auto& obs : obstacles){
            if(rayPolygonIntersect(robot, ray_direction, obs)){
                hits += 1;  // This direction is blocked
                break;      // Don't double-count if ray hits multiple obstacles
            }
        }
    }
    
    // Return fraction of directions that are blocked
    // Example: if 90 out of 360 rays hit → 90/360 = 0.25 = 25% blocked
    return (double)hits / samples;
}

int main(){
    Point robot(0, 0);
    // Note: Vertices must be in order (clockwise or counter-clockwise)
    Polygon square;
    square.vertices = {Point(3, -1), Point(5, -1), Point(5, 1), Point(3, 1)};
    
    std::vector<Polygon> obstacles = {square};
    
    // Calculate what fraction of view is blocked
    double prob = calculateProbability(robot, obstacles);
    
    // Output results
    std::cout << "Probability: " << prob << std::endl;
    std::cout << "Blocked angle: " << prob * 2 * M_PI << " radians" << std::endl;
    std::cout << "Blocked angle: " << prob * 360 << " degrees" << std::endl;

    return 0;
}
