#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <algorithm>
using std::cout;
using std::endl;
using std::ifstream;
using std::ios;
using std::reverse;
using std::string;

// read content of file and store it in a string
string read_file(string filename)
{
    string data;
    ifstream input(filename);
    if (input)
    {
        input.seekg(0, ios::end); // go to the end of the file
        data.resize(input.tellg()); // resize the string to fit the file size
        input.seekg(0, ios::beg); // go back to the beginning of the file
        input.read(&data[0], data.size()); // read the file content into the string
        input.close();
    }
    return data;
}

// predict the next char based on the previous k characters
char predict_char(const std::vector<int> &positions, std::vector<std::string> &chunks)
{
    // get first position of positions array
    int first_pos = positions.front();

    // return the last character of the chunk immediately following the first position
    return chunks[first_pos + 1].back();
}

// count the number of distinct characters
int count_distinct_chars(const string &str)
{
    std::set<char> distinct_chars(str.begin(), str.end());
    return distinct_chars.size();
}


int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        cout << "Usage: " << argv[0] << " <reference_filename> <target_filename> <k> <alpha> <fail_threshold>" << endl;
        return 1;
    }

    // parse command-line arguments
    string reference_filename = argv[1];
    string target_filename = argv[2];
    int k = std::stoi(argv[3]);
    double alpha = atof(argv[4]);
    int fail_threshold = std::stoi(argv[5]);

    // read reference and target files
    string reference_data = read_file(reference_filename);
    string target_data = read_file(target_filename); // text to be analyzed

    // same logic as in cpm but applied to the reference data
    std::map<std::string, std::vector<int>> chunk_positions;
    std::vector<std::string> chunks;
    int hits = 0;
    int fails = 0;
    double estimated_bits = 0;
    double prob = 0;

    // get the alphabet cardinality
    int N = count_distinct_chars(reference_data);

    // create copy model using the reference data
    for (int i = 0; i < reference_data.size() - k; ++i)
    {
        std::string current_chunk = reference_data.substr(i, k);
        chunks.push_back(current_chunk);

        if (chunk_positions.find(current_chunk) == chunk_positions.end())
        {
            chunk_positions[current_chunk] = std::vector<int>{i};
            continue;
        }

        chunk_positions[current_chunk].push_back(i);
    }

    std::map<int, int> positions_count; // count consecutive fails for each position

    // use the reference model to estimate the number of bits required to compress the target data
    for (int i = 0; i < target_data.size() - k; ++i)
    {
        std::string current_chunk = target_data.substr(i, k);

        if (chunk_positions.find(current_chunk) == chunk_positions.end())
        {
            estimated_bits += log2(N); // add log2(N) when new sequencies are found
            continue;
        }

        if (i + k >= target_data.size()) // check end of file
            break;

        char solution = target_data[(i + k)]; // get the actual char from text
        char prediction = predict_char(chunk_positions[current_chunk], chunks); // get the predicted char from vector

        if (prediction == solution)
        {
            hits++;
        }
        else
        {
            fails++; 

            int key = chunk_positions[current_chunk].front();
            auto it = positions_count.find(key);
            if (it != positions_count.end())
            {
                // check if fail for a certain position is below the threshold
                if (it->second < fail_threshold)
                {
                    it->second++;
                }
                else
                {
                    if (!chunk_positions[current_chunk].empty()) // add check for empty vector
                        chunk_positions[current_chunk].erase(chunk_positions[current_chunk].begin()); // remove first position of the array
                }
            }
            else
            {
                // key doesn't exist in map, add it with a value of 1
                positions_count[key] = 1;
            }
        }

        prob = (hits + alpha) / (hits + fails + 2 * alpha);
        estimated_bits += -log2(prob); // calculate estimated bits necessary for compression
    }

    double total_symbols = target_data.size();
    cout << "Hits: " << hits << endl;
    cout << "Fails: " << fails << endl;

    cout << "Estimated total bits: " << estimated_bits << endl;
    cout << "Average bits per symbol: " << estimated_bits / total_symbols << endl;

    return 0;
}