

#define SEMO_USE_EIGEN_LIB
#define SEMO_USE_SPDLOG_LIB
#include "mesh_io.h"

//#include "D:/work/SEMO_Mesh_Type/mesh.h"

//
//void print(
//	std::vector<std::array<double, 3>>& pos,
//	std::vector<std::vector<size_t>>& f2v,
//	std::vector<std::vector<size_t>>& c2v,
//	std::vector<std::vector<size_t>>& c2f
//) {
//	std::cout << "--------------------------------" << std::endl;
//	std::cout << "position size : " << pos.size() << std::endl;
//	std::cout << "cell size : " << c2v.size() << std::endl;
//	std::cout << "face size : " << f2v.size() << std::endl;
//
//	size_t n_tetra, n_hexa, n_prism, n_pyramid, n_polyhed;
//	n_tetra = n_hexa = n_prism = n_pyramid = n_polyhed = 0;
//	for (size_t i = 0; i < c2f.size(); ++i) {
//		auto n_f = c2f[i].size();
//		auto n_v = c2v[i].size();
//		if (n_f == 4 && n_v == 4) ++n_tetra;
//		else if (n_f == 6 && n_v == 8) ++n_hexa;
//		else if (n_f == 5 && n_v == 6) ++n_prism;
//		else if (n_f == 5 && n_v == 5) ++n_pyramid;
//		else ++n_polyhed;
//	}
//
//	std::cout << "tetra cell size : " << n_tetra << std::endl;
//	std::cout << "hexa cell size : " << n_hexa << std::endl;
//	std::cout << "prism cell size : " << n_prism << std::endl;
//	std::cout << "pyramid cell size : " << n_pyramid << std::endl;
//	std::cout << "polyhed cell size : " << n_polyhed << std::endl;
//	//std::cout << "min bound : " <<
//	//	min_bound[0] << " " <<
//	//	min_bound[1] << " " <<
//	//	min_bound[2] << std::endl;
//	//std::cout << "max bound : " <<
//	//	max_bound[0] << " " <<
//	//	max_bound[1] << " " <<
//	//	max_bound[2] << std::endl;
//	//std::cout << "--------------------------------" << std::endl;
//
//}
//

void test_ansys_load_save() {


	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	std::string filename = "E:/[[ meshs ]]/ansys/3D_Nobes_2014_test_grid";

	mesh_io.load(mesh,
		filename + ".cas",
		semo::MeshIO::FileLoadType::ansys);

	std::cout << mesh;

	mesh_io.save(mesh,
		filename + "_out.cas",
		semo::MeshIO::FileSaveType::ansys);

	mesh_io.save(mesh,
		filename + "_out.vtu",
		semo::MeshIO::FileSaveType::vtu_binary);

	mesh_io.save(mesh,
		filename + "_out.plt",
		semo::MeshIO::FileSaveType::tecplot_binary);



}


void test_off_load_save() {


	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	std::string filename = "E:/[[ meshs ]]/off/0";

	mesh_io.load(mesh,
		filename + ".off",
		semo::MeshIO::FileLoadType::off);

	std::cout << mesh;

	mesh_io.save(mesh,
		filename + "_out.off",
		semo::MeshIO::FileSaveType::off);

	mesh_io.save(mesh,
		filename + "_out.ply",
		semo::MeshIO::FileSaveType::ply);
	

}

//
//
//void test_ply_load_save() {
//
//	struct Mesh_v {
//		std::vector<std::array<double, 3>> pos;
//		std::vector<std::vector<size_t>> f2v;
//	};
//	Mesh_v mesh;
//
//	semo::Loader_ply loader;
//	loader.set_pos(&mesh.pos);
//	loader.set_f2v(&mesh.f2v);
//	//loader.load("E:/[[ meshs ]]/ply/tet.ply");
//	//assert(mesh.pos.size() == 4);
//	//assert(mesh.f2v.size() == 4);
//	loader.load("E:/[[ meshs ]]/ply/bun_zipper_res4.ply");
//
//
//	semo::Saver_ply saver;
//	saver.set_pos(&mesh.pos);
//	saver.set_f2v(&mesh.f2v);
//	//saver.save("E:/[[ meshs ]]/ply/tet_out.ply");
//	saver.save("E:/[[ meshs ]]/ply/bun_zipper_res4_out.ply");
//
//
//}
//
//
//
//
//void test_tecplot_load_save() {
//
//	// Open plt file
//	std::string file = "E:/[[ meshs ]]/tecplot/quad_zone_space.tec";
//
//
//
//	//int32_t debug = 1;
//	//int32_t fileIsDouble = 0;
//	//int32_t fileType = 0;
//	//int32_t fileFormat = 0; // 0 == PLT, 1 == SZPLT; Only PLT is currently supported for poly zones
//	//int32_t result;
//
//
//	//void* inputFileHandle = NULL;
//	//tecFileReaderOpen(file.c_str(), &inputFileHandle);
//
//	//result = tecini142(
//	//	"Polygonal Quads",
//	//	"X Y C",
//	//	file.c_str(),
//	//	".",
//	//	&fileFormat,
//	//	&fileType,
//	//	&debug,
//	//	&fileIsDouble);
//	//
//
//}
//
//
//
//void test_cgns_load_save() {
//
//
//
//}
//

void test_vtu_load_save() {


	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	mesh_io.load(mesh, "E:/[[ meshs ]]/vtu/11111.vtu", semo::MeshIO::FileLoadType::vtu);

	mesh.print();

	mesh_io.save(mesh, "E:/[[ meshs ]]/vtu/11111_out.vtu", semo::MeshIO::FileSaveType::vtu_binary);

}



void test_stl_load_save() {

	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	mesh_io.load(mesh,
		"E:/[[ meshs ]]/stl/Bunny.stl", 
		semo::MeshIO::FileLoadType::stl);

	std::cout << mesh;

	mesh_io.save(mesh,
		"E:/[[ meshs ]]/stl/Bunny_out_ascii.stl",
		semo::MeshIO::FileSaveType::stl_ascii);
	mesh_io.save(mesh,
		"E:/[[ meshs ]]/stl/Bunny_out_binary.stl",
		semo::MeshIO::FileSaveType::stl_binary);

}



void test_obj_load_save() {


	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	mesh_io.load(mesh,
		"E:/[[ meshs ]]/obj/elephav.obj",
		semo::MeshIO::FileLoadType::obj);

	std::cout << mesh;

	mesh_io.save(mesh, 
		"E:/[[ meshs ]]/obj/elephav_out.obj",
		semo::MeshIO::FileSaveType::obj);

	mesh_io.save(mesh,
		"E:/[[ meshs ]]/obj/elephav_out.stl",
		semo::MeshIO::FileSaveType::stl_binary);

	mesh_io.save(mesh,
		"E:/[[ meshs ]]/obj/elephav_out.ply",
		semo::MeshIO::FileSaveType::ply);

}




void test_openfoam_load_save() {


	semo::Mesh mesh;

	semo::MeshIO mesh_io;
	mesh_io.verbose = 10;

	std::string filename = "E:/[[ meshs ]]/openfoam/3D_Nobes_2014_test_grid";

	mesh_io.load(mesh,
		filename,
		semo::MeshIO::FileLoadType::openfoam);

	std::cout << mesh;

	mesh_io.save(mesh,
		filename + "_out.cas",
		semo::MeshIO::FileSaveType::ansys);

	mesh_io.save(mesh,
		filename + "_out.vtu",
		semo::MeshIO::FileSaveType::vtu_binary);

	mesh_io.save(mesh,
		filename + "_out.plt",
		semo::MeshIO::FileSaveType::tecplot_binary);


}


int main() {

	test_openfoam_load_save();

	//test_obj_load_save();

	//test_vtu_load_save();

	//test_cgns_load_save();

	//test_tecplot_load_save();

	//test_ply_load_save();


	//test_stl_load_save();

	//test_ansys_load_save();

	//test_off_load_save();

}