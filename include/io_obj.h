#pragma once

#include "common.h"

namespace semo {

    class Loader_obj {
    public:
        std::vector<semo::point3_t>* pos_p = nullptr;
        std::vector<std::vector<size_t>>* f2v_p = nullptr;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }

        void load(const std::string filename) {


            if (pos_p == nullptr) {
                std::cout << "not set pos pointer" << std::endl;
                return;
            }
            if (f2v_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;

            pos.clear();
            f2v.clear();



            // 파일 경로 지정
            std::ifstream file(filename, std::ios::in);
            if (!file) {
                throw std::runtime_error("");
            }

            // 파일 읽기
            std::string line;
            while (std::getline(file, line)) {
                std::istringstream stream(line);
                std::string type;
                stream >> type;
                type = semo::trim(type);

                if (type == "v") {
                    pos.push_back({});
                    stream >> pos.back()[0] >> pos.back()[1] >> pos.back()[2];
                }
                else if (type == "f") {
                    f2v.push_back({});
                    std::string vertex;
                    while (stream >> vertex) {
                        std::istringstream vertex_stream(vertex);
                        std::string vertex_index_str;
                        std::getline(vertex_stream, vertex_index_str, '/');
                        int vertex_index = std::stoi(vertex_index_str);
                        f2v.back().push_back(vertex_index - 1); // OBJ indexing starts at 1
                    }
                }
            }


            file.close();

        }


    private:
    };

    class Saver_obj {
    public:
        std::vector<semo::point3_t>* pos_p = nullptr;
        std::vector<std::vector<size_t>>* f2v_p = nullptr;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }

        void save(const std::string filename) {


            if (pos_p == nullptr) {
                std::cout << "not set pos pointer" << std::endl;
                return;
            }
            if (f2v_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;

            // 파일 기록
            std::ofstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Failed to open file: " << filename << std::endl;
                return;
            }

            for (const auto& v : pos) {
                file << "v " << v[0] << " " << v[1] << " " << v[2] << "\n";
            }

            for (const auto& f : f2v) {
                assert(f.size() == 3);
                file << "f " << f[0] + 1 << " " << f[1] + 1 << " " << f[2] + 1 << "\n";
            }

            file.close();


        }
    private:
    };


}