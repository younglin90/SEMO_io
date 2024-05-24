#pragma once

#include "helper.h"

namespace semo {

    class Loader_ply {
    public:

        std::vector<std::array<double, 3>>* pos_p{};
        std::vector<std::vector<size_t>>* f2v_p{};

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
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

            std::ifstream file;
            file.exceptions(std::ifstream::failbit | std::ifstream::badbit);
            try {
                file.open(filename, std::ifstream::binary);
            }
            catch (...) {
                std::cout << "File not found: " + std::string(filename) << std::endl;
                throw std::runtime_error(
                    "File not found: " + std::string(filename));
            }

            std::string line;
            // 첫 번째 줄이 'OFF'인지 확인
            std::getline(file, line);
            if (semo::trim(line) != "ply") {
                std::cout << "Expected the first line to be `ply`." << std::endl;
                std::cout << line << std::endl;
                throw std::runtime_error("Expected the first line to be `ply`.");
            }

            // 의미 있는 다음 줄로 이동
            while (true) {
                std::getline(file, line);
                line = semo::trim(line);
                if (!line.empty() && line[0] != '#') {
                    break;
                }
            }

            // 헤더 읽기
            std::string endianness;
            bool is_binary;
            if (line == "format ascii 1.0") {
                is_binary = false;
            }
            else if (line == "format binary_big_endian 1.0") {
                is_binary = true;
                endianness = ">";
            }
            else if (line == "format binary_little_endian 1.0") {
                is_binary = true;
                endianness = "<";
            }
            else {
                throw std::runtime_error("Invalid format");
            }

            // 정점, 면, 엣지의 수를 읽음
            std::getline(file, line);
            int num_verts = 0;
            int num_faces = 0;
            std::vector<std::string> point_data_formats;
            std::vector<std::string> point_data_names;
            std::vector<std::string> face_data_names;
            std::vector<std::string> face_data_dtypes;

            while (semo::trim(line) != "end_header") {
                std::smatch match;
                if (std::regex_search(line, match, std::regex("element vertex (\\d+)"))) {
                    num_verts = std::stoi(match[1]);
                    // Read point data
                    std::getline(file, line);
                    std::istringstream tok(line);
                    std::string s0, s1, s2;
                    tok >> s0 >> s1 >> s2;
                    while (semo::trim(s0) == "property" || 
                        semo::trim(s0) == "comment") {
                        if (semo::trim(s0) == "property") {
                            // Extract property data
                            point_data_formats.push_back(semo::trim(s1));
                            point_data_names.push_back(semo::trim(s2));
                        }
                        std::getline(file, line);
                        tok.clear();
                        tok.str(line);
                        tok >> s0 >> s1 >> s2;
                    }
                }
                else if (std::regex_search(line, match, std::regex("element face (\\d+)"))) {
                    num_faces = std::stoi(match[1]);
                    if (num_faces < 0) {
                        throw std::runtime_error("Expected positive num_faces");
                    }
                    // Read cell data
                    std::getline(file, line);
                    std::istringstream tok(line);
                    std::string s0, s1, s2, s3, s4, s5;
                    tok >> s0 >> s1 >> s2 >> s3 >> s4;
                    while (semo::trim(s0) == "property" ||
                        semo::trim(s0) == "comment") {
                        if (semo::trim(s0) == "property") {
                            // Extract property data
                            if (semo::trim(s1) == "list") {
                                face_data_dtypes.push_back(semo::trim(s2 + " " + s3));
                                face_data_names.push_back(semo::trim(s4));
                            }
                            else {
                                face_data_dtypes.push_back(semo::trim(s1));
                                face_data_names.push_back(semo::trim(s2));
                            }
                        }
                        std::getline(file, line);
                        tok.clear();
                        tok.str(line);
                        tok >> s0 >> s1 >> s2 >> s3 >> s4;
                    }
                }
                else {
                    std::getline(file, line);
                    //throw std::runtime_error("Expected `element vertex` or `element face`");
                }
            }

            if (!is_binary) {
                read_ascii(file, num_verts, num_faces,
                    point_data_formats, point_data_names,
                    face_data_dtypes, face_data_names);
            }
            else {

            }


        }

    private:


        void read_ascii(std::ifstream& file, int num_verts, int num_faces,
            std::vector<std::string>& point_data_formats,
            std::vector<std::string>& point_data_names,
            std::vector<std::string>& face_data_dtypes,
            std::vector<std::string>& face_data_names
        ) {
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;

            // 정점
            pos.resize(num_verts);
            for (int i = 0; i < num_verts; ++i) {
                std::string line;
                std::getline(file, line);
                std::istringstream tok(line);
                tok >> pos[i][0] >> pos[i][1] >> pos[i][2];
            }

            // 면
            f2v.resize(num_faces);
            for (int i = 0; i < num_faces; ++i) {
                std::string line;
                std::getline(file, line);
                std::istringstream tok(line);
                int f_size;
                tok >> f_size;
                f2v[i].resize(f_size);
                for (int j = 0; j < f_size; ++j) {
                    tok >> f2v[i][j];
                }
            }
        }


    };



    class Saver_ply {
    public:

        std::vector<std::array<double, 3>>* pos_p{};
        std::vector<std::vector<size_t>>* f2v_p{};

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }

        void save(const std::string& filename) {

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

            std::ofstream file(filename, std::ios::binary);
            if (!file) {
                throw std::runtime_error("Cannot open file: " + filename);
            }

            file << "ply\n";
            file << "format ascii 1.0\n";
            file << "element vertex " << pos.size() << "\n";
            file << "property float x\n";
            file << "property float y\n";
            file << "property float z\n";
            file << "element face " << f2v.size() << "\n";
            file << "property list uchar int vertex_indices\n";
            file << "end_header\n";

            // 정점 데이터 쓰기
            for (const auto& p : pos) {
                file << std::fixed << std::setprecision(6) <<
                    p[0] << " " << p[1] << " " << p[2] << "\n";
            }

            // 면 데이터 쓰기
            for (size_t i = 0; i < f2v.size(); ++i) {
                auto& fs = f2v[i];
                file << fs.size();
                for (auto& f : fs) file << " " << f;
                file << "\n";
            }
            //** ply 파일 끝에 공란이 있어야함.

        }


    private:
    };



}