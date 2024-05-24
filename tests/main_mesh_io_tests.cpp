
#include "semo_io.h"


void test_ansys_load_save() {


	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
		std::vector<std::vector<size_t>> f2c;
		std::vector<std::vector<size_t>> c2v;
		std::vector<std::vector<size_t>> c2f;
		std::vector<std::string> bc_names;
		std::vector<size_t> bc_nFaces;
		std::vector<size_t> bc_startFace;
		std::vector<size_t> bc_myProcNo;
		std::vector<size_t> bc_rightProcNo;
	};
	Mesh_v mesh;

	semo::Loader_ansys loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.set_f2c(&mesh.f2c);
	loader.set_bc_startFace(&mesh.bc_startFace);
	loader.set_bc_nFaces(&mesh.bc_nFaces);
	loader.load("E:/[[ meshs ]]/fluent_test1.cas");


	semo::Saver_ansys saver;
	saver.set_pos(&mesh.pos);
	saver.set_f2v(&mesh.f2v);
	saver.set_f2c(&mesh.f2c);
	saver.set_bc_startFace(&mesh.bc_startFace);
	saver.set_bc_nFaces(&mesh.bc_nFaces);
	saver.save("E:/[[ meshs ]]/fluent_test1_out.cas");


	return;



}


void test_off_load_save() {

	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
	};
	Mesh_v mesh;

	semo::Loader_off loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.load("E:/[[ meshs ]]/off/Apple.off");

	semo::Saver_off saver;
	saver.set_pos(&mesh.pos);
	saver.set_f2v(&mesh.f2v);
	saver.save("E:/[[ meshs ]]/off/Apple_out.off");
	

}



void test_ply_load_save() {

	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
	};
	Mesh_v mesh;

	semo::Loader_ply loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	//loader.load("E:/[[ meshs ]]/ply/tet.ply");
	//assert(mesh.pos.size() == 4);
	//assert(mesh.f2v.size() == 4);
	loader.load("E:/[[ meshs ]]/ply/bun_zipper_res4.ply");


	semo::Saver_ply saver;
	saver.set_pos(&mesh.pos);
	saver.set_f2v(&mesh.f2v);
	//saver.save("E:/[[ meshs ]]/ply/tet_out.ply");
	saver.save("E:/[[ meshs ]]/ply/bun_zipper_res4_out.ply");


}




void test_tecplot_load_save() {

	// Open plt file
	std::string file = "E:/[[ meshs ]]/tecplot/quad_zone_space.tec";



	//int32_t debug = 1;
	//int32_t fileIsDouble = 0;
	//int32_t fileType = 0;
	//int32_t fileFormat = 0; // 0 == PLT, 1 == SZPLT; Only PLT is currently supported for poly zones
	//int32_t result;


	//void* inputFileHandle = NULL;
	//tecFileReaderOpen(file.c_str(), &inputFileHandle);

	//result = tecini142(
	//	"Polygonal Quads",
	//	"X Y C",
	//	file.c_str(),
	//	".",
	//	&fileFormat,
	//	&fileType,
	//	&debug,
	//	&fileIsDouble);
	//

}



void test_cgns_load_save() {



}


#include "io_vtu.h"

void test_vtu_load_save() {


	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
		std::vector<std::vector<size_t>> c2v;
		std::vector<std::vector<size_t>> c2f;
	};
	Mesh_v mesh;

	semo::Loader_vtu loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.set_c2v(&mesh.c2v);
	loader.set_c2f(&mesh.c2f);
	loader.load("E:/[[ meshs ]]/11111.vtu");


}



void test_stl_load_and_save() {

	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
		std::vector<std::vector<size_t>> c2v;
		std::vector<std::vector<size_t>> c2f;
	};
	Mesh_v mesh;

	semo::Loader_stl loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.load("E:/[[ meshs ]]/300_polygon_sphere_100mm.STL");

	semo::Saver_stl saver;
	saver.set_pos(&(mesh.pos));
	saver.set_f2v(&(mesh.f2v));
	saver.save_ascii("E:/[[ meshs ]]/300_polygon_sphere_100mm_out.STL");


}



void test_obj_load_save() {

	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
	};
	Mesh_v mesh;

	semo::Loader_obj loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.load("E:/[[ meshs ]]/obj/elephav.obj");

	semo::Saver_obj saver;
	saver.set_pos(&mesh.pos);
	saver.set_f2v(&mesh.f2v);
	saver.save("E:/[[ meshs ]]/obj/elephav_out.obj");


}




void test_openfoam_load_save() {

	struct Mesh_v {
		std::vector<std::array<double, 3>> pos;
		std::vector<std::vector<size_t>> f2v;
		std::vector<std::vector<size_t>> f2c;
		std::vector<std::vector<size_t>> c2v;
		std::vector<std::vector<size_t>> c2f;
		std::vector<std::string> bc_names;
		std::vector<size_t> bc_nFaces;
		std::vector<size_t> bc_startFace;
		std::vector<size_t> bc_myProcNo;
		std::vector<size_t> bc_rightProcNo;
	};
	Mesh_v mesh;

	semo::Loader_openfoam loader;
	loader.set_pos(&mesh.pos);
	loader.set_f2v(&mesh.f2v);
	loader.set_f2c(&mesh.f2c);
	loader.set_bc_names(&mesh.bc_names);
	loader.set_bc_nFaces(&mesh.bc_nFaces);
	loader.set_bc_startFace(&mesh.bc_startFace);
	loader.set_bc_myProcNo(&mesh.bc_myProcNo);
	loader.set_bc_rightProcNo(&mesh.bc_rightProcNo);
	loader.load("E:/[[ meshs ]]/openfoam");

	semo::get_f2c_to_c2f(mesh.f2c, mesh.c2f);
	semo::get_c2f_f2v_to_c2v(mesh.c2f, mesh.f2v, mesh.c2v);

	//semo::Saver_vtu saver;
	//saver.set_pos(&mesh.pos);
	//saver.set_f2v(&mesh.f2v);
	//saver.set_c2v(&mesh.c2v);
	//saver.set_c2f(&mesh.c2f);
	//saver.save("E:/[[ meshs ]]/openfoam/out.vtu");

	semo::Saver_tecplot saver;
	saver.set_pos(&mesh.pos);
	saver.set_f2v(&mesh.f2v);
	saver.set_f2c(&mesh.f2c);
	saver.save("E:/[[ meshs ]]/openfoam/out.plt");

}


int main() {

	//test_openfoam_load_save();

	//test_obj_load_save();

	//test_vtu_load_save();

	//test_cgns_load_save();

	//test_tecplot_load_save();

	//test_ply_load_save();


	//test_stl_load_and_save();

	test_ansys_load_save();

	//test_off_load_save();

}