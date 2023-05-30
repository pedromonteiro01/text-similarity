#include <iostream>
#include <fstream>
#include <string>
#include <vector>
#include <map>
#include <cmath>
#include <set>
#include <algorithm>
#include <limits>
#include <dirent.h>
using namespace std;

// struct to store the data for CopyModel
struct CopyModel {
    map<string, vector<int>> chunk_positions;
    vector<string> chunks;
    int N;
};

// struct to store the data for FiniteContextModel
struct FiniteContextModel {
    map<string, map<char, int>> chunk_counts;
    map<string, int> symbol_counts;
    int N;
};

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

// function to predict the next char based on the previous k characters
char predict_char_cpm(const vector<int> &positions, vector<string> &chunks)
{
    // get first position of positions array
    int first_pos = positions.front();

    // return the last character of the chunk immediately following the first position
    return chunks[first_pos + 1].back();
}

// function to predict the next character for FiniteContextModel
char predict_char_fcm(const string &chunk, FiniteContextModel &model, double alpha)
{
    char best_char; // the best predicted next character
    double best_prob = 0.0; // the highest probability encountered so far

    // loop over each character that has appeared after this chunk
    for (auto &pair : model.chunk_counts[chunk])
    {
        char c = pair.first;
        int count = pair.second;

        // calculate the probability of this character being the next character
        double prob = (count + alpha) / (model.symbol_counts[chunk] + alpha * model.chunk_counts[chunk].size());
        if (prob > best_prob)
        {
            best_prob = prob;
            best_char = c;
        }
    }

    // return the character with the highest probability
    return best_char;
}

// function to estimate bits for a given language model and target data
double estimate_bits_cpm(map<string, vector<int>> &chunk_positions, vector<string> &chunks, string &target_data, int k, double alpha, int fail_threshold, int N)
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
        char prediction = predict_char_cpm(chunk_positions[current_chunk], chunks);

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

// function to estimate the number of bits for FiniteContextModel.
double estimate_bits_fcm(FiniteContextModel &model, string &target_data, int k, double alpha, int N)
{
    int hits = 0;
    int fails = 0;
    double estimated_bits = 0;

    for (int i = 0; i < target_data.size() - k; ++i)
    {
        string current_chunk = target_data.substr(i, k);

        if (model.chunk_counts.find(current_chunk) == model.chunk_counts.end())
        {
            estimated_bits += log2(N);
            continue;
        }

        if (i + k >= target_data.size())
            break;

        char solution = target_data[(i + k)];
        char prediction = predict_char_fcm(current_chunk, model, alpha);

        if (prediction == solution)
        {
            hits++;
        }
        else
        {
            fails++;
        }

        double prob = (hits + alpha) / (hits + fails + 2 * alpha);
        estimated_bits += -log2(prob);
    }

    return estimated_bits;
}


// count the number of distinct characters in a string
int count_distinct_chars(const string &str)
{
    std::set<char> distinct_chars(str.begin(), str.end());
    return distinct_chars.size();
}

int main(int argc, char *argv[])
{
    if (argc < 7)
    {
        cout << "Usage: " << argv[0] << " <model_type> <reference_directory> <target_filename> <segment_length> <k> <alpha> <fail_threshold>" << endl;
        return 1;
    }

    // parse command-line arguments
    string model_type = argv[1];
    string reference_directory = argv[2];
    string target_filename = argv[3];
    int k = stoi(argv[5]);
    double alpha = atof(argv[6]);
    int fail_threshold = stoi(argv[7]);

    // Segment length for language change detection
    int segment_length = stoi(argv[4]);

    // read target file
    string target_data = read_file(target_filename);

    // build language models for each reference file
    map<string, CopyModel> language_models;
    map<string, FiniteContextModel> finite_context_models;

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

            int N = count_distinct_chars(reference_data);

            if (model_type == "-c")
            {
                CopyModel model;
                model.N = N;

                for (int i = 0; i < reference_data.size() - k; ++i)
                {
                    string current_chunk = reference_data.substr(i, k);
                    model.chunks.push_back(current_chunk);

                    if (model.chunk_positions.find(current_chunk) == model.chunk_positions.end())
                    {
                        model.chunk_positions[current_chunk] = vector<int>{i};
                        continue;
                    }

                    model.chunk_positions[current_chunk].push_back(i);
                }

                language_models[reference_filename] = model;
            }
            else if (model_type == "-f")
            {
                FiniteContextModel model;
                model.N = N;

                for (int i = 0; i < reference_data.size() - k; ++i)
                {
                    string current_chunk = reference_data.substr(i, k);
                    char next_char = reference_data[i + k];

                    model.chunk_counts[current_chunk][next_char]++;
                    model.symbol_counts[current_chunk]++;
                }

                finite_context_models[reference_filename] = model;
            }
        }
        closedir(dir);
    }
    else
    {
        // could not open directory
        perror("");
        return EXIT_FAILURE;
    }

    // variables to store the starting position of each language segment, the current estimated bits, and the current predicted language
    int language_start_pos = 0;
    int current_bits = 0;
    string current_language = "";
    map<string, int> language_counts;

    for (int i = 0; i <= target_data.size() - segment_length; ++i)
    {
        // extract the current segment
        string segment = target_data.substr(i, segment_length);

        // initialize variables to store the best language and its estimated bits
        string best_language;
        double min_estimated_bits = numeric_limits<double>::max();

        if (model_type == "-c")
        {
            // compare the segment to each language model
            for (auto &pair : language_models)
            {
                string reference_filename = pair.first;
                CopyModel &model = pair.second;

                // estimate bits for this language
                double estimated_bits = estimate_bits_cpm(model.chunk_positions, model.chunks, segment, k, alpha, fail_threshold, model.N);

                // keep the language with the lowest estimated bits
                if (estimated_bits < min_estimated_bits)
                {
                    min_estimated_bits = estimated_bits;
                    best_language = reference_filename;
                }
            }
        }
        else if (model_type == "-f")
        {
            for (auto &pair : finite_context_models)
            {
                string reference_filename = pair.first;
                FiniteContextModel &model = pair.second;

                // estimate bits for this language
                double estimated_bits = estimate_bits_fcm(model, segment, k, alpha, model.N);

                // keep the language with the lowest estimated bits
                if (estimated_bits < min_estimated_bits)
                {
                    min_estimated_bits = estimated_bits;
                    best_language = reference_filename;
                }
            }
        }

        // if the language has not changed, accumulate the estimated bits
        if (current_language == best_language && i != target_data.size() - segment_length)
        {
            language_counts[current_language]++;
            current_bits += min_estimated_bits;
        }
        // if the language has changed, print the information of the previous language segment
        else
        {
            if (current_language != "")
                cout << "Segment: " << language_start_pos << "-" << i << " is likely in " << current_language << " with estimated bits: " << current_bits << endl;
            current_language = best_language;
            current_bits = min_estimated_bits;
            language_start_pos = i;
            language_counts[current_language] = 1;
        }
    }

    return 0;
}