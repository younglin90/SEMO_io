#pragma once

#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>

namespace semo {


	class Writer {
	public:


		void save_obj(std::string fileName,
			std::vector<std::array<double, 3>>& pos,
			std::vector<std::vector<size_t>>& f2v
			) {
			
			std::ofstream file(fileName);
			if (!file.is_open()) {
				std::cerr << "파일을 열 수 없습니다." << std::endl;
				return;
			}

			for (const auto& v : pos) {
				file << "v " << v[0] << " " << v[1] << " " << v[2] << std::endl;
			}
			for (const auto& v : f2v) {
				file << "f " << v[0] + 1 << " " << v[1] + 1 << " " << v[2] + 1 << std::endl;
			}

			file.close();
			std::cout << "파일이 성공적으로 저장되었습니다." << std::endl;

			std::cout << "Saving OBJ mesh to " << std::endl;


		}

		void save(std::string fileName, 
			std::vector<std::array<double, 3>>& pos,
			std::vector<std::vector<size_t>>& c2v,
			std::vector<std::vector<size_t>>& c2f,
			std::vector<std::vector<size_t>>& f2v
			) {

			std::ofstream outputFile;
			std::string filenamePlot = fileName;

			outputFile.open(filenamePlot);
			if (outputFile.fail()) {
				std::cerr << "Unable to write file for writing." << std::endl;
				return;
			}

			using namespace std;

			// string out_line;
			outputFile << "<?xml version=\"1.0\"?>" << endl;
			outputFile << " <VTKFile type=\"UnstructuredGrid\" version=\"1.0\" byte_order=\"LittleEndian\" header_type=\"UInt64\">" << endl;
			outputFile << "  <UnstructuredGrid>" << endl;
			outputFile << "   <Piece NumberOfPoints=\"" << pos.size() << "\" NumberOfCells=\"" << c2f.size() << "\">" << endl;

			// Points data
			outputFile << "    <PointData>" << endl;
			outputFile << "    </PointData>" << endl;
			// Cells data
			outputFile << "    <CellData>" << endl;
			outputFile << "    </CellData>" << endl;
			// Points
			outputFile << "    <Points>" << endl;
			// }
			outputFile << "     <DataArray type=\"Float64\" Name=\"NodeCoordinates\" NumberOfComponents=\"3\" format=\"ascii\">" << endl;

			stringstream streamXYZ;
			outputFile.precision(20);
			// for(auto iter=mesh.points.begin(); iter!=mesh.points.end(); iter++){
			for (auto& point : pos) {
				outputFile << scientific << point[0] << " " << point[1] << " " << point[2] << endl;

			}

			outputFile << "    </DataArray>" << endl;
			outputFile << "   </Points>" << endl;

			// cells
			outputFile << "   <Cells>" << endl;
			// connectivity (cell's points)
			outputFile << "    <DataArray type=\"Int64\" Name=\"connectivity\" format=\"ascii\">" << endl;

			for (auto& cell : c2v) {
				for (auto i : cell) {
					outputFile << i << " ";
				}
				outputFile << endl;
			}

			outputFile << "    </DataArray>" << endl;

			// offsets (cell's points offset)
			int cellFaceOffset = 0;
			outputFile << "    <DataArray type=\"Int64\" Name=\"offsets\" format=\"ascii\">" << endl;

			cellFaceOffset = 0;
			for (auto& cell : c2v) {
				cellFaceOffset += cell.size();
				outputFile << cellFaceOffset << " ";
			}
			outputFile << endl;

			outputFile << "    </DataArray>" << endl;

			// types (cell's type, 42 = polyhedron)
			outputFile << "    <DataArray type=\"UInt8\" Name=\"types\" format=\"ascii\">" << endl;

			for (auto& cell : c2v) {
				outputFile << "42" << " ";
			}
			outputFile << endl;

			outputFile << "    </DataArray>" << endl;

			{
				// faces (cell's faces number, each face's point number, cell's faces's points)
				outputFile << "    <DataArray type=\"Int64\" IdType=\"1\" Name=\"faces\" format=\"ascii\">" << endl;

				for (auto& cell : c2f) {
					outputFile << cell.size() << endl;
					for (auto& i : cell) {
						outputFile << f2v[i].size() << " ";
						int tmp_space = 0;
						for (auto& j : f2v[i]) {
							if (tmp_space != f2v[i].size() - 1) {
								outputFile << j << " ";
							}
							else {
								outputFile << j;
							}
							++tmp_space;
						}
						outputFile << endl;
					}
				}

				outputFile << "    </DataArray>" << endl;


			}

			int cellFacePointOffset = 0;

			outputFile << "    <DataArray type=\"Int64\" IdType=\"1\" Name=\"faceoffsets\" format=\"ascii\">" << endl;

			cellFacePointOffset = 0;
			for (auto& face : c2f) {
				int numbering = 1 + face.size();
				for (auto& i : face) {
					numbering += f2v[i].size();
				}
				cellFacePointOffset += numbering;
				outputFile << cellFacePointOffset << " ";
			}
			outputFile << endl;

			outputFile << "    </DataArray>" << endl;
			outputFile << "   </Cells>" << endl;


			outputFile << "  </Piece>" << endl;
			outputFile << " </UnstructuredGrid>" << endl;





			//// additional informations
			//{
			//	string saveFormat = "ascii";
			//	outputFile << " <DataArray type=\"Int32\" Name=\"owner\" format=\"" << saveFormat << "\">" << endl;
			//	// vector<int> values;
			//	for (auto& face : mesh.faces) {
			//		// values.push_back(face.owner);
			//		outputFile << face.iL << " ";
			//	}
			//	// writeDatasAtVTU(controls, outputFile, values);
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;
			//}
			//{
			//	string saveFormat = "ascii";
			//	outputFile << " <DataArray type=\"Int32\" Name=\"neighbour\" format=\"" << saveFormat << "\">" << endl;
			//	// vector<int> values;
			//	for (auto& face : mesh.faces) {
			//		// values.push_back(face.neighbour);
			//		if (face.getType() == MASCH_Face_Types::INTERNAL) {
			//			outputFile << face.iR << " ";
			//		}
			//	}
			//	// writeDatasAtVTU(controls, outputFile, values);
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;
			//}



			//// boundary informations
			//{
			//	outputFile << " <DataArray type=\"Char\" Name=\"bcName\" format=\"" << "ascii" << "\">" << endl;
			//	// for(auto& boundary : mesh.boundary){
			//		// // cout << boundary.name << endl;
			//		// // trim;
			//		// string bcName = boundary.name;

			//		// bcName.erase(std::find_if(bcName.rbegin(), bcName.rend(), std::not1(std::ptr_fun<int, int>(std::isspace))).base(), bcName.end());
			//		// bcName.erase(bcName.begin(), std::find_if(bcName.begin(), bcName.end(), std::not1(std::ptr_fun<int, int>(std::isspace))));


			//		// // outputFile << boundary.name << " ";
			//		// outputFile << bcName << " ";
			//	// }
			//	for (auto& boundary : mesh.boundaries) {
			//		// cout << boundary.name << endl;
			//		outputFile << boundary.name << " ";
			//	}
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;

			//	outputFile << " <DataArray type=\"Int32\" Name=\"bcStartFace\" format=\"" << "ascii" << "\">" << endl;
			//	for (auto& boundary : mesh.boundaries) {
			//		outputFile << boundary.startFace << " ";
			//	}
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;

			//	outputFile << " <DataArray type=\"Int32\" Name=\"bcNFaces\" format=\"" << "ascii" << "\">" << endl;
			//	for (auto& boundary : mesh.boundaries) {
			//		outputFile << boundary.nFaces << " ";
			//	}
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;

			//	outputFile << " <DataArray type=\"Int32\" Name=\"bcNeighbProcNo\" format=\"" << "ascii" << "\">" << endl;
			//	for (auto& boundary : mesh.boundaries) {
			//		// cout << boundary.rightProcNo << endl;
			//		outputFile << boundary.rightProcNo << " ";
			//	}
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;

			//	outputFile << " <DataArray type=\"Int32\" Name=\"connPoints\" format=\"" << "ascii" << "\">" << endl;
			//	for (int i = 0; i < mesh.points.size(); ++i) {
			//		auto& point = mesh.points[i];
			//		for (auto& [item, item2] : point.connPoints) {
			//			outputFile << i << " " << item << " " << item2 << " ";
			//		}
			//		// if(!point.connPoints.empty()) outputFile << endl;
			//	}
			//	outputFile << endl;
			//	outputFile << " </DataArray>" << endl;


			//}

			outputFile << "</VTKFile>" << endl;

			outputFile.close();

			// }



			std::cout << "파일이 성공적으로 저장되었습니다." << std::endl;




		}
	};




}