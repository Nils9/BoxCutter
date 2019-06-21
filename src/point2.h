#ifndef POINT2_H__
#define POINT2_H__

#include <cassert>
#include <algorithm>
#include <iostream>
#include <vector>
#include <cmath>
#include <float.h>

#define __USE_GSL_FOR_MAT22

#ifdef __USE_GSL_FOR_MAT22
#include <gsl/gsl_matrix.h>
#include <gsl/gsl_linalg.h>
#endif



template< typename T >
class point2
{
public:
    typedef T               type_t;

    static point2<T> Zero() { return point2<T>(0,0); }

    point2< T >(){v[0] = 0; v[1] = 0;}
    point2< T >( T x_ , T y_ ){v[0] = x_; v[1] = y_;}

    T x() const {return v[0];}
    T y() const {return v[1];}

    T operator [] (unsigned int c) const
    {
        return v[c];
    }
    T & operator [] (unsigned int c)
    {
        return v[c];
    }

    void operator += (const point2< T > & other)
    {
        v[0] += other.x();
        v[1] += other.y();
    }
    void operator -= (const point2< T > & other)
    {
        v[0] -= other.x();
        v[1] -= other.y();
    }
    void operator *= (int s)
    {
        v[0] *= s;
        v[1] *= s;
    }
    void operator *= (float s)
    {
        v[0] *= s;
        v[1] *= s;
    }
    void operator *= (double s)
    {
        v[0] *= s;
        v[1] *= s;
    }
    void operator /= (int s)
    {
        v[0] /= s;
        v[1] /= s;
    }
    void operator /= (float s)
    {
        v[0] /= s;
        v[1] /= s;
    }
    void operator /= (double s)
    {
        v[0] /= s;
        v[1] /= s;
    }



    inline static
    T dot( const point2< T > & p1 , const point2< T > & p2 )
    {
        return (p1.x()) * (p2.x()) + (p1.y()) * (p2.y());
    }
    inline static
    T cross( const point2< T > & p1 , const point2< T > & p2 )
    {
        return p1[0]*p2[1] - p1[1]*p2[0];
    }

    T sqrnorm() const
    {
        return v[0]*v[0] + v[1]*v[1];
    }
    T norm() const
    {
        return sqrt(v[0]*v[0] + v[1]*v[1]);
    }
    inline
    void normalize()
    {
        T n = norm();
        v[0] /= n;
        v[1] /= n;
    }


    template< class point_t1 , class point_t2 >
    inline static point2< T > min ( const point_t1  & p , const point_t2  & p2 )
    {
        return point2< T >( std::min<T>( (T)(p2[0]) , (T)(p[0]) ),
                std::min<T>( (T)(p2[1]) , (T)(p[1]) ) );
    }

    template< class point_t1 , class point_t2 >
    inline static point2< T > max ( const point_t1  & p , const point_t2  & p2 )
    {
        return point2< T >( std::max<T>( (T)(p2[0]) , (T)(p[0]) ),
                std::max<T>( (T)(p2[1]) , (T)(p[1]) ) );
    }


    inline static point2< T > Rand ( double randMagnitude )
    {
        T rx = randMagnitude * (2.0*(float)(rand()) / (float)( RAND_MAX ) - 1.0);
        T ry = randMagnitude * (2.0*(float)(rand()) / (float)( RAND_MAX ) - 1.0);
        return point2<T>( rx , ry );
    }



private:
    T v[2];
};


template< typename T >
inline point2< T > rotatePi_2( const point2< T > & p )// rotation of pi/2
{
    return point2< T >( -p.y() , p.x() );
}

template< typename T >
inline std::ostream & operator << (std::ostream & s , const point2< T > & p)
{
    s << p.x() << " " << p.y();
    return s;
}




template< typename T >
inline
point2< T > operator + (const point2< T > & p1 , const point2< T > & p2 )
{
    return point2< T >( p1.x() + p2.x() , p1.y() + p2.y() );
}





template< typename T >
inline
point2< T > operator - (const point2< T > & p1 , const point2< T > & p2 )
{
    return point2< T >( p1.x() - p2.x() , p1.y() - p2.y() );
}





template< typename T >
inline
point2< T > operator - (const point2< T > & p)
{
    return point2< T >( - p.x() , - p.y() );
}






template< typename T >
inline
point2< T > operator * (const point2< T > & p , int s)
{
    return point2< T >( s*p.x() , s*p.y() );
}
template< typename T >
inline
point2< T > operator * (const point2< T > & p , float s)
{
    return point2< T >( s*p.x() , s*p.y() );
}
template< typename T >
inline
point2< T > operator * (const point2< T > & p , double s)
{
    return point2< T >( s*p.x() , s*p.y() );
}






template< typename T >
inline
point2< T > operator / (const point2< T > & p , int s)
{
    return point2< T >( p.x()/s , p.y()/s );
}
template< typename T >
inline
point2< T > operator / (const point2< T > & p , float s)
{
    return point2< T >( p.x()/s , p.y()/s );
}
template< typename T >
inline
point2< T > operator / (const point2< T > & p , double s)
{
    return point2< T >( p.x()/s , p.y()/s );
}







template< typename T >
inline
point2< T > operator * ( int s , const point2< T > & p )
{
    return point2< T >( s*p.x() , s*p.y() );
}
template< typename T >
inline
point2< T > operator * ( float s , const point2< T > & p )
{
    return point2< T >( s*p.x() , s*p.y() );
}
template< typename T >
inline
point2< T > operator * ( double s , const point2< T > & p )
{
    return point2< T >( s*p.x() , s*p.y() );
}





template< typename T >
inline
T operator * (const point2< T > & p1 , const point2< T > & p2)
{
    return p1.x() * p2.x() + p1.y() * p2.y();
}





typedef point2< float >    point2f;
typedef point2< double >   point2d;








template< typename T >
class BBOX2
{
public:
    point2<T> bb,BB;

    BBOX2() { clear(); }

    void clear()
    {
        bb[0] = FLT_MAX;
        BB[0] = -FLT_MAX;
    }
    bool isCleared() const { return bb[0] <= BB[0]; }

    template< class point_t >
    void set( const point_t & p )
    {
        bb = point2<T>(p[0],p[1]);
        BB = point2<T>(p[0],p[1]);
    }

    template< class point_t >
    void set( const point_t & pbb, const point_t & pBB )
    {
        bb = point2<T>(pbb[0],pbb[1]);
        BB = point2<T>(pBB[0],pBB[1]);
    }

    template< class point_t >
    void add( const point_t & p )
    {
        bb = point2<T>::min( bb,p );
        BB = point2<T>::max( BB,p );
    }

    void add( const BBOX2<T> & b )
    {
        bb = point2<T>::min( bb,b.bb );
        BB = point2<T>::max( BB,b.BB );
    }

    T squareDiagonal() const
    {
        return (BB - bb).sqrnorm();
    }
    inline
    T diagonal() const
    {
        return sqrt( (BB - bb).sqrnorm() );
    }
    inline
    T radius() const
    {
        return diagonal() / 2.0;
    }
    inline
    T squareRadius() const
    {
        return squareDiagonal() / 4.0;
    }

    inline
    char getLargestExtent() const
    {
        if(  BB[0] - bb[0]  >  BB[1] - bb[1]  )
        {
            return 0;
        }
        else
        {
            return 1;
        }
    }

    inline
    void splitAlongAxis( char axis , T value , BBOX2<T> & bbox1 , BBOX2<T> & bbox2 )
    {
        // for safety:
        value = std::max<T>( std::min<T>( value , BB[axis] ) , bb[axis] );
        point2<T> BB1 = BB;
        BB1[axis] = value;
        point2<T> bb2 = bb;
        bb2[axis] = value;
        bbox1.set( bb , BB1 );
        bbox2.set( bb2 , BB );
    }
};

typedef BBOX2< float >    BBOX2f;
typedef BBOX2< double >   BBOX2d;






template< typename T >
class mat22
{
public:
    typedef T   type_t;

    mat22<T>()
    {
        vals[0] = 0;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 0;
    }
    mat22<T>( T v1 , T v2 , T v3 , T v4 )
    {
        vals[0] = v1;
        vals[1] = v2;
        vals[2] = v3;
        vals[3] = v4;
    }

    void set( const mat22<T> & m )
    {
        vals[0] = m(0);
        vals[1] = m(1);
        vals[2] = m(2);
        vals[3] = m(3);
    }

    static
    mat22<T> Id()
    {
        return mat22<T>(1,0,0,1);
    }
    static
    mat22<T> Zero()
    {
        return mat22<T>(0,0,0,0);
    }
    void setZero()
    {
        vals[0] = 0;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 0;
    }
    void setId()
    {
        vals[0] = 1;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 1;
    }
    void setRotation(T angle)
    {
        T cangle = std::cos(angle);
        T sangle = std::sin(angle);
        vals[0] = cangle;
        vals[1] = -sangle;
        vals[2] = sangle;
        vals[3] = cangle;
    }

    mat22<T> RPi_2()
    {
        return mat22<T>(0,-1,1,0);
    }

    void transpose()
    {
        T xy = vals[1];
        vals[1] = vals[2];
        vals[2] = xy;
    }
    mat22<T> getTranspose() const
    {
        return mat22<T>(vals[0],vals[2],vals[1],vals[3]);
    }

    T operator () (unsigned int i , unsigned int j) const
    {
        return vals[2*i + j];
    }

    T operator () (unsigned int v) const
    {
        return vals[v];
    }
    T & operator () (unsigned int i , unsigned int j)
    {
        return vals[2*i + j];
    }

    T & operator () (unsigned int v)
    {
        return vals[v];
    }


    template< class point_t >
    point_t getRow( unsigned int i )
    {
        return point_t( vals[2*i] , vals[2*i+1] );
    }
    template< class point_t >
    void setRow( unsigned int i , const point_t & p)
    {
        vals[2*i]   = p.x();
        vals[2*i+1] = p.y();
    }

    template< class point_t >
    point_t getCol(unsigned int j)
    {
        return point_t( vals[j] , vals[2+j] );
    }
    template< class point_t >
    void setCol(unsigned int j , const point_t & p)
    {
        vals[j]   = p.x();
        vals[2+j] = p.y();
    }

    template< class point_t >
    inline static mat22<T> getFromCols(const point_t & c1 , const point_t & c2 )
    {
        // 0 1
        // 2 3
        return mat22<T>( c1[0] , c2[0] ,
                c1[1] , c2[1]  );
    }
    template< class point_t >
    inline static mat22<T> getFromRows(const point_t & r1 , const point_t & r2 )
    {
        // 0 1
        // 2 3
        return mat22<T>( r1[0] , r1[1] ,
                r2[0] , r2[1] );
    }

    inline
    T trace() const {
        return vals[0] + vals[3];
    }

    inline
    T determinant() const
    {
        return vals[0] * vals[3] - vals[1] * vals[2];
    }

    void diagonalizeSymmetric( T & sx , T & sy , point2<T> & vx , point2<T> & vy )
    {
        T bb = vals[1]*vals[1] , aa = vals[0]*vals[0] , cc = vals[3]*vals[3];
        if( bb < 0.00001 * aa  &&  bb < 0.00001 * cc )
        {
            sx = vals[0];
            vx = point2<T>(1,0);
            sy = vals[3];
            vy = point2<T>(0,1);
            return;
        }
        T Trace = vals[0]+vals[3];
        T sqrtDelta = sqrt( ( vals[0]-vals[3] ) * ( vals[0]-vals[3] ) + 4.0 * vals[1]*vals[1] );
        sx = (Trace + sqrtDelta)/2.0;
        sy = (Trace - sqrtDelta)/2.0;
        T b = vals[1];
        {
            T aT = vals[0] - sx;
            T cT = vals[3] - sx;
            //T factor1 = -( aT*cT + b*b ) / (2.0 * b * cT);
            //T factor2 = -( aT*cT + b*b ) / (2.0 * b * aT);
            if( aT*aT  <  cT*cT )
            {
                // return estimate with x = 1 and y = -( aT*cT + b*b ) / (2.0 * b * cT) * x:
                vx[0] = 1.0;
                vx[1] = -( aT*cT + b*b ) / (2.0 * b * cT);
                vx.normalize();
            }
            else
            {
                // return estimate with y = 1 and x = -( aT*cT + b*b ) / (2.0 * b * aT) * y:
                vx[1] = 1.0;
                vx[0] = -( aT*cT + b*b ) / (2.0 * b * aT);
                vx.normalize();
            }
        }
        {
            T aT = vals[0] - sy;
            T cT = vals[3] - sy;
            //T factor1 = -( aT*cT + b*b ) / (2.0 * b * cT);
            //T factor2 = -( aT*cT + b*b ) / (2.0 * b * aT);
            if( aT*aT  <  cT*cT )
            {
                // return estimate with x = 1 and y = -( aT*cT + b*b ) / (2.0 * b * cT) * x:
                vy[0] = 1.0;
                vy[1] = -( aT*cT + b*b ) / (2.0 * b * cT);
                vy.normalize();
            }
            else
            {
                // return estimate with y = 1 and x = -( aT*cT + b*b ) / (2.0 * b * aT) * y:
                vy[1] = 1.0;
                vy[0] = -( aT*cT + b*b ) / (2.0 * b * aT);
                vy.normalize();
            }
        }
    }

#ifdef __USE_GSL_FOR_MAT22
    void SVD( mat22<T> & U , T & sx , T & sy , mat22<T> & V )
    {
        gsl_matrix * u = gsl_matrix_alloc(2,2);
        for(unsigned int i = 0 ; i < 4; ++i)
            u->data[i] = vals[i];
        gsl_matrix * v = gsl_matrix_alloc(2,2);
        gsl_vector * s = gsl_vector_alloc(2);
        gsl_vector * work = gsl_vector_alloc(2);

        gsl_linalg_SV_decomp (u,
                              v,
                              s,
                              work);

        sx = s->data[0];
        sy = s->data[1];
        U(0) = u->data[0];
        U(1) = u->data[1];
        U(2) = u->data[2];
        U(3) = u->data[3];
        V(0) = v->data[0];
        V(1) = v->data[1];
        V(2) = v->data[2];
        V(3) = v->data[3];

        gsl_matrix_free(u);
        gsl_matrix_free(v);
        gsl_vector_free(s);
        gsl_vector_free(work);

        // a transformation T is given as R.B.S.Bt, R = rotation , B = local basis (rotation matrix), S = scales in the basis B
        // it can be obtained from the svd decomposition of T = U Sigma Vt :
        // B = V
        // S = Sigma
        // R = U.Vt
    }

    mat22<T> getRotationalPart()
    {
        mat22<T> U,V;
        T sx,sy;
        SVD(U,sx,sy,V);
        V.transpose();
        const mat22<T> & res = U*V;
        if( res.determinant() < 0 )
        {
            U(0,0) = -U(0,0);
            U(0,1) = -U(0,1);
            return U*V;
        }
        // else
        return res;
    }

    void setRotation()
    {
        mat22<T> U,V;
        T sx,sy;
        SVD(U,sx,sy,V);
        V.transpose();
        const mat22<T> & res = U*V;
        if( res.determinant() < 0 )
        {
            U(0,0) = -U(0,0);
            U(0,1) = -U(0,1);
            set(U*V);
            return;
        }
        // else
        set(res);
    }

    void setSimilarity()
    {
        mat22<T> U,V;
        T sx,sy;
        SVD(U,sx,sy,V);
        mat22<T> S((sx+sy)/2,0,0,(sx+sy)/2);
        V.transpose();
        const mat22<T> & res = U*S*V;
        if( res.determinant() < 0 )
        {
            U(0,0) = -U(0,0);
            U(0,1) = -U(0,1);
            set(U* S * V);
            return;
        }
        // else
        set(res);
    }

#endif

    void operator += (const mat22<T> & m)
    {
        vals[0] += m(0);
        vals[1] += m(1);
        vals[2] += m(2);
        vals[3] += m(3);
    }
    void operator -= (const mat22<T> & m)
    {
        vals[0] -= m(0);
        vals[1] -= m(1);
        vals[2] -= m(2);
        vals[3] -= m(3);
    }
    void operator /= (double t)
    {
        vals[0] /= t;
        vals[1] /= t;
        vals[2] /= t;
        vals[3] /= t;
    }

    T sqrnorm()
    {
        return vals[0]*vals[0] + vals[1]*vals[1] + vals[2]*vals[2] + vals[3]*vals[3];
    }

    template< class point_2D_t >
    point_2D_t inverse_vector( const point_2D_t & v ) const
    {
        T det = determinant();
        return point_2D_t( compute_determinant_with_c0_replaced(v[0],v[1])/det ,
                compute_determinant_with_c1_replaced(v[0],v[1])/det );
    }

    mat22<T> getInverse() const {
        T det = determinant();
        return mat22<T>( vals[3]/det , - vals[1]/det , - vals[2]/det , vals[0]/det );
    }



    template< class point_t >
    inline static
    mat22<T> tensor( const point_t & p1 , const point_t & p2 )
    {
        return mat22<T>(
                    p1.x()*p2.x() , p1.x()*p2.y() ,
                    p1.y()*p2.x() , p1.y()*p2.y() );
    }


    static
    mat22<T> getRotationFromAngle( double u ) {
        T cosU = cos(u);
        T sinU = sin(u);
        return mat22<T>( cosU , -sinU , sinU , cosU );
    }


private:
    T vals[4];
    // will be noted as :
    // 0 1
    // 2 3




    inline
    T compute_determinant_with_c0_replaced(T x , T y) const
    {
        return x * vals[3] - vals[1] * y;
    }

    inline
    T compute_determinant_with_c1_replaced(T x , T y) const
    {
        return vals[0] * y - x * vals[2];
    }




};

template< class T >
mat22<T> operator + (const mat22<T> & m1 , const mat22<T> & m2)
{
    return mat22<T>( m1(0)+m2(0) , m1(1)+m2(1) , m1(2)+m2(2) , m1(3)+m2(3) );
}
template< class T >
mat22<T> operator - (const mat22<T> & m1 , const mat22<T> & m2)
{
    return mat22<T>( m1(0)-m2(0) , m1(1)-m2(1) , m1(2)-m2(2) , m1(3)-m2(3) );
}




template< class T >
mat22<T> operator * (int s , const mat22<T> & m)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}
template< class T >
mat22<T> operator * (float s , const mat22<T> & m)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}
template< class T >
mat22<T> operator * (double s , const mat22<T> & m)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}


template< class T >
mat22<T> operator * (const mat22<T> & m , int s)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}
template< class T >
mat22<T> operator * (const mat22<T> & m , float s)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}
template< class T >
mat22<T> operator * (const mat22<T> & m , double s)
{
    return mat22<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) );
}


template< class T >
mat22<T> operator / (const mat22<T> & m , int s)
{
    return mat22<T>( m(0)/s , m(1)/s , m(2)/s , m(3)/s );
}
template< class T >
mat22<T> operator / (const mat22<T> & m , float s)
{
    return mat22<T>( m(0)/s , m(1)/s , m(2)/s , m(3)/s );
}
template< class T >
mat22<T> operator / (const mat22<T> & m , double s)
{
    return mat22<T>( m(0)/s , m(1)/s , m(2)/s , m(3)/s );
}



template< typename T >
point2<T> operator * (const mat22<T> & m , const point2<T> & p) // computes m.p
{
    return point2<T>(
                m(0,0)*p.x() + m(0,1)*p.y() ,
                m(1,0)*p.x() + m(1,1)*p.y() );
}
template< typename T >
point2<T> operator * (const point2<T> & p , const mat22<T> & m) // computes p^t . m = (m^t . p)^t
{
    return point2<T>(
                m(0,0)*p.x() + m(1,0)*p.y() ,
                m(0,1)*p.x() + m(1,1)*p.y() );
}

template< typename T >
mat22<T> operator * (const mat22<T> & m1 , const mat22<T> & m2)
{
    return mat22<T>( m1(0,0)*m2(0,0) + m1(0,1)*m2(1,0) , m1(0,0)*m2(0,1) + m1(0,1)*m2(1,1),
                     m1(1,0)*m2(0,0) + m1(1,1)*m2(1,0) , m1(1,0)*m2(0,1) + m1(1,1)*m2(1,1));
}

template< typename T >
mat22<T> tensor( const point2<T> & p1 , const point2<T> & p2 )
{
    return mat22<T>(p1.x()*p2.x() , p1.x()*p2.y(),
                    p1.y()*p2.x() , p1.y()*p2.y());
}

typedef mat22< float >  mat22f;
typedef mat22< double > mat22d;





















template< typename T >
class mat222
{
public:
    typedef T   type_t;

    mat222<T>()
    {
        vals[0] = 0;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 0;
        vals[4] = 0;
        vals[5] = 0;
        vals[6] = 0;
        vals[7] = 0;
    }
    mat222<T>( T v1 , T v2 , T v3 , T v4 , T v5 , T v6 , T v7 , T v8 )
    {
        vals[0] = v1;
        vals[1] = v2;
        vals[2] = v3;
        vals[3] = v4;
        vals[4] = v5;
        vals[5] = v6;
        vals[6] = v7;
        vals[7] = v8;
    }
    mat222<T>( const mat22<T> & m , const mat22<T> & m2 )
    {
        vals[0] = m(0);
        vals[1] = m(1);
        vals[2] = m(2);
        vals[3] = m(3);
        vals[4] = m2(0);
        vals[5] = m2(1);
        vals[6] = m2(2);
        vals[7] = m2(3);
    }

    void set( const mat222<T> & m )
    {
        vals[0] = m(0);
        vals[1] = m(1);
        vals[2] = m(2);
        vals[3] = m(3);
        vals[4] = m(4);
        vals[5] = m(5);
        vals[6] = m(6);
        vals[7] = m(7);
    }
    void set( const mat22<T> & m , const mat22<T> & m2 )
    {
        vals[0] = m(0);
        vals[1] = m(1);
        vals[2] = m(2);
        vals[3] = m(3);
        vals[4] = m2(0);
        vals[5] = m2(1);
        vals[6] = m2(2);
        vals[7] = m2(3);
    }


    mat222<T> Zero()
    {
        return mat222<T>(0,0,0,0,0,0,0,0);
    }
    void setZero()
    {
        vals[0] = 0;
        vals[1] = 0;
        vals[2] = 0;
        vals[3] = 0;
        vals[4] = 0;
        vals[5] = 0;
        vals[6] = 0;
        vals[7] = 0;
    }




    T operator () (unsigned int i , unsigned int j , unsigned int k ) const
    {
        return vals[4*i + 2*j + k];
    }

    T operator () (unsigned int v) const
    {
        return vals[v];
    }
    T & operator () (unsigned int i , unsigned int j , unsigned int k)
    {
        return vals[4*i + 2*j + k];
    }

    T & operator () (unsigned int v)
    {
        return vals[v];
    }



    void operator += (const mat222<T> & m)
    {
        vals[0] += m(0);
        vals[1] += m(1);
        vals[2] += m(2);
        vals[3] += m(3);
        vals[4] += m(4);
        vals[5] += m(5);
        vals[6] += m(6);
        vals[7] += m(7);
    }
    void operator -= (const mat222<T> & m)
    {
        vals[0] -= m(0);
        vals[1] -= m(1);
        vals[2] -= m(2);
        vals[3] -= m(3);
        vals[4] -= m(4);
        vals[5] -= m(5);
        vals[6] -= m(6);
        vals[7] -= m(7);
    }
    void operator /= (double t)
    {
        vals[0] /= t;
        vals[1] /= t;
        vals[2] /= t;
        vals[3] /= t;
        vals[4] /= t;
        vals[5] /= t;
        vals[6] /= t;
        vals[7] /= t;
    }
    void operator *= (double t)
    {
        vals[0] *= t;
        vals[1] *= t;
        vals[2] *= t;
        vals[3] *= t;
        vals[4] *= t;
        vals[5] *= t;
        vals[6] *= t;
        vals[7] *= t;
    }

    T sqrnorm()
    {
        return vals[0]*vals[0] + vals[1]*vals[1] + vals[2]*vals[2] + vals[3]*vals[3]
                + vals[4]*vals[4] + vals[5]*vals[5] + vals[6]*vals[6] + vals[7]*vals[7];
    }


    mat22<T> layer( unsigned int l ) const
    {
        return mat22<T>( vals[4*l] , vals[4*l+1] , vals[4*l+2] , vals[4*l+3] );
    }

    mat222<T> inOtherBasis( const mat22<T> & basisChange )
    {
        // this is done so that this(x,y,z) = this.inOtherBasis(basisChange)( basisChange^tx , basisChange^tx , basisChange^tz ) for all x,y,z:
        const mat22<T> & basisChangeT = basisChange.getTranspose();
        mat22<T> layer1 = basisChangeT * layer(0) * basisChange;
        mat22<T> layer2 = basisChangeT * layer(1) * basisChange;
        return mat222<T>( layer1*basisChange(0,0) + layer2*basisChange(1,0)  ,  layer1*basisChange(0,1) + layer2*basisChange(1,1) );
    }

private:
    T vals[8];
    // will be noted as :
    // 0 1
    // 2 3 (first layer)
    // 4 5
    // 6 7 (second layer)
};






template< typename T >
mat22<T> operator * (const mat222<T> & m , const point2<T> & p)
{
    return mat22<T>( m(0,0,0)*p[0] + m(1,0,0)*p[1] , m(0,0,1)*p[0] + m(1,0,1)*p[1],
            m(0,1,0)*p[0] + m(1,1,0)*p[1] , m(0,1,1)*p[0] + m(1,1,1)*p[1]);
}

template< typename T >
T tensorProduct( const mat222<T> & m , const point2<T> & p1 , const point2<T> & p2 , const point2<T> & p3 )
{
    return point2<T>::dot((m*p1)*p3 , p2);
    //this should return  p2^t . (m*p1) . p3
}


template< class T >
mat222<T> operator * (int s , const mat222<T> & m)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}
template< class T >
mat222<T> operator * (float s , const mat222<T> & m)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}
template< class T >
mat222<T> operator * (double s , const mat222<T> & m)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}


template< class T >
mat222<T> operator * (const mat222<T> & m , int s)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}
template< class T >
mat222<T> operator * (const mat222<T> & m , float s)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}
template< class T >
mat222<T> operator * (const mat222<T> & m , double s)
{
    return mat222<T>( s*m(0) , s*m(1) , s*m(2) , s*m(3) , s*m(4) , s*m(5) , s*m(6) , s*m(7) );
}


typedef mat222< float >  mat222f;
typedef mat222< double > mat222d;















#endif
