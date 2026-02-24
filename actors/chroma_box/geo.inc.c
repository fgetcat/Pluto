#include "src/game/envfx_snow.h"

const GeoLayout chroma_box_geo[] = {
	GEO_NODE_START(),
	GEO_OPEN_NODE(),
		GEO_NODE_START(),
		GEO_OPEN_NODE(),
			GEO_DISPLAY_LIST(LAYER_OPAQUE, chroma_box_001_displaylist_mesh_layer_1),
		GEO_CLOSE_NODE(),
		GEO_DISPLAY_LIST(LAYER_OPAQUE, chroma_box_material_revert_render_settings),
	GEO_CLOSE_NODE(),
	GEO_END(),
};
