#ifndef _SPICA_PHOTON_MAPPING_H_
#define _SPICA_PHOTON_MAPPING_H_

#if defined(_WIN32) || defined(__WIN32__)
    #ifdef SPICA_PHOTON_MAPPING_EXPORT
        #define SPICA_PHOTON_MAPPING_DLL __declspec(dllexport)
    #else
        #define SPICA_PHOTON_MAPPING_DLL __declspec(dllimport)
    #endif
#else
    #define SPICA_PHOTON_MAPPING_DLL
#endif

#include "../utils/uncopyable.h"
#include "../utils/vector3.h"
#include "../utils/kdtree.h"
#include "../renderer/material.h"
#include "../renderer/scene.h"
#include "../renderer/camera.h"

namespace spica {

    class Photon {
    private:
        Vector3 _position;
        Color _flux;
        Vector3 _direction;

    public:
        Photon();
        explicit Photon(const Vector3& position, const Color& flux, const Vector3& direction);
        Photon(const Photon& photon);
        ~Photon();

        Photon& operator=(const Photon& photon);

        inline Vector3 position()  const { return _position; }
        inline Color   flux()      const { return _flux; }
        inline Vector3 direction() const { return _direction; }

        struct PhotonComparator {
            int dim;
            explicit PhotonComparator(int d = 0)
                : dim(d)
            {
            }

            bool operator()(const Photon& p1, const Photon& p2) const {
                return p1.position().get(dim) < p2.position().get(dim);
            }
        };

        static PhotonComparator compare(int dim);
    };

    class PhotonMap : public Uncopyable {
    private:
        KdTree<Photon> _kdtree;

    public:
        PhotonMap();
        ~PhotonMap();

        void clear();
        void construct(const std::vector<Photon>& photons);

        void findKNN(const Vector3& position);
    };

    class SPICA_PHOTON_MAPPING_DLL PMRenderer {
    private:
        PhotonMap photonMap;

    public:
        PMRenderer();
        PMRenderer(const PMRenderer& renderer);
        ~PMRenderer();

        PMRenderer& operator=(const PMRenderer& renderer);

        int render(const Scene& scne, const Camera& camera, const Random& rng);

        void buildPM(const Scene& scene, const Camera& camera, const Random& rng, const int numPhotons);
    };

}

#endif  // _SPICA_PHOTON_MAPPING_H_
