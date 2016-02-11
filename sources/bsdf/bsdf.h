/**
 * @defgroup bsdf_module BSDF
 * @brief BSDF classes.
 */

#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_BSDF_H_
#define _SPICA_BSDF_H_

#include <memory>

#include "../math/vector3d.h"
#include "../core/point3d.h"
#include "../core/normal3d.h"
#include "../core/spectrum.h"

namespace spica {

    class AbstractBSDF;
    class BSSRDF;
    class SubsurfaceIntegrator;

    /** The BSDF types.
     *  @ingroup bsdf_module
     *  @brief Enumerator to specify the BSDF types.
     */
    enum class BsdfType : int {
        Bssrdf      = 0x0100,  /**< BSSRDF */
        Scatter     = 0x0200,  /**< Scattering surface */
        Dielectric  = 0x0400,  /**< Dielectric surface */

        None        = 0x0000,  /**< No type specified */
        Lambertian  = 0x0001 | Scatter,  /**< Lambertian reflection */     
        Specular    = 0x0002 | Dielectric,  /**< Specular reflection */
        PhongBrdf   = 0x0004 | Scatter,  /**< Phong's BRDF */
        Refractive  = 0x0008 | Dielectric,  /**< Refractive object */
    };

    inline BsdfType operator|(BsdfType t1, BsdfType t2) {
        return static_cast<BsdfType>((static_cast<int>(t1) | static_cast<int>(t2)));
    }

    inline bool operator&(BsdfType t1, BsdfType t2) {
        return (static_cast<int>(t1) & static_cast<int>(t2)) != 0;
    }

    /** BSDF (Bidirectional Scattering Distributioh Function).
     *  @ingroup bsdf_module
     */
    class SPICA_EXPORTS BSDF {
    private:
        std::unique_ptr<const AbstractBSDF> _ptr;
        std::unique_ptr<const BSSRDF>   _bssrdf;
        BsdfType _type;

    public:
        BSDF();
        BSDF(const BSDF& bsdf);
        BSDF(BSDF&& bsdf);
        ~BSDF();

        BSDF& operator=(const BSDF& bsdf);
        BSDF& operator=(BSDF&& bsdf);

        const Spectrum& reflectance() const;

        Spectrum sample(const Vector3D& in, const Normal& normal, 
                        const Point2D& rands, Vector3D* out, double* pdf) const;

        double pdf(const Vector3D& in, const Normal& normal, const Vector3D& out) const;

        Spectrum evalBSSRDF(const Vector3D& in, 
                            const Point& pos,
                            const Normal& normal,
                            const SubsurfaceIntegrator& integr,
                            double* refPdf) const;

        BsdfType type() const;
        void setBssrdf(const BSSRDF& bssrdf);

    private:
        BSDF(const AbstractBSDF* ptr, BsdfType type);

        // Friend classes
        friend class LambertianBRDF;
        friend class SpecularBRDF;
        friend class PhongBRDF;
        friend class RefractiveBSDF;
        friend class SubsurfaceIntegrator;
    };

}  // namespace spica

#endif  // _SPICA_BSDF_H_
