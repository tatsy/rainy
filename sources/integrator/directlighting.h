#ifdef _MSC_VER
#pragma once
#endif

#ifndef _SPICA_DIRECTLIGHTING_H_
#define _SPICA_DIRECTLIGHTING_H_

#include "integrator.h"

namespace spica {

class SPICA_EXPORTS DirectLightingIntegrator : public SamplerIntegrator {
public:
    // Public methods
    DirectLightingIntegrator(const std::shared_ptr<const Camera>& camera,
                             const std::shared_ptr<Sampler>& sampler);

    Spectrum Li(const Scene& scene,
                const RenderParameters& params,
                const Ray& ray,
                Sampler& sampler,
                MemoryArena& arena,
                int depth = 0) const override;
};

}  // namespace spica

#endif  // _SPICA_DIRECTLIGHTING_H_