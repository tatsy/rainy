#ifndef _SPICA_QBVH_ACCEL_H_
#define _SPICA_QBVH_ACCEL_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef SPICA_QBVH_ACCEL_EXPORT
        #define SPICA_QBVH_ACCEL_DLL __declspec(dllexport)
    #else
        #define SPICA_QBVH_ACCEL_DLL __declspec(dllimport)
    #endif
#else
    #define SPICA_QBVH_ACCEL_DLL
#endif

#include <xmmintrin.h>

#include "../utils/uncopyable.h"
#include "accel_base.h"

namespace spica {

    class QBVHAccel : public AccelBase {
    private:
        struct QBVHNode : public Uncopyable {
            __m128 childBoxes[2][3];  // [min-max][x-y-z]
            QBVHNode* children[4];    // Child nodes
            Triangle* triangles;
            int  numTriangles;
            char sepAxes[3];          // top-left-right
            bool isLeaf;

            QBVHNode()
                : childBoxes()
                , children()
                , triangles(NULL)
                , numTriangles(0)
                , sepAxes()
                , isLeaf(false)
            {
            }

            ~QBVHNode()
            {
                delete[] triangles;
            }
        };

        static const int _maxNodeSize = 3;
        QBVHNode* _root;
        unsigned int* _numCopies;

    public:
        QBVHAccel();
        QBVHAccel(const QBVHAccel& qbvh);
        ~QBVHAccel();

        QBVHAccel& operator=(const QBVHAccel& qbvh);

        void construct(const std::vector<Triangle>& triangles);

        bool intersect(const Ray& ray, Hitpoint* hitpoint) const;

    private:
        void release();
        void deleteNode(QBVHNode* node);
        
        QBVHNode* constructRec(std::vector<Triangle>& triangles, int dim);

        static void splitVector(const std::vector<Triangle>& triangles, int dim, std::vector<Triangle>* left, std::vector<Triangle>* right);
    };

}

#endif  // _SPICA_QBVH_ACCEL_H_
