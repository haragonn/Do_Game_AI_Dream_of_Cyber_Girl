#ifndef INCLUDE_IDEA_IDEATYPE_H
#define INCLUDE_IDEA_IDEATYPE_H

#include "ideaMath.h"
#include "ideaColor.h"

namespace idea{
	// 2D���_�f�[�^�\����
	struct VertexData2D
	{
		Vector3D pos;
		Color color;
		Vector2D tex;
	};
}

#endif	// #ifndef INCLUDE_IDEA_IDEATYPE_H