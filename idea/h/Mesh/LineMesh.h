#ifndef INCLUDE_IDEA_LINEMESH_H
#define INCLUDE_IDEA_LINEMESH_H

#include "../3D/Cube.h"

namespace idea{
	class LineMesh : public Cube{
	public:
		LineMesh(){}
		~LineMesh(){}

		bool Create(float thickness);

		void SetPoint(Vector3D startPoint, Vector3D endPoint);

	private:
		float thickness_ = 0.0f;

		Vector3D startPoint_ = {};
		Vector3D endPoint_ = {};
	};
};

#endif	// #ifndef INCLUDE_IDEA_LINEMESH_H