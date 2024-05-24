#pragma once

#include "helper.h"

#include "TECIO.h"

namespace semo {

    class Loader_tecplot {
    public:



    private:
    };

    class Saver_tecplot {
    public:

        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* f2c_p;

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_f2c(std::vector<std::vector<size_t>>* f2c_p_in) {
            f2c_p = f2c_p_in;
        }


        void save(std::string filename) {

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


            int32_t debug = 1;
            int32_t fileIsDouble = 0;
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
                values[i + numNodes * 0] = pos[i][0];
                values[i + numNodes * 1] = pos[i][1];
                values[i + numNodes * 2] = pos[i][2];
                values[i + numNodes * 3] = pos[i][0]* pos[i][1]*pos[i][2];
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
                if (item.size() > 1) {
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