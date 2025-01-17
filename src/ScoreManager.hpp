#pragma once

#include <fstream>
#include <iostream>
#include <map>
#include <string>

// Maps the number of sides to the name of the shape
// e.g., 3 -> "triangle", 4 -> "square", etc.

std::string getShapeName(int sides) {
    static const std::map<int, std::string> shapeNames = {
        {3, "triangle"},
        {4, "square"},
        {5, "pentagon"},
        {6, "hexagon"},
        {7, "heptagon"},
        {8, "octagon"}
    };

    // Look up the shape name for the given number of sides
    auto it = shapeNames.find(sides);
    if (it != shapeNames.end()) {
        return it->second; // Return the name if found
    }
    return "unknown"; // Fallback if the number of sides is not recognized
}

// Loads the best scores from a file into the provided map
// The file should contain lines in the format: "shapeName score"
// Example: "triangle 3000"
void loadBestScores(const std::string& filename, std::map<std::string, int>& bestScores) {
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
        return;
    }

    // Read each line from the file and populate the map
    std::string shape;
    int score;
    while (file >> shape >> score) {
        bestScores[shape] = score; // Store the score for the shape
    }

    file.close(); // Close the file when done
}

// Saves the current best scores to a file
// The file will be overwritten with lines in the format: "shapeName score"
// Example: "triangle 3000"
void saveBestScores(const std::string& filename, const std::map<std::string, int>& bestScores) {
    std::ofstream file(filename, std::ios::trunc);
    if (!file.is_open()) {
        std::cerr << "Unable to open file: " << filename << "\n";
        return;
    }

    // Write each shape and its best score to the file
    for (const auto& [shape, score] : bestScores) {
        file << shape << " " << score << "\n"; // Format: shapeName score
    }

    file.close(); // Close the file when done
}
