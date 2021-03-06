#include "headers/reading_from_file.h"
#include <iostream>
#include <vector>
#include <map>
#include "headers/additional_functions.h"
#include <fstream>
#include <boost/algorithm/string/trim.hpp>
#include <fcntl.h>

void get_values_from_file(std::map<std::string, std::string>* values, std::vector<std::string> key_words, const std::string& path_to_file){
    /**
    * @brief This function reads from file and saves needed values form file to map. 
    * The function has parameters as follows:
    *
    * @param values
    * This parameter is a map of strings where all values will be stored.
    * 
    * @param key_words
    * This parameter is a vector of key words that will be used to identify which data from file is needed.
    * 
    * @param path_to_file
    * This parameter is a std::string which represents path to file.
    */
    std::fstream newfile;
    newfile.open(path_to_file, std::ios::in);
    if (newfile.is_open()){
        std::string tp;
        while(getline(newfile, tp)){
            std::vector<std::string> vec = split(tp, ":");
            if (vec.size() < 2){
                continue;
            }
            std::string line_key = vec[0];
            boost::algorithm::trim(line_key);
            for(std::string key_word : key_words){
                if (line_key == key_word){
                    (*values)[key_word] = vec[vec.size()-1];
                    key_words.erase(std::remove(key_words.begin(), key_words.end(), key_word), key_words.end());
                    if (key_words.empty()){
                        return;
                    }
                }
            }
        }
        newfile.close();
    }
    return;
}


std::vector<std::string> read_file_to_vector(const std::string& path_to_file){
    /**
    * @brief This function reads from file and returns vector with data from file. 
    * The function has parameters as follows:
    *
    * @param path_to_file
    * This parameter is a std::string which represents path to file.
    */
    int fd = open(path_to_file.c_str(), O_RDONLY);
        std::vector<std::string> values;

        char buffer[1000];
        std::string out_str;
        if(fd < 0){
            return values;
        }
        int len = read(fd, buffer, 1000);
        while(len != 0){
            if (len < 0){
                std::cout << "Len < 0" << std::endl;
                break;
            }
            if(len != 1000){
                buffer[len] = '\0';
            }
            out_str += buffer;
            memset(buffer, 0, 1000);
            len = read(fd, buffer, 1000);
        }
        close(fd);
        values = split(out_str, " ");
        return values;
}
