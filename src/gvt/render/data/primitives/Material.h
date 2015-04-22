/* 
 * File:   Material.h
 * Author: jbarbosa
 *
 * Created on April 18, 2014, 3:07 PM
 */

#ifndef GVT_RENDER_DATA_PRIMITIVES_MATERIAL_H
#define	GVT_RENDER_DATA_PRIMITIVES_MATERIAL_H

#include <gvt/core/Math.h>
#include <gvt/render/actor/Ray.h>
#include <gvt/render/data/scene/Light.h>

#include <boost/container/vector.hpp>
#include <time.h>

 namespace gvt {
    namespace render {
        namespace data {
            namespace primitives {

                class Material {
                public:
                    Material();
                    Material(const Material& orig);
                    virtual ~Material();

                    virtual gvt::core::math::Vector4f shade(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, const gvt::render::data::scene::Light* lightSource);
                    virtual gvt::render::actor::RayVector ao(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);
                    virtual gvt::render::actor::RayVector secondary(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);
                    
                    gvt::core::math::Vector4f CosWeightedRandomHemisphereDirection2(gvt::core::math::Vector4f n) {
                        float Xi1 = (float) rand() / (float) RAND_MAX;
                        float Xi2 = (float) rand() / (float) RAND_MAX;

                        float theta = acos(sqrt(1.0 - Xi1));
                        float phi = 2.0 * 3.1415926535897932384626433832795 * Xi2;

                        float xs = sinf(theta) * cosf(phi);
                        float ys = cosf(theta);
                        float zs = sinf(theta) * sinf(phi);

                        gvt::core::math::Vector3f y(n);
                        gvt::core::math::Vector3f h = y;
                        if (fabs(h.x) <= fabs(h.y) && fabs(h.x) <= fabs(h.z))
                            h[0] = 1.0;
                        else if (fabs(h.y) <= fabs(h.x) && fabs(h.y) <= fabs(h.z))
                            h[1] = 1.0;
                        else
                            h[2] = 1.0;


                        gvt::core::math::Vector3f x = (h ^ y);
                        gvt::core::math::Vector3f z = (x ^ y);

                        gvt::core::math::Vector4f direction = x * xs + y * ys + z * zs;
                        return direction.normalize();
                    }
                protected:


                };

                class Lambert : public Material {
                public:
                    Lambert(const gvt::core::math::Vector4f& kd = gvt::core::math::Vector4f());
                    Lambert(const Lambert& orig);
                    virtual ~Lambert();

                    virtual gvt::core::math::Vector4f shade(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, const gvt::render::data::scene::Light* lightSource);
                    virtual gvt::render::actor::RayVector ao(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);
                    virtual gvt::render::actor::RayVector secundary(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);

                protected:

                    gvt::core::math::Vector4f kd;
                };

                class Phong : public Material {
                public:
                    Phong(const gvt::core::math::Vector4f& kd = gvt::core::math::Vector4f(), const gvt::core::math::Vector4f& ks = gvt::core::math::Vector4f(), const float& alpha = 1.f);
                    Phong(const Phong& orig);
                    virtual ~Phong();

                    virtual gvt::core::math::Vector4f shade(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, const gvt::render::data::scene::Light* lightSource);
                    virtual gvt::render::actor::RayVector ao(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);
                    virtual gvt::render::actor::RayVector secundary(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);

                protected:

                    gvt::core::math::Vector4f kd;
                    gvt::core::math::Vector4f ks;
                    float alpha;

                };

                class BlinnPhong : public Material {
                public:
                    BlinnPhong(const gvt::core::math::Vector4f& kd = gvt::core::math::Vector4f(), const gvt::core::math::Vector4f& ks = gvt::core::math::Vector4f(), const float& alpha = 1.f);
                    BlinnPhong(const BlinnPhong& orig);
                    virtual ~BlinnPhong();

                    virtual gvt::core::math::Vector4f shade(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, const gvt::render::data::scene::Light* lightSource);
                    virtual gvt::render::actor::RayVector ao(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);
                    virtual gvt::render::actor::RayVector secundary(const gvt::render::actor::Ray&  ray, const gvt::core::math::Vector4f& sufaceNormal, float samples);

                protected:

                    gvt::core::math::Vector4f kd;
                    gvt::core::math::Vector4f ks;
                    float alpha;

                };
            }
        }
    }
}

#endif	/* GVT_RENDER_DATA_PRIMITIVES_MATERIAL_H */
