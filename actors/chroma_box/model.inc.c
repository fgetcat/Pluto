Lights1 chroma_box_f3d_material_lights = gdSPDefLights1(
	0x0, 0x0, 0x0,
	0x0, 0x0, 0x0, 0x28, 0x28, 0x28);

Vtx chroma_box_001_displaylist_mesh_layer_1_vtx_0[20] = {
	{{{-2000, 0, -2000},0, {880, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{2000, 0, 2000},0, {624, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{2000, 0, -2000},0, {624, 496},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{-2000, 0, 2000},0, {880, 240},{0x0, 0x7F, 0x0, 0xFF}}},
	{{{2000, 0, -2000},0, {624, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{2000, -200, 2000},0, {368, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{2000, -200, -2000},0, {368, 496},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{2000, 0, 2000},0, {624, 240},{0x7F, 0x0, 0x0, 0xFF}}},
	{{{-2000, 0, -2000},0, {624, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{2000, 0, -2000},0, {624, 496},{0x0, 0x0, 0x81, 0xFF}}},
	{{{2000, -200, -2000},0, {368, 496},{0x0, 0x0, 0x81, 0xFF}}},
	{{{-2000, -200, -2000},0, {368, 752},{0x0, 0x0, 0x81, 0xFF}}},
	{{{2000, 0, 2000},0, {624, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-2000, -200, 2000},0, {368, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{2000, -200, 2000},0, {368, 240},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-2000, 0, 2000},0, {624, -16},{0x0, 0x0, 0x7F, 0xFF}}},
	{{{-2000, 0, 2000},0, {624, 1008},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-2000, 0, -2000},0, {624, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-2000, -200, -2000},0, {368, 752},{0x81, 0x0, 0x0, 0xFF}}},
	{{{-2000, -200, 2000},0, {368, 1008},{0x81, 0x0, 0x0, 0xFF}}},
};

Gfx chroma_box_001_displaylist_mesh_layer_1_tri_0[] = {
	gsSPVertex(chroma_box_001_displaylist_mesh_layer_1_vtx_0 + 0, 16, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 3, 1, 0),
	gsSP1Triangle(4, 5, 6, 0),
	gsSP1Triangle(4, 7, 5, 0),
	gsSP1Triangle(8, 9, 10, 0),
	gsSP1Triangle(8, 10, 11, 0),
	gsSP1Triangle(12, 13, 14, 0),
	gsSP1Triangle(12, 15, 13, 0),
	gsSPVertex(chroma_box_001_displaylist_mesh_layer_1_vtx_0 + 16, 4, 0),
	gsSP1Triangle(0, 1, 2, 0),
	gsSP1Triangle(0, 2, 3, 0),
	gsSPEndDisplayList(),
};


Gfx mat_chroma_box_f3d_material[] = {
	gsDPPipeSync(),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 1),
	gsSPSetLights1(chroma_box_f3d_material_lights),
	gsSPEndDisplayList(),
};

Gfx chroma_box_001_displaylist_mesh_layer_1[] = {
	gsSPDisplayList(mat_chroma_box_f3d_material),
	gsSPDisplayList(chroma_box_001_displaylist_mesh_layer_1_tri_0),
	gsSPEndDisplayList(),
};

Gfx chroma_box_material_revert_render_settings[] = {
	gsDPPipeSync(),
	gsSPSetGeometryMode(G_LIGHTING),
	gsSPClearGeometryMode(G_TEXTURE_GEN),
	gsDPSetCombineLERP(0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT, 0, 0, 0, SHADE, 0, 0, 0, ENVIRONMENT),
	gsSPTexture(65535, 65535, 0, 0, 0),
	gsDPSetEnvColor(255, 255, 255, 255),
	gsDPSetAlphaCompare(G_AC_NONE),
	gsSPEndDisplayList(),
};

