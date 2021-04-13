#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

namespace idea{
	// 2D頂点データ構造体
	struct VertexData2D
	{
		Vector3D pos;
		Color color;
		Vector2D tex;
	};
}

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H