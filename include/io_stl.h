#pragma once

#include "helper.h"

namespace semo {

	class Loader_stl {
	public:

        std::vector<std::array<double, 3>>* pos_p = nullptr;
        std::vector<std::vector<size_t>>* f2v_p = nullptr;

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



            // ���� ��� ����
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

            // ascii ���� �б�
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
                        pos.push_back({ x_temp, y_temp, z_temp });
                        f2v.back().push_back(pos.size() - 1);
                        //x_avg += x_temp; y_avg += y_temp; z_avg += z_temp;
                        //mesh.connect_face_to_point(mesh.get_index_of_last_face(), mesh.get_last_point_index());

                        file >> s0 >> x_temp >> y_temp >> z_temp;
                        pos.push_back({ x_temp, y_temp, z_temp });
                        f2v.back().push_back(pos.size() - 1);
                        //x_avg += x_temp; y_avg += y_temp; z_avg += z_temp;
                        //mesh.connect_face_to_point(mesh.get_index_of_last_face(), mesh.get_last_point_index());

                        file >> s0 >> x_temp >> y_temp >> z_temp;
                        pos.push_back({ x_temp, y_temp, z_temp });
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
            // binary ���� �б�
            else {
                file.close();
                file.open(filename, std::ios::in | std::ios::binary);

                // STL ��� ���� �ǳʶٱ�
                char header[80];
                file.read(header, 80);

                // �ﰢ�� �� ���� �б�
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
                    pos.push_back({ vertex1[0], vertex1[1], vertex1[2] });
                    f2v.back().push_back(pos.size() - 1);
                    pos.push_back({ vertex2[0], vertex2[1], vertex2[2] });
                    f2v.back().push_back(pos.size() - 1);
                    pos.push_back({ vertex3[0], vertex3[1], vertex3[2] });
                    f2v.back().push_back(pos.size() - 1);
                    
                    //T x_avg = (vertex1[0] + vertex2[0] + vertex3[0]) / 3.0;
                    //T y_avg = (vertex1[1] + vertex2[1] + vertex3[1]) / 3.0;
                    //T z_avg = (vertex1[2] + vertex2[2] + vertex3[2]) / 3.0;
                    //mesh.get_last_face().set_normal(normal);
                    //mesh.get_last_face().set_center(x_avg, y_avg, z_avg);
                }
            }

            // ���� �ݱ�
            file.close();

            // ���� �б� �Ϸ�



        }



    private:


	};



	class Saver_stl {
	public:


        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
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

            uint32_t num_triangles = f2v.size();
            fh.write(reinterpret_cast<const char*>(&num_triangles), sizeof(uint32_t));

            for (size_t i = 0; i < f2v.size(); ++i) {
                std::array<float, 3> normal = { 0.0, 0.0, 0.0 };
                fh.write(reinterpret_cast<const char*>(normal.data()), 3 * sizeof(float));

                for (const auto& iv : f2v[i]) {
                    auto& loc_pos = pos[iv];
                    std::array<float, 3 > posf = { loc_pos[0], loc_pos[1], loc_pos[2] };
                    fh.write(reinterpret_cast<const char*>(posf.data()), 3 * sizeof(float));
                }

                uint16_t attr = 0;
                fh.write(reinterpret_cast<const char*>(&attr), sizeof(uint16_t));
            }
        }


	};



}