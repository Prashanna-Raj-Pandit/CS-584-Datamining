/*
Author Name: 

Prashanna Raj Pandit (800817018)
Asha Shah(800817984)
--------------------------------------------------------------------------------------------------------
Program Information: A C++ program to read the data inside ARFF file, store them, and print the data. 
This program will print the list of attributes followed by their values and then print the table data.
---------------------------------------------------------------------------------------------------------
Submitted on: Feb 11, 2024
---------------------------------------------------------------------------------------------------------
Deadline: Feb 18, 2024, 10:10 PM
---------------------------------------------------------------------------------------------------------
Message to grader: 
Features Added
1. Infinite Loop for Continuous User Input: Users can keep entering file names without restarting the program.
   Entering 0 exits the program.
2. Saves Output to a File:  Generates an output file named provided_filename.out. Stores the attributes and data table in the file.
3. Improved Error Handling: Displays an error message if a file cannot be opened.
Handles incorrect data row sizes properly.
4. Data Structures Used:
vector<Attribute>:Vector of Structs . This is used to store a list of attributes, where each attribute has a name and a list of possible values.
Attribute is a custom struct that holds both the name and values of an attribute.
Advantages:
Efficient indexing and retrieval of attributes.
Dynamic resizing allows flexible storage of attributes.

vector<vector<string>>: This is used to store the data extracted from the ARFF file. Each data row is stored as a vector<string>, 
and the entire data table is stored as a 2D vector.
Advantages:
Dynamic size management.
Easy row-wise access and modification.
Preserves original data format (string-based) for flexible processing

To compile: g++ -std=c++17 -o ca ca2.cpp
To Run: ./ca
---------------------------------------------------------------------------------------------------------
*/

#include <iostream>       
#include <fstream>        
#include <vector>         
#include <string>         
#include <sstream>        
#include <algorithm>      
#include <iomanip>        
using namespace std;      

// Function to trim leading and trailing whitespace
string trim(const string &str) {
    size_t first = str.find_first_not_of(" \t\r\n");  // Find first non-whitespace character
    if (first == string::npos) return "";  // Return empty string if no non-whitespace character is found
    size_t last = str.find_last_not_of(" \t\r\n");  // Find last non-whitespace character
    return str.substr(first, last - first + 1);  // Return substring between first and last non-whitespace character
}

// Struct to store attribute information
struct Attribute {
    string name;                  // Name of the attribute
    vector<string> values;        // List of possible values for the attribute
};

// Function to split a string by a delimiter
vector<string> split(const string &s, char delimiter) {
    vector<string> tokens;         // A vector to hold the split tokens
    string token;                  // Temporary string to hold each token
    istringstream tokenStream(s);  // Create a stringstream from the input string
    while (getline(tokenStream, token, delimiter)) {  // Split string by delimiter
        token = trim(token);  // Trim each token to remove leading/trailing spaces
        tokens.push_back(token);  // Add the token to the vector
    }
    return tokens;  // Return the vector of tokens
}

// Function to read ARFF file and correctly map data
bool readARFF(const string& filename, vector<Attribute>& attributes, vector<vector<string>>& data) {
    ifstream file(filename);  // Open the file for reading
    if (!file.is_open()) {    // Check if file was successfully opened
        cerr << "Error opening file: " << filename << endl;  // Print error if file can't be opened
        return false;  // Return false if file opening failed
    }

    string line;            // Variable to store each line from the file
    bool dataSection = false; // Flag to check if we are in the data section of ARFF file

    // Read the file line by line
    while (getline(file, line)) {
        line = trim(line);  // Trim the line to remove leading/trailing spaces
        if (line.empty() || line[0] == '%') continue;  // Skip comments and empty lines

        // Parse @ATTRIBUTE lines to extract attribute information
        if (line.find("@ATTRIBUTE") == 0 || line.find("@attribute") == 0) {
            istringstream iss(line);  // Create a string stream from the line
            string token, attrName, values;
            iss >> token;  // Skip "@ATTRIBUTE"

            // Extract attribute name (including hyphens)
            size_t braceStart = line.find('{');  // Check if there are possible attribute values
            if (braceStart != string::npos) {
                attrName = trim(line.substr(line.find(' ') + 1, braceStart - line.find(' ') - 1));
            } else {
                iss >> attrName;  // Just extract the attribute name
            }

            Attribute attr;            // Create an attribute object
            attr.name = attrName;      // Set the name of the attribute

            // Extract possible attribute values inside {}
            size_t braceEnd = line.find('}');  // Find closing brace
            if (braceStart != string::npos && braceEnd != string::npos) {
                string valuesStr = line.substr(braceStart + 1, braceEnd - braceStart - 1);
                attr.values = split(valuesStr, ',');  // Split the values by comma and store in the attribute
            }
            attributes.push_back(attr);  // Add the attribute to the vector of attributes
        } 
        // Parse @DATA section and extract the data
        else if (line.find("@DATA") == 0 || line.find("@data") == 0) {
            dataSection = true;  // Set the flag to true when data section begins
        } 
        else if (dataSection) {  // Read data rows after entering the data section
            vector<string> row = split(line, ',');  // Split each data row by commas
            if (row.size() == attributes.size()) {  // Ensure the row has the correct number of attributes
                data.push_back(row);  // Add the row to the data vector
            } else {
                cerr << "Warning: Data row has " << row.size() << " values, expected " << attributes.size() << "." << endl;
            }
        }
    }

    file.close();  // Close the file after reading
    return true;   // Return true if reading was successful
}

// Function to print attributes and save to file
void printAttributes(const vector<Attribute>& attributes, const vector<vector<string>>& data, ofstream& outFile) {
    cout << "Attributes: " << attributes.size() << endl;  // Print the number of attributes
    cout << "Examples: " << data.size() << endl << endl;  // Print the number of data rows

    outFile << "Attributes: " << attributes.size() << endl;  // Write number of attributes to output file
    outFile << "Examples: " << data.size() << endl << endl;  // Write number of data rows to output file

    // Iterate through the attributes and print their names and values
    for (const auto& attr : attributes) {
        cout << attr.name << "(" << attr.values.size() << "): ";  // Print attribute name and number of values
        outFile << attr.name << "(" << attr.values.size() << "): ";  // Write to file

        // Print values for the attribute
        for (size_t i = 0; i < attr.values.size(); ++i) {
            cout << attr.values[i];  // Print each value
            outFile << attr.values[i];  // Write each value to file

            if (i < attr.values.size() - 1) {
                cout << " ";  // Print space between values
                outFile << " ";  // Write space to file
            }
        }
        cout << endl;  // Newline after each attribute's values
        outFile << endl;  // Newline after writing to file
    }
    cout << endl;  // Add extra newline
    outFile << endl;  // Add extra newline to the file
}

// Function to print only the data table and save it to file
void printData(const vector<vector<string>>& data, ofstream& outFile) {
    cout << "Data Table:" << endl;  // Print the header for the data table
    outFile << "Data Table:" << endl;  // Write to file

    // Iterate through data rows and print each one
    for (const auto& row : data) {
        for (const auto& val : row) {
            cout << setw(15) << left << val;  // Format output in a table (adjust width)
            outFile << setw(15) << left << val;  // Write to file in the same format
        }
        cout << endl;  // Newline after each row
        outFile << endl;  // Write newline to file
    }
}

int main() {
    while (true) {  // Infinite loop to allow multiple file inputs
        string filename;
        cout << "\nEnter the ARFF filename (or enter 0 to exit): ";  // Ask for file input
        cin >> filename;  // Read the filename

        if (filename == "0") {  // Exit condition if user inputs "0"
            cout << "Exiting program..." << endl;
            break;  // Exit loop
        }

        vector<Attribute> attributes;  // Vector to store attributes
        vector<vector<string>> data;  // 2D vector to store data (each row is a vector of strings)

        if (!readARFF(filename, attributes, data)) {  // Attempt to read the ARFF file
            continue;  // If file reading fails, restart loop
        }

        // Create output filename (same as input but with ".out")
        string outputFilename = filename + ".out";  
        ofstream outFile(outputFilename);  // Open output file for writing
        if (!outFile.is_open()) {  // Check if output file was successfully opened
            cerr << "Error creating output file: " << outputFilename << endl;  // Error if not opened
            continue;  // Restart loop if file creation fails
        }

        printAttributes(attributes, data, outFile);  // Print and save attribute information
        printData(data, outFile);  // Print and save data table

        outFile.close();  // Close output file
        cout << "\nOutput saved to: " << outputFilename << endl;  // Inform user of the output location
    }

    return 0;  // Return 0 to indicate successful execution
}
