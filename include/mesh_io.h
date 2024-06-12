#pragma once

#include "mesh.h"
#include "./io_stl.h"
#include "./io_vtu.h"
#include "./io_openfoam.h"
#include "./io_obj.h"
#include "./io_off.h"
#include "./io_ply.h"
#include "./io_ansys.h"
#include "./io_tecplot.h"

namespace semo {


	class MeshIO {
	public:
		enum class FileLoadType {
			stl, ansys, off, ply, obj, openfoam, tecplot_ascii, vtu
		};
		enum class FileSaveType {
			stl_ascii, stl_binary, 
			ansys, off, ply, obj, openfoam, tecplot_binary, vtu_ascii, vtu_binary
		};

		void load(semo::Mesh& mesh, std::string filename, FileLoadType ft) {
			switch (ft) {
			case FileLoadType::stl:
			{
				semo::Loader_stl loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.load(filename);
				mesh.setup_f2v_to_v2f();
				mesh.setup_min_max_bounding();
				break;
			}
			case FileLoadType::vtu:
			{
				semo::Loader_vtu loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.set_c2v(&mesh.c2v);
				loader.set_c2f(&mesh.c2f);
				loader.load(filename);
				mesh.setup_c2v_to_v2c();
				if (verbose >= 1) semo::print("c2v_to_v2c success.");
				mesh.setup_f2v_v2c_c2v_to_f2c();
				if (verbose >= 1) semo::print("f2v_v2c_c2v_to_f2c success.");

				mesh.setup_ordering_own_ngb_to_f2c();
				if (verbose >= 1) semo::print("setup_ordering_own_ngb_to_f2c success.");

				mesh.setup_min_max_bounding();
				break;
			}
			case FileLoadType::openfoam: {
				semo::Loader_openfoam loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.set_f2c(&mesh.f2c);
				loader.set_bc_names(&bc_names);
				loader.set_bc_startFace(&bc_startFace);
				loader.set_bc_nFaces(&bc_nFaces);
				loader.set_bc_myProcNo(&bc_myProcNo);
				loader.set_bc_rightProcNo(&bc_rightProcNo);
				loader.load(filename);

				mesh.setup_f2c_to_c2f();
				mesh.setup_f2v_to_v2f();
				mesh.setup_c2f_f2v_to_c2v();
				mesh.setup_c2v_to_v2c();

				mesh.setup_min_max_bounding();

				break;
			}
			case FileLoadType::obj:
			{
				semo::Loader_obj loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.load(filename);

				mesh.setup_min_max_bounding();

				break;
			}
			case FileLoadType::off:
			{
				semo::Loader_off loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.load(filename);

				mesh.setup_min_max_bounding();

				break;
			}
			case FileLoadType::ply:
			{
				semo::Loader_ply loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.load(filename);

				mesh.setup_min_max_bounding();

				break;
			}
			case FileLoadType::ansys:
			{
				semo::Loader_ansys loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.set_f2c(&mesh.f2c);
				loader.set_bc_names(&bc_names);
				loader.set_bc_startFace(&bc_startFace);
				loader.set_bc_nFaces(&bc_nFaces);
				loader.set_bc_myProcNo(&bc_myProcNo);
				loader.set_bc_rightProcNo(&bc_rightProcNo);
				loader.load(filename);
				semo::print("load success");

				mesh.setup_f2c_to_c2f();
				mesh.setup_f2v_to_v2f();
				mesh.setup_c2f_f2v_to_c2v();
				mesh.setup_c2v_to_v2c();

				mesh.setup_min_max_bounding();

				break;
			}
			case FileLoadType::tecplot_ascii:
			{
				semo::Loader_tecplot loader;
				loader.set_pos(&mesh.pos);
				loader.set_f2v(&mesh.f2v);
				loader.set_f2c(&mesh.f2c);
				loader.load_ascii(filename);

				mesh.setup_f2c_to_c2f();
				mesh.setup_f2v_to_v2f();
				mesh.setup_c2f_f2v_to_c2v();
				mesh.setup_c2v_to_v2c();

				mesh.setup_min_max_bounding();

				break;
			}
			}

			// 바운더리 추출
			for (int i = 0; i < bc_names.size(); ++i) {
				semo::Mesh::Boundary bd;
				bd.name = bc_names[i];
				bd.startFace = bc_startFace[i];
				bd.nFaces = bc_nFaces[i];
				bd.myProcNo = bc_myProcNo[i];
				bd.rightProcNo = bc_rightProcNo[i];
				mesh.boundaries.push_back(bd);
			}

			if (verbose >= 1) std::cout << "load success." << std::endl;


		}


		void save(semo::Mesh& mesh, std::string filename, FileSaveType ft) {

			// 바운더리 주입
			bc_names.clear();
			bc_startFace.clear();
			bc_nFaces.clear();
			bc_myProcNo.clear();
			bc_rightProcNo.clear();
			for (auto& bc : mesh.boundaries) {
				bc_names.push_back(bc.name);
				bc_startFace.push_back(bc.startFace);
				bc_nFaces.push_back(bc.nFaces);
				bc_myProcNo.push_back(bc.myProcNo);
				bc_rightProcNo.push_back(bc.rightProcNo);
			}


			switch (ft) {
			case FileSaveType::stl_ascii:
			case FileSaveType::stl_binary:
			{
				semo::Saver_stl saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				if (ft == FileSaveType::stl_ascii) {
					saver.save_ascii(filename);
				}
				else {
					saver.save_binary(filename);
				}
				break;
			}
			case FileSaveType::vtu_ascii :  
			case FileSaveType::vtu_binary:
			{
				semo::Saver_vtu saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);
				assert(mesh.c2v.size() != 0);
				assert(mesh.c2f.size() != 0);
				assert(mesh.f2c.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.set_c2v(&mesh.c2v);
				saver.set_c2f(&mesh.c2f);
				saver.set_f2c(&mesh.f2c);
				if (ft == FileSaveType::vtu_ascii) {
					saver.save(filename, false);
				}
				else {
					saver.save(filename);
				}
				break;
			}
			case FileSaveType::openfoam: 
			{
				semo::Saver_openfoam saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);
				assert(mesh.f2c.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.set_f2c(&mesh.f2c);
				saver.set_bc_names(&bc_names);
				saver.set_bc_startFace(&bc_startFace);
				saver.set_bc_nFaces(&bc_nFaces);
				saver.set_bc_myProcNo(&bc_myProcNo);
				saver.set_bc_rightProcNo(&bc_rightProcNo);
				saver.save(filename);
				break;
			}
			case FileSaveType::obj:
			{
				semo::Saver_obj saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.save(filename);

				break;
			}
			case FileSaveType::off:
			{
				semo::Saver_off saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.save(filename);

				break;
			}
			case FileSaveType::ply:
			{
				semo::Saver_ply saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.save(filename);

				break;
			}
			case FileSaveType::ansys:
			{
				semo::Saver_ansys saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);
				assert(mesh.f2c.size() != 0);
				
				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.set_f2c(&mesh.f2c);
				saver.set_bc_names(&bc_names);
				saver.set_bc_startFace(&bc_startFace);
				saver.set_bc_nFaces(&bc_nFaces);
				saver.set_bc_myProcNo(&bc_myProcNo);
				saver.set_bc_rightProcNo(&bc_rightProcNo);

				saver.save(filename);
				break;
			}
			case FileSaveType::tecplot_binary:
			{
				semo::Saver_tecplot saver;

				assert(mesh.pos.size() != 0);
				assert(mesh.f2v.size() != 0);
				assert(mesh.f2c.size() != 0);

				saver.set_pos(&mesh.pos);
				saver.set_f2v(&mesh.f2v);
				saver.set_f2c(&mesh.f2c);
				saver.save(filename);

				break;
			}
			}
			if (verbose >= 1) std::cout << "save success." << std::endl;
		}


		int verbose = 0;

	private:


		std::vector<std::string> bc_names;
		std::vector<semo::index_t> bc_nFaces;
		std::vector<semo::index_t> bc_startFace;
		std::vector<semo::index_t> bc_myProcNo;
		std::vector<semo::index_t> bc_rightProcNo;

		//std::map<FileType, std::string> file_type_to_string_ = {
		//	{FileType::stl, "stl"},
		//	{FileType::ansys, "ansys"},
		//	{FileType::off, "off"},
		//	{FileType::ply, "ply"},
		//	{FileType::obj, "obj"},
		//	{FileType::openfoam, "openfoam"},
		//	{FileType::tecplot, "tecplot"},
		//	{FileType::vtu, "vtu"},
		//};

		//void filename_check_(std::string filename, FileType ft) {
		//	std::string desiredExtension = file_type_to_string_[ft];
		//	assert(filename.ends_with(desiredExtension));
		//}

		//std::vector<semo::point3_t> pos;
		//std::vector<std::vector<semo::index_t>> f2v;

	};


}
