#pragma once

#include "common.h"

//#include "zlib/zlib.h"

namespace semo {


    // @ref ANSYS_Fluent_Users_Guide
	class Loader_ansys {
	public:

        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::array<size_t, 2>>* f2c_p;

        std::vector<std::string>* bc_names_p{};
        std::vector<std::size_t>* bc_nFaces_p{};
        std::vector<std::size_t>* bc_startFace_p{};
        std::vector<std::size_t>* bc_myProcNo_p{};
        std::vector<std::size_t>* bc_rightProcNo_p{};


        struct BC {
            std::string name;
            size_t startFace;
            size_t nFaces;
            size_t myProcNo;
            size_t rightProcNo;
        };

        // int 키에 대해 정렬됨
        std::map<int, BC> boundaries_map;

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


        void load(const std::string& filename) {

            std::ifstream file(filename, std::ios::binary);
            if (!file.is_open()) {
                std::cout << "Failed to open file." << std::endl;
                throw std::runtime_error("Failed to open file.");
            }

            std::string line;
            while (std::getline(file, line)) {
                if (line.empty()) continue;


                std::regex re("\\s*\\((\\d+) ");
                std::smatch match;

                if (!std::regex_search(line, match, re)) continue; 

                std::string index = match.str(1); // 숫자 추출

                // Comment. or header or dimensionality
                if (index == "0" || index == "1" || index == "2") continue;

                int first_point_index_overall = 0;

                // points
                if (index == "10") {
                    read_points(file, line, first_point_index_overall, 0, false, false);
                }
                else if (index == "2010") {
                    read_points(file, line, first_point_index_overall, 0, true, false);
                }
                else if (index == "3010") {
                    read_points(file, line, first_point_index_overall, 0, true, true);
                }
                // cells
                if (index == "12") {
                    read_cells(file, line, false, false);
                }
                else if (index == "2012") {
                    read_cells(file, line, true, false);
                }
                else if (index == "3012") {
                    read_cells(file, line, true, true);
                }
                // faces
                if (index == "13") {
                    read_faces(file, line, false, false);
                }
                else if (index == "2013") {
                    read_faces(file, line, true, false);
                }
                else if (index == "3013") {
                    read_faces(file, line, true, true);
                }


                // periodic shadow faces
                // (18 (first-index last-index periodic-zone shadow-zone)(
                if (index == "18") {
                    continue;
                }

                // 존 구체화
                if (index == "39") {
                    std::cerr << "Zone specification not supported yet. Skipping." << std::endl;
                    continue;
                }

                // 바운더리
                if (index == "45") {
                    read_boundaries(file, line);
                }

            }

            // 바운더리 추출
            bc_names_p->clear();
            bc_startFace_p->clear();
            bc_nFaces_p->clear();
            bc_myProcNo_p->clear();
            bc_rightProcNo_p->clear();
            for (auto& item : boundaries_map) {
                if (item.second.startFace == semo::SEMO_MAX_INDEX) continue;
                bc_names_p->push_back(semo::trim(item.second.name));
                bc_startFace_p->push_back(item.second.startFace);
                bc_nFaces_p->push_back(item.second.nFaces);
                bc_myProcNo_p->push_back(item.second.myProcNo);
                bc_rightProcNo_p->push_back(item.second.rightProcNo);
            }

            file.close();

        }




        //void load_gz(const std::string& filename) {

        //    gzFile file_gz = gzopen("E:/[[ meshs ]]/fluent_test2.cas.gz", "rb");
        //    if (!file_gz) {
        //        throw std::runtime_error("Failed to open gzip file: ");
        //    }

        //    const int bufferSize = 4096;
        //    char buffer[bufferSize];
        //    std::string c2s;
        //    while (gzgets(file_gz, buffer, bufferSize) != Z_NULL) {
        //        std::string c2s_tmp(buffer);
        //        for (auto& item : c2s_tmp) {
        //            c2s.push_back(item);
        //        }

        //    }
        //    std::istringstream file(c2s);



        //    std::string line;
        //    while (std::getline(file, line)) {
        //        if (line.empty()) continue;

        //        std::regex re("\\s*\\((\\d+) ");
        //        std::smatch match;

        //        if (!std::regex_search(line, match, re)) continue;

        //        std::string index = match.str(1); // 숫자 추출

        //        // Comment. or header or dimensionality
        //        if (index == "0" || index == "1" || index == "2") continue;

        //        int first_point_index_overall = 0;

        //        // points
        //        if (index == "10") {
        //            read_points(file, line, first_point_index_overall, 0, false, false);
        //        }
        //        else if (index == "2010") {
        //            read_points(file, line, first_point_index_overall, 0, true, false);
        //        }
        //        else if (index == "3010") {
        //            read_points(file, line, first_point_index_overall, 0, true, true);
        //        }
        //        // cells
        //        if (index == "12") {
        //            read_cells(file, line, false, false);
        //        }
        //        else if (index == "2012") {
        //            read_cells(file, line, true, false);
        //        }
        //        else if (index == "3012") {
        //            read_cells(file, line, true, true);
        //        }
        //        // faces
        //        if (index == "13") {
        //            read_faces(file, line, false, false);
        //        }
        //        else if (index == "2013") {
        //            read_faces(file, line, true, false);
        //        }
        //        else if (index == "3013") {
        //            read_faces(file, line, true, true);
        //        }


        //        // periodic shadow faces
        //        // (18 (first-index last-index periodic-zone shadow-zone)(
        //        if (index == "18") {
        //            continue;
        //        }

        //        // 존 구체화
        //        if (index == "39") {
        //            std::cerr << "Zone specification not supported yet. Skipping." << std::endl;
        //            continue;
        //        }

        //        // 바운더리
        //        if (index == "45") {
        //            std::regex obj("\\(45 \\(\\d+ ([\\S]+) ([\\S]+)\\)");
        //            if (std::regex_match(line, match, obj)) {
        //                std::cerr << "Zone specification not supported yet (" << match[1] << ", " << match[2] << "). Skipping." << std::endl;
        //            }
        //            else {
        //                std::cerr << "Zone specification not supported yet." << std::endl;
        //            }
        //        }

        //    }

        //}


    private:


        // 파일에서 점 데이터를 읽어오고 처리하는 함수
        void read_points(
            std::ifstream& file,
            //std::istringstream& file,
            const std::string& line, 
            int& first_point_index_overall, 
            int last_point_index,
            bool is_binary,
            bool is_64bit
        ) {
            if (std::count(line.begin(), line.end(), '(') == std::count(line.begin(), line.end(), ')')) {
                return; // self-contained line은 무시
            }


            auto& pos = *pos_p;

            // (3010 (zone-id first-index last-index type ND)
            std::regex re2("\\(\\d+ \\(([^)]+)\\)");
            std::smatch match;
            std::regex_search(line, match, re2);


            // 추출된 문자열을 공백을 기준으로 분할하여 각 숫자를 추출
            std::vector<int> numbers;
            std::istringstream iss(match.str(1));
            std::string hex_num;

            while (iss >> hex_num) {
                try {
                    numbers.push_back(std::stoi(hex_num, nullptr, 16));
                }
                catch (const std::exception& e) {
                    std::cerr << "Error converting '" << hex_num << "' to decimal: " << e.what() << std::endl;
                }
            }
            // 필수 데이터 확인
            assert(numbers.size() > 4);

            int first_point_index = numbers[1];
            if (first_point_index_overall == 0) {
                first_point_index_overall = first_point_index;
            }
            if (last_point_index != 0) {
                assert(last_point_index + 1 == first_point_index);
            }
            last_point_index = numbers[2];
            int num_points = last_point_index - first_point_index + 1;
            int dim = numbers[4];

            // 데이터 읽기
            pos.resize(num_points);

            // 문자열 끝의 공백 제거
            std::string copy_line = line;
            semo::rtrim(copy_line);


            // 문자열에 '('가 있는지 확인
            bool has_opening_bracket = !copy_line.empty() && copy_line.back() == '(';

            // 다음 줄 시작 부분으로 이동
            if (!has_opening_bracket) {
                char ch = ' ';
                while (ch != '(') {
                    file.get(ch);
                }
            }


            if (!is_binary) {
                // ASCII 데이터
                for (int k = 0; k < num_points; ++k) {
                    std::string data_line;
                    do {
                        std::getline(file, data_line);
                        data_line = data_line.substr(data_line.find_first_not_of(' '));
                    } while (data_line.empty());

                    std::istringstream data_iss(data_line);
                    for (int d = 0; d < dim; ++d) {
                        if (!(data_iss >> pos[k][d])) {
                            throw std::runtime_error("Invalid data format");
                        }
                    }
                }
            }
            else {
                int length = dim * num_points;
                if (is_64bit) {
                    std::vector<double> datas(length);
                    file.read(reinterpret_cast<char*>(datas.data()), sizeof(double) * length);
                    for (int i = 0, ii = 0; i < num_points; ++i) {
                        for (int d = 0; d < dim; ++d) {
                            pos[i][d] = datas[ii++];
                        }
                    }
                }
                else {
                    std::vector<float> datas(length);
                    file.read(reinterpret_cast<char*>(datas.data()), sizeof(float) * length);
                    for (int i = 0, ii = 0; i < num_points; ++i) {
                        for (int d = 0; d < dim; ++d) {
                            pos[i][d] = datas[ii++];
                        }
                    }
                }
            }

        }


        // 파일에서 셀 데이터를 읽어오고 처리하는 함수
        void read_cells(
            std::ifstream& file,
            //std::istringstream& file,
            const std::string& line, 
            bool is_binary, bool is_64bit) {

            // self-contained line 여부 확인
            if (std::count(line.begin(), line.end(), '(') == std::count(line.begin(), line.end(), ')')) {
                return; // self-contained line은 무시
            }

            // (12 (zone-id first-index last-index type element-type))
            std::regex re2("\\(\\d+ \\(([^)]+)\\)");
            std::smatch match;
            std::regex_search(line, match, re2);
            // 추출된 문자열을 공백을 기준으로 분할하여 각 숫자를 추출
            std::vector<int> numbers;
            std::istringstream iss(match.str(1));
            std::string hex_num;
            while (iss >> hex_num) {
                try {
                    // stoi 함수를 사용하여 16진수 문자열을 정수로 변환
                    int decimal_num = std::stoi(hex_num, nullptr, 16);
                    numbers.push_back(decimal_num);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error converting '" << hex_num << "' to decimal: " << e.what() << std::endl;
                }
            }
            // 필수 데이터 확인
            if (numbers.size() <= 3) {
                throw std::runtime_error("Not enough data");
            }

            // 데이터 처리
            int first_index = numbers[1];
            int last_index = numbers[2];
            int num_cells = last_index - first_index + 1;
            int zone_type = numbers[3];
            int element_type = numbers[4];

            if (zone_type == 0) {
                return; // dead zone은 무시
            }

            std::string key;
            std::vector<std::vector<int>> data;

            // 키와 노드 수 추출
            // num_nodes_per_cell, num_faces_per_cell
            std::vector<std::pair<int, int>> elm_types =
            {
                {0, 0}, // "mixed"
                {3, 3}, // "triangular"
                {4, 4}, // "tetrahedral"
                {4, 4}, // "quadrilateral"
                {8, 6}, // "hexahedral"
                {5, 5}, // "pyramid"
                {6, 5}, // "wedge"
                {0, 0}, // "polyhedral"
            };

            //// `(`로 건너뛰고 이전에 나온 `)`와 `(` 사이에 비공백 문자가 있는지 확인
            //if (line.back() != '(') {
            //    char c;
            //    while (true) {
            //        c = file.get();
            //        if (c == '(') {
            //            break;
            //        }
            //        if (!std::isspace(c)) {
            //            // `(` 이전에 비공백 문자를 발견하면 선언 라인으로 간주하고
            //            // `)`까지 건너뜁니다.
            //            while (true) {
            //                c = file.get();
            //                if (c == ')') {
            //                    break;
            //                }
            //            }
            //            return;
            //        }
            //    }
            //}

            //if (key == "mixed") {
            //    // 데이터 읽기
            //    // TODO: mixed 셀 읽기 구현
            //    data = {};
            //}
            //else {
            //    // ASCII 또는 바이너리 데이터 읽기
            //    data.resize(num_cells, std::vector<int>(num_nodes_per_cell));
            //    if (match[1].str() == "") {
            //        // ASCII 데이터
            //        for (int k = 0; k < num_cells; ++k) {
            //            std::string data_line;
            //            std::getline(file, data_line);
            //            std::istringstream data_iss(data_line);
            //            for (int d = 0; d < num_nodes_per_cell; ++d) {
            //                if (!(data_iss >> std::hex >> data[k][d])) {
            //                    throw std::runtime_error("Invalid cell data");
            //                }
            //            }
            //        }
            //    }
            //    else {
            //        if (key == "mixed") {
            //            throw std::runtime_error("Cannot read mixed cells in binary mode yet");
            //        }
            //        // binary cells
            //        std::string bin_key = match[1].str();
            //        std::vector<std::vector<int64_t>> bin_data(num_cells, std::vector<int64_t>(num_nodes_per_cell));
            //        std::vector<int> bin_numbers(num_cells * num_nodes_per_cell);
            //        if (bin_key == "20") {
            //            file.read(reinterpret_cast<char*>(bin_numbers.data()), sizeof(int32_t) * num_cells * num_nodes_per_cell);
            //        }
            //        else if (bin_key == "30") {
            //            file.read(reinterpret_cast<char*>(bin_numbers.data()), sizeof(int64_t) * num_cells * num_nodes_per_cell);
            //        }
            //        else {
            //            throw std::runtime_error("Invalid binary key");
            //        }
            //        for (int i = 0; i < num_cells; ++i) {
            //            for (int j = 0; j < num_nodes_per_cell; ++j) {
            //                data[i][j] = bin_numbers[i * num_nodes_per_cell + j];
            //            }
            //        }
            //    }
            //}
        }




        // 파일에서 얼굴 데이터를 읽어오고 처리하는 함수
        void read_faces(
            std::ifstream& file,
            //std::istringstream& file,
            const std::string& line,
            bool is_binary, bool is_64bit) {

            // self-contained line 여부 확인
            if (std::count(line.begin(), line.end(), '(') == std::count(line.begin(), line.end(), ')')) {
                return; // self-contained line은 무시
            }

            auto& f2v = *f2v_p;
            auto& f2c = *f2c_p;

            // (13 (zone-id first-index last-index type element-type))

            // 정규식을 사용하여 데이터를 추출
            std::regex re2(R"(\(\d+ \(([^)]+)\))");
            std::smatch match;
            std::regex_search(line, match, re2);

            // 문자열을 공백을 기준으로 분할하여 각 숫자를 추출
            std::vector<int> numbers;
            std::istringstream iss(match.str(1));
            std::string hex_num;
            while (iss >> hex_num) {
                try {
                    numbers.push_back(std::stoi(hex_num, nullptr, 16));
                }
                catch (const std::exception& e) {
                    std::cerr << "Error converting '" << hex_num << "' to decimal: " << e.what() << std::endl;
                }
            }

            // 필수 데이터 확인
            assert(numbers.size() > 4);

            // 데이터 처리
            int zone_id = numbers[0];
            int first_index = numbers[1];
            int last_index = numbers[2];
            int num_faces = last_index - first_index + 1;
            int element_type = numbers[4];
            int bc_type = numbers[3];

            //==============================================================
            // 바운더리 셋팅
            auto it = boundaries_map.find(zone_id);
            if (it == boundaries_map.end()) {

                BC bc;
                bc.name = "";
                bc.startFace = first_index - 1;
                bc.nFaces = num_faces;
                bc.myProcNo = semo::SEMO_MAX_INDEX;
                bc.rightProcNo = semo::SEMO_MAX_INDEX;

                boundaries_map.insert(std::make_pair(zone_id, bc));

            }
            else {
                it->second.startFace = first_index - 1;
                it->second.nFaces = num_faces;
            }

            //==============================================================


            // 키와 노드 수 추출
            // face-type
            // nodes/face
            std::vector<int> num_nodes =
            {
                0, // "mixed"
                0, // 없음
                2, // "linear"
                3, // "triangular"
                4, // "quadrilateral"
                0, // "polygonal"
            };

            // bc-type
            // nodes/face
            std::vector<int> elm_types =
            {
                0, 0, // "없음"
                0, // "interior"
                2, // "wall"
                3, // "pressure-inlet, inlet-vent, intake-fan"
                4, // "pressure-outlet, exhaust-fan, outlet-vent"
                0, // "없음"
                0, // "symmetry"
                0, // "periodic-shadow"
                0, // "pressure-far-field"
                0, // "없음"
                0, // "velocity-inlet"
                0, // "없음"
                0, // "periodic"
                0, 0, 0, 0, 0, // "없음"
                0, // "fan, porous-jump, radiator"
                0, 0, 0, // "없음"
                0, // "mass-flow-inlet"
                0, 0, 0, 0, 0, 0, // "없음"
                0, // "interface"
                0, 0, 0, 0, // "없음"
                0, // "parent (hanging node)"
                0, // "outflow"
                0, // "axis"
            };


            // 문자열 끝의 공백 제거
            std::string copy_line = line;
            semo::rtrim(copy_line);


            // 문자열에 '('가 있는지 확인
            bool has_opening_bracket = !copy_line.empty() && copy_line.back() == '(';

            // 다음 줄 시작 부분으로 이동
            if (!has_opening_bracket) {
                char ch = ' ';
                while (ch != '(') {
                    file.get(ch);
                }
            }


            // Boundary condition 데이터 저장
            auto& bc_nFaces = *bc_nFaces_p;
            auto& bc_startFace = *bc_startFace_p;
            if (bc_type != 2) {
                bc_startFace.push_back(f2v.size());
                bc_nFaces.push_back(num_faces);
            }

            f2v.resize(first_index + num_faces - 1);
            f2c.resize(first_index + num_faces - 1,
                { semo::SEMO_MAX_INDEX, semo::SEMO_MAX_INDEX });


            // 데이터 읽기
            // ASCII 데이터
            if (is_binary == false) {
                for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                    std::string data_line;
                    do {
                        std::getline(file, data_line);
                        data_line = data_line.substr(data_line.find_first_not_of(' '));
                    } while (data_line.empty());

                    std::istringstream data_iss(data_line);
                    size_t v_in;

                    // mixed
                    if (element_type == 0 || element_type == 5) {
                        size_t num_v;
                        data_iss >> std::hex >> num_v;
                        for (int i = 0; i < num_v; ++i) {
                            data_iss >> std::hex >> v_in;
                            f2v[k].push_back(v_in - 1);
                        }
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) f2c[k][0] = (v_in - 1);
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) f2c[k][1] = (v_in - 1);
                    }
                    else {
                        int num_nodes_per_cell = num_nodes[element_type];
                        for (int i = 0; i < num_nodes_per_cell; ++i) {
                            data_iss >> std::hex >> v_in;
                            f2v[k].push_back(v_in - 1);
                        }
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) f2c[k][0] = (v_in - 1);
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) f2c[k][1] = (v_in - 1);
                    }
                }
            }
            // 바이너리 데이터
            else {

                // 파일의 현재 시작부터
                // 이진 데이터 길이를 계산하고, 파일 포인터를 원래 위치로 되돌림
                //size_t length = semo::calculate_binary_length(file, "End of Binary");

                if (is_64bit == false) {

                    using read_data_t = uint16_t;

                    read_data_t datas;
                    // mixed
                    if (element_type == 0 || element_type == 5) {

                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            read_data_t fsize = datas;
                            for (size_t i = 0; i < fsize; ++i) {
                                file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                                f2v[k].push_back(datas - 1);
                            }
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][0] = (datas - 1);
                            else assert(false);
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][1] = (datas - 1);
                            else f2c[k][1] = semo::SEMO_MAX_INDEX;
                        }

                    }
                    else {
                        auto num_nodes_per_cell = num_nodes[element_type];
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            for (size_t i = 0; i < num_nodes_per_cell; ++i) {
                                file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                                f2v[k].push_back(datas - 1);
                            }
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][0] = (datas - 1);
                            else assert(false);
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][1] = (datas - 1);
                            else f2c[k][1] = semo::SEMO_MAX_INDEX;
                        }

                    }
                }
                else {
                    using read_data_t = uint32_t;

                    read_data_t datas;
                    // mixed
                    if (element_type == 0 || element_type == 5) {

                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            read_data_t fsize = datas;
                            for (size_t i = 0; i < fsize; ++i) {
                                file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                                f2v[k].push_back(datas - 1);
                            }
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][0] = (datas - 1);
                            else assert(false);
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][1] = (datas - 1);
                            else f2c[k][1] = semo::SEMO_MAX_INDEX;
                        }

                    }
                    else {
                        auto num_nodes_per_cell = num_nodes[element_type];
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            for (size_t i = 0; i < num_nodes_per_cell; ++i) {
                                file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                                f2v[k].push_back(datas - 1);
                            }
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][0] = (datas - 1);
                            else assert(false);
                            file.read(reinterpret_cast<char*>(&datas), sizeof(read_data_t));
                            if (datas != 0) f2c[k][1] = (datas - 1);
                            else f2c[k][1] = semo::SEMO_MAX_INDEX;
                        }

                    }
                }

            }
        }


        // 바운더리
        void read_boundaries(
            std::ifstream& file,
            const std::string& line) {

            // (45 (zone-id fluid-bc-id bc-name))

            // 정규식을 사용하여 데이터를 추출
            std::regex re2(R"(\((\d+) \((.*?)\))"); // 수정된 정규식
            std::smatch match;
            if (std::regex_search(line, match, re2)) {
                // zone-id
                //std::string zone_id = match[1].str();
                // fluid unspecified 부분 추출
                std::string data = match[2].str();

                //std::cout << "Zone ID: " << zone_id << std::endl;
                std::istringstream iss(data);
                std::string data0, bc_type_fluent, bc_name;
                iss >> data0 >> bc_type_fluent >> bc_name; // 수정된 부분
                int zone_id = std::stoi(data0);

                auto& bc_names = *bc_names_p;
                auto& bc_nFaces = *bc_nFaces_p;
                auto& bc_startFace = *bc_startFace_p;
                auto& bc_myProcNo = *bc_myProcNo_p;
                auto& bc_rightProcNo = *bc_rightProcNo_p;


                auto it = boundaries_map.find(zone_id);
                if (it == boundaries_map.end()) {

                    BC bc;
                    bc.name = semo::trim(bc_name);
                    bc.startFace = semo::SEMO_MAX_INDEX;
                    bc.nFaces = semo::SEMO_MAX_INDEX;
                    bc.myProcNo = semo::SEMO_MAX_INDEX;
                    bc.rightProcNo = semo::SEMO_MAX_INDEX;

                    boundaries_map.insert(std::make_pair(zone_id, bc));
                }
                else {
                    it->second.name = semo::trim(bc_name);
                }



            }
            else {
                std::cerr << "No match found" << std::endl;
            }


        }

	};



	class Saver_ansys {
	public:

        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector <std::array<size_t, 2>>* f2c_p;

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

		void save(const std::string& filename, bool is_binary = false) {

            auto& pos = *pos_p;
            auto& f2v = *f2v_p;
            auto& f2c = *f2c_p;
            auto& bc_startFace = *bc_startFace_p;
            auto& bc_nFaces = *bc_nFaces_p;


            //{
            //    std::ofstream file_test("E:/[[ meshs ]]/ansys/test.cas");
            //    //write_face_func(file_test, bc_startFace[0], bc_startFace[0] + bc_nFaces[0]);

            //    for (size_t k = bc_startFace[0]; k < bc_startFace[0] + bc_nFaces[0]; ++k) {
            //        auto& ivs = f2v[k];
            //        auto& ics = f2c[k];
            //        int data = ivs.size();
            //        file_test.write((char*)&data, sizeof(int));
            //        //for (auto iv : ivs) {
            //        //    int data = iv;
            //        //    file_test.write((char*)&data, sizeof(int));
            //        //}
            //        // 이진 모드에서 데이터 쓰기
            //        for (auto iv : ivs) {
            //            char byte_data[sizeof(int)];
            //            // iv를 char 배열로 형변환하여 파일에 씀
            //            for (size_t i = 0; i < sizeof(int); ++i) {
            //                byte_data[i] = static_cast<char>((iv >> (8 * i)) & 0xFF);
            //            }
            //            std::cout << iv << std::endl;
            //            file_test.write(byte_data, sizeof(int));
            //        }

            //        if (ics[0] != semo::SEMO_MAX_INDEX) {
            //            int data = ics[0];
            //            file_test.write((char*)&data, sizeof(int));
            //        }
            //        else {
            //            int data = 0;
            //            file_test.write((char*)&data, sizeof(int));
            //        }
            //        if (ics[1] != semo::SEMO_MAX_INDEX) {
            //            int data = ics[1];
            //            file_test.write((char*)&data, sizeof(int));
            //        }
            //        else {
            //            int data = 0;
            //            file_test.write((char*)&data, sizeof(int));
            //        }
            //    }


            //    //for (size_t k = bc_startFace[0]; k < bc_startFace[0] + bc_nFaces[0]; ++k) {
            //    //    auto& ivs = f2v[k];
            //    //    auto& ics = f2c[k];
            //    //    int32_t value;
            //    //    std::cout << "뭐꼬 " << k << " " << ivs.size() << std::endl;
            //    //    value = ivs.size();
            //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));

            //    //    for (size_t jj = 0; jj < ivs.size(); ++jj) {
            //    //        int32_t iv_in = ivs[jj] + 1;
            //    //        file_test.write(reinterpret_cast<const char*>(&iv_in), sizeof(int32_t));
            //    //    }

            //    //    //for (int jj = 0; jj < 4; ++jj) {
            //    //    //    //value = iv + 1;
            //    //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    //}

            //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));

            //    //    //if (ics[0] != semo::SEMO_MAX_INDEX) {
            //    //    //    value = ics[0] + 1;
            //    //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    //}
            //    //    //else {
            //    //    //    value = 0;
            //    //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    //}
            //    //    //if (ics[1] != semo::SEMO_MAX_INDEX) {
            //    //    //    value = ics[1] + 1;
            //    //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    //}
            //    //    //else {
            //    //    //    value = 0;
            //    //    //    file_test.write(reinterpret_cast<const char*>(&value), sizeof(int32_t));
            //    //    //}
            //    //}
            //    file_test.close();
            //}
            //{
            //    std::ifstream file_test("E:/[[ meshs ]]/ansys/test.cas");
            //    int data;
            //    std::cout << bc_startFace[0] << " " << bc_startFace[0] + bc_nFaces[0] << std::endl;
            //    for (int k = bc_startFace[0]; k < bc_startFace[0] + bc_nFaces[0]; ++k) {
            //        file_test.read((char*)&data, sizeof(int));
            //        int fsize = data;
            //        std::cout << fsize << std::endl;
            //        for (size_t i = 0; i < fsize; ++i) {
            //            file_test.read((char*)&data, sizeof(int));
            //        }
            //        file_test.read((char*)&data, sizeof(int));
            //        file_test.read((char*)&data, sizeof(int));
            //        if (file_test.eof()) {
            //            std::cout << k << std::endl;
            //            std::cout << "End of file reached." << std::endl;
            //            break;
            //        }
            //    }
            //    file_test.close();
            //}




            std::ofstream file(filename);
            if (!file) {
                throw std::runtime_error("Could not open file " + filename);
            }








            size_t num_cells = 0;
            for (auto& ics : f2c) {
                for (auto& ic : ics) {
                    if (ic != semo::SEMO_MAX_INDEX && 
                        ic > num_cells) {
                        num_cells = ic;
                    }
                }
            }
            ++num_cells;


            file << "(1 \"Exported from SEMO\")\n";
            file << "\n";
            file << "(2 3)\n";
            file << "\n";
            file << "(10 (" << "0 1 " << std::hex << pos.size() << " 0 3" << "))\n";
            file << "(12 (" << "0 1 " << std::hex << num_cells << " 0" << "))\n";
            file << "(13 (" << "0 1 " << std::hex << f2v.size() << " 0" << "))\n";
            file << "\n";

            // ansys 인덱스는 1부터 시작..
            // 바이너리는 3010, 3012, 3013,...
            // 20 은 16비트, 30 은 32비트




            // 포인트 데이터
            // (10 (zone-id first-index last-index type(의미없음) ND(차원))
            if (is_binary == false) {
                file << "(10 (" << "1 1 " << std::hex << pos.size() << " 1 3" << ")(\n";
                for (auto& p : pos) {
                    file << p[0] << " " << p[1] << " " << p[2] << std::endl;
                }
                file << "))\n";
            }
            else {
                file << "(3010 (" << "1 1 " << std::hex << pos.size() << " 1 3" << ")\n";
                file.write("(", 1);
                for (auto& p : pos) {
                    file.write(
                        reinterpret_cast<const char*>(p.data()),
                        3 * sizeof(double));
                }
                file.write(")\n", 2);
                file << "End of Binary Section 3010)\n";
            }
            file << "\n";






            // 셀 데이터
            // (12 (zone-id first-index last-index type element-type))
            //cell element-type = {
            //      # "mixed": 0,
            //      "triangle" : 1,
            //      "tetra" : 2,
            //      "quad" : 3,
            //      "hexahedron" : 4,
            //      "pyramid" : 5,
            //      "wedge" : 6,
            //      # "polyhedral": 7,
            //}
            if (is_binary == false) {
                file << "(12 (" << "2 1 " << std::hex << num_cells << " 1 7" << "))\n";
                //file << "(12 (" << "2 1 " << std::hex << num_cells << " 1 4" << "))\n";
            }
            else {
                file << "(3012 (" << "2 1 " << std::hex << num_cells << " 1 7" << ")\n";
                file << "End of Binary Section   3012)\n";
            }
            file << "\n";



            // 페이스 데이터
            // (13 (zone-id first-index last-index type element-type))
            //face element-type = {
            //    ("mixed", None),
            //    ("triangle", 3),
            //    ("tetra", 4),
            //    ("quad", 4),
            //    ("hexahedron", 8),
            //    ("pyramid", 5),
            //    ("wedge", 6),
            //}

            if (is_binary == false) {

                auto write_face_func = [&](int str, int end) {

                    for (size_t i = str; i < end; ++i) {
                        auto& ivs = f2v[i];
                        auto& ics = f2c[i];
                        file << std::hex << ivs.size();
                        for (auto& iv : ivs) {
                            file << " " << std::hex << iv + 1;
                        }
                        if (ics[0] != semo::SEMO_MAX_INDEX) {
                            file << " " << std::hex << ics[0] + 1;
                        }
                        else {
                            file << " " << 0;
                        }
                        if (ics[1] != semo::SEMO_MAX_INDEX) {
                            file << " " << std::hex << ics[1] + 1;
                        }
                        else {
                            file << " " << 0;
                        }
                        file << "\n";
                    }
                };


                // 내부 페이스
                file << "(13 (" <<
                    std::hex << 3 << " " <<
                    std::hex << 1 << " " <<
                    std::hex << bc_startFace[0] << " " <<
                    std::hex << 2 << " 5)(\n";
                write_face_func(0, bc_startFace[0]);
                file << "))\n\n";

                // 바운더리 페이스
                for (int ibc = 0; ibc < bc_startFace.size(); ++ibc) {
                    auto startFace = bc_startFace[ibc];
                    auto nFaces = bc_nFaces[ibc];
                    file << "(13 (" <<
                        std::hex << ibc + 4 << " " <<
                        std::hex << startFace + 1 << " " <<
                        std::hex << startFace + nFaces << " " <<
                        std::hex << 3 << " 5)(\n";
                    write_face_func(startFace, startFace + nFaces);
                    file << "))\n\n";
                }
            }
            else {

                using write_data_t = int32_t;


                auto write_face_func = [&f2v, &f2c](std::ofstream& fff, size_t str, size_t end) {

                    for (size_t i = str; i < end; ++i) {
                        auto& ivs = f2v[i];
                        auto& ics = f2c[i];
                        semo::write_binary<write_data_t>(fff,
                            static_cast<write_data_t>(ivs.size()));
                        for (auto& iv : ivs) {
                            semo::write_binary<write_data_t>(fff,
                                static_cast<write_data_t>(iv + 1));
                        }
                        if (ics[0] != semo::SEMO_MAX_INDEX) {
                            semo::write_binary<write_data_t>(fff,
                                static_cast<write_data_t>(ics[0] + 1));
                        }
                        else {
                            semo::write_binary<write_data_t>(fff,
                                static_cast<write_data_t>(0));
                        }
                        if (ics[1] != semo::SEMO_MAX_INDEX) {
                            semo::write_binary<write_data_t>(fff,
                                static_cast<write_data_t>(ics[1] + 1));
                        }
                        else {
                            semo::write_binary<write_data_t>(fff,
                                static_cast<write_data_t>(0));
                        }
                    }
                };


                // 내부 페이스
                file << "(3013 (" <<
                    std::hex << 3 << " " <<
                    std::hex << 1 << " " <<
                    std::hex << bc_startFace[0] << " " <<
                    std::hex << 2 << " 5)\n";
                file.write("(", 1);



                write_face_func(file, 0, bc_startFace[0]);
                file.write(")\n", 2);
                file << "End of Binary Section   3013)\n\n";

                // 바운더리 페이스
                for (int ibc = 0; ibc < bc_startFace.size(); ++ibc) {
                    auto startFace = bc_startFace[ibc];
                    auto nFaces = bc_nFaces[ibc];
                    file << "(3013 (" <<
                        std::hex << ibc + 4 << " " <<
                        std::hex << startFace + 1 << " " <<
                        std::hex << startFace + nFaces << " " <<
                        std::hex << 3 << " 5)\n";
                    file.write("(", 1);
                    write_face_func(file, startFace, startFace + nFaces);
                    file.write(")\n", 2);
                    file << "End of Binary Section   3013)\n\n";
                }




            }
            file << "\n";

            // 바운더리
            //file << "(45 (" << "4 wall unspecified" << ")())\n";
            auto& bc_names = *bc_names_p;
            for (int ibc = 0; ibc < bc_startFace.size(); ++ibc) {
                auto startFace = bc_startFace[ibc];
                auto nFaces = bc_nFaces[ibc];
                file << "(45 (" <<
                    std::hex << ibc + 4 << " " <<
                    std::hex << "wall" << " " <<
                    std::hex << bc_names[ibc];
                file << ")())\n\n";
            }



            //file << "(13 (" << "4 91 f0 3 4" << ")(\n";
            //file << "\n";
            //file << "))\n";




        }


    private:

	};



}