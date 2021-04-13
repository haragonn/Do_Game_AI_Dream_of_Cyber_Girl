#include "Panel.h"

using namespace idea;

class Panel::Impl{
public:
	// ƒZƒ‹À•W
	Cell cell_;

	// ” 
	Cube cube_;
};

Panel::Panel() :
	pImpl_(new Panel::Impl)
{
	Assert(pImpl_);
}

Panel::~Panel()
{
	SafeDelete(pImpl_);
}

void Panel::Init()
{
	// ŽÀ‘•
	auto& impl = (*pImpl_);

	impl.cube_.Init(Vector3DZero());
	impl.cube_.SetScale(10.0f, 0.1f, 10.0f);
	impl.cube_.SetColor(ideaColor::TRANSLUCENT);
}

void Panel::Update()
{
	// ŽÀ‘•
	auto& impl = (*pImpl_);

	impl.cube_.SetColor(ideaColor::TRANSLUCENT);
	impl.cube_.SetPosition(position_);
}

void Panel::SetCell(int x, int y)
{
	// ŽÀ‘•
	auto& impl = (*pImpl_);

	impl.cell_.x = x;
	impl.cell_.y = y;
}

void Panel::SetColor(Color color)
{
	pImpl_->cube_.SetColor(color);
}

Cube& Panel::GetCube()
{
	return pImpl_->cube_;
}

BoxCollider Panel::GetCollider()
{
	return pImpl_->cube_.GetCollider();
}
