#pragma once

#include "helper.h"

#define SEMO_USE_VTK_LIB

#ifdef SEMO_USE_VTK_LIB
#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDataSet.h>
#endif


namespace semo {

	class Loader_vtu {
	public:

        enum CellTypes {
            VTK_VERTEX = 1,
            VTK_POLY_VERTEX = 2,
            VTK_LINE = 3,
            VTK_POLY_LINE = 4,
            VTK_TRIANGLE = 5,
            VTK_TRIANGLE_STRIP = 6,
            VTK_POLYGON = 7,
            VTK_PIXEL = 8,
            VTK_QUAD = 9,
            VTK_TETRA = 10,
            VTK_VOXEL = 11,
            VTK_HEXAHEDRON = 12,
            VTK_WEDGE = 13,
            VTK_PYRAMID = 14,
            VTK_PENTAGONAL_PRISM = 15,
            VTK_HEXAGONAL_PRISM = 16,
            VTK_QUADRATIC_TRIANGLE = 22,
            VTK_QUADRATIC_QUAD = 23,
            VTK_POLYHEDRON = 42,
        };

        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* c2v_p;
        std::vector<std::vector<size_t>>* c2f_p;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_c2v(std::vector<std::vector<size_t>>* c2v_p_in) {
            c2v_p = c2v_p_in;
        }
        void set_c2f(std::vector<std::vector<size_t>>* c2f_p_in) {
            c2f_p = c2f_p_in;
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
            if (c2v_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            if (c2f_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;
            auto& c2v = *c2v_p;
            auto& c2f = *c2f_p;

            pos.clear();
            f2v.clear();
            c2v.clear();
            c2f.clear();


            // VTU 파일 리더 생성
            vtkNew<vtkXMLUnstructuredGridReader> reader;
            reader->SetFileName(filename.c_str());

            // VTU 파일 읽기
            reader->Update();

            // 읽어들인 Unstructured Grid 얻기
            vtkSmartPointer<vtkUnstructuredGrid> unstructuredGrid =
                reader->GetOutput();

            // Unstructured Grid 사용 예시
            // 예시: 점의 개수 출력
            size_t numPoints = unstructuredGrid->GetNumberOfPoints();
            size_t numCells = unstructuredGrid->GetNumberOfCells();
            //std::cout << "Number of points: " << unstructuredGrid->GetNumberOfPoints() << std::endl;
            pos.resize(numPoints);
            c2v.resize(numCells);

            // 정점
            vtkPoints* points = unstructuredGrid->GetPoints();
            for (vtkIdType i = 0; i < points->GetNumberOfPoints(); ++i) {
                double* p = points->GetPoint(i);
                // p[0], p[1], p[2]는 각각 x, y, z 좌표입니다.
                auto& pos_i = pos[i];
                pos_i[0] = p[0]; pos_i[1] = p[1]; pos_i[2] = p[2];
                //std::cout << "Point " << i << ": (" << p[0] << ", " << p[1] << ", " << p[2] << ")" << std::endl;
            }

            // 셀
            for (size_t i = 0; i < numCells; ++i) {
                vtkCell* cell = unstructuredGrid->GetCell(i);
                //std::cout << "Cell " << i << ":";
                c2v[i].resize(cell->GetNumberOfPoints());
                for (vtkIdType j = 0; j < cell->GetNumberOfPoints(); ++j)
                {
                    //std::cout << " " << cell->GetPointId(j);
                    c2v[i][j] = cell->GetPointId(j);
                }
                //std::cout << std::endl;
            }

            // 면
            std::set<std::pair<std::set<size_t>, size_t>,
                decltype([](auto& a, auto& b) {
                    return a.first < b.first;
                })
            > set_f2v_finder;
            size_t num_faces = 0;
            for (size_t i = 0; i < numCells; ++i) {
                vtkCell* cell = unstructuredGrid->GetCell(i);
                size_t numFaces = cell->GetNumberOfFaces();
                std::vector<size_t> tmp_c2f_order;
                tmp_c2f_order.reserve(numFaces);
                for (size_t faceId = 0; faceId < numFaces; ++faceId) {
                    vtkCell* face = cell->GetFace(static_cast<int>(faceId));
                    std::pair<std::set<size_t>, size_t> tmp_faces;
                    for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
                        vtkIdType vertexId = face->GetPointId(i);
                        tmp_faces.first.insert(vertexId);
                    }
                    tmp_faces.second = num_faces;
                    auto result_b = set_f2v_finder.insert(tmp_faces);
                    if (result_b.second == true) {
                        //// 새로운 요소가 삽입되었을 때만 second 값을 설정
                        //auto& inserted_element = 
                        //    const_cast<std::pair<std::set<size_t>, size_t>&>(*result_b.first);
                        //inserted_element.second = 5;

                        // f2v 적용
                        std::vector<size_t> tmp_faces_order;
                        tmp_faces_order.reserve(face->GetNumberOfPoints());
                        int type_id = face->GetCellType();
                        switch (type_id) {
                            case CellTypes::VTK_PIXEL: {
                                tmp_faces_order.push_back(face->GetPointId(0));
                                tmp_faces_order.push_back(face->GetPointId(1));
                                tmp_faces_order.push_back(face->GetPointId(3));
                                tmp_faces_order.push_back(face->GetPointId(2));
                                break;
                            }
                            case CellTypes::VTK_QUADRATIC_TRIANGLE: {
                                tmp_faces_order.push_back(face->GetPointId(0));
                                tmp_faces_order.push_back(face->GetPointId(3));
                                tmp_faces_order.push_back(face->GetPointId(1));
                                tmp_faces_order.push_back(face->GetPointId(4));
                                tmp_faces_order.push_back(face->GetPointId(2));
                                tmp_faces_order.push_back(face->GetPointId(5));
                                break;
                            }
                            case CellTypes::VTK_QUADRATIC_QUAD: {
                                tmp_faces_order.push_back(face->GetPointId(0));
                                tmp_faces_order.push_back(face->GetPointId(4));
                                tmp_faces_order.push_back(face->GetPointId(1));
                                tmp_faces_order.push_back(face->GetPointId(5));
                                tmp_faces_order.push_back(face->GetPointId(2));
                                tmp_faces_order.push_back(face->GetPointId(6));
                                tmp_faces_order.push_back(face->GetPointId(3));
                                tmp_faces_order.push_back(face->GetPointId(7));
                                break;
                            }
                            default: {
                                for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
                                    vtkIdType vertexId = face->GetPointId(i);
                                    tmp_faces_order.push_back(vertexId);
                                }
                                break;
                            }
                        }

                        f2v.push_back(tmp_faces_order);
                        tmp_c2f_order.push_back(num_faces);
                        ++num_faces;
                    }
                    else {
                        tmp_c2f_order.push_back(result_b.first->second);
                    }
                }
                assert(tmp_c2f_order.size() >= 4);
                c2f.push_back(tmp_c2f_order);
            }

            // 체크
            for (auto& ivs : f2v) {
                assert(ivs.size() >= 3);
                for (auto& iv : ivs) {
                    assert(iv < pos.size());
                }
            }
            for (auto& ivs : c2v) {
                assert(ivs.size() >= 4);
                for (auto& iv : ivs) {
                    assert(iv < pos.size());
                }
            }
            for (auto& ifs : c2f) {
                assert(ifs.size() >= 4);
                for (auto& f : ifs) {
                    assert(f < f2v.size());
                }
            }


            // 면
            //std::set<std::set<size_t>> set_f2v;
            //for (size_t i = 0; i < numCells; ++i) {
            //    vtkCell* cell = unstructuredGrid->GetCell(i);
            //    size_t numFaces = cell->GetNumberOfFaces();
            //    for (size_t faceId = 0; faceId < numFaces; ++faceId) {
            //        vtkCell* face = cell->GetFace(static_cast<int>(faceId));
            //        std::set<size_t> tmp_faces;
            //        for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
            //            vtkIdType vertexId = face->GetPointId(i);
            //            tmp_faces.insert(vertexId);
            //        }
            //        set_f2v.insert(tmp_faces);
            //    }
            //}
            ////std::vector<std::set<size_t>> set_f2v_v(set_f2v.size());
            //f2v.resize(set_f2v.size());
            //{
            //    size_t num = 0;
            //    for (auto& item : set_f2v) {
            //        //set_f2v_v[num] = item;
            //        std::copy(item.begin(), item.end(), std::back_inserter(f2v[num]));
            //        //if (std::equal(item.begin(),item.end(), mesh.f2v[num].begin())) std::cout << "a" << std::endl;
            //        ++num;
            //    }
            //}

            //c2f.resize(numCells);
            //for (size_t i = 0, size = c2f.size(); i < size; i++) {
            //    vtkCell* cell = unstructuredGrid->GetCell(i);
            //    for (vtkIdType faceId = 0; faceId < cell->GetNumberOfFaces(); ++faceId) {
            //        vtkCell* face = cell->GetFace(faceId);
            //        std::set<size_t> tmp_faces;
            //        for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
            //            vtkIdType vertexId = face->GetPointId(i);
            //            tmp_faces.insert(vertexId);
            //        }
            //        //auto iter = std::lower_bound(set_f2v_v.begin(), set_f2v_v.end(), tmp_faces);
            //        //mesh.c2f[i].push_back(std::distance(set_f2v_v.begin(), iter));
            //        auto iter = std::lower_bound(f2v.begin(), f2v.end(), tmp_faces,
            //            [](auto& a, auto& b) {
            //                return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
            //            });
            //        c2f[i].push_back(std::distance(f2v.begin(), iter));
            //        //auto iter = set_f2v.find(tmp_faces);
            //    }
            //}



		}

    private:


	};



	class Saver_vtu {
	public:


        std::vector<semo::point3_t>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* c2v_p;
        std::vector<std::vector<size_t>>* c2f_p;
        std::vector<std::array<size_t, 2>>* f2c_p;

        void set_pos(std::vector<semo::point3_t>* pos_p_in) {
            pos_p = pos_p_in;
        }
        void set_f2v(std::vector<std::vector<size_t>>* f2v_p_in) {
            f2v_p = f2v_p_in;
        }
        void set_c2v(std::vector<std::vector<size_t>>* c2v_p_in) {
            c2v_p = c2v_p_in;
        }
        void set_c2f(std::vector<std::vector<size_t>>* c2f_p_in) {
            c2f_p = c2f_p_in;
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
            if (c2v_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            if (c2f_p == nullptr) {
                std::cout << "not set f2v pointer" << std::endl;
                return;
            }
            auto& pos = *pos_p;
            auto& f2v = *f2v_p;
            auto& c2v = *c2v_p;
            auto& c2f = *c2f_p;
            auto& f2c = *f2c_p;



#ifdef SEMO_USE_VTK_LIB
			// Octree 포인터 벡터 leafs 생성
            vtkNew<vtkUnstructuredGrid> ugrid;
            vtkNew<vtkPoints> points;
            vtkNew<vtkCellArray> msh;;

			// 페이스 만들기
			for (auto& p : pos) points->InsertNextPoint(p[0], p[1], p[2]);
			ugrid->SetPoints(points);

			for (size_t i = 0, size = c2v.size(); i < size; i++) {

				vtkNew<vtkIdList> cellPointIds;
                size_t psize = c2v[i].size();

				vtkNew<vtkIdList> cellFaceIds;
                size_t fsize = c2f[i].size();


                for (auto& iv : c2v[i]) {
                    cellPointIds->InsertNextId(iv);
                }

                for (auto& iface : c2f[i]) {
                    size_t vsize = f2v[iface].size();
                    cellFaceIds->InsertNextId(vsize);

                    // own->ngb 방향 고려
                    if (f2c[iface][0] == i) {
                        for (auto& pid : f2v[iface]) {
                            cellFaceIds->InsertNextId(pid);
                        }
                    }
                    else {
                        // 역방향
                        for (auto rit = f2v[iface].rbegin(); rit != f2v[iface].rend(); ++rit) {
                            cellFaceIds->InsertNextId(*rit);
                        }

                    }

                }

                // tetrahedron
                if (psize == 4 && fsize == 4) {

                    ugrid->InsertNextCell(VTKCellType::VTK_TETRA,
                        psize, cellPointIds->GetPointer(0),
                        fsize, cellFaceIds->GetPointer(0));
                }
                // hexahedron
                else if (psize == 8 && fsize == 6) {

                    // reordering
                    auto ivs = cellPointIds->GetPointer(0);
                    auto iface_down = c2f[i][0];
                    auto& ivs_down = f2v[iface_down];

                    // upper face 찾기
                    size_t iface_up = semo::SEMO_MAX_INDEX;
                    for (size_t indx = 1; indx < 6; ++indx) {
                        auto iface = c2f[i][indx];
                        auto& ivs_up = f2v[iface];

                        int count = 0;
                        for (auto& iv : ivs_up) {
                            if (std::find(ivs_down.begin(), ivs_down.end(), iv) != ivs_down.end()) {
                                ++count;
                                break;
                            }
                        }
                        if (count == 0) {
                            iface_up = iface;
                            break;
                        }

                    }

                    assert(iface_up != semo::SEMO_MAX_INDEX);

                    auto& ivs_up = f2v[iface_up];

                    size_t iv_down0 = ivs_down[0];
                    size_t iv_down1 = ivs_down[1];
                    size_t iv_down2 = ivs_down[2];
                    size_t iv_down3 = ivs_down[3];

                    std::vector<size_t> iv_down_tar = { iv_down0, iv_down1 };

                    //SPDLOG_INFO("{} {} {} {}", ivs_down[0], ivs_down[1], ivs_down[2], ivs_down[3]);
                    //SPDLOG_INFO("{} {} {} {}", ivs_up[0], ivs_up[1], ivs_up[2], ivs_up[3]);

                    //SPDLOG_INFO("{} {}", iv_down0, iv_down1);

                    size_t iv_up0 = semo::SEMO_MAX_INDEX;
                    size_t iv_up1 = semo::SEMO_MAX_INDEX;
                    for (size_t indx = 1; indx < 6; ++indx) {
                        auto iface = c2f[i][indx];
                        if (iface == iface_up) continue;
                        
                        auto& ivs_side = f2v[iface];

                        std::vector<size_t> ivs_side_tmp;
                        ivs_side_tmp.reserve(f2v[iface].size() + 2);
                        ivs_side_tmp.push_back(f2v[iface].back());
                        for (auto& item : f2v[iface]) ivs_side_tmp.push_back(item);
                        ivs_side_tmp.push_back(f2v[iface].front());

                        //SPDLOG_INFO("{} {} {} {}", ivs_side[0], ivs_side[1], ivs_side[2], ivs_side[3]);

                        // 정방향
                        auto it = std::search(
                            ivs_side_tmp.begin(), ivs_side_tmp.end(),
                            iv_down_tar.begin(), iv_down_tar.end()
                        );
                        if (it != ivs_side_tmp.end()) {
                            size_t order = it - ivs_side_tmp.begin();
                            if (order == 0) order = 3;
                            else if (order == 5) order = 0;
                            else order = order - 1;

                            iv_up0 = ivs_side[(order - 1 + 4) % 4];
                            iv_up1 = ivs_side[(order - 2 + 4) % 4];
                            break;
                        }

                        // 역방향
                        it = std::search(
                            ivs_side_tmp.begin(), ivs_side_tmp.end(),
                            iv_down_tar.rbegin(), iv_down_tar.rend()
                        );
                        if (it != ivs_side_tmp.end()) {
                            size_t order = it - ivs_side_tmp.begin();
                            if (order == 0) order = 3;
                            else if (order == 5) order = 0;
                            else order = order - 1;
                            //std::cout << order << std::endl;
                            iv_up0 = ivs_side[(order - 2 + 4) % 4];
                            iv_up1 = ivs_side[(order - 1 + 4) % 4];
                            break;
                        }

                    }

                    assert(iv_up0 != semo::SEMO_MAX_INDEX);
                    assert(iv_up1 != semo::SEMO_MAX_INDEX);

                    //std::cout << iv_up0 << " " << iv_up1 << std::endl;
                    //for (auto& item : ivs_up) {
                    //    std::cout << item << " ";
                    //}
                    //std::cout << std::endl;

                    // 나머지 찾기
                    std::vector<size_t> iv_up_tar = { iv_up0, iv_up1 };
                    size_t iv_up2 = semo::SEMO_MAX_INDEX;
                    size_t iv_up3 = semo::SEMO_MAX_INDEX;

                    //SPDLOG_INFO("{} {} {} {}", ivs_up[0], ivs_up[1], ivs_up[2], ivs_up[3]);
                    //SPDLOG_INFO("{} {} {} {}", ivs_down[0], ivs_down[1], ivs_down[2], ivs_down[3]);
                    //SPDLOG_INFO("{} {}", iv_up0, iv_up1);
                    //SPDLOG_INFO("{} {}", iv_down0, iv_down1);

                    std::vector<size_t> ivs_up_tmp;
                    ivs_up_tmp.reserve(ivs_up.size() + 2);
                    ivs_up_tmp.push_back(ivs_up.back());
                    for (auto& item : ivs_up) ivs_up_tmp.push_back(item);
                    ivs_up_tmp.push_back(ivs_up.front());
                    // 정방향
                    {
                        auto it = std::search(
                            ivs_up_tmp.begin(), ivs_up_tmp.end(),
                            iv_up_tar.begin(), iv_up_tar.end()
                        );
                        if (it != ivs_up_tmp.end()) {
                            size_t order = it - ivs_up_tmp.begin();
                            if (order == 0) order = 3;
                            else if (order == 5) order = 0;
                            else order = order - 1;
                            iv_up2 = ivs_up[(order - 2 + 4) % 4];
                            iv_up3 = ivs_up[(order - 1 + 4) % 4];
                        }
                    }
                    // 역방향
                    if(iv_up2 == semo::SEMO_MAX_INDEX)
                    {
                        auto it = std::search(
                            ivs_up_tmp.begin(), ivs_up_tmp.end(),
                            iv_up_tar.rbegin(), iv_up_tar.rend()
                        );
                        if (it != ivs_up_tmp.end()) {
                            size_t order = it - ivs_up_tmp.begin();
                            if (order == 0) order = 3;
                            else if (order == 5) order = 0;
                            else order = order - 1;
                            iv_up2 = ivs_up[(order - 1 + 4) % 4];
                            iv_up3 = ivs_up[(order - 2 + 4) % 4];
                            //iv_up2 = ivs_up[(order + 1) % 4];
                            //iv_up3 = ivs_up[(order + 2) % 4];
                        }
                    }

                    assert(iv_up2 != semo::SEMO_MAX_INDEX);
                    assert(iv_up3 != semo::SEMO_MAX_INDEX);

                    ivs[0] = iv_down0;
                    ivs[1] = iv_down1;
                    ivs[2] = iv_down2;
                    ivs[3] = iv_down3;
                    ivs[4] = iv_up0;
                    ivs[5] = iv_up1;
                    ivs[6] = iv_up2;
                    ivs[7] = iv_up3;

                    ugrid->InsertNextCell(VTKCellType::VTK_HEXAHEDRON,
                        psize, cellPointIds->GetPointer(0),
                        fsize, cellFaceIds->GetPointer(0));
                }
                //// wedge
                //else if (psize == 6 && fsize == 5) {

                //    // reordering

                //    ugrid->InsertNextCell(VTKCellType::VTK_WEDGE,
                //        psize, cellPointIds->GetPointer(0),
                //        fsize, cellFaceIds->GetPointer(0));
                //}
                // pyramid
                else if (psize == 5 && fsize == 5) {

                    // reordering
                    size_t quad_face_id = semo::SEMO_MAX_INDEX;
                    size_t indx = 0;
                    for (auto& iface : c2f[i]) {
                        size_t vsize = f2v[iface].size();
                        if (vsize == 4) {
                            quad_face_id = indx;
                            break;
                        }
                        ++indx;
                    }
                    assert(quad_face_id != semo::SEMO_MAX_INDEX);
                    auto& ivs = f2v[c2f[i][quad_face_id]];
                    auto p0 = cellPointIds->GetPointer(0);
                    for (int ind = 0; ind < 5; ++ind) {
                        auto iv_tar = p0[ind];
                        auto iter = std::find(ivs.begin(), ivs.end(), iv_tar);
                        if (iter == ivs.end()) {
                            p0[0] = ivs[0];
                            p0[1] = ivs[1];
                            p0[2] = ivs[2];
                            p0[3] = ivs[3];
                            p0[4] = iv_tar;
                            break;
                        }
                    }

                    ugrid->InsertNextCell(VTKCellType::VTK_PYRAMID,
                        psize, cellPointIds->GetPointer(0),
                        fsize, cellFaceIds->GetPointer(0));

                }
                // polyhedron
                else {

                    ugrid->InsertNextCell(VTKCellType::VTK_POLYHEDRON,
                        psize, cellPointIds->GetPointer(0),
                        fsize, cellFaceIds->GetPointer(0));
                }

			}
			// 결과를 파일로 저장 (옵션)
			vtkNew<vtkXMLUnstructuredGridWriter> writer;

			// 현재 확장자가 .vtu 가 아니면 .vtu 추가
			std::string desiredExtension = ".vtu";
			if (!filename.ends_with(desiredExtension)) {
                filename += desiredExtension;
			}

            if(is_binary == false) 
                writer->SetDataModeToAscii();

			writer->SetFileName(filename.c_str());
			writer->SetInputData(ugrid);
			writer->Write();

#else

            std::ofstream outputFile;

            std::string saveFormat = "ascii";

            outputFile.open(filename);
            if (outputFile.fail()) {
                std::cerr << "Unable to write file for writing." << endl;
            }
            outputFile << "<?xml version=\"1.0\"?>" << endl;

            outputFile << " <VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt64\">" << endl;
            outputFile << "  <UnstructuredGrid>" << endl;

            // vtk 파일 만들기
            // Field data
            outputFile << "    <FieldData>" << endl;
            outputFile << "    </FieldData>" << endl;

            outputFile << "   <Piece NumberOfPoints=\"" <<
                pos.size() <<
                "\" NumberOfCells=\"" <<
                c2v.size() <<
                // mesh.cells.size() + 1 << 
                "\">" << endl;

            // Points data
            outputFile << "    <PointData>" << endl;
            outputFile << "    </PointData>" << endl;



            // Cells data
            outputFile << "    <CellData>" << endl;
            outputFile << "    </CellData>" << endl;

            // Points
            outputFile << "    <Points>" << endl;
            {
                outputFile << "     <DataArray type=\"Float64\" Name=\"NodeCoordinates\" NumberOfComponents=\"3\" format=\"" << saveFormat << "\">" << endl;
                for (auto& p : pos) {
                    outputFile << p[0] << " " << p[1] << " " << p[2] << std::endl;
                }
                outputFile << "     </DataArray>" << endl;
            }
            outputFile << "   </Points>" << endl;





            // cells
            outputFile << "   <Cells>" << endl;
            // connectivity (cell's points)
            {
                outputFile << "    <DataArray type=\"Int32\" Name=\"connectivity\" format=\"" << saveFormat << "\">" << endl;
                for (auto& ivs : c2v) {
                    for (auto& iv : ivs) {
                        outputFile << iv << " ";
                    }
                    outputFile << std::endl;
                }
                outputFile << "     </DataArray>" << endl;
            }


    // offsets (cell's points offset)
            {
                outputFile << "    <DataArray type=\"Int32\" Name=\"offsets\" format=\"" << saveFormat << "\">" << endl;
                int cellFaceOffset = 0;
                for (auto& ivs : c2v) {
                    cellFaceOffset += ivs.size();
                    outputFile << cellFaceOffset << " ";
                }
                outputFile << std::endl;
                outputFile << "     </DataArray>" << endl;
            }

    // types (cell's type, 42 = polyhedron)
            {
                outputFile << "    <DataArray type=\"Int32\" Name=\"types\" format=\"" << saveFormat << "\">" << endl;
                for (auto& ivs : c2v) {
                    outputFile << "42" << " ";
                }
                outputFile << std::endl;
                outputFile << "     </DataArray>" << endl;
            }


            // faces (cell's faces number, each face's point number, cell's faces's points)
            {
                outputFile << "    <DataArray type=\"Int32\" IdType=\"1\" Name=\"faces\" format=\"" << saveFormat << "\">" << endl;
                for (auto& ifs : c2f) {
                    outputFile << ifs.size() << " ";
                    for (auto& f : ifs) {
                        auto& ivs = f2v[f];
                        outputFile << ivs.size() << " ";
                        for (auto& j : ivs) {
                            outputFile << j << " ";
                        }
                    }
                }
                outputFile << std::endl;
                outputFile << "     </DataArray>" << endl;
            }


            // faceoffsets (cell's face offset)
            {
                outputFile << "    <DataArray type=\"Int32\" IdType=\"1\" Name=\"faceoffsets\" format=\"" << saveFormat << "\">" << endl;
                int cellFacePointOffset = 0;
                for (auto& ifs : c2f) {
                    int numbering = 1 + ifs.size();
                    for (auto& f : ifs) {
                        auto& ivs = f2v[f];
                        numbering += ivs.size();
                    }
                    cellFacePointOffset += numbering;
                    outputFile << cellFacePointOffset << " ";
                }
                outputFile << "     </DataArray>" << endl;
            }

            outputFile << std::endl;
            outputFile << "   </Cells>" << endl;


            outputFile << "  </Piece>" << endl;
            outputFile << " </UnstructuredGrid>" << endl;


            outputFile << "</VTKFile>" << endl;

            outputFile.close();

#endif





		}


    private:
	};



}