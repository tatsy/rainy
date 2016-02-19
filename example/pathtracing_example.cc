#include <cstdio>
#include <iostream>
#include <memory>

#include "../include/spica.h"
using namespace spica;

int main(int argc, char **argv) {
    const int width   = argc >= 2 ? atoi(argv[1]) : 400;
    const int height  = argc >= 3 ? atoi(argv[2]) : 300;
    const int samples = argc >= 4 ? atoi(argv[3]) : 32;

    std::cout << "--- uni-directional path tracing ---" << std::endl;
    std::cout << "    width: " << width   << std::endl;
    std::cout << "   height: " << height  << std::endl;
    std::cout << "  samples: " << samples << std::endl << std::endl;

    std::unique_ptr<Filter> boxFilter = std::make_unique<BoxFilter>(Vector2D(0.5, 0.5));
    auto film = std::make_unique<Film>(Point2i(width, height),
                                       boxFilter,
                                       kOutputDirectory + "pathtrace_%03d.png");

    RectF screen(-2.5, -2.5, 5.0, 5.0);
    double fov = PI / 24.0;

    Point3D  eye(0.0, 0.0, 5.0 / tan(fov / 2.0));
    Point3D  look(0.0, 0.0, 0.0);
    Vector3D up(0.0, 1.0, 0.0);

    double focal = std::abs((look - eye).z());
    double lensR = 0.2;

    Scene scene;
    std::shared_ptr<Camera> camera = std::make_shared<PerspectiveCamera>(
        Transform::lookAt(eye, look, up),
        screen, lensR, focal, fov, film.get());

    std::shared_ptr<Sampler> sampler = std::make_unique<Random>(0);

    cornellBox(&scene, nullptr, 0, 0);
    // kittenBox(&scene, &camera, width, height);
    // kittenEnvmap(&scene, &camera, width, height);
    
    Timer timer;
    timer.start();

    RenderParameters params(samples);
    params.bounceLimit(24);
    params.castPhotons(500000);
    params.saveFilenameFormat();

    PathIntegrator integr(camera, sampler);
    integr.render(scene, params);

    printf("Timer: %f sec\n", timer.stop());
}
