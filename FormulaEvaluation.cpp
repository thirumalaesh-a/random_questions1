#include <iostream>
#include <vector>
#include <string>
#include <unordered_map>
#include <unordered_set>
#include <sstream>
#include <algorithm>
#include <stdexcept>

using namespace std;

bool isNumber(const string& s) {
    if (s.empty()) return false;
    size_t start = (s[0] == '-') ? 1 : 0;
    if (start >= s.length()) return false;
    
    for (size_t i = start; i < s.length(); i++) {
        if (!isdigit(s[i])) return false;
    }
    return true;
}

vector<string> extractVariables(const string& expr) {
    vector<string> vars;
    istringstream iss(expr);
    string token;
    
    while (iss >> token) {
        if (token != "+" && token != "-" && !isNumber(token)) {
            vars.push_back(token);
        }
    }
    return vars;
}

int evaluateExpression(const string& expr, unordered_map<string, int>& values) {
    istringstream iss(expr);
    string token;
    int result = 0;
    int sign = 1;
    
    while (iss >> token) {
        if (token == "+") {
            sign = 1;
        } else if (token == "-") {
            sign = -1;
        } else {
            int value;
            if (isNumber(token)) {
                value = stoi(token);
            } else {
                if (values.find(token) == values.end()) {
                    throw runtime_error("Variable not found: " + token);
                }
                value = values[token];
            }
            result += sign * value;
        }
    }
    return result;
}

// DFS for topological sort with cycle detection
void dfs(const string& node, 
         unordered_map<string, vector<string>>& graph,
         unordered_set<string>& visited,
         unordered_set<string>& recStack,
         vector<string>& topoOrder) {
    
    if (recStack.find(node) != recStack.end()) {
        throw runtime_error("Cyclic dependency detected at variable: " + node);
    }
    
    if (visited.find(node) != visited.end()) {
        return;
    }
    
    visited.insert(node);
    recStack.insert(node);
    
    // Visit all dependencies first
    if (graph.find(node) != graph.end()) {
        for (const string& neighbor : graph[node]) {
            dfs(neighbor, graph, visited, recStack, topoOrder);
        }
    }
    
    recStack.erase(node);
    topoOrder.push_back(node);  // Add to order after all dependencies
}

unordered_map<string, int> solveFormulas(vector<string>& formulas) {
    unordered_map<string, int> values;           // Cache
    unordered_map<string, string> varToExpr;     // Variable -> expression
    unordered_map<string, vector<string>> graph; // Dependency graph
    unordered_set<string> allVars;
    
    // Parse formulas
    for (const string& formula : formulas) {
        size_t eqPos = formula.find('=');
        if (eqPos == string::npos) {
            throw runtime_error("Invalid formula: " + formula);
        }
        
        string var = formula.substr(0, eqPos);
        string expr = formula.substr(eqPos + 1);
        
        // Trim spaces
        var.erase(0, var.find_first_not_of(" \t"));
        var.erase(var.find_last_not_of(" \t") + 1);
        expr.erase(0, expr.find_first_not_of(" \t"));
        expr.erase(expr.find_last_not_of(" \t") + 1);
        
        varToExpr[var] = expr;
        allVars.insert(var);
    }
    
    // Build dependency graph (var depends on its expression variables)
    for (auto& [var, expr] : varToExpr) {
        vector<string> deps = extractVariables(expr);
        for (const string& dep : deps) {
            allVars.insert(dep);
            graph[var].push_back(dep);  // var depends on dep
        }
    }
    
    // Check for missing variables
    for (const string& var : allVars) {
        if (varToExpr.find(var) == varToExpr.end()) {
            throw runtime_error("Missing definition for variable: " + var);
        }
    }
    
    // DFS-based topological sort
    unordered_set<string> visited;
    unordered_set<string> recStack;
    vector<string> topoOrder;
    
    for (const string& var : allVars) {
        if (visited.find(var) == visited.end()) {
            dfs(var, graph, visited, recStack, topoOrder);
        }
    }
    
    // topoOrder now has variables in dependency order (dependencies first)
    // Evaluate in this order
    for (const string& var : topoOrder) {
        values[var] = evaluateExpression(varToExpr[var], values);
    }
    
    return values;
}

int main() {
    // Test case 1: Basic
    vector<string> formulas1 = {"a = b + 3", "b = 5"};
    try {
        auto result = solveFormulas(formulas1);
        cout << "Test 1:\n";
        for (auto& [var, val] : result) {
            cout << var << " = " << val << "\n";
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    
    // Test case 2: Complex dependencies
    vector<string> formulas2 = {"a = b + c", "b = 5", "c = d - 2", "d = 10"};
    try {
        auto result = solveFormulas(formulas2);
        cout << "\nTest 2:\n";
        for (auto& [var, val] : result) {
            cout << var << " = " << val << "\n";
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    
    // Test case 3: Cyclic dependency
    vector<string> formulas3 = {"a = b + 1", "b = c + 1", "c = a + 1"};
    try {
        auto result = solveFormulas(formulas3);
        cout << "\nTest 3:\n";
        for (auto& [var, val] : result) {
            cout << var << " = " << val << "\n";
        }
    } catch (const exception& e) {
        cout << "\nTest 3 Error: " << e.what() << "\n";
    }
    
    // Test case 4: Missing variable
    vector<string> formulas4 = {"a = b + 3"};
    try {
        auto result = solveFormulas(formulas4);
        cout << "\nTest 4:\n";
        for (auto& [var, val] : result) {
            cout << var << " = " << val << "\n";
        }
    } catch (const exception& e) {
        cout << "\nTest 4 Error: " << e.what() << "\n";
    }
    
    // Test case 5: Multiple independent chains
    vector<string> formulas5 = {"a = b + 1", "b = 2", "x = y - 3", "y = 10"};
    try {
        auto result = solveFormulas(formulas5);
        cout << "\nTest 5:\n";
        for (auto& [var, val] : result) {
            cout << var << " = " << val << "\n";
        }
    } catch (const exception& e) {
        cout << "Error: " << e.what() << "\n";
    }
    
    return 0;
}