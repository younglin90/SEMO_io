#pragma once

#include "helper.h"

//#include "zlib/zlib.h"

namespace semo {


    // @ref ANSYS_Fluent_Users_Guide
	class Loader_ansys {
	public:

        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* f2c_p;

        std::vector<std::string>* bc_names_p{};
        std::vector<std::size_t>* bc_nFaces_p{};
        std::vector<std::size_t>* bc_startFace_p{};
        std::vector<std::size_t>* bc_myProcNo_p{};
        std::vector<std::size_t>* bc_rightProcNo_p{};

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_f2c(std::vector<std::vector<size_t>>* f2c_p_in) {
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
                    std::regex obj("\\(45 \\(\\d+ ([\\S]+) ([\\S]+)\\)");
                    if (std::regex_match(line, match, obj)) {
                        std::cerr << "Zone specification not supported yet (" << match[1] << ", " << match[2] << "). Skipping." << std::endl;
                    }
                    else {
                        std::cerr << "Zone specification not supported yet." << std::endl;
                    }
                }

            }

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
                    // stoi 함수를 사용하여 16진수 문자열을 정수로 변환
                    int decimal_num = std::stoi(hex_num, nullptr, 16);
                    numbers.push_back(decimal_num);
                }
                catch (const std::exception& e) {
                    std::cerr << "Error converting '" << hex_num << "' to decimal: " << e.what() << std::endl;
                }
            }
            // 필수 데이터 확인
            if (numbers.size() <= 4) {
                throw std::runtime_error("Not enough data");
            }

            int first_point_index = numbers[1];
            if (first_point_index_overall == 0) {
                first_point_index_overall = first_point_index;
            }
            if (last_point_index != 0) {
                if (last_point_index + 1 != first_point_index) {
                    throw std::runtime_error("Point arrays are not subsequent");
                }
            }
            last_point_index = numbers[2];
            int num_points = last_point_index - first_point_index + 1;
            int dim = numbers[4];


            // 데이터 읽기
            pos.resize(num_points);

            // 문자열 끝의 공백 제거
            std::string copy_line = line;
            while (!copy_line.empty() && std::isspace(copy_line.back())) {
                copy_line.pop_back();
            }
            // 문자열에 '('가 있는지 확인
            bool has_opening_bracket = !copy_line.empty() && copy_line.back() == '(';

            bool goingo = false;
            if (!has_opening_bracket) {
                // 다음 줄 시작부분으로,
                std::string data_line;
                char ch;
                file.get(ch);
                if (ch == '(') {
                    goingo = true;
                    //file.seekg(1, std::ios_base::cur);
                }
                else {
                    //file.seekg(1, std::ios_base::cur);
                    file.get(ch);
                    if (ch == '(') {
                        goingo = true;
                    }
                }
            }
            else {
                goingo = true;
            }

            if (goingo == false) return;

            if (is_binary == false) {
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
                // 바이너리 데이터
                if (is_64bit == false) {
                    int length = dim * num_points;
                    int buf_size = sizeof(float) * length;
                    std::vector<float> datas;
                    datas.resize(length);
                    // read 하면 읽은 부분 뒤쪽으로 알아서 이동하더라...
                    file.read(reinterpret_cast<char*>(datas.data()), buf_size);

                    size_t ii = 0;
                    for (size_t i = 0; i < num_points; ++i) {
                        pos[i][0] = datas[ii++];
                        pos[i][1] = datas[ii++];
                        pos[i][2] = datas[ii++];
                    }
                }
                else {
                    int length = dim * num_points;
                    int buf_size = sizeof(double)* length;
                    std::vector<double> datas;
                    datas.resize(length);
                    // read 하면 읽은 부분 뒤쪽으로 알아서 이동하더라...
                    file.read(reinterpret_cast<char*>(datas.data()), buf_size);

                    size_t ii = 0;
                    for (size_t i = 0; i < num_points; ++i) {
                        pos[i][0] = datas[ii++];
                        pos[i][1] = datas[ii++];
                        pos[i][2] = datas[ii++];
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
            if (numbers.size() <= 4) {
                throw std::runtime_error("Not enough data");
            }

            // 데이터 처리
            int first_index = numbers[1];
            int last_index = numbers[2];
            int num_faces = last_index - first_index + 1;
            int element_type = numbers[4];
            int bc_type = numbers[3];


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
            while (!copy_line.empty() && std::isspace(copy_line.back())) {
                copy_line.pop_back();
            }
            // 문자열에 '('가 있는지 확인
            bool has_opening_bracket = !copy_line.empty() && copy_line.back() == '(';

            bool goingo = false;
            if (!has_opening_bracket) {
                // 다음 줄 시작부분으로,
                std::string data_line;
                char ch;
                file.get(ch);
                if (ch == '(') {
                    goingo = true;
                    //file.seekg(1, std::ios_base::cur);
                }
                else {
                    //file.seekg(1, std::ios_base::cur);
                    file.get(ch);
                    if (ch == '(') {
                        goingo = true;
                    }
                }
            }
            else {
                goingo = true;
            }

            if (goingo == false) return;



            auto& bc_nFaces = *bc_nFaces_p;
            auto& bc_startFace = *bc_startFace_p;
            if (bc_type != 2) {
                bc_startFace.push_back(f2v.size());
                bc_nFaces.push_back(num_faces);
            }

            f2v.resize(first_index + num_faces - 1);
            f2c.resize(first_index + num_faces - 1);


            if (is_binary == false) {
                // ASCII 데이터
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
                        if (v_in != 0) {
                            f2c[k].push_back(v_in - 1);
                        }
                        else {
                            f2c[k].push_back(std::numeric_limits<size_t>::max());
                        }
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) {
                            f2c[k].push_back(v_in - 1);
                        }
                        else {
                            f2c[k].push_back(std::numeric_limits<size_t>::max());
                        }
                    }
                    else {
                        int num_nodes_per_cell = num_nodes[element_type];


                        for (int i = 0; i < num_nodes_per_cell; ++i) {
                            data_iss >> std::hex >> v_in;
                            f2v[k].push_back(v_in - 1);
                        }
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) {
                            f2c[k].push_back(v_in - 1);
                        }
                        else {
                            f2c[k].push_back(std::numeric_limits<size_t>::max());
                        }
                        data_iss >> std::hex >> v_in;
                        if (v_in != 0) {
                            f2c[k].push_back(v_in - 1);
                        }
                        else {
                            f2c[k].push_back(std::numeric_limits<size_t>::max());
                        }
                    }

                }

            }
            else {
                // 바이너리 데이터


                int length = 0;// num_faces* (num_nodes_per_cell + 2);
                // 버퍼 사이즈를 모를때,
                
                // 파일 위치 저장
                std::streampos savedPosition = file.tellg();
                {
                    // 현재 위치를 저장할 버퍼
                    std::vector<char> currentBuffer;
                    char ch;
                    while (file.get(ch)) {

                        std::streampos savedPosition2 = file.tellg();

                        if (ch == ')') {
                            char buffer[41]; // 40 characters + 1 null terminator
                            file.read(buffer, 40); // Read 10 characters
                            std::streamsize bytesRead = file.gcount(); // Get the number of characters actually read
                            buffer[bytesRead] = '\0'; // Null-terminate the buffer
                            std::string eksdj = std::string(buffer);
                            if (eksdj.find("End of Binary") != std::string::npos) {
                                break;
                            }
                            else {
                                // 이전에 저장한 위치로 돌아가기
                                file.seekg(savedPosition2);
                            }
                        }

                        // 현재 읽은 문자 버퍼에 저장
                        currentBuffer.push_back(ch);

                    }
                    length = currentBuffer.size();
                }


                // 이전에 저장한 위치로 돌아가기
                file.seekg(savedPosition);

                if (is_64bit == false) {
                    int buf_size = length;
                    std::vector<int16_t> datas;
                    datas.resize(length / sizeof(int16_t));
                    // read 하면 읽은 부분 뒤쪽으로 알아서 이동하더라...
                    file.read(reinterpret_cast<char*>(datas.data()), buf_size);

                    int ii = 0;
                    // mixed
                    if (element_type == 0 || element_type == 5) {
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            int num_v = datas[ii];
                            ++ii;
                            for (int i = 0; i < num_v; ++i) {
                                f2v[k].push_back(datas[ii] - 1);
                                ++ii;
                            }
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                        }
                    }
                    else {
                        int num_nodes_per_cell = num_nodes[element_type];
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            for (int i = 0; i < num_nodes_per_cell; ++i) {
                                f2v[k].push_back(datas[ii] - 1);
                                ++ii;
                            }
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                        }
                    }
                    
                }
                else {
                    int buf_size = length;
                    std::vector<int32_t> datas;
                    datas.resize(length / sizeof(int32_t));
                    // read 하면 읽은 부분 뒤쪽으로 알아서 이동하더라...
                    file.read(reinterpret_cast<char*>(datas.data()), buf_size);

                    int ii = 0;
                    // mixed
                    if (element_type == 0 || element_type == 5) {
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            int num_v = datas[ii];
                            ++ii;
                            for (int i = 0; i < num_v; ++i) {
                                f2v[k].push_back(datas[ii] - 1);
                                ++ii;
                            }
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                        }
                    }
                    else {
                        int num_nodes_per_cell = num_nodes[element_type];
                        for (int k = first_index - 1; k < first_index + num_faces - 1; ++k) {
                            for (int i = 0; i < num_nodes_per_cell; ++i) {
                                f2v[k].push_back(datas[ii] - 1);
                                ++ii;
                            }
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                            if (datas[ii] != 0) {
                                f2c[k].push_back(datas[ii] - 1);
                            }
                            else {
                                f2c[k].push_back(std::numeric_limits<size_t>::max());
                            }
                            ++ii;
                        }
                    }
                }
            }

        }






	};



	class Saver_ansys {
	public:

        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* f2c_p;

        std::vector<std::string>* bc_names_p{};
        std::vector<std::size_t>* bc_nFaces_p{};
        std::vector<std::size_t>* bc_startFace_p{};
        std::vector<std::size_t>* bc_myProcNo_p{};
        std::vector<std::size_t>* bc_rightProcNo_p{};

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_f2c(std::vector<std::vector<size_t>>* f2c_p_in) {
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

		void save(const std::string& filename) {

            std::ofstream file(filename);
            if (!file) {
                throw std::runtime_error("Could not open file " + filename);
            }

            auto& pos = *pos_p;
            auto& f2v = *f2v_p;
            auto& f2c = *f2c_p;

            size_t num_cells = 0;
            for (auto& ics : f2c) {
                for (auto& ic : ics) {
                    if (ic > num_cells && 
                        ic != std::numeric_limits<size_t>::max()) {
                        num_cells = ic;
                    }
                }
            }
            ++num_cells;


            file << "(1 \"Exported from SEMO\")\n";
            file << "(2 3)\n";
            file << "(10 (" << "0 1 " << std::hex << pos.size() << " 0 3" << "))\n";
            file << "(13 (" << "0 1 " << std::hex << f2v.size() << " 0" << "))\n";
            file << "(12 (" << "0 1 " << std::hex << num_cells << " 0" << "))\n";
            file << "(10 (" << "1 1 " << std::hex << pos.size() << " 1 3" << ")(\n";
            for (auto& p : pos) {
                file << p[0] << " " << p[1] << " " << p[2] << std::endl;
            }
            file << "))\n";
            file << "(12 (" << "2 1 " << std::hex << num_cells << " 1 7" << "))\n";
            //file << "(45 (" << "2 fluid unspecified" << ")())\n";


            auto& bc_startFace = *bc_startFace_p;
            auto& bc_nFaces = *bc_nFaces_p;


            file << "(13 (" <<
                std::hex << 3 << " " <<
                std::hex << 1 << " " <<
                std::hex << bc_startFace[0] << " " <<
                std::hex << 2 << " 5)(\n";
            for (size_t i = 0; i < bc_startFace[0]; ++i) {
                auto& ivs = f2v[i];
                auto& ics = f2c[i];
                file << std::hex << ivs.size();
                for (auto& iv : ivs) {
                    file << " " << std::hex << iv + 1;
                }
                if (ics[0] != std::numeric_limits<size_t>::max()) {
                    file << " " << std::hex << ics[0] + 1;
                }
                else {
                    file << " " << 0;
                }
                if (ics[1] != std::numeric_limits<size_t>::max()) {
                    file << " " << std::hex << ics[1] + 1;
                }
                else {
                    file << " " << 0;
                }
                file << "\n";
            }
            file << "))\n";


            for (int ibc = 0; ibc < bc_startFace.size(); ++ibc) {
                auto startFace = bc_startFace[ibc];
                auto nFaces = bc_nFaces[ibc];

                file << "(13 (" << 
                    std::hex << ibc + 4 << " " <<
                    std::hex << startFace + 1 << " " <<
                    std::hex << startFace + nFaces << " " <<
                    std::hex << 3 << " 5)(\n";
                for (size_t i = startFace; i < startFace + nFaces; ++i) {
                    auto& ivs = f2v[i];
                    auto& ics = f2c[i];
                    file << std::hex << ivs.size();
                    for (auto& iv : ivs) {
                        file << " " << std::hex << iv + 1;
                    }
                    if (ics[0] != std::numeric_limits<size_t>::max()) {
                        file << " " << std::hex << ics[0] + 1;
                    }
                    else {
                        file << " " << 0;
                    }
                    if (ics[1] != std::numeric_limits<size_t>::max()) {
                        file << " " << std::hex << ics[1] + 1;
                    }
                    else {
                        file << " " << 0;
                    }
                    file << "\n";
                }
                file << "))\n";
            }
            //file << "(45 (" << "3 interior interior - unspecified" << ")())\n";

            //file << "(13 (" << "4 91 f0 3 4" << ")(\n";
            //file << "\n";
            //file << "))\n";
            //file << "(45 (" << "4 wall unspecified" << ")())\n";


        }


    private:

	};



}