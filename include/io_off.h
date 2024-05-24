#pragma once

#include "helper.h"

namespace semo {

	class Loader_off {
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
            if (semo::trim(line) != "OFF") {
                std::cout << "Expected the first line to be `OFF`." << std::endl;
                std::cout << line << std::endl;
                throw std::runtime_error("Expected the first line to be `OFF`.");
            }

            // 의미 있는 다음 줄로 이동
            while (true) {
                std::getline(file, line);
                line = semo::trim(line);
                if (!line.empty() && line[0] != '#') {
                    break;
                }
            }

            // 다음 줄에서 정점, 면, 엣지의 수를 읽음
            std::istringstream tokenStream(line);
            int num_verts, num_faces;
            tokenStream >> num_verts >> num_faces;

            // 정점 데이터를 읽음
            //std::vector<std::array<double, 3>> verts(num_verts);
            pos.resize(num_verts);
            for (auto& vert : pos) {
                std::getline(file, line);
                std::istringstream tok(line);
                tok >> vert[0] >> vert[1] >> vert[2];
            }

            // 면 데이터를 읽음
            //std::vector<std::vector<int>> f2v(num_faces);
            f2v.resize(num_faces);
            for (int i = 0; i < num_faces; ++i) {
                std::getline(file, line);
                std::istringstream tok(line);
                int num_vertices;
                tok >> num_vertices;
                f2v[i].resize(num_vertices);
                for (int j = 0; j < num_vertices; ++j) {
                    tok >> f2v[i][j];
                }
            }

		}

    private:


	};



	class Saver_off {
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

            file << "OFF\n";
            file << pos.size() << " " << f2v.size() << " 0\n";

            // 정점 데이터 쓰기
            for (const auto& p : pos) {
                file << std::scientific << std::setprecision(6) <<
                    p[0] << " " << p[1] << " " << p[2] << "\n";
            }

            // 면 데이터 쓰기
            for (size_t i = 0; i < f2v.size(); ++i) {
                auto& fs = f2v[i];
                file << fs.size() << " ";
                for (auto& f : fs) file << f << " ";
                if (i != f2v.size() - 1) file << "\n";
                
            }

        }


    private:
	};



}