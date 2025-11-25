/*
 * VEHICLE COLLISION DETECTION SIMULATION
 * ======================================
 * 
 * APPROACH:
 * 1. Vehicle Shape: Circle (radius r)
 *    - Simplest collision detection: just check if distance between centers <= r1 + r2
 *    - O(1) collision check vs O(n) for polygons
 * 
 * 2. Physics Model: Constant velocity
 *    - x(t+dt) = x(t) + dx * dt
 *    - y(t+dt) = y(t) + dy * dt
 *    - No acceleration, straight-line motion
 * 
 * 3. Collision Detection:
 *    - Calculate distance between vehicle centers: d = sqrt((x2-x1)^2 + (y2-y1)^2)
 *    - Collision occurs when: d <= r1 + r2
 * 
 * 4. Non-collision scenarios:
 *    - Vehicles moving in parallel (same direction, offset positions)
 *    - Vehicles moving away from each other
 *    - Trajectories that don't intersect
 * 
 * ALGORITHM:
 * - Loop through time in steps of dt
 * - At each timestep:
 *   a) Check all pairs of vehicles for collision
 *   b) If collision found, return current time
 *   c) Update all vehicle positions: position += velocity * dt
 * - If no collision by max_time, return -1
 * 
 * COMPLEXITY:
 * - Time: O(n^2 * T/dt) where n = number of vehicles, T = collision time
 * - Space: O(n)
 */

#include <iostream>
#include <vector>
#include <cmath>

using namespace std;

// Vehicle structure based on input format: (object_id, initial_x, initial_y, dx, dy)
struct Vehicle {
    int id;         // unique identifier
    double x, y;    // current position
    double dx, dy;  // velocity components (units per second)
    double r;       // collision radius
};

/**
 * Check if two circular vehicles are colliding
 * 
 * Collision occurs when distance between centers <= sum of radii
 * Uses Euclidean distance formula: d = sqrt((x2-x1)^2 + (y2-y1)^2)
 * 
 * @param a First vehicle
 * @param b Second vehicle
 * @return true if vehicles are colliding, false otherwise
 */
bool collides(const Vehicle& a, const Vehicle& b) {
    // Calculate distance between centers
    double dist_x = b.x - a.x;
    double dist_y = b.y - a.y;
    double dist = sqrt(dist_x*dist_x + dist_y*dist_y);
    
    // Check if distance is less than or equal to sum of radii
    return dist <= (a.r + b.r);
}

/**
 * Simulate vehicles and find the first collision time
 * 
 * Algorithm:
 * 1. Start at t=0
 * 2. Check all pairs of vehicles for collision
 * 3. If collision found, return current time
 * 4. Update all vehicle positions by velocity * dt
 * 5. Increment time by dt
 * 6. Repeat until collision or max_time reached
 * 
 * @param vehicles List of vehicles to simulate (modified in-place)
 * @param dt Timestep duration (seconds)
 * @param max_time Maximum simulation time (seconds)
 * @return Time of first collision, or -1 if no collision occurs
 */
double findFirstCollision(vector<Vehicle>& vehicles, double dt, double max_time) {
    double t = 0;  // Current simulation time
    
    // Main simulation loop
    while (t < max_time) {
        // Check all pairs for collision (O(n^2) per timestep)
        for (size_t i = 0; i < vehicles.size(); i++) {
            for (size_t j = i + 1; j < vehicles.size(); j++) {
                if (collides(vehicles[i], vehicles[j])) {
                    printf("Collision at t=%.2f between vehicles %d and %d\n", 
                           t, vehicles[i].id, vehicles[j].id);
                    return t;
                }
            }
        }
        
        // Update positions of all vehicles (Euler integration)
        // New position = old position + velocity * timestep
        for (auto& v : vehicles) {
            v.x += v.dx * dt;
            v.y += v.dy * dt;
        }
        
        // Advance time
        t += dt;
    }
    
    // No collision found within time limit
    printf("No collision within %.1fs\n", max_time);
    return -1;
}

/**
 * Test cases demonstrating the collision detection
 */
int main() {
    // Simulation parameters (given by interviewer)
    double dt = 0.1;  // Timestep: 0.1 seconds
    double r = 0.5;   // Vehicle radius: 0.5 units
    
    printf("========================================\n");
    printf("VEHICLE COLLISION DETECTION\n");
    printf("========================================\n");
    printf("dt = %.1fs, radius = %.1f units\n\n", dt, r);
    
    // Test 1: Head-on collision
    // Two vehicles moving directly toward each other
    printf("Test 1: Head-on collision\n");
    printf("---------------------------\n");
    vector<Vehicle> v1 = {
        {1, 0.0, 0.0, 1.0, 0.0, r},   // Vehicle 1: at origin, moving right (dx=1.0)
        {2, 5.0, 0.0, -1.0, 0.0, r}   // Vehicle 2: at x=5, moving left (dx=-1.0)
    };
    findFirstCollision(v1, dt, 10.0);
    
    // Test 2: T-bone collision
    // One vehicle moving horizontally, another moving vertically
    printf("\nTest 2: T-bone collision\n");
    printf("---------------------------\n");
    vector<Vehicle> v2 = {
        {1, 0.0, 0.0, 2.0, 0.0, r},   // Vehicle 1: moving right at 2.0 units/s
        {2, 3.0, -3.0, 0.0, 2.0, r}   // Vehicle 2: moving up at 2.0 units/s
    };
    findFirstCollision(v2, dt, 10.0);
    
    // Test 3: Near miss - no collision
    // Vehicles pass by each other with enough separation
    printf("\nTest 3: Near miss (no collision)\n");
    printf("---------------------------\n");
    vector<Vehicle> v3 = {
        {1, 0.0, 0.0, 1.0, 0.0, r},   // Vehicle 1: at y=0, moving right
        {2, 5.0, 2.0, -1.0, 0.0, r}   // Vehicle 2: at y=2, moving left (offset by 2 units)
    };
    findFirstCollision(v3, dt, 10.0);
    
    printf("\n========================================\n");
    
    return 0;
}