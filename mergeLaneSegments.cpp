/*
LANE SEGMENT MERGING FOR COLLINEAR SEGMENTS

PROBLEM: Merge overlapping lane segments that lie on the same line
- Input: Multiple lane segments (sequences of points)
- Output: Merged segments where collinear overlapping lanes are combined
- Handles both vertical and non-vertical lines

BRUTE FORCE APPROACH:
- Compare every segment with every other segment for overlap
- Check point-by-point if segments are collinear
- Time Complexity: O(n²·m) where n = segments, m = points per segment
- Space Complexity: O(n·m)

OPTIMIZED APPROACH (Current Implementation):
- Group segments by their line equation (slope-intercept form)
- Use hash map for O(1) line lookup
- Merge segments on same line by collecting unique points
- Time Complexity: O(n·m·log(m)) due to sorting points
- Space Complexity: O(n·m) for storing grouped segments
*/

#include <vector>
#include <algorithm>
#include <unordered_map>
#include <cmath>

// ============================================================================
// BASIC DATA STRUCTURES
// ============================================================================

// 2D point with x and y coordinates
struct Point {
    double x, y;
    // Constructor to initialize point
    Point(double x_, double y_) : x(x_), y(y_) {}
};

// A segment is just a list of points that form a lane
using Segment = std::vector<Point>;

// ============================================================================
// LINE REPRESENTATION
// ============================================================================

// Represents a line using equation: y = slope*x + intercept
// Special handling for vertical lines (infinite slope)
struct Line {
    double slope;          // m in y = mx + b
    double intercept;      // b in y = mx + b
    bool is_vertical;      // true if line is vertical (x = constant)
    double x_vertical;     // x-coordinate if line is vertical
    
    // Constructor
    Line(double m, double b, bool v, double x) 
        : slope(m), intercept(b), is_vertical(v), x_vertical(x) {}
    
    // Check if two lines are equal (with small tolerance for floating point)
    bool operator==(const Line& other) const {
        // Different types of lines (one vertical, one not) are not equal
        if (is_vertical != other.is_vertical) 
            return false;
        
        // For vertical lines, compare x-coordinates
        if (is_vertical) 
            return std::abs(x_vertical - other.x_vertical) < 1e-9;
        
        // For non-vertical lines, compare slope and intercept
        return std::abs(slope - other.slope) < 1e-9 && 
               std::abs(intercept - other.intercept) < 1e-9;
    }
};

// ============================================================================
// HASH FUNCTION FOR LINE (needed for unordered_map)
// ============================================================================

struct LineHash {
    size_t operator()(const Line& line) const {
        // Hash vertical lines by their x-coordinate
        if (line.is_vertical) {
            return std::hash<double>()(line.x_vertical);
        }
        // Hash non-vertical lines by combining slope and intercept
        return std::hash<double>()(line.slope) ^ std::hash<double>()(line.intercept);
    }
};

// ============================================================================
// HELPER FUNCTIONS
// ============================================================================

// Calculate the line equation from a segment's start and end points
// Returns: Line object representing the segment
// Time: O(1)
Line getLine(const Segment& seg) {
    double x1 = seg[0].x;        // First point x
    double y1 = seg[0].y;        // First point y
    double x2 = seg.back().x;    // Last point x
    double y2 = seg.back().y;    // Last point y
    
    double dx = x2 - x1;  // Change in x
    
    // Check if line is vertical (no change in x)
    if (std::abs(dx) < 1e-9) {
        // Return vertical line at x = x1
        return Line{0, 0, true, x1};
    }
    
    // Calculate slope: rise over run (change in y / change in x)
    double slope = (y2 - y1) / dx;
    
    // Calculate y-intercept using: b = y - mx
    double intercept = y1 - slope * x1;
    
    // Return non-vertical line
    return Line{slope, intercept, false, 0};
}

// Check if a point lies on a given segment
// Returns: true if point is on the line AND within segment bounds
// Time: O(1)
bool isPointOnSegment(const Point& p, const Segment& seg, const Line& line) {
    // Handle vertical lines separately
    if (line.is_vertical) {
        // Check if point's x matches the vertical line's x
        bool on_vertical_line = std::abs(p.x - line.x_vertical) < 1e-9;
        
        // Check if point's y is within segment's y-range
        double min_y = std::min(seg[0].y, seg.back().y);
        double max_y = std::max(seg[0].y, seg.back().y);
        bool within_y_bounds = (min_y - 1e-9 <= p.y) && (p.y <= max_y + 1e-9);
        
        return on_vertical_line && within_y_bounds;
    }
    
    // For non-vertical lines:
    // Calculate expected y-value at point's x using line equation
    double expected_y = line.slope * p.x + line.intercept;
    
    // Check if point's actual y matches expected y (within tolerance)
    bool on_line = std::abs(expected_y - p.y) < 1e-9;
    if (!on_line) 
        return false;
    
    // Check if point's x is within segment's x-range
    double min_x = std::min(seg[0].x, seg.back().x);
    double max_x = std::max(seg[0].x, seg.back().x);
    bool within_x_bounds = (min_x - 1e-9 <= p.x) && (p.x <= max_x + 1e-9);
    
    return within_x_bounds;
}

// Merge multiple segments that lie on the same line
// Returns: Single merged segment containing all relevant points
// Time: O(n·m·log(m)) where n = number of segments, m = total points
Segment mergeSegments(const std::vector<Segment>& segments) {
    // Step 1: Collect all endpoints from all segments
    std::vector<Point> points;
    for (const auto& seg : segments) {
        points.push_back(seg[0]);        // Add first point
        points.push_back(seg.back());    // Add last point
    }
    
    // Step 2: Sort points by x-coordinate (then by y if x is same)
    std::sort(points.begin(), points.end(), 
        [](const Point& a, const Point& b) {
            // If x-coordinates are equal, compare y-coordinates
            if (a.x == b.x) 
                return a.y < b.y;
            // Otherwise compare x-coordinates
            return a.x < b.x;
        }
    );
    
    // Step 3: Remove duplicate points
    points.erase(
        std::unique(points.begin(), points.end(), 
            [](const Point& a, const Point& b) {
                // Two points are duplicates if both x and y are very close
                return std::abs(a.x - b.x) < 1e-9 && 
                       std::abs(a.y - b.y) < 1e-9;
            }
        ), 
        points.end()
    );

    // Step 4: Build merged segment from points that lie on any input segment
    Segment result;
    for (const auto& p : points) {
        // Check if this point lies on at least one of the original segments
        for (const auto& seg : segments) {
            if (isPointOnSegment(p, seg, getLine(seg))) {
                result.push_back(p);
                break;  // Found one segment containing this point, that's enough
            }
        }
    }
    
    return result;
}

// ============================================================================
// MAIN FUNCTION: Merge all lanes
// ============================================================================

// Group lanes by their line equation and merge overlapping segments
// Returns: Vector of merged segments (one per unique line)
// Time: O(n·m·log(m))
std::vector<Segment> mergeLanes(const std::vector<Segment>& lanes) {
    // Step 1: Create a hash map to group segments by their line
    // Key = Line equation, Value = all segments on that line
    std::unordered_map<Line, std::vector<Segment>, LineHash> line_groups;
    
    // Step 2: Group each lane by its line equation
    for (const auto& lane : lanes) {
        // Skip invalid segments (need at least 2 points to form a line)
        if (lane.size() < 2) 
            continue;
        
        // Calculate line for this lane and add to appropriate group
        Line lane_line = getLine(lane);
        line_groups[lane_line].push_back(lane);
    }

    // Step 3: Merge segments for each unique line
    std::vector<Segment> result;
    for (const auto& [line, segments] : line_groups) {
        // Merge all segments on this line
        Segment merged = mergeSegments(segments);
        
        // Add to result if merge produced valid segment
        if (!merged.empty()) {
            result.push_back(merged);
        }
    }
    
    return result;
}