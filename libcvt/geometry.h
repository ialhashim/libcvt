#ifndef GEOMETRY_INCLUDED // -*- C++ -*-
#define GEOMETRY_INCLUDED

#include <iostream>
#include <fstream>

#include <cmath>
#include <iostream>

using namespace std;

#define INF DBL_MAX

#define EPS 1E-5

#define MAX_DIM 400
#define MIXED_DERIVATIVES_TOLERANCE 1E-5

#define rep(i,n) for(int i=0; i<n; i++)
#define urep(i,n) for(unsigned int i=0; i<n; i++)

//#define TRACE(x...) x
//#define PRINT(x...) TRACE(printf(x))
//#define WATCH(x) TRACE(cerr << #x" = " << x << "\n")
#define TRACE(x)
#define PRINT(X)
#define WATCH(X)

#define real double

int cmp(real x, real y = 0, real tol = EPS);

class Point2
{
	public:
		int x, y;

		Point2( int _x = 0, int _y = 0);
		~Point2();

		Point2 operator -(Point2 q) const;
		Point2 operator +(Point2 q) const;
		Point2 operator *(real k) const;
		real operator %(Point2 q) const;
		Point2 operator +=(Point2 q);
		Point2 operator -=(Point2 q);
		Point2 orthogonal();

		int cmp(Point2 q) const;
		bool operator ==(Point2 q) const { return cmp(q) == 0; }
		bool operator < (Point2 q) const { return cmp(q) < 0; }
		
		friend ostream &operator<<(ostream &stream, Point2 v);

		int& operator[](int i);
};

real distance( Point2 p1, Point2 p2 );
real norm( Point2 p );
real norm2( Point2 p );

class Vector3
{
	public:
		real x, y, z;

		Vector3( real _x = 0, real _y = 0, real _z = 0);
		Vector3( const Vector3& v );
		~Vector3();

		bool normalize();
		real norm();
		real norm2();
		void correct_foresight();

		void operator=(Vector3 q);

		Vector3 operator *(const real k);
		Vector3 operator +(Vector3 q);
		Vector3 operator -(Vector3 q);
		Vector3 operator ^(Vector3 q);
		real operator *(Vector3 q);
		Vector3 operator %(Vector3 q);
		
		Vector3 operator +=(Vector3 q);
		Vector3 operator -=(Vector3 q);
		Vector3 operator *=(real k);

		int cmp(Vector3 q) const;
		bool operator == (Vector3 q) const { return cmp(q) == 0; }
		bool operator != (Vector3 q) const { return cmp(q) != 0; }

		real& operator[](int i);
		
		friend ostream &operator<<(ostream &stream, Vector3 v);
};

real norm( Vector3 p );
real norm2( Vector3 p );

typedef Vector3 Vec2f;
typedef Point2 PixelAddress;
//typedef Vector3 Color;

void normalize( real& x, real& y, real& z );
bool seg_intersect(Point2 p, Point2 q, Point2 r, Point2 s);
real seg_distance(Vector3 p, Vector3 q, Vector3 r);


template<class T>
class Raster
{
private:
    T *data;
    int W, H, nchan;

public:
    Raster(int w, int h, int c)
    {
	W=w; H=h; nchan=c;
	data = new T[length()];
    }
    virtual ~Raster() { delete[] data; }

    int width() const { return W; }
    int height() const { return H; }
    int channels() const { return nchan; }
    int length() const { return W*H*nchan; }

    T& operator[](int i)       { return data[i]; }
    T  operator[](int i) const { return data[i]; }

    virtual T       *pixel(int i, int j)       { return data + (j*W+i)*nchan; }
    virtual const T *pixel(int i, int j) const { return data + (j*W+i)*nchan; }
    T       *pixel(PixelAddress &a)       { return pixel(a[0], a[1]); }

    virtual T   &pixelr(int i, int j)       { return *(data + (j*W+i)*nchan); }
    //virtual T 	&pixelr(int i, int j) 	   { return *(data + (j*W+i)*nchan); }
    T       &pixelr(PixelAddress &a)       { return pixel(a[0], a[1]); }
    
    T       *head()       { return data; }
    const T *head() const { return data; }

    void reverse(int start=0, int end=-1);
    void hflip();
    void vflip();

    bool is_valid_address(PixelAddress& a) const
    {
        return is_valid_address(a[0], a[1]);
    }

    virtual bool is_valid_address(int x, int y) const
    {
        return ( (x >= 0) && (x < W) && (y >= 0) && (y < H) );
    }
    
    void clear(T val)
    {
    	urep(i,W) urep(j,H) urep(v,nchan) data[ (j*W+i)*nchan + v ] = val;
    }
};

real random1();



using namespace std;

int cmp(real x, real y, real tol)
{
	return (x <= y + tol) ? (x + tol < y) ? -1 : 0 : 1;
}

void normalize( real& x, real& y, real& z )
{
	real norm = sqrt( x*x + y*y + z*z );
	if ( cmp(norm) == 0 )
	{
		cerr << "normalize: zero norm" << endl;
		norm = EPS;
	}
	x /= norm; y /= norm; z /= norm;
}

Point2::Point2( int _x, int _y ) : x(_x), y(_y)
{
}

Point2::~Point2()
{
}

Point2 Point2::operator *(real k) const
{ 
	return Point2( (int)(k*x), (int)(k*y));
}

Point2 Point2::operator -(Point2 q) const
{ 
	return Point2(x - q.x, y - q.y);
}

Point2 Point2::operator +(Point2 q) const
{ 
	return Point2(x + q.x, y + q.y);
}

Point2 Point2::operator +=(Point2 q)
{
	x += q.x; y += q.y;
	return *this;
}

Point2 Point2::operator -=(Point2 q)
{
	x -= q.x; y -= q.y;
	return *this;
}

real Point2::operator %(Point2 q) const
{
	return x * q.y - y * q.x;
}

Point2 Point2::orthogonal()
{ 
	return Point2(-y, x);
}

int& Point2::operator[](int i)
{
	if (i==0) return x;
	else return y;
}

/*
bool Point2::operator ==(Point2 q) const { return cmp(q) == 0; }
bool Point2::operator < (Point2 q) const { return cmp(q) < 0; }
*/
int Point2::cmp(Point2 q) const
{
	if (int t = ::cmp(x, q.x)) return t;
	return ::cmp(y, q.y);
}

ostream &operator<<(ostream &stream, Point2 p)
{
	stream << "( " << p.x << ", " << p.y << " )";
	return stream;
}


real norm2( Point2 p )
{
	return (p.x)*(p.x) + (p.y)*(p.y);
}

real norm( Point2 p )
{
	return sqrt( norm2(p) );
}

real norm( Vector3 v )
{
	return v.norm();
}

real norm2( Vector3 v )
{
	return v.norm2();
}

real distance( Point2 p1, Point2 p2 )
{
	return sqrt( (p1.x - p2.x)*(p1.x - p2.x) + (p1.y - p2.y)*(p1.y - p2.y) );
}

Vector3::Vector3( real _x, real _y, real _z) : x(_x), y(_y), z(_z)
{
}

Vector3::Vector3( const Vector3& v ) : x(v.x), y(v.y), z(v.z)
{
}

Vector3::~Vector3()
{
}

real Vector3::norm()
{
	return sqrt( norm2() );
}

real Vector3::norm2()
{
	return x*x + y*y + z*z;
}

bool Vector3::normalize()
{
	real n = norm();
	if ( n < EPS/100 )
	{
		//cout << "ERROR NORM = 0" << endl;
		return false;
	}
	x /= n; y /= n; z /= n;
	return true;
}

void Vector3::correct_foresight()
{
	if ( fabs( z ) > EPS )
	{
		x /= z;
		y /= z;
		z = 1;
	}
}

real& Vector3::operator[](int i)
{
	if ( i == 0 ) return x;
	else if ( i == 1 ) return y;
	else return z;
}

void Vector3::operator=(Vector3 q)
{
	x = q.x; y = q.y; z = q.z;
}

Vector3 Vector3::operator *(const real k)
{ 
	return Vector3(k*x, k*y, k*z);
}

Vector3 Vector3::operator +(Vector3 q)
{ 
	return Vector3(x + q.x, y + q.y, z + q.z);
}

Vector3 Vector3::operator -(Vector3 q)
{ 
	return Vector3(x - q.x, y - q.y, z - q.z);
}

Vector3 Vector3::operator ^(Vector3 q)
{
	return Vector3(y*q.z - z*q.y, z*q.x - x*q.z, x*q.y - y*q.x);	
}

real Vector3::operator *(Vector3 q)
{
	return x*q.x + y*q.y + z*q.z;
}

Vector3 Vector3::operator -=(Vector3 q)
{
	x -= q.x; y -= q.y; z -= q.z;
	return *this;
}

Vector3 Vector3::operator +=(Vector3 q)
{
	x += q.x; y += q.y; z += q.z;
	return *this;
}

Vector3 Vector3::operator *=(real k)
{
	x *= k; y *= k; z *= k;
	return *this;
}

Vector3 Vector3::operator %(Vector3 q)
{
	Vector3& p = *this;
	return Vector3( p.y*q.z - p.z*q.y, p.z*q.x - p.x*q.z, p.x*q.y - p.y*q.x );
}

int Vector3::cmp(Vector3 q) const
{
	int t;
	if (t = ::cmp(x, q.x)) return t;
	else if (t = ::cmp(y, q.y)) return t;
	else return ::cmp(z,q.z);
}

ostream &operator<<(ostream &stream, Vector3 v)
{
	stream << "( " << v.x << ", " << v.y << ", " << v.z << " )";
	return stream;
}

bool seg_intersect(Point2 p, Point2 q, Point2 r, Point2 s)
{
	Point2 A = q - p, B = s - r, C = r - p, D = s - q;

	int a = cmp(A % C) + 2 * cmp(A % D);
	int b = cmp(B % C) + 2 * cmp(B % D);

	if (a == 3 || a == -3 || b == 3 || b == -3) return false;
	if (a || b || p == r || p == s || q == r || q == s) return true;
	int t = (p < r) + (p < s) + (q < r) + (q < s);
	return t != 0 && t != 4;
}

real seg_distance(Vector3 p, Vector3 q, Vector3 r)
{
   Vector3 A = r - q, B = r - p, C = q - p;
   real a = A * A, b = B * B, c = C * C;
   if (cmp(b, a + c) >= 0) return sqrt(a);
   else if (cmp(a, b + c) >= 0) return sqrt(b);
   else return fabs((A % B).z) / sqrt(c);
}

real random1()
{
	const int mm = 99999;
    return ( rand() % mm ) / (real) mm;
}

#endif
