/* 
 * File:   Ray.h
 * Author: jbarbosa
 *
 * Created on March 28, 2014, 1:29 PM
 */

#ifndef GVT_RENDER_ACTOR_RAY_H
#define	GVT_RENDER_ACTOR_RAY_H
#include <gvt/core/Debug.h>
#include <gvt/core/Math.h>
#include <gvt/render/data/scene/ColorAccumulator.h>

#include <limits>
#include <boost/aligned_storage.hpp>
#include <boost/container/set.hpp>
#include <boost/container/vector.hpp>
#include <boost/pool/pool.hpp>
#include <boost/pool/pool_alloc.hpp>
#include <boost/smart_ptr.hpp>

#include <vector>

 namespace gvt {
    namespace render {
        namespace actor {
            typedef struct intersection 
            {
                int domain;
                float d;

                intersection(int dom) : domain(dom),d(FLT_MAX) {}
                intersection(int dom, float dist) : domain(dom),d(dist) {}

                operator int(){return domain;}
                operator float(){return d;}
                friend inline bool operator == (const intersection& lhs, const intersection& rhs ) 
                { return (lhs.d == rhs.d) && (lhs.d == rhs.d); } 
                friend inline bool operator < (const intersection& lhs, const intersection& rhs ) 
                { return (lhs.d < rhs.d) || ((lhs.d==rhs.d) && (lhs.domain < rhs.domain)); } 

            } isecDom;

            typedef boost::container::vector<isecDom> isecDomList;


            class Ray 
            {      
            public:

                enum RayType 
                {
                    PRIMARY,
                    SHADOW,
                    SECONDARY
                };



            //GVT_CONVERTABLE_OBJ(gvt::render::data::primitives::Ray);

                Ray(gvt::core::math::Point4f origin = gvt::core::math::Point4f(0, 0, 0, 1), 
                    gvt::core::math::Vector4f direction = gvt::core::math::Vector4f(0, 0, 0, 0), 
                    float contribution = 1.f, 
                    RayType type = PRIMARY, 
                    int depth = 10
		    );
                Ray(Ray &ray, gvt::core::math::AffineTransformMatrix<float> &m);
                Ray(const Ray& orig);
                Ray(const unsigned char* buf);

                virtual ~Ray();

                void setDirection(gvt::core::math::Vector4f dir);
                void setDirection(double *dir);
                void setDirection(float *dir);

                int packedSize();

                int pack(unsigned char* buffer);

                friend std::ostream& operator<<(std::ostream& stream, Ray const& ray) 
                {
                    stream << ray.origin << "-->" << ray.direction << "[" << ray.type << "]";
                    return stream;
                }

                mutable gvt::core::math::Point4f    origin;
                mutable gvt::core::math::Vector4f   direction;
                mutable gvt::core::math::Vector4f   inverseDirection;
    //            mutable int sign[3];


                int id; ///<! index into framebuffer
                int depth; ///<! sample rate 
//            float r; ///<! sample rate
                float w; ///<! weight of image contribution
                mutable float t;
                mutable float t_min;
                mutable float t_max;
                GVT_COLOR_ACCUM color;
                isecDomList domains;
                int type;

                const static float RAY_EPSILON;
            
            protected:

            };

            typedef std::vector< Ray, boost::pool_allocator<Ray> > RayVector;
        }
    }
}

#endif	/* GVT_RENDER_ACTOR_RAY_H */
