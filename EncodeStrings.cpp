/*
MINIMAL WORD ENCODING WITH CONFLICT RESOLUTION

PROBLEM: Encode words in format prefix + middleCount + lastChar, minimizing prefix length
- Input: ["bobble", "boggle"]
- Encoding format: prefix + number_of_middle_chars + last_char
- Goal: Find shortest unique encoding for each word
- Example: "bobble" → "bob2e" (bob + 2 middle chars + e)

BRUTE FORCE APPROACH:
- Try all possible prefix lengths for each word (1 to word.length-1)
- Check uniqueness of each combination across all words
- Time Complexity: O(n² × m) where n = number of words, m = average word length
- Space Complexity: O(n × m)

OPTIMIZED APPROACH (Current Implementation):
- Start all words with prefix length = 1
- Iteratively increase prefix only for conflicting encodings
- Stop when all encodings are unique
- Time Complexity: O(n × m × k) where k = iterations needed (usually small)
- Space Complexity: O(n × m) for tracking conflicts and results
*/

#include <string>
#include <vector>
#include <unordered_map>
#include <unordered_set>
#include <iostream>
// Encode a single word with specified prefix length
// Returns: prefix + middleCount + lastChar format
// Time: O(m) where m = word length
std::string encode(const std::string& word, int prefixLen) {
    // Words with 2 or fewer characters can't be encoded (need at least prefix + last)
    if (word.length() <= 2) 
        return word;
    
    // Index of last character
    int suffixStart = word.length() - 1;
    
    // Count characters between prefix and last char
    // Example: "bobble" (6 chars), prefixLen=3
    // middleCount = 6 - 1 (last 'e') - 3 (prefix 'bob') = 2 (chars 'bl')
    int middleCount = suffixStart - prefixLen;
    
    // If no middle characters exist, return word as-is
    if (middleCount <= 0) 
        return word;
    
    // Build encoded string: prefix + count + last_character
    // Example: "bobble" with prefixLen=3
    // → "bob" (prefix) + "2" (middle count) + "e" (last char) = "bob2e"
    return word.substr(0, prefixLen) + std::to_string(middleCount) + word.back();
}

// Find minimal encoding for all words by resolving conflicts iteratively
// Returns: map of encoding → original word
// Time: O(n × m × k) where n = words, m = avg length, k = iterations
std::unordered_map<std::string, std::string> minimalEncode(const std::vector<std::string>& words) {
    // Final result: encoding → original word mapping
    std::unordered_map<std::string, std::string> result;
    
    // Track current prefix length being used for each word
    std::unordered_map<std::string, int> prefixLen;
    
    // Track conflicts: encoding → list of words that produce this encoding
    std::unordered_map<std::string, std::vector<std::string>> conflicts;
    
    // ========================================================================
    // STEP 1: Initialize all words with minimum prefix length (1 character)
    // ========================================================================
    for (const auto& word : words) {
        // Start each word with single-character prefix
        prefixLen[word] = 1;
        
        // Encode word with prefix length 1
        std::string encoded = encode(word, 1);
        
        // Group words by their encoding to detect conflicts
        // Multiple words with same encoding = conflict
        conflicts[encoded].push_back(word);
    }
    
    // ========================================================================
    // STEP 2: Iteratively resolve conflicts by increasing prefix length
    // ========================================================================
    bool hasConflicts = true;  // Flag to continue loop while conflicts exist
    
    while (hasConflicts) {
        hasConflicts = false;  // Assume no conflicts until we find one
        
        // Prepare fresh conflicts map for this iteration
        std::unordered_map<std::string, std::vector<std::string>> newConflicts;
        
        // Examine each encoding and its associated words
        for (const auto& [encoded, wordList] : conflicts) {
            // Conflict exists if multiple words share same encoding
            if (wordList.size() > 1) {
                hasConflicts = true;  // Found conflict, need another iteration
                
                // Resolve conflict by expanding prefix for all conflicting words
                for (const auto& word : wordList) {
                    // Increase prefix length by 1
                    prefixLen[word]++;
                    
                    // Re-encode with longer prefix
                    std::string newEncoded = encode(word, prefixLen[word]);
                    
                    // Add to new conflicts map (may create new conflicts or resolve old ones)
                    newConflicts[newEncoded].push_back(word);
                }
            } 
            // No conflict: only one word maps to this encoding
            else {
                // Keep this encoding as-is (no changes needed)
                newConflicts[encoded] = wordList;
            }
        }
        
        // Update conflicts map for next iteration
        conflicts = std::move(newConflicts);
    }
    
    // ========================================================================
    // STEP 3: Build final result from resolved encodings
    // ========================================================================
    for (const auto& [encoded, wordList] : conflicts) {
        // Each encoding should now map to exactly one word
        if (!wordList.empty()) {
            result[encoded] = wordList[0];
        }
    }
    
    return result;
}

/*
DETAILED EXAMPLE WALKTHROUGH:
Input: ["bobble", "boggle"]

ITERATION 1 (prefixLen = 1):
- "bobble": b + 4 middle chars (obbl) + e = "b4e"
- "boggle": b + 4 middle chars (oggl) + e = "b4e"
- CONFLICT! Both map to "b4e"
- conflicts["b4e"] = ["bobble", "boggle"]

ITERATION 2 (prefixLen = 2):
- "bobble": bo + 3 middle chars (bbl) + e = "bo3e"
- "boggle": bo + 3 middle chars (ggl) + e = "bo3e"
- CONFLICT! Both still map to "bo3e"
- conflicts["bo3e"] = ["bobble", "boggle"]

ITERATION 3 (prefixLen = 3):
- "bobble": bob + 2 middle chars (bl) + e = "bob2e"
- "boggle": bog + 2 middle chars (gl) + e = "bog2e"
- NO CONFLICT! Encodings are unique!
- conflicts["bob2e"] = ["bobble"]
- conflicts["bog2e"] = ["boggle"]

Final Result:
- "bob2e" → "bobble"
- "bog2e" → "boggle"

WHY THIS WORKS:
- Start with minimal prefix (1 char) for shortest encoding
- Only increase prefix when conflicts occur
- Guarantees minimal encoding length for each word
- Terminates when all encodings are unique
*/



int main() {
    // Test with example words
    std::vector<std::string> words = {"bobble", "boggle"};
    
    std::cout << "=== MINIMAL WORD ENCODING ===\n";
    std::cout << "Input words: bobble, boggle\n\n";
    
    // Run the encoding algorithm
    auto result = minimalEncode(words);
    
    std::cout << "=== FINAL ENCODINGS ===\n";
    for (const auto& [enc, word] : result) {
        std::cout << enc << " -> " << word << "\n";
    }
    
    return 0;
}