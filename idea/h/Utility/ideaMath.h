#ifndef INCLUDE_IDEA_IDEAMATH_H
#define INCLUDE_IDEA_IDEAMATH_H

#include "ideaUtility.h"

#include <math.h>

namespace idea{
	// 定数群
	namespace ideaMath{
		constexpr float PI = 3.141592654f;			// π
		constexpr float PI2 = 6.283185307f;			// 2π
		constexpr float PIDIV2 = 1.570796327f;		// π/2
		constexpr float ROOT2 = 1.414213562f;		// √2
		constexpr float EPSILON = 1.192092896e-07f;	// floatの誤差
	}

	// 値の符号を返す
	template <typename T>
	inline T Sign(const T& val)
	{
		return ((val < T(0)) ? T(-1) : T(1));
	}

	// 値の絶対値を返す
	template <typename T>
	inline T Abs(const T& val)
	{
		return (val * Sign(val));
	}

	// 値を比較し大きい方の値を返す
	template <typename T>
	inline T Max(const T& val1, const T& val2)
	{
		return ((val1 > val2) ? val1 : val2);
	}

	// 値を比較し小さい方の値を返す
	template <typename T>
	inline T Min(const T& val1, const T& val2)
	{
		return ((val1 < val2) ? val1 : val2);
	}

	// 値を指定された範囲内に収める
	template <typename T>
	inline T Clamp(const T& val, const T& min, const T& max)
	{
		Assert(min < max);

		return Max(min, Min(max, val));
	}

	// 値が指定された範囲内に収まっているか調べる
	template <typename T>
	inline bool IsInRange(const T& val, const T& min, const T& max)
	{
		return ((min <= val) && (val <= max));
	}

	// 値を指定された範囲で繰り返す
	inline int Wrap(int val, int min, int max)
	{
		bool assert = (min < max + 1);

		Assert(assert);

		const int ret = (val - min) % (max + 1 - min);

		return (ret >= 0) ? (ret + min) : (ret + max + 1);
	}

	// 値を指定された範囲で繰り返す
	inline float Wrap(float val, float min, float max)
	{
		bool assert = (min < max);

		Assert(assert);

		const float ret = std::fmodf(val - min, max - min);

		return (ret >= 0.0f) ? (ret + min) : (ret + max);
	}

	// 指定した範囲を行き来する値を求める
	inline int Iteration(int cnt, int min, int max)
	{
		bool assert = (min < max);

		Assert(assert);

		max -= min;

		int max2 = max * 2;

		cnt = cnt % max2;

		if(cnt >= max){
			cnt = max2 - cnt;
		}

		return cnt + min;
	}

	// 線形補間
	template <typename T>
	inline T Lerp(const T& val1, const T& val2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		return (val1 * (1.0f - t) + val2 * t);
	}

	// 右揃え
	inline float RightAlign(float right, float gap, int cnt)
	{
		return ((gap * cnt) + right);
	}

	// 左揃え
	inline float LeftAlign(float left, float gap, int cnt)
	{
		return (-(gap * cnt) + left);
	}

	// 中央揃え
	inline float CenterAlign(float center, float gap, int cnt, int division)
	{
		bool assert = (cnt < division) && (cnt >= 0);

		Assert(assert);

		return ((gap * cnt) - (gap * 0.5f * (division - 1)) + center);
	}

	// Degree角からRadian角へ変換
	inline float DegreeToRadian(float dig)
	{
		return (dig / 180.0f * ideaMath::PI);
	}

	// Radian角からDegree角へ変換
	inline float RadianToDegree(float rad)
	{
		return (rad / ideaMath::PI * 180.0f);
	}

	// Degree角の正規化
	inline float WrapDegree(float dig)
	{
		return Wrap(dig, 0.0f, 360.0f);
	}

	// Radian角の正規化
	inline float WrapRadian(float rad)
	{
		return Wrap(rad, 0.0f, ideaMath::PI2);
	}

	// 秒からフレームへ変換
	inline int SecondToFrame(int sec, int fps = 60)
	{
		return (sec * fps);
	}

	// 分からフレームへ変換
	inline int MinuteToFrame(int min, int fps = 60)
	{
		return SecondToFrame(min * 60, fps);
	}

	// 時間からフレームへ変換
	inline int HourToFrame(int hour, int fps = 60)
	{
		return MinuteToFrame(hour * 60, fps);
	}

	// フレームから秒へ変換
	inline int FrameToSecond(int frame, int fps = 60)
	{
		Assert(fps);

		return (frame / SecondToFrame(1, fps));
	}

	// フレームから分へ変換
	inline int FrameToMinute(int frame, int fps = 60)
	{
		Assert(fps);

		return (frame / MinuteToFrame(1, fps));
	}

	// フレームから時間へ変換
	inline int FrameToHour(int frame, int fps = 60)
	{
		Assert(fps);

		return (frame / HourToFrame(1, fps));
	}

	// 2次元ベクトル
	struct Vector2D{
		union{
			struct{
				float x;
				float y;
			};

			float r[2];
		};

		Vector2D() : x(0.0f), y(0.0f){}
		Vector2D(float x, float y) : x(x), y(y){}

		Vector2D& operator=(const Vector2D& v)
		{
			x = v.x;
			y = v.y;

			return (*this);
		}

		Vector2D& operator+()
		{
			return (*this);
		}

		Vector2D operator-()const
		{
			return Vector2D(x * -1.0f, y * -1.0f);
		}

		Vector2D operator+(const Vector2D& v)const
		{
			return Vector2D(x + v.x, y + v.y);
		}

		Vector2D& operator+=(const Vector2D& v)
		{
			x += v.x;
			y += v.y;

			return (*this);
		}

		Vector2D operator-(const Vector2D& v)const
		{
			return Vector2D(x - v.x, y - v.y);
		}

		Vector2D& operator-=(const Vector2D& v)
		{
			x -= v.x;
			y -= v.y;

			return (*this);
		}

		Vector2D operator*(float f)const
		{
			return Vector2D(x * f, y * f);
		}

		Vector2D& operator*=(float f)
		{
			x *= f;
			y *= f;

			return (*this);
		}

		Vector2D operator*(const Vector2D& v)const
		{
			return Vector2D(x * v.x, y * v.y);
		}

		Vector2D operator/(float f)const
		{
			Assert(f != 0.0f);

			return Vector2D(x / f, y / f);
		}

		Vector2D& operator/=(float f)
		{
			Assert(f != 0.0f);

			x /= f;
			y /= f;

			return (*this);
		}

		Vector2D operator/(const Vector2D& v)const
		{
			Assert(v.x != 0.0f && v.y != 0.0f);

			return Vector2D(x / v.x, y / v.y);
		}

		friend Vector2D operator*(float f, const Vector2D& v)
		{
			return Vector2D(v.x * f, v.y * f);
		}

		friend Vector2D operator/(float f, const Vector2D& v)
		{
			Assert(f != 0.0f);

			return Vector2D(v.x / f, v.y / f);
		}

		bool operator==(const Vector2D& v)const
		{
			return ((x == v.x) && (y == v.y));
		}

		bool operator!=(const Vector2D& v)const
		{
			return ((x != v.x) || (y != v.y));
		}

		// ベクトルの長さ
		float Length()const
		{
			return sqrtf(x * x + y * y);
		}

		// ベクトルの長さの2乗
		float LengthSquare()const
		{
			return (x * x + y * y);
		}

		// ベクトルの角度
		float Angle()const
		{
			if(LengthSquare() == 0.0f){ return 0.0f; }

			return atan2f(y, x);
		}

		// 正規化されたベクトル
		Vector2D Normalized()const
		{
			const float len = Length();

			if(len == 0.0f){ return Vector2D(0.0f, 0.0f); }

			return  Vector2D(x / len, y / len);
		}

		// X座標を差し替えたベクトル
		Vector2D ReplaceX(float rx)const
		{
			return Vector2D(rx, y);
		}

		// Y座標を差し替えたベクトル
		Vector2D ReplaceY(float ry)const
		{
			return Vector2D(x, ry);
		}

		// ベクトルの内積
		float Dot(const Vector2D& v)const
		{
			return (x * v.x + y * v.y);
		}

		// ベクトルの外積
		float Cross(const Vector2D& v)const
		{
			return (x * v.y - y * v.x);
		}

		// ベクトル間の距離
		float Distance(const Vector2D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;

			return sqrtf(X * X + Y * Y);
		}

		// ベクトル間の距離の2乗
		float DistanceSquare(const Vector2D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;

			return (X * X + Y * Y);
		}

		// ベクトル間の交差角度(0.0～2π)
		float CrossingAngle(const Vector2D& v)const
		{
			float d = Length() * v.Length();

			if(d == 0.0f){ return 0.0f; }

			float c = Dot(v) / d;
			float s = Cross(v) / d;

			if(c == 0.0f){
				if(s > 0.0f){ return ideaMath::PI * 0.5f; } else{ return ideaMath::PI * 1.5f; }
			}

			return atanf(s / c);
		}
	};

	// 2次元の線分
	struct Segment2D{
		Vector2D startPoint;
		Vector2D endPoint;

		Segment2D() :
			startPoint({}),
			endPoint({}){}

		Segment2D(const Vector2D& sp, const Vector2D& ep) :
			startPoint(sp),
			endPoint(ep){}

		Segment2D(float spX, float spY, float epX, float epY) :
			startPoint({ spX, spY }),
			endPoint({ epX, epY }){}

		Vector2D Vector()
		{
			return endPoint - startPoint;
		}

		Vector2D PositionOnSegmentFromX(float x){
			float rate = (x - startPoint.x) / (endPoint.x - startPoint.x);

			return (Vector() * rate + startPoint);
		}

		Vector2D PositionOnSegmentFromY(float y){
			float rate = (y - startPoint.y) / (endPoint.y - startPoint.y);

			return (Vector() * rate + startPoint);
		}
	};

	// 3次元ベクトル
	struct Vector3D{
		union{
			struct{
				float x;
				float y;
				float z;
			};

			float r[3];
		};

		Vector3D() : x(0.0f), y(0.0f), z(0.0f){}
		Vector3D(const Vector2D& v, float z) : x(v.x), y(v.y), z(z){}
		Vector3D(float x, float y, float z) : x(x), y(y), z(z){}

		Vector3D& operator=(const Vector3D& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;

			return (*this);
		}

		Vector3D& operator+()
		{
			return (*this);
		}

		Vector3D operator-()const
		{
			return Vector3D(x * -1.0f, y * -1.0f, z * -1.0f);
		}

		Vector3D operator+(const Vector3D& v)const
		{
			return Vector3D(x + v.x, y + v.y, z + v.z);
		}

		Vector3D& operator+=(const Vector3D& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;

			return (*this);
		}

		Vector3D operator-(const Vector3D& v)const
		{
			return Vector3D(x - v.x, y - v.y, z - v.z);
		}

		Vector3D& operator-=(const Vector3D& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;

			return (*this);
		}

		Vector3D operator*(float f)const
		{
			return Vector3D(x * f, y * f, z * f);
		}

		Vector3D& operator*=(float f)
		{
			x *= f;
			y *= f;
			z *= f;

			return (*this);
		}

		Vector3D operator*(const Vector3D& v)const
		{
			return Vector3D(x * v.x, y * v.y, z * v.z);
		}

		Vector3D operator/(float f)const
		{
			Assert(f != 0.0f);

			return Vector3D(x / f, y / f, z / f);
		}

		Vector3D& operator/=(float f)
		{
			Assert(f != 0.0f);

			x /= f;
			y /= f;
			z /= f;

			return (*this);
		}

		Vector3D operator/(const Vector3D& v)const
		{
			Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f);

			return Vector3D(x / v.x, y / v.y, z / v.z);
		}

		Vector3D& operator/=(const Vector3D& v)
		{
			(*this) = (*this) / v;

			return (*this);
		}

		friend Vector3D operator*(float f, const Vector3D& v)
		{
			return Vector3D(v.x * f, v.y * f, v.z * f);
		}

		friend Vector3D operator/(float f, const Vector3D& v)
		{
			Assert(f != 0.0f);

			return Vector3D(v.x / f, v.y / f, v.z / f);
		}

		bool operator==(const Vector3D& v)const
		{
			return ((x == v.x) && (y == v.y) && (z == v.z));
		}

		bool operator!=(const Vector3D& v)const
		{
			return ((x != v.x) || (y != v.y) || (z != v.z));
		}

		// ベクトルの長さ
		float Length()const
		{
			float sq = LengthSquare();

			if(sq == 0.0f){
				return 0.0f;
			}

			return sqrtf(sq);
		}

		// ベクトルの長さの2乗
		float LengthSquare()const
		{
			return (x * x + y * y + z * z);
		}

		// 正規化されたベクトル
		Vector3D Normalized()const
		{
			const float len = Length();

			if(len == 0.0f){ return Vector3D(0.0f, 0.0f, 0.0f); }

			return  Vector3D(x / len, y / len, z / len);
		}

		// X座標を差し替えたベクトル
		Vector3D ReplaceX(float rx)const
		{
			return Vector3D(rx, y, z);
		}

		// Y座標を差し替えたベクトル
		Vector3D ReplaceY(float ry)const
		{
			return Vector3D(x, ry, z);
		}

		// Z座標を差し替えたベクトル
		Vector3D ReplaceZ(float rz)const
		{
			return Vector3D(x, y, rz);
		}

		// ベクトルの内積
		float Dot(const Vector3D& v)const
		{
			return (x * v.x + y * v.y + z * v.z);
		}

		// ベクトルの外積
		Vector3D Cross(const Vector3D& v)const
		{
			return Vector3D((y * v.z) - (z * v.y), (z * v.x) - (x * v.z), (x * v.y) - (y * v.x));
		}

		// ベクトル間の距離
		float Distance(const Vector3D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;
			float Z = v.z - z;

			return sqrtf(X * X + Y * Y + Z * Z);
		}

		// ベクトル間の距離の2乗
		float DistanceSquare(const Vector3D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;
			float Z = v.z - z;

			return (X * X + Y * Y + Z * Z);
		}

		// ベクトルの角度
		Vector3D PitchYaw()const
		{
			Vector2D xz = Vector2D(x, z);
			Vector2D yxz = Vector2D(y, sqrt(x * x + z * z));

			float pich = -(yxz.Angle() + ideaMath::PIDIV2);
			float yaw = -(xz.Angle() + ideaMath::PIDIV2);

			return Vector3D(pich, yaw, 0.0f);
		}

		// ベクトル間の交差角度(0.0～π)
		float CrossingAngle(const Vector3D& v)const
		{
			float d = Length() * v.Length();

			if(d == 0.0f){ return 0.0f; }

			float c = Dot(v) / d;

			if(c >= 1.0f){ return 0.0f; }
			if(c <= -1.0f){ return ideaMath::PI; }

			return acosf(c);
		}
	};

	// 3次元の線分
	struct Segment3D{
		Vector3D startPoint;
		Vector3D endPoint;

		Segment3D() :
			startPoint({}),
			endPoint({}){}

		Segment3D(const Vector3D& sp, const Vector3D& ep) :
			startPoint(sp),
			endPoint(ep){}

		Segment3D(float spX, float spY, float spZ, float epX, float epY, float epZ) :
			startPoint({ spX, spY, spZ }),
			endPoint({ epX, epY, epZ }){}

		Vector3D Vector()
		{
			return endPoint - startPoint;
		}

		Vector3D PositionOnSegmentFromX(float x){
			float rate = (x - startPoint.x) / (endPoint.x - startPoint.x);

			return (Vector() * rate + startPoint);
		}

		Vector3D PositionOnSegmentFromY(float y){
			float rate = (y - startPoint.y) / (endPoint.y - startPoint.y);

			return (Vector() * rate + startPoint);
		}

		Vector3D PositionOnSegmentFromZ(float z){
			float rate = (z - startPoint.z) / (endPoint.z - startPoint.z);

			return (Vector() * rate + startPoint);
		}
	};

	// 4次元ベクトル
	struct Vector4D{
		union{
			struct{
				float x;
				float y;
				float z;
				float w;
			};

			float r[4];
		};

		Vector4D() : x(0.0f), y(0.0f), z(0.0f), w(0.0f){}
		Vector4D(const Vector3D& v, float w) : x(v.x), y(v.y), z(v.z), w(w){}
		Vector4D(const Vector2D& v, float z, float w) : x(v.x), y(v.y), z(z), w(w){}
		Vector4D(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}

		Vector4D& operator=(const Vector4D& v)
		{
			x = v.x;
			y = v.y;
			z = v.z;
			w = v.w;

			return (*this);
		}

		Vector4D& operator+()
		{
			return (*this);
		}

		Vector4D operator-()const
		{
			return Vector4D(x * -1.0f, y * -1.0f, z * -1.0f, w * -1.0f);
		}

		Vector4D operator+(const Vector4D& v)const
		{
			return Vector4D(x + v.x, y + v.y, z + v.z, w + v.w);
		}

		Vector4D& operator+=(const Vector4D& v)
		{
			x += v.x;
			y += v.y;
			z += v.z;
			w += v.w;

			return (*this);
		}

		Vector4D operator-(const Vector4D& v)const
		{
			return Vector4D(x - v.x, y - v.y, z - v.z, w - v.w);
		}

		Vector4D& operator-=(const Vector4D& v)
		{
			x -= v.x;
			y -= v.y;
			z -= v.z;
			w -= v.w;

			return (*this);
		}

		Vector4D operator*(float f)const
		{
			return Vector4D(x * f, y * f, z * f, w * f);
		}

		Vector4D& operator*=(float f)
		{
			x *= f;
			y *= f;
			z *= f;
			w *= f;

			return (*this);
		}

		Vector4D operator*(const Vector4D& v)const
		{
			return Vector4D(x * v.x, y * v.y, z * v.z, w * v.w);
		}

		Vector4D& operator*=(const Vector4D& v)
		{
			(*this) = (*this) * v;

			return (*this);
		}

		Vector4D operator/(float f)const
		{
			Assert(f != 0.0f);

			return Vector4D(x / f, y / f, z / f, w / f);
		}

		Vector4D& operator/=(float f)
		{
			Assert(f != 0.0f);

			x /= f;
			y /= f;
			z /= f;
			w /= f;

			return (*this);
		}

		Vector4D operator/(const Vector4D& v)const
		{
			Assert(v.x != 0.0f && v.y != 0.0f && v.z != 0.0f && v.w == 0.0f);

			return Vector4D(x / v.x, y / v.y, z / v.z, w / v.w);
		}

		Vector4D& operator/=(const Vector4D& v)
		{
			(*this) = (*this) / v;

			return (*this);
		}

		friend Vector4D operator*(float f, const Vector4D& v)
		{
			return Vector4D(v.x * f, v.y * f, v.z * f, v.w / f);
		}

		friend Vector4D operator/(float f, const Vector4D& v)
		{
			Assert(f != 0.0f);

			return Vector4D(v.x / f, v.y / f, v.z / f, v.w / f);
		}

		bool operator==(const Vector4D& v)const
		{
			return ((x == v.x) && (y == v.y) && (z == v.z) && (w == v.w));
		}

		bool operator!=(const Vector4D& v)const
		{
			return ((x != v.x) || (y != v.y) || (z != v.z) || (w != v.w));
		}

		// ベクトルの長さ
		float Length()const
		{
			return sqrtf(x * x + y * y + z * z + w * w);
		}

		// ベクトルの長さの2乗
		float LengthSquare()const
		{
			return (x * x + y * y + z * z + w * w);
		}

		// 正規化されたベクトル
		Vector4D Normalized()const
		{
			const float len = Length();

			if(len == 0.0f){ return Vector4D(0.0f, 0.0f, 0.0f, 0.0f); }

			return  Vector4D(x / len, y / len, z / len, w / len);
		}

		// X座標を差し替えたベクトル
		Vector4D ReplaceX(float rx)const
		{
			return Vector4D(rx, y, z, w);
		}

		// Y座標を差し替えたベクトル
		Vector4D ReplaceY(float ry)const
		{
			return Vector4D(x, ry, z, w);
		}

		// Z座標を差し替えたベクトル
		Vector4D ReplaceZ(float rz)const
		{
			return Vector4D(x, y, rz, w);
		}

		// W座標を差し替えたベクトル
		Vector4D ReplaceW(float rw)const
		{
			return Vector4D(x, y, z, rw);
		}

		// ベクトルの内積
		float Dot(const Vector4D& v)const
		{
			return (x * v.x + y * v.y + z * v.z + w * v.w);
		}

		// ベクトル間の距離
		float Distance(const Vector4D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;
			float Z = v.z - z;
			float W = v.w - w;

			return sqrtf(X * X + Y * Y + Z * Z + W * W);
		}

		// ベクトル間の距離の2乗
		float DistanceSquare(const Vector4D& v)const
		{
			float X = v.x - x;
			float Y = v.y - y;
			float Z = v.z - z;
			float W = v.w - w;

			return (X * X + Y * Y + Z * Z + W * W);
		}

		// ベクトル間の交差角度(0.0～π)
		float Angle(const Vector4D& v)const
		{
			float d = Length() * v.Length();

			if(d == 0.0f){ return 0.0f; }

			float c = Dot(v) / d;

			if(c >= 1.0f){ return 0.0f; }
			if(c <= -1.0f){ return ideaMath::PI; }

			return acosf(c);
		}
	};

	// 4次元の線分
	struct Segment4D{
		Vector4D startPoint;
		Vector4D endPoint;

		Segment4D() :
			startPoint({}),
			endPoint({}){}

		Segment4D(const Vector4D& sp, const Vector4D& ep) :
			startPoint(sp),
			endPoint(ep){}

		Segment4D(float spX, float spY, float spZ, float spW, float epX, float epY, float epZ, float epW) :
			startPoint({ spX, spY, spZ, spW }),
			endPoint({ epX, epY, epZ, epW }){}

		Vector4D Vector()
		{
			return endPoint - startPoint;
		}

		Vector4D PositionOnSegmentFromX(float x){
			float rate = (x - startPoint.x) / (endPoint.x - startPoint.x);

			return (Vector() * rate + startPoint);
		}

		Vector4D PositionOnSegmentFromY(float y){
			float rate = (y - startPoint.y) / (endPoint.y - startPoint.y);

			return (Vector() * rate + startPoint);
		}

		Vector4D PositionOnSegmentFromZ(float z){
			float rate = (z - startPoint.z) / (endPoint.z - startPoint.z);

			return (Vector() * rate + startPoint);
		}

		Vector4D PositionOnSegmentFromW(float w){
			float rate = (w - startPoint.w) / (endPoint.w - startPoint.w);

			return (Vector() * rate + startPoint);
		}
	};

	// 3x3行列
	struct Matrix3x3{
		union{
			struct{
				float _11, _12, _13;
				float _21, _22, _23;
				float _31, _32, _33;
			};

			float r[3][3];

			float m[9];
		};

		Matrix3x3() : _11(1.0f), _12(0.0f), _13(0.0f), _21(0.0f), _22(1.0f), _23(0.0f), _31(0.0f), _32(0.0f), _33(1.0f){}

		Matrix3x3(float m11, float m12, float m13, float m21, float m22, float m23, float m31, float m32, float m33)
		{
			_11 = m11; _12 = m12; _13 = m13;
			_21 = m21; _22 = m22; _23 = m23;
			_31 = m31; _32 = m32; _33 = m33;
		}

		Matrix3x3& operator=(const Matrix3x3& m)
		{
			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					r[i][j] = m.r[i][j];
				}
			}

			return (*this);
		}

		Matrix3x3 operator-() const
		{
			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = -this->r[i][j];
				}
			}

			return ret;
		}

		Matrix3x3 operator+(const Matrix3x3& m) const
		{
			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] + m.r[i][j];
				}
			}

			return ret;
		}

		Matrix3x3& operator+=(const Matrix3x3& m)
		{
			(*this) = (*this) + m;

			return (*this);
		}

		Matrix3x3 operator-(const Matrix3x3& m) const
		{
			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] - m.r[i][j];
				}
			}

			return ret;
		}

		Matrix3x3& operator-=(const Matrix3x3& m)
		{
			(*this) = (*this) - m;

			return (*this);
		}

		Matrix3x3 operator*(float f) const
		{
			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] * f;
				}
			}

			return ret;
		}

		Matrix3x3& operator*=(float f)
		{
			(*this) = (*this) * f;

			return (*this);
		}

		Matrix3x3 operator*(const Matrix3x3& m) const
		{
			Matrix3x3 ret;

			for(int i = 0; i < 3; ++i){
				for(int j = 0; j < 3; ++j){
					ret.r[i][j] = 0.0f;
					for(int k = 0; k < 3; ++k){
						ret.r[i][j] += r[i][k] * m.r[k][j];
					}
				}
			}

			return ret;
		}

		Matrix3x3& operator*=(const Matrix3x3& m)
		{
			(*this) = (*this) * m;

			return (*this);
		}

		Matrix3x3 operator/(float f) const
		{
			Assert(f != 0.0f);

			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] / f;
				}
			}

			return ret;
		}

		Matrix3x3& operator/=(float f)
		{
			(*this) = (*this) / f;

			return (*this);
		}

		// スカラー乗算
		Matrix3x3 Multiply(float f)const
		{
			return ((*this) * f);
		}

		// 行列の乗算
		Matrix3x3 Multiply(const Matrix3x3& m)const
		{
			return ((*this) * m);
		}

		// 2Dベクトルを行列で変換する
		Vector2D Vector2DTransform(const Vector2D& v)const
		{
			return Vector2D(v.x * _11 + v.y * _21 + _31, v.x * _12 + v.y * _22 + _32);
		}

		// 逆行列
		Matrix3x3 Inverse()const
		{
			Matrix3x3 ret;
			Matrix3x3 tmp = (*this);

			float det;

			for(int i = 0; i < 3; ++i){
				if(r[i][i] == 0.0f){ return tmp; }
				det = 1 / tmp.r[i][i];

				for(int j = 0; j < 3; ++j){
					tmp.r[i][j] *= det;
					ret.r[i][j] *= det;
				}

				for(int j = 0; j < 3; ++j){
					if(i != j){
						det = tmp.r[j][i];
						for(int k = 0; k < 3; ++k){
							tmp.r[j][k] -= tmp.r[i][k] * det;
							ret.r[j][k] -= ret.r[i][k] * det;
						}
					}
				}
			}

			return ret;
		}

		// 転置行列
		Matrix3x3 Transpose()const
		{
			Matrix3x3 ret;

			for(int i = 3 - 1; i >= 0; --i){
				for(int j = 3 - 1; j >= 0; --j){
					ret.r[i][j] = r[j][i];
				}
			}

			return ret;
		}
	};

	// 4x4行列
	struct Matrix4x4{
		union{
			struct{
				float _11, _12, _13, _14;
				float _21, _22, _23, _24;
				float _31, _32, _33, _34;
				float _41, _42, _43, _44;
			};

			float r[4][4];

			float m[16];
		};

		Matrix4x4() : _11(1.0f), _12(0.0f), _13(0.0f), _14(0.0f), _21(0.0f), _22(1.0f), _23(0.0f), _24(0.0f), _31(0.0f), _32(0.0f), _33(1.0f), _34(0.0f), _41(0.0f), _42(0.0f), _43(0.0f), _44(1.0f){}

		Matrix4x4(float m11, float m12, float m13, float m14, float m21, float m22, float m23, float m24, float m31, float m32, float m33, float m34, float m41, float m42, float m43, float m44)
		{
			_11 = m11; _12 = m12; _13 = m13; _14 = m14;
			_21 = m21; _22 = m22; _23 = m23; _24 = m24;
			_31 = m31; _32 = m32; _33 = m33; _34 = m34;
			_41 = m41; _42 = m42; _43 = m43; _44 = m44;
		}

		Matrix4x4& operator=(const Matrix4x4& m)
		{
			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					r[i][j] = m.r[i][j];
				}
			}

			return (*this);
		}

		Matrix4x4& operator+()
		{
			return (*this);
		}

		Matrix4x4 operator-() const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = -this->r[i][j];
				}
			}

			return ret;
		}

		Matrix4x4 operator+(const Matrix4x4& m) const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] + m.r[i][j];
				}
			}

			return ret;
		}

		Matrix4x4& operator+=(const Matrix4x4& m)
		{
			(*this) = (*this) + m;

			return (*this);
		}

		Matrix4x4 operator-(const Matrix4x4& m) const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] - m.r[i][j];
				}
			}

			return ret;
		}

		Matrix4x4& operator-=(const Matrix4x4& m)
		{
			(*this) = (*this) - m;

			return (*this);
		}

		Matrix4x4 operator*(float f) const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] * f;
				}
			}

			return ret;
		}

		Matrix4x4& operator*=(float f)
		{
			(*this) = (*this) * f;

			return (*this);
		}

		Matrix4x4 operator*(const Matrix4x4& m) const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = 0.0f;
					for(int k = 0; k < 4; ++k){
						ret.r[i][j] += r[i][k] * m.r[k][j];
					}
				}
			}

			return ret;
		}

		Matrix4x4& operator*=(const Matrix4x4& m)
		{
			(*this) = (*this) * m;

			return (*this);
		}

		Matrix4x4 operator/(float f) const
		{
			Assert(f != 0.0f);

			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = r[i][j] / f;
				}
			}

			return ret;
		}

		Matrix4x4& operator/=(float f)
		{
			(*this) = (*this) / f;

			return (*this);
		}

		// スカラー乗算
		Matrix4x4 Multiply(float f)const
		{
			return ((*this) * f);
		}

		// 行列の乗算
		Matrix4x4 Multiply(const Matrix4x4& m)const
		{
			return ((*this) * m);
		}

		// 3Dベクトルを行列で変換する
		Vector3D Vector3DTransform(const Vector3D& v)const
		{
			float X = ((((v.x * _11) + (v.y * _21)) + (v.z * _31)) + _41);
			float Y = ((((v.x * _12) + (v.y * _22)) + (v.z * _32)) + _42);
			float Z = ((((v.x * _13) + (v.y * _23)) + (v.z * _33)) + _43);
			float W = ((((v.x * _14) + (v.y * _24)) + (v.z * _34)) + _44);

			return Vector3D(X / W, Y / W, Z / W);
		}

		// 行列式を求めます
		float Determinant()const
		{
			float det =
				_11 * _22 * _33 * _44 + _11 * _23 * _34 * _42 +
				_11 * _24 * _32 * _43 + _12 * _21 * _34 * _43 +
				_12 * _23 * _31 * _44 + _12 * _24 * _33 * _41 +
				_13 * _21 * _32 * _44 + _13 * _22 * _34 * _41 +
				_13 * _24 * _31 * _42 + _14 * _21 * _33 * _42 +
				_14 * _22 * _31 * _43 + _14 * _23 * _32 * _41 -
				_11 * _22 * _34 * _43 - _11 * _23 * _32 * _44 -
				_11 * _24 * _33 * _42 - _12 * _21 * _33 * _44 -
				_12 * _23 * _34 * _41 - _12 * _24 * _31 * _43 -
				_13 * _21 * _34 * _42 - _13 * _22 * _31 * _44 -
				_13 * _24 * _32 * _41 - _14 * _21 * _32 * _43 -
				_14 * _22 * _33 * _41 - _14 * _23 * _31 * _42;

			return det;
		}

		// 逆行列
		Matrix4x4 Inverse()const
		{
			Matrix4x4 result;
			register float det = Determinant();
			Assert(det != 0.0f);

			result._11 = _22 * _33 * _44 + _23 * _34 * _42 + _24 * _32 * _43 - _22 * _34 * _43 - _23 * _32 * _44 - _24 * _33 * _42;
			result._12 = _12 * _34 * _43 + _13 * _32 * _44 + _14 * _33 * _42 - _12 * _33 * _44 - _13 * _34 * _42 - _14 * _32 * _43;
			result._13 = _12 * _23 * _44 + _13 * _24 * _42 + _14 * _22 * _43 - _12 * _24 * _43 - _13 * _22 * _44 - _14 * _23 * _42;
			result._14 = _12 * _24 * _33 + _13 * _22 * _34 + _14 * _23 * _32 - _12 * _23 * _34 - _13 * _24 * _32 - _14 * _22 * _33;

			result._21 = _21 * _34 * _43 + _23 * _31 * _44 + _24 * _33 * _41 - _21 * _33 * _44 - _23 * _34 * _41 - _24 * _31 * _43;
			result._22 = _11 * _33 * _44 + _13 * _34 * _41 + _14 * _31 * _43 - _11 * _34 * _43 - _13 * _31 * _44 - _14 * _33 * _41;
			result._23 = _11 * _24 * _43 + _13 * _21 * _44 + _14 * _23 * _41 - _11 * _23 * _44 - _13 * _24 * _41 - _14 * _21 * _43;
			result._24 = _11 * _23 * _34 + _13 * _24 * _31 + _14 * _21 * _33 - _11 * _24 * _33 - _13 * _21 * _34 - _14 * _23 * _31;

			result._31 = _21 * _32 * _44 + _22 * _34 * _41 + _24 * _31 * _42 - _21 * _34 * _42 - _22 * _31 * _44 - _24 * _32 * _41;
			result._32 = _11 * _34 * _42 + _12 * _31 * _44 + _14 * _32 * _41 - _11 * _32 * _44 - _12 * _34 * _41 - _14 * _31 * _42;
			result._33 = _11 * _22 * _44 + _12 * _24 * _41 + _14 * _21 * _42 - _11 * _24 * _42 - _12 * _21 * _44 - _14 * _22 * _41;
			result._34 = _11 * _24 * _32 + _12 * _21 * _34 + _14 * _22 * _31 - _11 * _22 * _34 - _12 * _24 * _31 - _14 * _21 * _32;

			result._41 = _21 * _33 * _42 + _22 * _31 * _43 + _23 * _32 * _41 - _21 * _32 * _43 - _22 * _33 * _41 - _23 * _31 * _42;
			result._42 = _11 * _32 * _43 + _12 * _33 * _41 + _13 * _31 * _42 - _11 * _33 * _42 - _12 * _31 * _43 - _13 * _32 * _41;
			result._43 = _11 * _23 * _42 + _12 * _21 * _43 + _13 * _22 * _41 - _11 * _22 * _43 - _12 * _23 * _41 - _13 * _21 * _42;
			result._44 = _11 * _22 * _33 + _12 * _23 * _31 + _13 * _21 * _32 - _11 * _23 * _32 - _12 * _21 * _33 - _13 * _22 * _31;

			result._11 /= det;
			result._12 /= det;
			result._13 /= det;
			result._14 /= det;

			result._21 /= det;
			result._22 /= det;
			result._23 /= det;
			result._24 /= det;

			result._31 /= det;
			result._32 /= det;
			result._33 /= det;
			result._34 /= det;

			result._41 /= det;
			result._42 /= det;
			result._43 /= det;
			result._44 /= det;

			return result;
		}

		// 転置行列
		Matrix4x4 Transpose()const
		{
			Matrix4x4 ret;

			for(int i = 4 - 1; i >= 0; --i){
				for(int j = 4 - 1; j >= 0; --j){
					ret.r[i][j] = r[j][i];
				}
			}

			return ret;
		}
	};

	// 四元数
	struct Quaternion{
		union{
			struct{
				float x;
				float y;
				float z;
				float w;
			};

			struct{
				Vector3D v;
				float w;
			};

			float r[4];
		};

		Quaternion() : x(0.0f), y(0.0f), z(0.0f), w(1.0f){}
		Quaternion(float x, float y, float z, float w) : x(x), y(y), z(z), w(w){}
		Quaternion(const Vector3D& v, float w)
		{
			(*this).v = v;
			(*this).w = w;
		}

		Quaternion& operator=(Quaternion q)
		{
			x = q.x;
			y = q.y;
			z = q.z;
			w = q.w;

			return (*this);
		}

		Quaternion& operator+()
		{
			return (*this);
		}

		Quaternion operator-()const
		{
			return Quaternion(x * -1.0f, y * -1.0f, z * -1.0f, w * -1.0f);
		}

		Quaternion operator+(const Quaternion& q)const
		{
			return Quaternion(x + q.x, y + q.y, z + q.z, w + q.w);
		}

		Quaternion& operator+=(const Quaternion& q)
		{
			x += q.x;
			y += q.y;
			z += q.z;
			w += q.w;

			return (*this);
		}

		Quaternion operator-(const Quaternion& q)const
		{
			return Quaternion(x - q.x, y - q.y, z - q.z, w - q.w);
		}

		Quaternion& operator-=(const Quaternion& q)
		{
			x -= q.x;
			y -= q.y;
			z -= q.z;
			w -= q.w;

			return (*this);
		}

		Quaternion operator*(float f)const
		{
			return Quaternion(x * f, y * f, z * f, w * f);
		}

		Quaternion operator*(const Quaternion& q)const
		{
			float wx = w * q.w - (v.Dot(q.v));

			Vector3D vx = q.v * w + v * q.w + v.Cross(q.v);

			return Quaternion(vx, wx).Normalized();
		}

		bool operator==(const Quaternion& q)const
		{
			return ((x == q.x) && (y == q.y) && (z == q.z) && (w == q.w));
		}

		bool operator!=(const Quaternion& q)const
		{
			return ((x != q.x) || (y != q.y) || (z != q.z) || (w != q.w));
		}

		// 四元数の長さ
		float Length()const
		{
			return sqrtf(x * x + y * y + z * z + w * w);
		}

		// 四元数の長さの2乗
		float LengthSquare()const
		{
			return (x * x + y * y + z * z + w * w);
		}

		// 正規化された四減数
		Quaternion Normalized()const
		{
			float mag;

			mag = sqrtf(w * w + x * x + y * y + z * z);

			Quaternion res;

			if(mag > ideaMath::EPSILON) {
				mag = 1.0f / mag;
				res.x *= mag;
				res.y *= mag;
				res.z *= mag;
				res.w *= mag;
			}

			return res;
		}

		// 四元数の内積
		float Dot(const Quaternion& q)const
		{
			return (x * q.x + y * q.y + z * q.z + w * q.w);
		}

		// 四元数の共役
		Quaternion Conjugate()const
		{
			return Quaternion(-x, -y, -z, w);
		}

		// 四元数の回転
		Quaternion& Rotate(const Vector3D& axis, float rad)
		{
			v = (v.Cross(axis) * sinf(rad) + axis * cosf(rad) + (1.0f - cosf(rad)) * (v.Dot(axis)) * axis);

			return (*this);
		}

		// オイラー角へ変換
		Vector3D ToEuler()const
		{
			float mtx[3][3] = {};

			float x2 = x * x;
			float y2 = y * y;
			float z2 = z * z;

			float xy = x * y;
			float xz = x * z;
			float yz = y * z;
			float wx = w * x;
			float wy = w * y;
			float wz = w * z;

			mtx[0][0] = 1.0f - (2.0f * y2) - (2.0f * z2);
			mtx[0][1] = (2.0f * xy) + (2.0f * wz);

			mtx[1][0] = (2.0f * xy) - (2.0f * wz);
			mtx[1][1] = 1.0f - (2.0f * x2) - (2.0f * z2);

			mtx[2][0] = (2.0f * xz) + (2.0f * wy);
			mtx[2][1] = (2.0f * yz) - (2.0f * wx);
			mtx[2][2] = 1.0f - (2.0f * x2) - (2.0f * y2);

			Vector3D ret;

			if(mtx[2][1] == 1.0f){
				ret.x = ideaMath::PI * 0.5f;
				ret.y = 0.0f;
				ret.z = atan2f(mtx[1][0], mtx[0][0]);

			} else if(mtx[2][1] == -1.0f){
				ret.x = -ideaMath::PI * 0.5f;
				ret.y = 0.0f;
				ret.z = atan2f(mtx[1][0], mtx[0][0]);
			} else{
				ret.x = asinf(-mtx[2][1]);
				ret.y = atan2f(mtx[2][0], mtx[2][2]);
				ret.z = atan2f(mtx[0][1], mtx[1][1]);
			}

			return ret;
		}
	};

	// Vector2D -------------------------------
	// 空のベクトル
	inline Vector2D Vector2DZero()
	{
		return Vector2D(0.0f, 0.0f);
	}

	// 交差の確認
	inline bool IsIentersected(const Vector2D& s1, const Vector2D& e1, const Vector2D& s2, const Vector2D& e2)
	{
		return (((s1.x - e1.x) * (s2.y - s1.y) + (s1.y - e1.y) * (s1.x - s2.x)) * ((s1.x - e1.x) * (e2.y - s1.y) + (s1.y - e1.y) * (s1.x - e2.x)) <= 0.0f && ((s2.x - e2.x) * (s1.y - s2.y) + (s2.y - e2.y) * (s2.x - s1.x)) * ((s2.x - e2.x) * (e1.y - s2.y) + (s2.y - e2.y) * (s2.x - e1.x)) <= 0.0f);
	}

	// ベジェ補間
	inline float Bezier(float x, const Vector2D& p1, const Vector2D& p2, int n)
	{
		if(p1.x == p1.y && p2.x == p2.y){ return x; }

		const float k0 = 1.0f + 3.0f * p1.x - 3.0f * p2.x;
		const float k1 = 3.0f * p2.x - 6.0f * p1.x;
		const float k2 = 3.0f * p1.x;

		float t = x;

		for(int i = 0; i < n; ++i){
			float ft = k0 * t * t * t + k1 * t * t + k2 * t - x;

			if(abs(ft) < ideaMath::EPSILON){ break; }

			t -= ft * 0.5f;
		}

		float r = 1.0f - t;

		return (t * t * t + 3.0f * t * t * r * p2.y + 3.0f * t * r * r * p1.y);
	}

	// Vector3D -------------------------------
	// 空のベクトル
	inline Vector3D Vector3DZero()
	{
		return Vector3D(0.0f, 0.0f, 0.0f);
	}

	// 三角形の法線を求める
	inline Vector3D Vector3DTriangleNormal(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3)
	{
		Vector3D v1 = p2 - p1;
		Vector3D v2 = p3 - p1;
		Vector3D ret = v1.Cross(v2);

		return ret.Normalized();
	}

	// 四角形の法線を求める
	inline Vector3D Vector3DSquareNormal(const Vector3D& p1, const Vector3D& p2, const Vector3D& p3, const Vector3D& p4)
	{
		Vector3D ret;

		Vector3D n1a = Vector3DTriangleNormal(p1, p2, p3);
		Vector3D n1b = Vector3DTriangleNormal(p1, p3, p4);
		Vector3D n2a = Vector3DTriangleNormal(p2, p3, p4);
		Vector3D n2b = Vector3DTriangleNormal(p2, p4, p1);

		if(n1a.Dot(n1b) > n2a.Dot(n2b)){
			ret = n1a + n1b;
		} else{
			ret = n2a + n2b;
		}

		return ret.Normalized();
	}

	// Vector4D -------------------------------
	// 空のベクトル
	inline Vector4D Vector4DZero()
	{
		return Vector4D(0.0f, 0.0f, 0.0f, 0.0f);
	}

	// Matrix3x3 ------------------------------
	// 空の行列
	inline Matrix3x3 Matrix3x3Zero()
	{
		Matrix3x3 ret = {};

		return ret;
	}

	// 単位行列
	inline Matrix3x3 Matrix3x3Identity()
	{
		return Matrix3x3(1.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	// 平行移動行列を生成する
	inline Matrix3x3 Matrix3x3Translation(float x, float y)
	{
		Matrix3x3 ret;

		ret._31 = x;
		ret._32 = y;

		return ret;
	}

	// 平行移動行列を生成する
	inline Matrix3x3 Matrix3x3Translation(const Vector2D& t)
	{
		return Matrix3x3Translation(t.x, t.y);
	}

	// 回転行列を生成する
	inline Matrix3x3 Matrix3x3Rotation(float rad)
	{
		Matrix3x3 ret;

		float cosRad = cosf(rad);
		float sinRad = sinf(rad);

		ret._11 = cosRad;
		ret._12 = -sinRad;
		ret._21 = sinRad;
		ret._22 = cosRad;

		return ret;
	}

	// Matrix4x4 ------------------------------
	// 空の行列
	inline Matrix4x4 Matrix4x4Zero()
	{
		Matrix4x4 ret = {};

		return ret;
	}

	// 単位行列
	inline Matrix4x4 Matrix4x4Identity()
	{
		return Matrix4x4(1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 0.0f, 0.0f, 0.0f, 1.0f);
	}

	// 平行移動行列を生成する
	inline Matrix4x4 Matrix4x4Translation(float x, float y, float z)
	{
		Matrix4x4 ret;

		ret._41 = x;
		ret._42 = y;
		ret._43 = z;

		return ret;
	}

	// 平行移動行列を生成する
	inline Matrix4x4 Matrix4x4Translation(const Vector3D& t)
	{
		return Matrix4x4Translation(t.x, t.y, t.z);
	}

	// X軸回りの回転行列を生成する
	inline Matrix4x4 Matrix4x4RotationPitch(float pitch)
	{
		Matrix4x4 ret;

		float cosRad = cosf(pitch);
		float sinRad = sinf(pitch);

		ret._22 = cosRad;
		ret._23 = sinRad;
		ret._32 = -sinRad;
		ret._33 = cosRad;

		return ret;
	}

	// Y軸回りの回転行列を生成する
	inline Matrix4x4 Matrix4x4RotationYaw(float yaw)
	{
		Matrix4x4 ret;

		float cosRad = cosf(yaw);
		float sinRad = sinf(yaw);

		ret._11 = cosRad;
		ret._13 = -sinRad;
		ret._31 = sinRad;
		ret._33 = cosRad;

		return ret;
	}

	// Z軸回りの回転行列を生成する
	inline Matrix4x4 Matrix4x4RotationRoll(float roll)
	{
		Matrix4x4 ret;

		float cosRad = cosf(roll);
		float sinRad = sinf(roll);

		ret._11 = cosRad;
		ret._12 = sinRad;
		ret._21 = -sinRad;
		ret._22 = cosRad;

		return ret;
	}

	// ピッチ・ヨー・ロール角から回転行列を生成する
	inline Matrix4x4 Matrix4x4Rotation(float pitch, float yaw, float roll)
	{
		Matrix4x4 ret;

		ret = Matrix4x4RotationRoll(roll) * Matrix4x4RotationPitch(pitch) * Matrix4x4RotationYaw(yaw);

		return ret;
	}

	// ピッチ・ヨー・ロール角から回転行列を生成する
	inline Matrix4x4 Matrix4x4Rotation(const Vector3D& pitchYawRoll)
	{
		return Matrix4x4Rotation(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	}

	// 軸と角度から回転行列を生成する
	inline Matrix4x4 Matrix4x4FromAxisAngle(const Vector3D& axis, float rad)
	{
		Matrix4x4 ret;

		float sinRad = sinf(rad);
		float cosRad = cosf(rad);

		float a = 1.0f - cosRad;

		float ab = axis.x * axis.y * a;
		float bc = axis.y * axis.z * a;
		float ca = axis.z * axis.x * a;

		float tx = axis.x * axis.x;
		float ty = axis.y * axis.y;
		float tz = axis.z * axis.z;

		ret._11 = tx + cosRad * (1.0f - tx);
		ret._12 = ab + axis.z * sinRad;
		ret._13 = ca - axis.y * sinRad;
		ret._14 = 0.0f;

		ret._21 = ab - axis.z * sinRad;
		ret._22 = ty + cosRad * (1.0f - ty);
		ret._23 = bc + axis.x * sinRad;
		ret._24 = 0.0f;

		ret._31 = ca + axis.y * sinRad;
		ret._32 = bc - axis.x * sinRad;
		ret._33 = tz + cosRad * (1.0f - tz);
		ret._34 = 0.0f;

		ret._41 = 0.0f;
		ret._42 = 0.0f;
		ret._43 = 0.0f;
		ret._44 = 1.0f;

		return ret;
	}

	// 拡大縮小行列を生成する
	inline Matrix4x4 Matrix4x4Scaling(float x, float y, float z)
	{
		Matrix4x4 ret;

		ret._11 = x;
		ret._22 = y;
		ret._33 = z;

		return ret;
	}

	// 拡大縮小行列を生成する
	inline Matrix4x4 Matrix4x4Scaling(const Vector3D& s)
	{
		return Matrix4x4Scaling(s.x, s.y, s.z);
	}

	// 4x4マトリクスから座標を求める
	inline Vector3D TransformFromMatrix4x4(const Matrix4x4& matrix)
	{
		return Vector3D(matrix._41, matrix._42, matrix._43);
	}

	// 4x4マトリクスからスケールを求める
	inline Vector3D ScalingFromMatrix4x4(const Matrix4x4& matrix)
	{
		Vector3D scale;

		scale.x = Vector3D(matrix._11, matrix._12, matrix._13).Length();
		scale.y = Vector3D(matrix._21, matrix._22, matrix._23).Length();
		scale.z = Vector3D(matrix._31, matrix._32, matrix._33).Length();

		return scale;
	}

	// 4x4マトリクスから回転行列を求める
	inline Matrix4x4 RotationMatrixFromMatrix4x4(const Matrix4x4& matrix)
	{
		Matrix4x4 ret = matrix;
		Vector3D scale = ScalingFromMatrix4x4(matrix);

		Assert(scale.x != 0.0f && scale.y != 0.0f && scale.z != 0.0f);

		ret._11 /= scale.x;
		ret._12 /= scale.x;
		ret._13 /= scale.x;
		ret._21 /= scale.y;
		ret._22 /= scale.y;
		ret._23 /= scale.y;
		ret._31 /= scale.z;
		ret._32 /= scale.z;
		ret._33 /= scale.z;
		ret._41 = 0.0f;
		ret._42 = 0.0f;
		ret._43 = 0.0f;

		return ret;
	}

	// 四元数から行列を生成する
	inline Matrix4x4 Matrix4x4FromQuaternion(const Quaternion& q)
	{
		Matrix4x4 ret;

		float xx = q.x * q.x;
		float yy = q.y * q.y;
		float zz = q.z * q.z;
		float xy = q.x * q.y;
		float yw = q.y * q.w;
		float yz = q.y * q.z;
		float xw = q.x * q.w;
		float zx = q.z * q.x;
		float zw = q.z * q.w;

		ret._11 = 1.0f - (2.0f * (yy + zz));
		ret._12 = 2.0f * (xy + zw);
		ret._13 = 2.0f * (zx - yz);
		ret._14 = 0.0f;

		ret._21 = 2.0f * (xy - zw);
		ret._22 = 1.0f - (2.0f * (zz + xx));
		ret._23 = 2.0f * (yz + xw);
		ret._24 = 0.0f;

		ret._31 = 2.0f * (zx + yw);
		ret._32 = 2.0f * (yz - xw);
		ret._33 = 1.0f - (2.0f * (yy + xx));
		ret._34 = 0.0f;

		ret._41 = 0.0f;
		ret._42 = 0.0f;
		ret._43 = 0.0f;
		ret._44 = 1.0f;

		return ret;
	}

	// Quaternion -----------------------------
	// 軸と角度から四元数を生成する
	inline Quaternion  QuaternionFromAxisAngle(const Vector3D& axis, float rad)
	{
		float halfRad = rad * 0.5f;
		float sinX = sinf(halfRad);

		return Quaternion(axis.x * sinX, axis.y * sinX, axis.z * sinX, cosf(halfRad));
	}

	// オイラー角から四元数を生成する
	inline Quaternion QuaternionFromEuler(float pitch, float yaw, float roll)
	{
		float halfPitch = pitch * 0.5f;
		float halfYaw = yaw * 0.5f;
		float halfRoll = roll * 0.5f;

		float cp = cosf(halfPitch);
		float cy = cosf(halfYaw);
		float cr = cosf(halfRoll);

		float sp = sinf(halfPitch);
		float sy = sinf(halfYaw);
		float sr = sinf(halfRoll);

		float qx = cr * cy * sp + sr * sy * cp;
		float qy = cr * sy * cp - sr * cy * sp;
		float qz = sr * cy * cp - cr * sy * sp;
		float qw = cr * cy * cp + sr * sy * sp;

		return Quaternion(qx, qy, qz, qw);
	}

	// オイラー角から四元数を生成する
	inline Quaternion QuaternionFromEuler(const Vector3D& pitchYawRoll)
	{
		return QuaternionFromEuler(pitchYawRoll.x, pitchYawRoll.y, pitchYawRoll.z);
	}

	// 回転行列から四元数を生成する
	inline Quaternion QuaternionFromRotationMatrix(const Matrix4x4& m)
	{
		if((m._11 + m._22 + m._33) > 0.0f){
			float M1 = sqrtf(m._11 + m._22 + m._33 + 1.0f);
			Assert(M1 != 0.0f);

			float W = M1 * 0.5f;

			M1 = 0.5f / M1;

			return Quaternion((m._23 - m._32) * M1, (m._31 - m._13) * M1, (m._12 - m._21) * M1, W);
		}

		if((m._11 >= m._22) && (m._11 >= m._33)){
			float M2 = sqrtf(1.0f + m._11 - m._22 - m._33);
			Assert(M2 != 0.0f);

			float M3 = 0.5f / M2;

			return Quaternion(0.5f * M2, (m._12 + m._21) * M3, (m._13 + m._31) * M3, (m._23 - m._32) * M3);
		}

		if(m._22 > m._33){
			float M4 = sqrtf(1.0f + m._22 - m._11 - m._33);
			Assert(M4 != 0.0f);

			float M5 = 0.5f / M4;

			return Quaternion((m._21 + m._12) * M5, 0.5f * M4, (m._32 + m._23) * M5, (m._31 - m._13) * M5);
		}

		float M6 = sqrtf(1.0f + m._33 - m._11 - m._22);
		Assert(M6 != 0.0f);

		float M7 = 0.5f / M6;

		return Quaternion((m._31 + m._13) * M7, (m._32 + m._23) * M7, 0.5f * M6, (m._12 - m._21) * M7);
	}

	// 球面補間
	inline Quaternion QuaternionSLerp(const Quaternion& q1, const Quaternion& q2, float t)
	{
		t = Clamp(t, 0.0f, 1.0f);

		Quaternion res;

		float qdotr = Clamp(q1.x * q2.x + q1.y * q2.y + q1.z * q2.z + q1.w * q2.w, -1.0f, 1.0f);

		if(qdotr < 0.0) {
			float theta = acosf(-qdotr);

			if(fabs(theta) < ideaMath::EPSILON) {
				return q1;
			}

			float st = sinf(theta);
			Assert(st != 0.0f);
			float invSt = 1.0f / st;
			float c0 = sinf((1.0f - t) * theta) * invSt;
			float c1 = sinf(t * theta) * invSt;

			res.x = c0 * q1.x - c1 * q2.x;
			res.y = c0 * q1.y - c1 * q2.y;
			res.z = c0 * q1.z - c1 * q2.z;
			res.w = c0 * q1.w - c1 * q2.w;

		} else {
			float theta = acosf(qdotr);

			if(fabsf(theta) < ideaMath::EPSILON) {
				return q1;
			}

			float st = sinf(theta);
			Assert(st != 0.0f);
			float invSt = 1.0f / st;
			float c0 = sinf((1.0f - t) * theta) * invSt;
			float c1 = sinf(t * theta) * invSt;

			res.x = c0 * q1.x + c1 * q2.x;
			res.y = c0 * q1.y + c1 * q2.y;
			res.z = c0 * q1.z + c1 * q2.z;
			res.w = c0 * q1.w + c1 * q2.w;
		}

		return res;
	}
}

#endif	// #ifndef INCLUDE_IDEA_IDEAMATH_H