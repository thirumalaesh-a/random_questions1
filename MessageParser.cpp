#include <iostream>
#include <string>
#include <unordered_map>
#include <sstream>
#include <vector>

class MessageParser {
private:
    // Store message name -> list of (type, field_name) pairs
    std::unordered_map<std::string, std::vector<std::string>> messages;
    
    // Cache computed sizes to avoid recalculation
    mutable std::unordered_map<std::string, int> size_cache;
    
    // Get size of primitive types
    int get_primitive_size(const std::string& type) const {
        if (type == "float") return 4;
        if (type == "int") return 4;
        if (type == "string") return 256;
        return -1; // Not a primitive
    }
    
    // Recursive size calculation with cycle detection
    int calculate_size(const std::string& type, 
                      std::unordered_map<std::string, bool>& visiting) const {
        // Check if already computed
        if (size_cache.count(type)) {
            return size_cache[type];
        }
        
        // Check if it's a primitive type
        int prim_size = get_primitive_size(type);
        if (prim_size != -1) {
            return prim_size;
        }
        
        // Check if message exists
        if (!messages.count(type)) {
            throw std::runtime_error("Unknown type: " + type);
        }
        
        // Cycle detection: if we're currently visiting this type, it's circular
        if (visiting[type]) {
            throw std::runtime_error("Circular dependency detected: " + type);
        }
        
        // Mark as visiting
        visiting[type] = true;
        
        // Calculate total size by summing all field sizes
        int total = 0;
        for (const auto& field_type : messages.at(type)) {
            total += calculate_size(field_type, visiting);
        }
        
        // Unmark visiting (backtrack)
        visiting[type] = false;
        
        // Cache result
        size_cache[type] = total;
        return total;
    }

public:
    // Parse the input and build message definitions
    void parse(const std::string& input) {
        std::istringstream stream(input);
        std::string line;
        std::string current_message;
        
        while (std::getline(stream, line)) {
            // Skip empty lines and comments
            if (line.empty() || line.find("//") == 0) continue;
            
            std::istringstream line_stream(line);
            std::string first_word;
            line_stream >> first_word;
            
            // New message definition
            if (first_word == "Message") {
                std::string msg_name;
                line_stream >> msg_name;
                current_message = msg_name;
                messages[current_message] = {}; // Initialize empty
            }
            // Field definition: "type field_name"
            else if (!current_message.empty()) {
                std::string field_name;
                line_stream >> field_name;
                // Store only the type (first_word is the type)
                messages[current_message].push_back(first_word);
            }
        }
    }
    
    // Public API: Get size of a message type
    int get_size(const std::string& type) const {
        std::unordered_map<std::string, bool> visiting;
        return calculate_size(type, visiting);
    }
};

// Test the parser
int main() {
    MessageParser parser;
    
    // Test Case 1: Simple primitives
    std::string input1 = R"(
Message Vehicle
float x_position
float y_position
float velocity
float acceleration
int num_wheels
string name

Message Vector2d
float x_position
float y_position
)";
    
    parser.parse(input1);
    std::cout << "Test 1 - Simple Messages:\n";
    std::cout << "Vehicle: " << parser.get_size("Vehicle") << " bytes\n";     // 276
    std::cout << "Vector2d: " << parser.get_size("Vector2d") << " bytes\n";   // 8
    std::cout << "float: " << parser.get_size("float") << " bytes\n\n";       // 4
    
    // Test Case 2: Nested messages
    MessageParser parser2;
    std::string input2 = R"(
Message Vehicle
Pose pose
float velocity
float acceleration
int num_wheels
string name

Message Pose
Vector2d Position
float yaw

Message Vector2d
float px
float py
)";
    
    parser2.parse(input2);
    std::cout << "Test 2 - Nested Messages:\n";
    std::cout << "Vector2d: " << parser2.get_size("Vector2d") << " bytes\n";  // 8
    std::cout << "Pose: " << parser2.get_size("Pose") << " bytes\n";          // 12
    std::cout << "Vehicle: " << parser2.get_size("Vehicle") << " bytes\n";    // 280
    std::cout << "float: " << parser2.get_size("float") << " bytes\n";        // 4
    
    return 0;
}