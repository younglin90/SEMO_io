#pragma once

#include "common.h"

namespace semo {

    class Loader_openfoam {
    public:

		std::vector<semo::point3_t>* pos_p{};
		std::vector<std::vector<size_t>>* f2v_p{};
		std::vector<std::array<size_t, 2>>* f2c_p{};

		std::vector<std::string>* bc_names_p{};
		std::vector<std::size_t>* bc_nFaces_p{};
		std::vector<std::size_t>* bc_startFace_p{};
		std::vector<std::size_t>* bc_myProcNo_p{};
		std::vector<std::size_t>* bc_rightProcNo_p{};

		void set_pos(std::vector<semo::point3_t>* pos_p_in) {
			pos_p = pos_p_in;
		}
		void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
			f2v_p = f2v_p_in;
		}
		void set_f2c(std::vector<std::array<size_t, 2>>* f2c_p_in) {
			f2c_p = f2c_p_in;
		}
		void set_bc_names(std::vector<std::string>* bc_names_in) {
			bc_names_p = bc_names_in;
		}
		void set_bc_nFaces(std::vector<std::size_t>* bc_nFaces_in) {
			bc_nFaces_p = bc_nFaces_in;
		}
		void set_bc_startFace(std::vector<std::size_t>* bc_startFace_in) {
			bc_startFace_p = bc_startFace_in;
		}
		void set_bc_myProcNo(std::vector<std::size_t>* bc_myProcNo_in) {
			bc_myProcNo_p = bc_myProcNo_in;
		}
		void set_bc_rightProcNo(std::vector<std::size_t>* bc_rightProcNo_in) {
			bc_rightProcNo_p = bc_rightProcNo_in;
		}


		


        void load(std::string folder) {

			if (pos_p == nullptr) {
				std::cout << "not set pos pointer" << std::endl;
				return;
			}
			if (f2v_p == nullptr) {
				std::cout << "not set f2v pointer" << std::endl;
				return;
			}
			if (f2c_p == nullptr) {
				std::cout << "not set f2c pointer" << std::endl;
				return;
			}
			if (bc_names_p == nullptr ||
				bc_nFaces_p == nullptr ||
				bc_startFace_p == nullptr ||
				bc_myProcNo_p == nullptr ||
				bc_rightProcNo_p == nullptr) {
				std::cout << "not set f2c pointer" << std::endl;
				return;
			}
			auto& pos = *pos_p;
			auto& f2v = *f2v_p;
			auto& f2c = *f2c_p;
			auto& bc_names = *bc_names_p;
			auto& bc_nFaces = *bc_nFaces_p;
			auto& bc_startFace = *bc_startFace_p;
			auto& bc_myProcNo = *bc_myProcNo_p;
			auto& bc_rightProcNo = *bc_rightProcNo_p;

			pos.clear();
			f2v.clear();
			f2c.clear();
			bc_names.clear();
			bc_nFaces.clear();
			bc_startFace.clear();
			bc_myProcNo.clear();
			bc_rightProcNo.clear();

			std::string points_file = folder + "/points";
			std::string faces_file = folder + "/faces";
			std::string owner_file = folder + "/owner";
			std::string neighbour_file = folder + "/neighbour";
			std::string boundary_file = folder + "/boundary";

			std::ifstream file;

			// points 읽기
			file.open(points_file);
			if (file.fail()) {
				std::cerr << 
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}
			std::string line;
			bool startInput = false;
			size_t num_points = semo::find_front_integer(file, '(');
			pos.reserve(num_points);
			while (std::getline(file, line)) {
				std::string asignToken;

				if (startInput) {
					if (asignToken.assign(line, 0, 1) == ")") {
						break;
					}
					else {
						semo::point3_t xyz{};
						line.erase(line.find("("), 1);
						line.erase(line.find(")"), 1);
						std::stringstream sstream(line);
						std::string word;
						int num = 0;
						while (std::getline(sstream, word, ' ')) {
							xyz[num] = std::stold(word);
							++num;
						}
						pos.push_back(xyz);
					}
				}
				else {
					if (asignToken.assign(line, 0, 1) == "(") {
						startInput = true;
					}
				}
			}
			file.close();



			// faces 읽기
			file.open(faces_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << faces_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}
			startInput = false;
			bool continueInput = false;
			std::string saveToken;
			size_t num_faces = semo::find_front_integer(file, '(');
			f2v.reserve(num_faces);
			while (std::getline(file, line)) {
				std::string asignToken;

				if (startInput) {
					if (asignToken.assign(line, 0, 1) == ")" && !continueInput) {
						break;
					}
					else {
						if (line.size() == 1) continue;

						if (line.find(")") == std::string::npos) {
							saveToken.append(" ");
							semo::rtrim(line);
							saveToken.append(line);
							continueInput = true;
							continue;
						}
						saveToken.append(" ");
						semo::rtrim(line);
						saveToken.append(line);

						saveToken.replace(saveToken.find("("), 1, " ");
						saveToken.replace(saveToken.find(")"), 1, " ");
						std::istringstream iss(saveToken);
						size_t tempint;
						iss >> tempint;

						f2v.push_back({});

						while (iss >> tempint) {
							f2v.back().push_back(tempint);
						}
						saveToken.clear();
						continueInput = false;
					}
				}
				else {
					if (asignToken.assign(line, 0, 1) == "(") {
						startInput = true;
					}
				}
			}
			file.close();

			

			// owner
			file.open(owner_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << owner_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}
			int temp_num = 0;
			startInput = false;
			f2c.resize(num_faces, { semo::SEMO_MAX_INDEX, semo::SEMO_MAX_INDEX });
			while (std::getline(file, line)) {
				std::string asignToken;

				if (startInput) {
					if (asignToken.assign(line, 0, 1) == ")") {
						break;
					}
					else {
						std::istringstream iss(line);
						size_t tempint;
						while (iss >> tempint) {
							f2c[temp_num][0] = (tempint);
							++temp_num;
						}
					}
				}
				else {
					if (asignToken.assign(line, 0, 1) == "(") {
						startInput = true;
					}
				}
			}
			file.close();

			

			// neighbour
			file.open(neighbour_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << neighbour_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}
			temp_num = 0;
			startInput = false;
			while (std::getline(file, line)) {
				std::string asignToken;

				if (startInput) {
					if (asignToken.assign(line, 0, 1) == ")") {
						break;
					}
					else {
						std::istringstream iss(line);
						size_t tempint;
						while (iss >> tempint) {
							if (tempint < 0) break;
							f2c[temp_num][1] = (tempint);
							++temp_num;
						}
					}
				}
				else {
					if (asignToken.assign(line, 0, 1) == "(") {
						startInput = true;
					}
				}
			}
			file.close();


			// boundary
			file.open(boundary_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << boundary_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}


			std::string backToken;
			startInput = false;
			std::vector<std::string> setToken;
			while (std::getline(file, line)) {
				std::string asignToken;
				if (startInput) {
					if (asignToken.assign(line, 0, 1) == ")") {
						break;
					}
					setToken.push_back(line.c_str());
				}
				else {
					if (asignToken.assign(line, 0, 1) == "(") {
						startInput = true;
					}
				}
				backToken = line;
			}

			std::string name;
			std::vector<std::string> setToken2;
			startInput = false;
			for (auto& item : setToken) {
				std::string asignToken;
				if (startInput) {
					if (item.find("}") != std::string::npos) {
						semo::trim(name);

						bc_names.push_back(name);
						int l = 0;
						for (auto& item2 : setToken2) {
							if (item2.find("nFaces") != std::string::npos) {
								std::istringstream iss(item2);
								std::string temptemp;
								bc_nFaces.push_back({});
								iss >> temptemp >> bc_nFaces.back();
							}
							if (item2.find("startFace") != std::string::npos) {
								std::istringstream iss(item2);
								std::string temptemp;
								bc_startFace.push_back({});
								iss >> temptemp >> bc_startFace.back();
							}
							if (item2.find("myProcNo") != std::string::npos) {
								std::istringstream iss(item2);
								std::string temptemp;
								bc_myProcNo.push_back({});
								iss >> temptemp >> bc_myProcNo.back();
								++l;
							}
							if (item2.find("neighbProcNo") != std::string::npos) {
								std::istringstream iss(item2);
								std::string temptemp;
								bc_rightProcNo.push_back({});
								iss >> temptemp >> bc_rightProcNo.back();
								++l;
							}
						}
						if (l == 0) {
							bc_myProcNo.push_back(semo::SEMO_MAX_INDEX);
							bc_rightProcNo.push_back(semo::SEMO_MAX_INDEX);
						}

						startInput = false;
						setToken2.clear();
					}
					setToken2.push_back(item.c_str());
				}
				else {
					if (item.find("{") != std::string::npos) {
						name.clear();
						name = backToken;
						startInput = true;
					}
				}
				backToken = item;
			}
			file.close();

        }



    private:
    };

    class Saver_openfoam {
    public:
		std::vector<semo::point3_t>* pos_p{};
		std::vector<std::vector<size_t>>* f2v_p{};
		std::vector<std::array<size_t, 2>>* f2c_p{};

		std::vector<std::string>* bc_names_p{};
		std::vector<std::size_t>* bc_nFaces_p{};
		std::vector<std::size_t>* bc_startFace_p{};
		std::vector<std::size_t>* bc_myProcNo_p{};
		std::vector<std::size_t>* bc_rightProcNo_p{};

		void set_pos(std::vector<semo::point3_t>* pos_p_in) {
			pos_p = pos_p_in;
		}
		void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
			f2v_p = f2v_p_in;
		}
		void set_f2c(std::vector<std::array<size_t, 2>>* f2c_p_in) {
			f2c_p = f2c_p_in;
		}
		void set_bc_names(std::vector<std::string>* bc_names_in) {
			bc_names_p = bc_names_in;
		}
		void set_bc_nFaces(std::vector<std::size_t>* bc_nFaces_in) {
			bc_nFaces_p = bc_nFaces_in;
		}
		void set_bc_startFace(std::vector<std::size_t>* bc_startFace_in) {
			bc_startFace_p = bc_startFace_in;
		}
		void set_bc_myProcNo(std::vector<std::size_t>* bc_myProcNo_in) {
			bc_myProcNo_p = bc_myProcNo_in;
		}
		void set_bc_rightProcNo(std::vector<std::size_t>* bc_rightProcNo_in) {
			bc_rightProcNo_p = bc_rightProcNo_in;
		}



		void save(std::string folder) {

			if (pos_p == nullptr) {
				std::cout << "not set pos pointer" << std::endl;
				return;
			}
			if (f2v_p == nullptr) {
				std::cout << "not set f2v pointer" << std::endl;
				return;
			}
			if (f2c_p == nullptr) {
				std::cout << "not set f2c pointer" << std::endl;
				return;
			}
			if (bc_names_p == nullptr ||
				bc_nFaces_p == nullptr ||
				bc_startFace_p == nullptr ||
				bc_myProcNo_p == nullptr ||
				bc_rightProcNo_p == nullptr) {
				std::cout << "not set f2c pointer" << std::endl;
				return;
			}
			auto& pos = *pos_p;
			auto& f2v = *f2v_p;
			auto& f2c = *f2c_p;
			auto& bc_names = *bc_names_p;
			auto& bc_nFaces = *bc_nFaces_p;
			auto& bc_startFace = *bc_startFace_p;
			auto& bc_myProcNo = *bc_myProcNo_p;
			auto& bc_rightProcNo = *bc_rightProcNo_p;


			// 폴더 없으면 만들기
			std::filesystem::create_directories(folder);

			std::string points_file = folder + "/points";
			std::string faces_file = folder + "/faces";
			std::string owner_file = folder + "/owner";
			std::string neighbour_file = folder + "/neighbour";
			std::string boundary_file = folder + "/boundary";

			std::ofstream file;

			// points 쓰기
			file.open(points_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}

			file << pos.size() << std::endl;
			file << "(" << std::endl;
			for (auto& p : pos) {
				file << "(" << p[0] << " " << p[1] << " " << p[2] << ")" << std::endl;
			}
			file << ")" << std::endl;
			file.close();


			// faces 쓰기
			file.open(faces_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}

			file << f2v.size() << std::endl;
			file << "(" << std::endl;
			for (auto& ivs : f2v) {
				file << ivs.size() << "(";
				for (size_t i = 0; i < ivs.size() - 1; ++i) {
					file << ivs[i] << " ";
				}
				file << ivs.back() << ")";
			}
			file << ")" << std::endl;
			file.close();


			// owner 쓰기
			file.open(owner_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}

			file << f2c.size() << std::endl;
			file << "(" << std::endl;
			int iiii = 0;
			for (auto& ics : f2c) {
				file << ics[0] << " ";
				if (iiii++ > 10) {
					file << std::endl;
					iiii = 0;
				}
			}
			file << ")" << std::endl;
			file.close();


			// neighbour 쓰기
			file.open(neighbour_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}

			// neighbour 개수 찾기
			size_t num_neighbour = 0;
			for (auto& item : f2c) if (item.size() == 2) ++num_neighbour;

			file << num_neighbour << std::endl;
			file << "(" << std::endl;
			iiii = 0;
			for (size_t i = 0; i < num_neighbour; ++i) {
				file << f2c[i][1] << " ";
				if (iiii++ > 10) {
					file << std::endl;
					iiii = 0;
				}
			}
			file << ")" << std::endl;
			file.close();


			// boundary 쓰기
			file.open(boundary_file);
			if (file.fail()) {
				std::cerr <<
					"Unable to open file for reading : " << points_file << std::endl;
				throw std::runtime_error("Unable to open file for reading");
			}

			file << bc_names.size() << std::endl;
			file << "(" << std::endl;
			for (int i = 0; i < bc_names.size(); ++i) {
				file << bc_names[i] << std::endl;
				file << "{" << std::endl;
				file << "type Unspecified;" << std::endl;
				file << "nFaces " << bc_nFaces[i] << ";" << std::endl;
				file << "startFace " << bc_startFace[i] << ";" << std::endl;
				file << "}" << std::endl;
			}
			file << ")" << std::endl;
			file.close();

		}


    private:
    };

}