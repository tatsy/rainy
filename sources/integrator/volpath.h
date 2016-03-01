#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_VOLPATH_H_
#define _SPICA_VOLPATH_H_

#include "../core/common.h"
#include "../core/forward_decl.h"

#include "pathtrace.h"

namespace spica {

class SPICA_EXPORTS VolPathIntegrator : public PathIntegrator {
public:
    VolPathIntegrator(std::shared_ptr<Camera>& camera,
                      std::shared_ptr<Sampler>& sampler);
    Spectrum Li(const Scene& scene, const RenderParameters& params,
                const Ray& ray, Sampler& sampler, MemoryArena& arena,
                int depth) const override;
};

}  // namespace spica


#endif  // _SPICA_VOLPATH_H_
