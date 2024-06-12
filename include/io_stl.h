#pragma once

#include "common.h"
#include "helper.h"

namespace semo {

	class Loader_stl {
	public:

        std::vector<semo::point3_t>* pos_p = nullptr;
        std::vector<std::vector<semo::index_t>>* f2v_p = nullptr;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<semo::index_t>>* f2v_p_in) {
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
                assert(false);
            }

            char buf[6]{};
            buf[5] = 0;
            file.read(buf, 5);
            const std::string asciiHeader = "solid";

            auto calculateNormalVector = [](
                std::array<double, 3>& v1,
                std::array<double, 3>& v2,
                std::array<double, 3>& v3)->std::array<double, 3> {

                std::array<double, 3> v1v2 = { v2[0] - v1[0], v2[1] - v1[1], v2[2] - v1[2] };
                std::array<double, 3> v1v3 = { v3[0] - v1[0], v3[1] - v1[1], v3[2] - v1[2] };
                std::array<double, 3> normal = {
                    v1v2[1] * v1v3[2] - v1v2[2] * v1v3[1],
                    v1v2[2] * v1v3[0] - v1v2[0] * v1v3[2],
                    v1v2[0] * v1v3[1] - v1v2[1] * v1v3[0]
                };
                double length = std::sqrt(
                    normal[0] * normal[0] + normal[1] * normal[1] + normal[2] * normal[2]);
                normal[0] /= length; normal[1] /= length; normal[2] /= length;
                return normal;
            };

            // ascii 파일 읽기
            if (std::string(buf) == asciiHeader) {
                file.seekg(0, std::ios::beg);
                std::string s0, s1;
                while (!file.eof()) {
                    file >> s0;
                    if (s0 == "facet") {
                        f2v.push_back({});
                        double x_temp{}, y_temp{}, z_temp{};
                        double x_avg = 0.0, y_avg = 0.0, z_avg = 0.0;
                        file >> s1 >> x_temp >> y_temp >> z_temp;
                        file >> s0 >> s1;
                        //mesh.get_last_face().set_normal(x_temp, y_temp, z_temp);

                        file >> s0 >> x_temp >> y_temp >> z_temp;
                        semo::point3_t pos_tmp = { x_temp, y_temp, z_temp };
                        pos.push_back(pos_tmp);
                        f2v.back().push_back(pos.size() - 1);
                        //x_avg += x_temp; y_avg += y_temp; z_avg += z_temp;
                        //mesh.connect_face_to_point(mesh.get_index_of_last_face(), mesh.get_last_point_index());

                        file >> s0 >> x_temp >> y_temp >> z_temp;
                        pos_tmp = { x_temp, y_temp, z_temp };
                        pos.push_back(pos_tmp);
                        f2v.back().push_back(pos.size() - 1);
                        //x_avg += x_temp; y_avg += y_temp; z_avg += z_temp;
                        //mesh.connect_face_to_point(mesh.get_index_of_last_face(), mesh.get_last_point_index());

                        file >> s0 >> x_temp >> y_temp >> z_temp;
                        pos_tmp = { x_temp, y_temp, z_temp };
                        pos.push_back(pos_tmp);
                        f2v.back().push_back(pos.size() - 1);
                        //x_avg += x_temp; y_avg += y_temp; z_avg += z_temp;
                        //mesh.connect_face_to_point(mesh.get_index_of_last_face(), mesh.get_last_point_index());

                        file >> s0;
                        file >> s0;

                        //mesh.get_last_face().set_normal(calculateNormalVector(v1, v2, v3));
                        //mesh.get_last_face().set_center(x_avg / 3.0, y_avg / 3.0, z_avg / 3.0);

                    }
                    else if (s0 == "endsolid") {
                        break;
                    }

                }
            }
            // binary 파일 읽기
            else {
                file.close();
                file.open(filename, std::ios::in | std::ios::binary);

                // STL 헤더 정보 건너뛰기
                char header[80];
                file.read(header, 80);

                // 삼각형 면 개수 읽기
                uint32_t num_triangles = 0;
                file.read(reinterpret_cast<char*>(&num_triangles), sizeof(uint32_t));

                for (uint32_t idx = 0; idx < num_triangles; ++idx) {
                    f2v.push_back({});

                    float normal_t[3]{}, vertex1_t[3]{},
                        vertex2_t[3]{}, vertex3_t[3]{};
                    uint16_t attribute{};
                    file.read(reinterpret_cast<char*>(normal_t), sizeof(float) * 3);
                    file.read(reinterpret_cast<char*>(vertex1_t), sizeof(float) * 3);
                    file.read(reinterpret_cast<char*>(vertex2_t), sizeof(float) * 3);
                    file.read(reinterpret_cast<char*>(vertex3_t), sizeof(float) * 3);
                    file.read(reinterpret_cast<char*>(&attribute), sizeof(uint16_t));

                    std::array<double, 3> normal{};
                    std::array<double, 3> vertex1{}, vertex2{}, vertex3{};
                    for (size_t i = 0; i < 3; ++i) {
                        normal[i] = static_cast<double>(normal_t[i]);
                        vertex1[i] = static_cast<double>(vertex1_t[i]);
                        vertex2[i] = static_cast<double>(vertex2_t[i]);
                        vertex3[i] = static_cast<double>(vertex3_t[i]);
                    }
                    semo::point3_t pos_tmp = { vertex1[0], vertex1[1], vertex1[2] };
                    pos.push_back(pos_tmp);
                    f2v.back().push_back(pos.size() - 1);
                    pos_tmp = { vertex2[0], vertex2[1], vertex2[2] };
                    pos.push_back(pos_tmp);
                    f2v.back().push_back(pos.size() - 1);
                    pos_tmp = { vertex3[0], vertex3[1], vertex3[2] };
                    pos.push_back(pos_tmp);
                    f2v.back().push_back(pos.size() - 1);
                    
                    //T x_avg = (vertex1[0] + vertex2[0] + vertex3[0]) / 3.0;
                    //T y_avg = (vertex1[1] + vertex2[1] + vertex3[1]) / 3.0;
                    //T z_avg = (vertex1[2] + vertex2[2] + vertex3[2]) / 3.0;
                    //mesh.get_last_face().set_normal(normal);
                    //mesh.get_last_face().set_center(x_avg, y_avg, z_avg);
                }
            }

            // 파일 닫기
            file.close();

            // 파일 읽기 완료


            // 포인트 중복 삭제 및 f2v 정리
            pos_f2v_rearange();


        }

        void pos_f2v_rearange() {

            auto& pos = *pos_p;
            auto& f2v = *f2v_p;


#ifdef SEMO_USE_EIGEN_LIB
            std::vector<std::array<double, 3>> pos_tmp;
            pos_tmp.reserve(pos.size());
            for (auto& p : pos) {
                pos_tmp.push_back({p[0],p[1],p[2]});
            }
            auto indices = semo::sort_and_unique_indices(pos_tmp);
            pos.clear();
            pos.reserve(pos_tmp.size());
            for (auto& p : pos_tmp) {
                semo::point3_t pos_tmp = { p[0], p[1], p[2] };
                pos.push_back(pos_tmp);
            }
#else
            auto indices = semo::sort_and_unique_indices(pos);
#endif

            // face to point connectivity를 새로운 인덱스로 바꾸기
            for (auto& ivs : f2v) {
                std::vector<std::size_t> new_points;
                for (const auto& iv : ivs) {
                    new_points.push_back(indices[iv]);
                }
                ivs = std::move(new_points);
            }

        }



    private:


	};



	class Saver_stl {
	public:


        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }

		void save_ascii(const std::string& filename) {
            _write_ascii(filename);
        }
        void save_binary(const std::string& filename) {
            _write_binary(filename);
        }


    private:
        // ASCII write function
        void _write_ascii(const std::string& filename) {
            std::ofstream fh(filename);
            if (!fh) {
                throw std::runtime_error("Could not open file " + filename);
            }

            auto& pos = *pos_p;
            auto& f2v = *f2v_p;

            fh << "solid\n";
            for (size_t i = 0; i < f2v.size(); ++i) {
                const auto& ivs = f2v[i];
                assert(ivs.size() == 3);
                //fh << "facet normal " << normal.x() << " " << normal.y() << " " << normal.z() << "\n";
                fh << "facet normal " << 0.0 << " " << 0.0 << " " << 0.0 << "\n";
                fh << " outer loop\n";
                for (auto& iv : ivs) {
                const auto& vertex = pos[iv];
                    fh << "  vertex " << vertex[0] << " " << vertex[1] << " " << vertex[2] << "\n";
                }
                fh << " endloop\n";
                fh << "endfacet\n";
            }
            fh << "endsolid\n";
        }

        // Binary write function
        void _write_binary(const std::string& filename) {
            std::ofstream fh(filename, std::ios::binary);
            if (!fh) {
                throw std::runtime_error("Could not open file " + filename);
            }

            std::string header = "This file was generated by semo";
            header.resize(80, ' ');
            fh.write(header.c_str(), 80);

            auto& pos = *pos_p;
            auto& f2v = *f2v_p;

            uint32_t num_triangles = static_cast<uint32_t>(f2v.size());
            fh.write(reinterpret_cast<const char*>(&num_triangles), sizeof(uint32_t));

            for (size_t i = 0; i < f2v.size(); ++i) {
                const auto& ivs = f2v[i];
                std::array<float, 3> normal = { 0.0, 0.0, 0.0 };
                fh.write(reinterpret_cast<const char*>(normal.data()), 3 * sizeof(float));

                assert(ivs.size() == 3);

                for (const auto& iv : ivs) {
                    auto& loc_pos = pos[iv];
                    std::array<float, 3 > posf = { 
                        static_cast<float>(loc_pos[0]), 
                        static_cast<float>(loc_pos[1]),
                        static_cast<float>(loc_pos[2]) };
                    fh.write(reinterpret_cast<const char*>(posf.data()), 3 * sizeof(float));
                }

                uint16_t attr = 0;
                fh.write(reinterpret_cast<const char*>(&attr), sizeof(uint16_t));
            }
        }


	};



}