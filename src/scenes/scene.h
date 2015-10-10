#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_SCENE_H_
#define _SPICA_SCENE_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef SPICA_SCENE_EXPORT
        #define SPICA_SCENE_DLL __declspec(dllexport)
    #else
        #define SPICA_SCENE_DLL __declspec(dllimport)
    #endif
#elif defined(linux) || defined(__linux)
    #define SPICA_SCENE_DLL
#endif

#include <vector>
#include <memory>
#include <type_traits>

#include "../core/common.h"
#include "../core/stack.h"
#include "../core/uncopyable.h"

#include "../math/vector3d.h"
#include "../shape/shape.h"
#include "../light/lighting.h"

#include "../bsdf/bsdf.h"
#include "../renderer/ray.h"

#include "triangle_data.h"

extern void* enabler;

namespace spica {

    class Camera;

    class SPICA_SCENE_DLL Scene {
    private:
        std::vector<VertexData>   _vertices;
        std::vector<TriangleData> _triangles;

        std::vector<unsigned int> _bsdfIds;
        std::vector<unsigned int> _lightIds;

        std::vector<BSDF>          _bsdfs;
        std::shared_ptr<AccelBase> _accel;

        Lighting  _lighting;
        AccelType _accelType;

        std::shared_ptr<Image> _texture;

    public:
        Scene();
        Scene(const Scene& scene);
        ~Scene();

        Scene& operator=(const Scene& scene);

        template <class T, typename std::enable_if<std::is_base_of<IShape, T>::value>::type *& = enabler>
        void addShape(const T& shape, const BSDF& bsdf);
        
        //! Set area light to the scene
        template <class T, typename std::enable_if<std::is_base_of<IShape, T>::value>::type *& = enabler>
        void setLight(const T& shape, const Color& emittance);

        //! Set environment map to the scene
        // @param[in] filename: name of the environment map image file
        // @param[in] camera: it's necessary for computing environment sphere
        void setEnvmap(const std::string& filename, const Camera& camera);

        //! Set enviroment map to the scene
        // @param[in] image: the image for enviroment map
        // @param[in] camera: it's necessary for computing environment sphere
        void setEnvmap(const Image& image, const Camera& camera);

        //! Compute bounding sphere
        Sphere boundingSphere(const Camera& camera) const;

        //! Return triangle (make new instance in the function)
        Triangle getTriangle(int id) const;
        
        const BSDF& getBsdf(int id) const;

        const Color& getReflectance(const Intersection& isect) const;

        // Get direct lightinf from specified direction
        Color directLight(const Vector3D& dir) const; 

        //! Sample vertex on the light. This method consumes three random numbers
        LightSample sampleLight(Stack<double>& rstack) const;

        //! area of light
        double lightArea() const;

        void clear();

        void setAccelType(AccelType accel);

        // Check specified triangle is light or not
        // This method is O(log N) implementation
        bool isLightCheck(int id) const;

        void computeAccelerator();

        // Call both "computeAccelerator" and "computeLightPdfs" 
        // to finalize the scene. If you update the scene, you should
        // call this function again.
        void finalize();

        bool intersect(const Ray& ray, Intersection& isect) const;

        inline bool isTextured(int triID) const {
            Assertion(triID >= 0 && triID < _triangles.size(), "Triangle index out of bounds!!");
            return _triangles[triID].isTextured();
        }

        inline int numTriangles() const {
            return static_cast<int>(_triangles.size());
        }

    private:
        //! Add triangles
        void addTriangles(const std::vector<Triangle>& tris);

        //! Add a BSDF
        // @param[in] bsdf: new BSDF to be added
        // @param[in] numTris: # of triangles added
        void addBsdf(const BSDF& bsdf, int numTris);
    };

}  // namespace spica

// Implementation for the template functions
#include "scene_detail.h"

#endif  // _SPICA_SCENE_H_
