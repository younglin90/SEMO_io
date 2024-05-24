#pragma once

#include "helper.h"

#include <vtkSmartPointer.h>
#include <vtkXMLUnstructuredGridReader.h>
#include <vtkXMLUnstructuredGridWriter.h>
#include <vtkUnstructuredGrid.h>
#include <vtkCell.h>
#include <vtkCellArray.h>
#include <vtkPoints.h>
#include <vtkPolyData.h>
#include <vtkDataSet.h>


namespace semo {

	class Loader_vtu {
	public:

        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* c2v_p;
        std::vector<std::vector<size_t>>* c2f_p;

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
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

            std::cout << "11" << std::endl;

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
            for (vtkIdType i = 0; i < numCells; ++i) {
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
            std::set<std::set<size_t>> set_f2v;
            for (vtkIdType i = 0; i < numCells; ++i) {
                vtkCell* cell = unstructuredGrid->GetCell(i);
                size_t numFaces = cell->GetNumberOfFaces();
                for (size_t faceId = 0; faceId < numFaces; ++faceId) {
                    vtkCell* face = cell->GetFace(faceId);
                    std::set<size_t> tmp_faces;
                    for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
                        vtkIdType vertexId = face->GetPointId(i);
                        tmp_faces.insert(vertexId);
                    }
                    set_f2v.insert(tmp_faces);
                }
            }
            //std::vector<std::set<size_t>> set_f2v_v(set_f2v.size());
            f2v.resize(set_f2v.size());
            {
                size_t num = 0;
                for (auto& item : set_f2v) {
                    //set_f2v_v[num] = item;
                    std::copy(item.begin(), item.end(), std::back_inserter(f2v[num]));
                    //if (std::equal(item.begin(),item.end(), mesh.f2v[num].begin())) std::cout << "a" << std::endl;
                    ++num;
                }
            }

            c2f.resize(numCells);
            for (size_t i = 0, size = c2f.size(); i < size; i++) {
                vtkCell* cell = unstructuredGrid->GetCell(i);
                for (vtkIdType faceId = 0; faceId < cell->GetNumberOfFaces(); ++faceId) {
                    vtkCell* face = cell->GetFace(faceId);
                    std::set<size_t> tmp_faces;
                    for (vtkIdType i = 0; i < face->GetNumberOfPoints(); ++i) {
                        vtkIdType vertexId = face->GetPointId(i);
                        tmp_faces.insert(vertexId);
                    }
                    //auto iter = std::lower_bound(set_f2v_v.begin(), set_f2v_v.end(), tmp_faces);
                    //mesh.c2f[i].push_back(std::distance(set_f2v_v.begin(), iter));
                    auto iter = std::lower_bound(f2v.begin(), f2v.end(), tmp_faces,
                        [](auto& a, auto& b) {
                            return std::lexicographical_compare(a.begin(), a.end(), b.begin(), b.end());
                        });
                    c2f[i].push_back(std::distance(f2v.begin(), iter));
                    //auto iter = set_f2v.find(tmp_faces);
                }
            }



		}

    private:


	};



	class Saver_vtu {
	public:


        std::vector<std::array<double, 3>>* pos_p;
        std::vector<std::vector<size_t>>* f2v_p;
        std::vector<std::vector<size_t>>* c2v_p;
        std::vector<std::vector<size_t>>* c2f_p;

        void set_pos(std::vector<std::array<double, 3>>* pos_p_in) {
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


		void save(std::string filename) {

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

			// Octree 포인터 벡터 leafs 생성
            vtkNew<vtkUnstructuredGrid> ugrid;
            vtkNew<vtkPoints> points;
            vtkNew<vtkCellArray> msh;;

			// 페이스 만들기
			for (auto& p : pos) points->InsertNextPoint(p[0], p[1], p[2]);
			ugrid->SetPoints(points);

			for (size_t i = 0, size = c2v.size();
				i < size; i++) {

				vtkNew<vtkIdList> cellPointIds;
                size_t psize = c2v[i].size();
                for (auto& iv : c2v[i]) {
                    cellPointIds->InsertNextId(iv);
                }

				vtkNew<vtkIdList> cellFaceIds;
                size_t fsize = c2f[i].size();
				for (auto& iface : c2f[i]) {
					size_t vsize = f2v[iface].size();
					cellFaceIds->InsertNextId(vsize);
					for (auto& pid : f2v[iface]) {
						cellFaceIds->InsertNextId(pid);
					}
				}

				ugrid->InsertNextCell(VTK_POLYHEDRON,
					psize, cellPointIds->GetPointer(0),
					fsize, cellFaceIds->GetPointer(0));

			}
			// 결과를 파일로 저장 (옵션)
			vtkNew<vtkXMLUnstructuredGridWriter> writer;

			// 현재 확장자가 .vtu 가 아니면 .vtu 추가
			std::string desiredExtension = ".vtu";
			if (!filename.ends_with(desiredExtension)) {
                filename += desiredExtension;
			}

			writer->SetFileName(filename.c_str());
			writer->SetInputData(ugrid);
			writer->Write();

		}


    private:
	};



}