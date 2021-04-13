#include "../../h/3D/Collision3D.h"
#include "../../h/Utility/Debug.h"

#include <DirectXMath.h>

using namespace idea;
using namespace DirectX;

namespace idea{
	class Collision3DHelper{
	public:
		static inline Vector3D ClosetPointForPointAndSegment(const Vector3D& pt, const Vector3D& segPt1, const Vector3D& segPt2){
			const Vector3D ab = segPt2 - segPt1;

			float t = (pt - segPt1).Dot(ab) / (ab).Dot(ab);

			t = Clamp(t, 0.0f, 1.0f);

			return (segPt1 + (ab * t));
		}

		static inline float ClosestPointForSegmentAndSegment(const Vector3D& p1, const Vector3D& q1, const Vector3D& p2, const Vector3D& q2){
			Vector3D d1 = q1 - p1;
			Vector3D d2 = q2 - p2;
			Vector3D r = p1 - p2;

			float a = d1.Dot(d1);
			float e = d2.Dot(d2);
			float f = d2.Dot(r);

			float s, t;
			Vector3D c1, c2;

			if(f <= ideaMath::EPSILON && e <= ideaMath::EPSILON){
				s = t = 0.0f;

				c1 = p1;
				c2 = p2;

				return (c1 - c2).Dot(c1 - c2);
			}

			if(a <= ideaMath::EPSILON){
				s = 0.0f;
				t = Clamp(f / e, 0.0f, 1.0f);
			} else{
				float c = d1.Dot(r);

				if(e <= ideaMath::EPSILON){
					t = 0.0f;
					s = Clamp(-c / a, 0.0f, 1.0f);
				} else{
					float b = d1.Dot(d2);

					float denom = a * e - b * b;

					if(denom != 0.0f){
						s = Clamp((b * f - c * e) / denom, 0.0f, 1.0f);
					} else{
						s = 0.0f;
					}

					t = (b * s + f) / e;

					if(t < 0.0f){
						t = 0.0f;
						s = Clamp(-c / a, 0.0f, 1.0f);
					} else if(t > 1.0f){
						t = 1.0f;
						s = Clamp((b - c) / a, 0.0f, 1.0f);
					}
				}
			}

			c1 = p1 + d1 * s;
			c2 = p2 + d2 * t;

			return (c1 - c2).Dot(c1 - c2);
		}

		static inline float SqDistanceForPointAndSegment(const Vector3D& a, const Vector3D& b, const Vector3D& c){
			Vector3D ab = b - a;
			Vector3D ac = c - a;
			Vector3D bc = c - b;

			float e = ac.Dot(ab);

			if(e <= 0.0f){ return ac.Dot(ac); }

			float f = ab.Dot(ab);

			if(e >= f){ return bc.Dot(bc); }

			return (ac.Dot(ac) - e * e / f);
		}

		static inline float LengthForPointAndOBB(const Vector3D& pt, const BoxCollider& bc)
		{
			Vector3D ret;

			for(int i = 3 - 1; i >= 0; --i){
				float l = bc.obb_.e.r[i];

				if(l <= 0.0f){
					continue;
				}

				float s = Abs((pt - bc.obb_.c).Dot(bc.obb_.u[i]) / l);

				if(s > 1.0f){
					ret += (1.0f - s) * l * bc.obb_.u[i];
				}
			}

			return ret.Length();
		}

		static inline  Vector3D ClosestPointForPointAndOBB(const Vector3D& pt, const BoxCollider& bc)
		{
			Vector3D d = pt - bc.obb_.c;

			Vector3D ret = bc.obb_.c;

			float dist;

			for(int i = 3 - 1; i >= 0; --i){
				dist = d.Dot(bc.obb_.u[i]);

				if(dist > bc.obb_.e.r[i])
				{
					dist = bc.obb_.e.r[i];
				}

				if(dist < -bc.obb_.e.r[i])
				{
					dist = -bc.obb_.e.r[i];
				}

				ret += dist * bc.obb_.u[i].Normalized();
			}

			return ret;
		}

		static inline  Vector3D ClosestPointForCapsuleAndOBB(const CapsuleCollider& cc, const BoxCollider& bc){
			SphereCollider sc;

			sc.position_ = cc.bottom_;
			sc.radius_ = cc.radius_;

			Vector3D ret = ClosestPointForPointAndOBB(sc.position_, bc);

			Vector3D base = (cc.top_ - cc.bottom_).Normalized();

			Vector3D dest = ret - cc.bottom_;

			float dot = base.Dot(dest);

			if(dot <= 0.0f){ return ret; }

			float len = (cc.top_ - cc.bottom_).Length();

			if(dot > len){
				sc.position_ = cc.top_;

				ret = ClosestPointForPointAndOBB(sc.position_, bc);

				return ret;
			}

			ret = ClosestPointForPointAndOBB(cc.position_, bc);

			Vector3D segPt = ClosetPointForPointAndSegment(ret, cc.bottom_, cc.top_);

			segPt += (ret - segPt).Normalized() * cc.radius_;

			return segPt;
		}

		static inline bool ObbVsObb(const BoxCollider& bc1, const BoxCollider& bc2)
		{
			float r[3][3], absR[3][3];

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					r[i][j] = bc1.obb_.u[i].Dot(bc2.obb_.u[j]);
					absR[i][j] = Abs(r[i][j]) + ideaMath::EPSILON;
				}
			}

			Vector3D t = bc2.obb_.c - bc1.obb_.c;
			t = Vector3D(t.Dot(bc1.obb_.u[0]), t.Dot(bc1.obb_.u[1]), t.Dot(bc1.obb_.u[2]));

			float ra, rb;

			//Ž²L=A0, L=A1, L=A2”»’è
			for(int i = 3 - 1; i >= 0; --i){
				ra = bc1.obb_.e.r[i];
				rb = bc2.obb_.e.r[0] * absR[i][0] + bc2.obb_.e.r[1] * absR[i][1] + bc2.obb_.e.r[2] * absR[i][2];
				if(Abs(t.r[i]) > ra + rb){ return false; }
			}

			//Ž²L=B0, L=B1, L=B2”»’è
			for(int i = 3 - 1; i >= 0; --i){
				ra = bc1.obb_.e.r[0] * absR[0][i] + bc1.obb_.e.r[1] * absR[1][i] + bc1.obb_.e.r[2] * absR[2][i];
				rb = bc2.obb_.e.r[i];
				if(Abs(t.r[0] * r[0][i] + t.r[1] * r[1][i] + t.r[2] * r[2][i]) > ra + rb){ return false; }
			}

			//Ž²L=A0 X B0”»’è
			ra = bc1.obb_.e.r[1] * absR[2][0] + bc1.obb_.e.r[2] * absR[1][0];
			rb = bc2.obb_.e.r[1] * absR[0][2] + bc2.obb_.e.r[2] * absR[0][1];
			if(Abs(t.r[2] * r[1][0] - t.r[1] * r[2][0]) > ra + rb){ return false; }

			//Ž²L=A0 X B1”»’è
			ra = bc1.obb_.e.r[1] * absR[2][1] + bc1.obb_.e.r[2] * absR[1][1];
			rb = bc2.obb_.e.r[0] * absR[0][2] + bc2.obb_.e.r[2] * absR[0][0];
			if(Abs(t.r[2] * r[1][1] - t.r[1] * r[2][1]) > ra + rb){ return false; }

			//Ž²L=A0 X B2”»’è
			ra = bc1.obb_.e.r[1] * absR[2][2] + bc1.obb_.e.r[2] * absR[1][2];
			rb = bc2.obb_.e.r[0] * absR[0][1] + bc2.obb_.e.r[1] * absR[0][0];
			if(Abs(t.r[2] * r[1][2] - t.r[1] * r[2][2]) > ra + rb){ return false; }

			//Ž²L=A1 X B0”»’è
			ra = bc1.obb_.e.r[0] * absR[2][0] + bc1.obb_.e.r[2] * absR[0][0];
			rb = bc2.obb_.e.r[1] * absR[1][2] + bc2.obb_.e.r[2] * absR[1][1];
			if(Abs(t.r[0] * r[2][0] - t.r[2] * r[0][0]) > ra + rb){ return false; }

			//Ž²L=A1 X B1”»’è
			ra = bc1.obb_.e.r[0] * absR[2][1] + bc1.obb_.e.r[2] * absR[0][1];
			rb = bc2.obb_.e.r[0] * absR[1][2] + bc2.obb_.e.r[2] * absR[1][0];
			if(Abs(t.r[0] * r[2][1] - t.r[2] * r[0][1]) > ra + rb){ return false; }

			//Ž²L=A1 X B2”»’è
			ra = bc1.obb_.e.r[0] * absR[2][2] + bc1.obb_.e.r[2] * absR[0][2];
			rb = bc2.obb_.e.r[0] * absR[1][1] + bc2.obb_.e.r[1] * absR[1][0];
			if(Abs(t.r[0] * r[2][2] - t.r[2] * r[0][2]) > ra + rb){ return false; }

			//Ž²L=A2 X B0”»’è
			ra = bc1.obb_.e.r[0] * absR[1][0] + bc1.obb_.e.r[1] * absR[0][0];
			rb = bc2.obb_.e.r[1] * absR[2][2] + bc2.obb_.e.r[2] * absR[2][1];
			if(Abs(t.r[1] * r[0][0] - t.r[0] * r[1][0]) > ra + rb){ return false; }

			//Ž²L=A2 X B1”»’è
			ra = bc1.obb_.e.r[0] * absR[1][1] + bc1.obb_.e.r[1] * absR[0][1];
			rb = bc2.obb_.e.r[0] * absR[2][2] + bc2.obb_.e.r[2] * absR[2][0];
			if(Abs(t.r[1] * r[0][1] - t.r[0] * r[1][1]) > ra + rb){ return false; }

			//Ž²L=A2 X B2”»’è
			ra = bc1.obb_.e.r[0] * absR[1][2] + bc1.obb_.e.r[1] * absR[0][2];
			rb = bc2.obb_.e.r[0] * absR[2][1] + bc2.obb_.e.r[1] * absR[2][0];
			if(Abs(t.r[1] * r[0][2] - t.r[0] * r[1][2]) > ra + rb){ return false; }

			return true;
		}

		static inline bool SphereVsSphere(const SphereCollider& sc1, const SphereCollider& sc2)
		{
			float r = sc1.radius_ + sc2.radius_;

			Vector3D v = sc1.position_ - sc2.position_;

			return (v.LengthSquare() <= r * r);
		}

		static inline bool SphereVsObb(const SphereCollider& sc, const BoxCollider& bc)
		{
			Vector3D v = ClosestPointForPointAndOBB(sc.position_, bc) - sc.position_;

			return (v.Dot(v) <= sc.radius_ * sc.radius_);
		}

		static inline bool SphereVsCapsule(const SphereCollider& sc, const CapsuleCollider& cc)
		{
			Vector3D nor = (sc.position_ - ClosetPointForPointAndSegment(sc.position_, cc.bottom_, cc.top_)).Normalized();

			nor *= cc.radius_;

			float r = sc.radius_ + cc.radius_;

			return (SqDistanceForPointAndSegment(cc.bottom_, cc.top_, sc.position_) <= r * r);

		}

		static inline bool CapsuleVsCapsule(const CapsuleCollider& cc1, const CapsuleCollider& cc2){
			float radius = cc1.radius_ + cc2.radius_;

			return (ClosestPointForSegmentAndSegment(cc1.bottom_, cc1.top_, cc2.bottom_, cc2.top_) <= radius * radius);
		}

		static inline bool InvestigationSphereVsObb(const SphereCollider& srcSc, const Vector3D& srcVelocity, const BoxCollider& destObb, float startTime, float endTime){
			SphereCollider sc;

			float mid = (startTime + endTime) * 0.5f;

			sc.position_ = srcSc.position_ + srcVelocity * mid;

			sc.radius_ = (mid - startTime) * srcVelocity.Length() + srcSc.radius_;

			if(!SphereVsObb(sc, destObb)){ return false; }

			if(endTime - startTime < 0.01f){ return true; }

			if(InvestigationSphereVsObb(srcSc, srcVelocity, destObb, startTime, mid)){ return true; }

			return InvestigationSphereVsObb(srcSc, srcVelocity, destObb, mid, endTime);
		}

		static inline bool CapsuleVsOBB(const CapsuleCollider& cp, const BoxCollider& obb){
			SphereCollider startSc, endSc;

			startSc.SetSphere(cp.bottom_, cp.radius_, Vector3DZero());
			endSc.SetSphere(cp.top_, cp.radius_, Vector3DZero());

			Vector3D velocity = endSc.position_ - startSc.position_;

			if(InvestigationSphereVsObb(startSc, velocity, obb, 0, 1.0f)){ return true; }

			return false;
		}
	};
}

void BoxCollider::SetOBB(const Matrix4x4& world, const Vector3D& prePosition)
{
	prePosition_ = prePosition;

	obb_.c = TransformFromMatrix4x4(world);

	Matrix4x4 rotMat = RotationMatrixFromMatrix4x4(world);

	obb_.u[0] = Vector3D(rotMat.r[0][0], rotMat.r[0][1], rotMat.r[0][2]);
	obb_.u[1] = Vector3D(rotMat.r[1][0], rotMat.r[1][1], rotMat.r[1][2]);
	obb_.u[2] = Vector3D(rotMat.r[2][0], rotMat.r[2][1], rotMat.r[2][2]);

	obb_.e = ScalingFromMatrix4x4(world) * 0.5f;
}

bool BoxCollider::CheckHit(const BoxCollider& bc)
{
	return Collision3DHelper::ObbVsObb((*this), bc);
}

bool BoxCollider::CheckHit(const SphereCollider& sc)
{
	return Collision3DHelper::SphereVsObb(sc, (*this));
}

bool BoxCollider::CheckHit(const CapsuleCollider& cc)
{
	return Collision3DHelper::CapsuleVsOBB(cc, (*this));
}

Vector3D BoxCollider::GetClosetPoint(const Vector3D& pt)
{
	return Collision3DHelper::ClosestPointForPointAndOBB(pt, (*this));
}

Vector3D BoxCollider::CollisionEscape(const BoxCollider& bc, float grip)
{
	if(CheckHit(bc)){
		Vector3D moveVec = (obb_.c - Collision3DHelper::ClosestPointForPointAndOBB(obb_.c, bc)).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - obb_.c).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (obb_.c - bc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		BoxCollider tmpBC;

		tmpBC.obb_.c = obb_.c;
		tmpBC.obb_.u[0] = obb_.u[0];
		tmpBC.obb_.u[1] = obb_.u[1];
		tmpBC.obb_.u[2] = obb_.u[2];
		tmpBC.obb_.e = obb_.e;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpBC.obb_.c += moveVec * 0.01f;

			if(!tmpBC.CheckHit(bc)){ break; }
		}

		return tmpBC.obb_.c - obb_.c;
	}

	return Vector3DZero();
}

Vector3D BoxCollider::CollisionEscape(const SphereCollider& sc, float grip)
{
	if(CheckHit(sc)){
		Vector3D moveVec = -(sc.GetPosition() - Collision3DHelper::ClosestPointForPointAndOBB(sc.GetPosition(), (*this))).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - obb_.c).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (obb_.c - sc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		BoxCollider tmpBC;
		
		tmpBC.obb_.c = obb_.c;
		tmpBC.obb_.u[0] = obb_.u[0];
		tmpBC.obb_.u[1] = obb_.u[1];
		tmpBC.obb_.u[2] = obb_.u[2];
		tmpBC.obb_.e = obb_.e;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpBC.obb_.c += moveVec * 0.01f;

			if(!tmpBC.CheckHit(sc)){ break; }
		}

		return tmpBC.obb_.c - obb_.c;
	}

	return Vector3DZero();
}

Vector3D BoxCollider::CollisionEscape(const CapsuleCollider& cc, float grip)
{
	if(CheckHit(cc)){
		Vector3D moveVec = (obb_.c - Collision3DHelper::ClosetPointForPointAndSegment(obb_.c, cc.GetBottom(), cc.GetTop())).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - obb_.c).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (obb_.c - cc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		BoxCollider tmpBC;
		
		tmpBC.obb_.c = obb_.c;
		tmpBC.obb_.u[0] = obb_.u[0];
		tmpBC.obb_.u[1] = obb_.u[1];
		tmpBC.obb_.u[2] = obb_.u[2];
		tmpBC.obb_.e = obb_.e;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpBC.obb_.c += moveVec * 0.01f;

			if(!tmpBC.CheckHit(cc)){ break; }
		}

		return tmpBC.obb_.c - obb_.c;
	}

	return Vector3DZero();
}

void idea::SphereCollider::SetSphere(const Vector3D& position, float radius, const Vector3D& prePosition)
{
	prePosition_ = prePosition;

	position_ = position;

	radius_ = Abs(radius);
}

bool SphereCollider::CheckHit(const BoxCollider& bc)
{
	return Collision3DHelper::SphereVsObb((*this), bc);
}

bool SphereCollider::CheckHit(const SphereCollider& sc)
{
	return Collision3DHelper::SphereVsSphere((*this), sc);
}

bool SphereCollider::CheckHit(const CapsuleCollider& cc)
{
	return Collision3DHelper::SphereVsCapsule((*this), cc);
}

Vector3D SphereCollider::GetClosetPoint(const Vector3D& pt)
{
	Vector3D nor;

	nor = (pt - position_).Normalized();

	return (position_ + (nor * radius_));
}

Vector3D SphereCollider::CollisionEscape(const BoxCollider& bc, float grip)
{
	if(CheckHit(bc)){
		Vector3D moveVec = (position_ - Collision3DHelper::ClosestPointForPointAndOBB(position_, bc)).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (position_ - bc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		SphereCollider tmpSC;
		
		tmpSC.position_ = position_;
		tmpSC.radius_ = radius_;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpSC.position_ += moveVec * 0.01f;

			if(!tmpSC.CheckHit(bc)){ break; }
		}

		return tmpSC.position_ - position_;
	}

	return Vector3DZero();
}

Vector3D SphereCollider::CollisionEscape(const SphereCollider& sc, float grip)
{
	if(CheckHit(sc)){
		Vector3D moveVec = (position_ - sc.position_).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		SphereCollider tmpSC;
		
		tmpSC.position_ = position_;
		tmpSC.radius_ = radius_;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpSC.position_ += moveVec * 0.01f;

			if(!tmpSC.CheckHit(sc)){ break; }
		}

		return tmpSC.position_ - position_;
	}

	return Vector3DZero();
}

Vector3D SphereCollider::CollisionEscape(const CapsuleCollider& cc, float grip)
{
	if(CheckHit(cc)){
		Vector3D moveVec = (position_ - Collision3DHelper::ClosetPointForPointAndSegment(position_, cc.GetBottom(), cc.GetTop())).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (position_ - cc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		SphereCollider tmpSC;

		tmpSC.position_ = position_;
		tmpSC.radius_ = radius_;

		for(int i = 10000 - 1; i >= 0; --i){
			tmpSC.position_ += moveVec * 0.01f;

			if(!tmpSC.CheckHit(cc)){ break; }
		}

		return tmpSC.position_ - position_;
	}

	return Vector3DZero();
}

void CapsuleCollider::SetCapsule(const Matrix4x4& world, float radius, float height, const Vector3D& prePosition)
{
	prePosition_ = prePosition;

	world_ = world;

	position_ = TransformFromMatrix4x4(world_);

	radius_ = Abs(radius);

	height_ = Abs(height);

	Matrix4x4 rotMat = RotationMatrixFromMatrix4x4(world_);

	top_ = rotMat.Vector3DTransform(Vector3D(0.0f, height * 0.5f - radius_, 0.0f)) + position_;

	bottom_ = rotMat.Vector3DTransform(Vector3D(0.0f, -(height * 0.5f - radius_), 0.0f)) + position_;
}

bool CapsuleCollider::CheckHit(const BoxCollider& bc)
{
	return Collision3DHelper::CapsuleVsOBB((*this), bc);
}

bool CapsuleCollider::CheckHit(const SphereCollider& sc)
{
	return Collision3DHelper::SphereVsCapsule(sc, (*this));
}

bool CapsuleCollider::CheckHit(const CapsuleCollider& cc)
{
	return Collision3DHelper::CapsuleVsCapsule((*this), cc);
}

Vector3D CapsuleCollider::GetClosetPoint(const Vector3D& pt)
{
	Vector3D cpt, nor;

	cpt = Collision3DHelper::ClosetPointForPointAndSegment(pt, bottom_, top_);

	nor = (pt - cpt).Normalized();

	return (cpt + (nor * radius_));
}

Vector3D CapsuleCollider::CollisionEscape(const BoxCollider& bc, float grip)
{
	if(CheckHit(bc)){
		Vector3D moveVec = (position_ - Collision3DHelper::ClosestPointForPointAndOBB(position_, bc)).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (position_ - bc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		CapsuleCollider tmpCC;
		Matrix4x4 tmpMat = world_;

		tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

		for(int i = 10000 - 1; i >= 0; --i){
			tmpMat._41 += moveVec.x * 0.01f;
			tmpMat._42 += moveVec.y * 0.01f;
			tmpMat._43 += moveVec.z * 0.01f;

			tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

			if(!tmpCC.CheckHit(bc)){
				break;
			}
		}

		return TransformFromMatrix4x4(tmpCC.world_) - position_;
	}

	return Vector3DZero();
}

Vector3D CapsuleCollider::CollisionEscape(const SphereCollider& sc, float grip)
{
	if(CheckHit(sc)){
		Vector3D moveVec = (position_ - sc.GetPosition()).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (position_ - sc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		CapsuleCollider tmpCC;
		Matrix4x4 tmpMat = world_;

		tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

		for(int i = 10000 - 1; i >= 0; --i){
			tmpMat._41 += moveVec.x * 0.01f;
			tmpMat._42 += moveVec.y * 0.01f;
			tmpMat._43 += moveVec.z * 0.01f;

			tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

			if(!tmpCC.CheckHit(sc)){
				break;
			}
		}

		return TransformFromMatrix4x4(tmpCC.world_) - position_;
	}

	return Vector3DZero();
}

Vector3D CapsuleCollider::CollisionEscape(const CapsuleCollider& cc, float grip)
{
	if(CheckHit(cc)){
		Vector3D moveVec = (position_ - Collision3DHelper::ClosetPointForPointAndSegment(position_, cc.bottom_, cc.top_)).Normalized();

		if(moveVec.LengthSquare() > 0.0f){
			if(moveVec.y > 1.0f - Clamp(grip, 0.0f, 1.0f)){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			} else{
				moveVec = moveVec.ReplaceY(0.0f).Normalized();
			}
		} else{
			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (prePosition_ - position_).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = (position_ - cc.GetPosition()).Normalized();
			}

			if(moveVec.LengthSquare() == 0.0f){
				moveVec = Vector3D(0.0f, 1.0f, 0.0f);
			}
		}

		CapsuleCollider tmpCC;
		Matrix4x4 tmpMat = world_;

		tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

		for(int i = 10000 - 1; i >= 0; --i){
			tmpMat._41 += moveVec.x * 0.01f;
			tmpMat._42 += moveVec.y * 0.01f;
			tmpMat._43 += moveVec.z * 0.01f;

			tmpCC.SetCapsule(tmpMat, radius_, height_, prePosition_);

			if(!tmpCC.CheckHit(cc)){
				break;
			}
		}

		return TransformFromMatrix4x4(tmpCC.world_) - position_;
	}

	return Vector3DZero();
}