#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <algorithm>
#include <set>
#include <dirent.h>
using namespace std;

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

// count the number of distinct characters
int count_distinct_chars(const string &str)
{
    set<char> distinct_chars(str.begin(), str.end());
    return distinct_chars.size();
}

// predict the next char based on the previous k characters
char predict_char(const vector<int> &positions, vector<string> &chunks)
{
    // get first position of positions array
    int first_pos = positions.front();

    // return the last character of the chunk immediately following the first position
    return chunks[first_pos + 1].back();
}

// function to estimate bits for a given language model and target data
double estimate_bits(map<string, vector<int>> &chunk_positions, vector<string> &chunks, string &target_data, int k, double alpha, int fail_threshold, int N)
{
    int hits = 0;
    int fails = 0;
    double estimated_bits = 0;
    map<int, int> positions_count;

    for (int i = 0; i < target_data.size() - k; ++i)
    {
        string current_chunk = target_data.substr(i, k);

        if (chunk_positions.find(current_chunk) == chunk_positions.end())
        {
            estimated_bits += log2(N);
            continue;
        }

        if (i + k >= target_data.size())
            break;

        char solution = target_data[(i + k)];
        char prediction = predict_char(chunk_positions[current_chunk], chunks);

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
                if (it->second < fail_threshold)
                {
                    it->second++;
                }
                else
                {
                    if (!chunk_positions[current_chunk].empty())
                        chunk_positions[current_chunk].erase(chunk_positions[current_chunk].begin());
                }
            }
            else
            {
                positions_count[key] = 1;
            }
        }

        double prob = (hits + alpha) / (hits + fails + 2 * alpha);
        estimated_bits += -log2(prob);
    }

    return estimated_bits;
}

int main(int argc, char *argv[])
{
    if (argc < 6)
    {
        cout << "Usage: " << argv[0] << " <reference_directory> <target_filename> <k> <alpha> <fail_threshold>" << endl;
        return 1;
    }

    // parse command-line arguments
    string reference_directory = argv[1];
    string target_filename = argv[2];
    int k = stoi(argv[3]);
    double alpha = atof(argv[4]);
    int fail_threshold = stoi(argv[5]);

    // read target file
    string target_data = read_file(target_filename);

    // read reference directory
    DIR *dir;
    struct dirent *ent;
    if ((dir = opendir(reference_directory.c_str())) != NULL)
    {
        // for each file in the directory
        while ((ent = readdir(dir)) != NULL)
        {
            string reference_filename = string(ent->d_name);
            if (reference_filename == "." || reference_filename == "..") continue; // skip . and ..

            // read reference file
            string reference_data = read_file(reference_directory + "/" + reference_filename);

            // calculate the number of distinct characters in the reference file
            int N = count_distinct_chars(reference_data);

            // create copy model using the reference data
            map<string, vector<int>> chunk_positions;
            vector<string> chunks;

            for (int i = 0; i < reference_data.size() - k; ++i)
            {
                string current_chunk = reference_data.substr(i, k);
                chunks.push_back(current_chunk);

                if (chunk_positions.find(current_chunk) == chunk_positions.end())
                {
                    chunk_positions[current_chunk] = vector<int>{i};
                    continue;
                }

                chunk_positions[current_chunk].push_back(i);
            }

            // estimate bits for this language
            double estimated_bits = estimate_bits(chunk_positions, chunks, target_data, k, alpha, fail_threshold, N);

            // print estimated bits for this language
            cout << "Estimated total bits for " << reference_filename << ": " << estimated_bits << endl;
        }
        closedir(dir);
    }
    else
    {
        // could not open directory
        perror("");
        return EXIT_FAILURE;
    }

    return 0;
}
