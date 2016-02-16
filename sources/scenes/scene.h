#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_SCENE_H_
#define _SPICA_SCENE_H_

#include <vector>
#include <memory>

#include "../core/common.h"
#include "../core/forward_decl.h"

#include "../core/primitive.h"
#include "../core/bound3d.h"
#include "../light/light.h"

namespace spica {

/**
 * Scene provides the interface for scene graph.
 */
class SPICA_EXPORTS Scene : private Uncopyable {
public:
    Scene();
    Scene(const std::shared_ptr<Primitive>& aggregate,
            const std::vector<std::shared_ptr<Light> >& lights);
    Scene(Scene&& scene);

    virtual ~Scene();

    Scene& operator=(Scene&& scene);

    bool intersect(Ray& ray, SurfaceInteraction* isect) const;

    inline const Bound3d& worldBound() const { return worldBound_; }

    inline const std::vector<std::shared_ptr<Light> >& lights() const {
        return lights_;
    }

private:
    std::shared_ptr<Primitive> aggregate_;
    std::vector<std::shared_ptr<Light> > lights_;
    Bound3d worldBound_;

};  // class Scene

}  // namespace spica

#endif  // _SPICA_SCENE_H_