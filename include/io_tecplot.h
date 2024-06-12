#pragma once

#include "common.h"

#include "TECIO.h"

namespace semo {

    class Loader_tecplot {
    public:
        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::array<size_t, 2>>* f2c_p;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_f2c(std::vector<std::array<size_t, 2>>* f2c_p_in) {
            f2c_p = f2c_p_in;
        }

        //void read_binary(std::string filename) {
        //    int32_t Debug = 1;
        //    int32_t FileFormat = 0;
        //    int32_t FileType = 0;
        //    int32_t FileIsDouble = 0;

        //    // Open the file for reading
        //    INTEGER4 FileHandle = -1;
        //    tecFileReaderOpen
        //    int32_t Result = TECOPEN(filename.c_str(), &FileFormat, &FileType,
        //        &FileHandle, &FileIsDouble, &Debug);
        //    if (Result != 0) {
        //        std::cerr << "Error: Unable to open file " << filename << std::endl;
        //        return;
        //    }

        //    // Read zone information
        //    INTEGER4 ZoneType, NumNodes, NumElems, NumFaces, NumBndryFaces;
        //    INTEGER4 NumBndryConns, ShrConn, StrandID, ParentZone, TotalNumFaces, ValueLocation;
        //    LgIndex_t NumValues;
        //    FieldDataType_t FldDataType;
        //    char* FName;
        //    Result = TECZONE(FileHandle, &ZoneType, &NumNodes, &NumElems,
        //        &NumFaces, &NumBndryFaces, &NumBndryConns,
        //        &ShrConn, &StrandID, &ParentZone,
        //        &TotalNumFaces, &ValueLocation,
        //        &NumValues, &FldDataType, &FName);
        //    if (Result != 0) {
        //        std::cerr << "Error: Unable to read zone information" << std::endl;
        //        TECCLOSE(FileHandle);
        //        return;
        //    }

        //    // Allocate memory for data
        //    std::vector<semo::point3_t> pos(NumNodes);
        //    std::vector<std::vector<size_t>> f2v(NumFaces);
        //    std::vector<std::array<size_t, 2>> f2c(NumFaces);

        //    // Read node coordinates
        //    Result = TECREADVALUES(FileHandle, &pos[0][0], 1, NumNodes, &pos[0][1], 2,
        //        &pos[0][2], 3, ValueLocation);
        //    if (Result != 0) {
        //        std::cerr << "Error: Unable to read node coordinates" << std::endl;
        //        TECCLOSE(FileHandle);
        //        return;
        //    }

        //    // Read connectivity data
        //    Result = TECREADCONNECT(FileHandle, &f2v[0][0], 1, NumFaces, &f2v[0][1], 1);
        //    if (Result != 0) {
        //        std::cerr << "Error: Unable to read face-to-vertex connectivity data" << std::endl;
        //        TECCLOSE(FileHandle);
        //        return;
        //    }

        //    // Read face-to-cell connectivity data
        //    Result = TECREADFACECONNECT(FileHandle, &f2c[0][0], &f2c[0][1], NumFaces);
        //    if (Result != 0) {
        //        std::cerr << "Error: Unable to read face-to-cell connectivity data" << std::endl;
        //        TECCLOSE(FileHandle);
        //        return;
        //    }

        //    // Close the file
        //    TECCLOSE(FileHandle);

        //    // Assign data to member variables
        //    pos_p = &pos;
        //    f2v_p = &f2v;
        //    f2c_p = &f2c;
        //}

        void load_ascii(std::string filename) {
            // Open the file
            std::ifstream file(filename);
            if (!file.is_open()) {
                std::cerr << "Error: Unable to open file " << filename << std::endl;
                return;
            }

            // Variables for reading data
            std::string line;
            bool readingDataSection = false;

            // Read the data
            while (std::getline(file, line)) {
                // If the line indicates the beginning of data section, set the flag
                if (line.find("TECPOLYZNE142") != std::string::npos) {
                    readingDataSection = true;
                    continue; // Skip the line containing section header
                }

                // Read data section
                if (readingDataSection) {
                    // Example: Reading node coordinates
                    if (line.find("TECDAT142") != std::string::npos) {
                        read_node_coordinates(file);
                    }
                    // Example: Reading connectivity data
                    else if (line.find("TECPOLYFACE142") != std::string::npos) {
                        read_connectivity_data(file);
                    }
                }
            }

            // Close the file
            file.close();
        }


    private:

        // Function to read node coordinates
        void read_node_coordinates(std::ifstream& file) {
            std::string line;
            auto& pos = *pos_p;
            pos.clear();
            while (std::getline(file, line)) {
                if (line.find("TECDAT142") != std::string::npos) {
                    // Read node coordinates
                    size_t numNodes;
                    file >> numNodes; // Assuming the number of nodes is given in the file
                    for (size_t i = 0; i < numNodes; ++i) {
                        semo::point3_t node;
                        file >> node[0] >> node[1] >> node[2]; // Assuming X, Y, Z coordinates are given
                        pos.push_back(node);
                    }
                    break; // Exit the loop after reading node coordinates
                }
            }
        }

        // Function to read connectivity data
        void read_connectivity_data(std::ifstream& file) {
            std::string line;
            auto& f2v = *f2v_p;
            auto& f2c = *f2c_p;
            f2v.clear();
            f2c.clear();
            while (std::getline(file, line)) {
                if (line.find("TECPOLYFACE142") != std::string::npos) {
                    // Read connectivity data
                    size_t numFaces;
                    file >> numFaces; // Assuming the number of faces is given in the file
                    f2v.resize(numFaces);
                    f2c.resize(numFaces);
                    for (size_t i = 0; i < numFaces; ++i) {
                        size_t numNodes, leftElem, rightElem;
                        file >> numNodes; // Number of nodes in the face
                        f2v[i].resize(numNodes);
                        for (size_t j = 0; j < numNodes; ++j) {
                            file >> f2v[i][j]; // Assuming node indices are given
                        }
                        file >> leftElem >> rightElem; // Left and right elements
                        f2c[i][0] = leftElem;
                        f2c[i][1] = rightElem;
                    }
                    break; // Exit the loop after reading connectivity data
                }
            }
        }




    };

    class Saver_tecplot {
    public:

        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::array<size_t, 2>>* f2c_p;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_f2c(std::vector<std::array<size_t, 2>>* f2c_p_in) {
            f2c_p = f2c_p_in;
        }


        void save(std::string filename, bool is_binary = true) {

            if (pos_p == nullptr) {
                std::cout << "not set pos pointer" << std::endl;
                return;
            }
            if (f2v_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            if (f2c_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;
            auto& f2c = *f2c_p;


            int32_t debug = 0;
            int32_t fileIsDouble = 1;
            int32_t fileType = 0;
            int32_t fileFormat = 0; // 0 == PLT, 1 == SZPLT; Only PLT is currently supported for poly zones
            int32_t result;

            result = TECINI142(
                "Polyhedral Hexahedrons",
                "X Y Z C",
                filename.c_str(),
                ".",
                &fileFormat,
                &fileType,
                &debug,
                &fileIsDouble);

            if (result != 0)
            {
                printf("Polyhexahedrons: error calling TECINI\n");
                return;
            }



            int32_t numVars = 4;

            int32_t zoneType = 7;     // FEPolyhedron
            int32_t numNodes = static_cast<int32_t>(pos.size());  // Number of nodes in the zone
            int32_t numElems = 0; // Number of elements in the zone.
            for (auto& ics : f2c) {
                for (auto& ic : ics) {
                    if (ic == semo::SEMO_MAX_INDEX) continue;
                    if (static_cast<int32_t>(ic) > numElems) {
                        numElems = static_cast<int32_t>(ic);
                    }
                }
            }
            ++numElems;

            int64_t numFaces = f2v.size();
            int64_t numTotFaceNodes = 0;
            for (auto& item : f2v) numTotFaceNodes += item.size();
            double  solTime = 0.0;
            int32_t strandID = 0;     // Static Zone
            int32_t unused = 0;       // ParentZone is no longer used
            int32_t numBoundaryFaces = 0;
            int32_t numBoundaryConnections = 0;
            int32_t shareConnectivity = 0;


            result = TECPOLYZNE142(
                "Polyhedral Hexahedron Zone",
                &zoneType,
                &numNodes,
                &numElems,
                &numFaces,
                &numTotFaceNodes,
                &solTime,
                &strandID,
                &unused,
                &numBoundaryFaces,
                &numBoundaryConnections,
                NULL,
                NULL,  // All nodal variables
                NULL,
                &shareConnectivity);
            if (result != 0)
            {
                printf("Polyhexahedrons: error calling TECPOLYZNE\n");
                return;
            }


            // write variable values out var by var, calling TECDAT at the end of each k-plane to use less temporary memory
            int32_t dIsDouble = 0;
            int32_t count = numNodes * 4;
            std::vector<float> values(count);
            for (int i = 0; i < numNodes; ++i) {
                values[i + numNodes * 0] = static_cast<float>(pos[i][0]);
                values[i + numNodes * 1] = static_cast<float>(pos[i][1]);
                values[i + numNodes * 2] = static_cast<float>(pos[i][2]);
                values[i + numNodes * 3] = static_cast<float>(pos[i][0] * pos[i][1] * pos[i][2]);
            }

            result = TECDAT142(&count, values.data(), &dIsDouble);


            // now write connectivity, calling TECPOLYFACE at the end of each k-plane to use less temporary memory
            std::vector<int32_t> faceNodeCounts;
            faceNodeCounts.reserve(numFaces);
            for (auto& item : f2v) faceNodeCounts.push_back(item.size());
            std::vector<int32_t> faceNodes;
            faceNodes.reserve(numTotFaceNodes);
            for (auto& item : f2v) {
                for (auto& it : item) {
                    faceNodes.push_back(it + 1);
                }
            }
            std::vector<int32_t> faceLeftElems;
            std::vector<int32_t> faceRightElems;
            faceLeftElems.reserve(numFaces);
            faceRightElems.reserve(numFaces);
            for (auto& item : f2c) {
                faceLeftElems.push_back(item[0] + 1);
                if (item[1] != semo::SEMO_MAX_INDEX) {
                    faceRightElems.push_back(item[1] + 1);
                }
                else {
                    faceRightElems.push_back(0);
                }
            }


            INTEGER4 numFaces32 = INTEGER4(numFaces);


            int ii = 0;
            for (int i = 0; i < numFaces; ++i) {
                int size = faceNodeCounts[i];
                assert(faceLeftElems[i] >= 0 && faceLeftElems[i] <= numElems);
                assert(faceRightElems[i] >= 0 && faceRightElems[i] <= numElems);
                for (int j = 0; j < size; ++j) {
                    assert(faceNodes[ii] <= numNodes);
                    ++ii;
                }
            }

            result = TECPOLYFACE142(
                &numFaces32,
                faceNodeCounts.data(),
                faceNodes.data(),
                faceLeftElems.data(),
                faceRightElems.data());


            result = TECEND142();
            if (result != 0)
            {
                printf("Polyhexahedrons: error calling TECEND\n");
                return;
            }


        }
    private:
    };



}