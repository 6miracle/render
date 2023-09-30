#ifndef __RENDER_LOAD_H__
#define __RENDER_LOAD_H__

#include "pch.h"
#include "nlohmann/json.hpp"
#include <filesystem>
#include <sstream>

inline std::string load() {
    std::vector<std::string> vecs;
    int i = 0;
    for (const auto & file : std::filesystem::directory_iterator("../assets/json")) {
      vecs.emplace_back(file.path().string());
      std::string path = file.path().filename().string();
      size_t pos = path.find_last_of(".");
      std::cout << i++ << ". "<< path.substr(0, pos) << '\n';
    }
    int choice = 0;
    std::cin >> choice;
    std::cout << "==============================choice =  "<<  choice << "\n";
    for(auto i : vecs) {
      std::cout << i << '\n';
    }
    return vecs[choice];
}


#endif