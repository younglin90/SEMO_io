#pragma once

#include <iostream>
#include <fstream>
#include <vector>
#include <array>
#include <string>
#include <algorithm>
#include <filesystem>
#include <cmath>
#include <functional>
#include <regex>
#include <set>
#include <cassert>

namespace semo {


    std::string rtrim(std::string& str) {
        size_t end = str.size();
        while (end > 0 && std::isspace(str[end - 1])) {
            --end;
        }
        return str.substr(0, end);
    }

    std::string ltrim(std::string& str) {
        size_t start = 0;
        while (start < str.size() && std::isspace(str[start])) {
            ++start;
        }
        return str.substr(start);
    }


    std::string trim(std::string& str) {
        ltrim(str);
        rtrim(str);
        return str;
    }
    std::string trim(std::string&& str) {
        ltrim(str);
        rtrim(str);
        return str;
    }

    //// 문자열 앞뒤 공백을 자르는 함수
    //std::string trim(std::string str_in) {
    //    std::string_view str(str_in);
    //    // 앞의 공백 자르기
    //    str.remove_prefix(std::distance(str.begin(), std::ranges::find_if(str, [](unsigned char ch) {
    //        return !std::isspace(ch);
    //        })));
    //    // 뒤의 공백 자르기
    //    str.remove_suffix(std::distance(str.rbegin(), std::ranges::find_if(str.rbegin(), str.rend(), [](unsigned char ch) {
    //        return !std::isspace(ch);
    //        })));
    //    std::string result(str);
    //    return result;
    //}


    // 파일에서 특정 문자 바로 앞에 있는 정수 추출
    size_t find_front_integer(std::ifstream& file, char fc) {
        std::string bf_line;
        std::string line;
        while (std::getline(file, line)) {
            size_t pos = line.find(fc);
            if (pos != std::string::npos) {
                // '('의 바로 앞에 있는 정수 추출
                size_t numEnd = pos;
                while (numEnd > 0 && std::isspace(line[numEnd - 1])) {
                    --numEnd;
                }
                size_t numStart = numEnd;
                while (numStart > 0 && std::isdigit(line[numStart - 1])) {
                    --numStart;
                }
                if (numStart == numEnd) {
                    size_t number = std::stol(bf_line);
                    file.seekg(0, std::ios::beg);
                    return number;
                }

                if (numStart != numEnd) {
                    size_t number = std::stol(line.substr(numStart, numEnd - numStart));
                    file.seekg(0, std::ios::beg);
                    return number;
                }
            }
            bf_line.clear();
            bf_line = line;
        }
        std::cout << "NOT find : " << fc << std::endl;
        file.seekg(0, std::ios::beg);
        return std::numeric_limits<size_t>::max();

    }


    void get_f2c_to_c2f(
        std::vector<std::vector<size_t>>& f2c,
        std::vector<std::vector<size_t>>& c2f
    ) {

        size_t num_cells = 0;
        for (auto& ics : f2c) {
            for (auto& ic : ics) {
                if (ic > num_cells) {
                    num_cells = ic;
                }
            }
        }
        ++num_cells;

        c2f.clear();
        c2f.resize(num_cells);
        for (size_t i = 0; i < f2c.size(); ++i) {
            for (auto& ic : f2c[i]) {
                c2f[ic].push_back(i);
            }
        }


    }

    void get_c2f_f2v_to_c2v(
        std::vector<std::vector<size_t>>& c2f,
        std::vector<std::vector<size_t>>& f2v,
        std::vector<std::vector<size_t>>& c2v
    ) {
        c2v.clear();
        c2v.resize(c2f.size());
        for (size_t i = 0; i < c2f.size(); ++i) {
            std::set<size_t> ivs;
            for (auto& iface : c2f[i]) {
                for (auto& iv : f2v[iface]) {
                    ivs.insert(iv);
                }
            }
            c2v[i].resize(ivs.size());
            for (auto& item : ivs) {
                c2v[i].push_back(item);
            }
        }

    }




}