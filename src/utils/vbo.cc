#define SPICA_VBO_EXPORT
#include "vbo.h"

#include <typeinfo>
#include <string>

namespace spica {

    VBO::VBO()
        : _vertices()
        , _normals()
        , _colors()
        , _indices()
    {
    }

    VBO::VBO(const VBO& vbo)
        : _vertices()
        , _normals()
        , _colors()
        , _indices()
    {
        operator=(vbo);
    }

    VBO::~VBO()
    {
    }

    VBO& VBO::operator=(const VBO& vbo) {
        this->_vertices = vbo._vertices;
        this->_normals = vbo._normals;
        this->_colors = vbo._colors;
        this->_indices = vbo._indices;
        return *this;
    }

    void VBO::add(const IGeometry* p, const Color& color) {
        if (typeid(*p) == typeid(Quad)) {
            const Quad* quad = reinterpret_cast<const Quad*>(p);
            add(*quad, color);
        } else if (typeid(*p) == typeid(Sphere)) {
            const Sphere* sphere = reinterpret_cast<const Sphere*>(p);
            add(*sphere, color);
        } else if (typeid(*p) == typeid(Trimesh)) {
            const Trimesh* trimesh = reinterpret_cast<const Trimesh*>(p);
            add(*trimesh, color);
        } else if (typeid(*p) == typeid(Disk)) { 
            const Disk* disk = reinterpret_cast<const Disk*>(p);
            add(*disk, color);
        } else {
            const std::string typname = typeid(*p).name();
            Assertion(false, (typname + "is not supported").c_str());        
        }
    }

    void VBO::add(const Vector3D& v, const Vector3D& normal, const Color& color) {
        _vertices.push_back(v.x());
        _vertices.push_back(v.y());
        _vertices.push_back(v.z());
        _normals.push_back(normal.x());
        _normals.push_back(normal.y());
        _normals.push_back(normal.z());
        _colors.push_back(color.red());
        _colors.push_back(color.green());
        _colors.push_back(color.blue());
    }

    void VBO::add(const Quad& quad, const Color& color) {
        unsigned int idx = static_cast<unsigned int>(_vertices.size()) / 3;
        add(quad[0], quad.normal(), color);            
        add(quad[1], quad.normal(), color);            
        add(quad[2], quad.normal(), color);            
        add(quad[3], quad.normal(), color);
        _indices.push_back(idx);
        _indices.push_back(idx + 1);
        _indices.push_back(idx + 2);
        _indices.push_back(idx + 2);
        _indices.push_back(idx + 3);
        _indices.push_back(idx + 0);
    }

    void VBO::add(const Triangle& tri, const Color& color) {
        unsigned int idx = static_cast<unsigned int>(_vertices.size()) / 3;
        add(tri[0], tri.normal(), color);
        add(tri[1], tri.normal(), color);
        add(tri[2], tri.normal(), color);
        _indices.push_back(idx);
        _indices.push_back(idx + 1);
        _indices.push_back(idx + 2);
    }

    void VBO::add(const Sphere& sphere, const Color& color) {
        static const int nTheta = 32;
        static const int nPhi   = 64;

        for (int i = 0; i < nTheta; i++) {
            for (int j = 0; j < nPhi; j++) {
                double theta0 = PI * i / nTheta;
                double theta1 = PI * (i + 1) / nTheta; 
                double phi0 = 2.0 * PI * j / nPhi;
                double phi1 = 2.0 * PI * (j + 1) / nPhi;

                double st0 = sin(theta0);
                double st1 = sin(theta1);
                double ct0 = cos(theta0);
                double ct1 = cos(theta1);
                double sp0 = sin(phi0);
                double sp1 = sin(phi1);
                double cp0 = cos(phi0);
                double cp1 = cos(phi1);

                Vector3D v00 = sphere.center() + sphere.radius() * Vector3D(cp0 * st0, sp0 * st0, ct0);
                Vector3D v01 = sphere.center() + sphere.radius() * Vector3D(cp0 * st1, sp0 * st1, ct1);
                Vector3D v10 = sphere.center() + sphere.radius() * Vector3D(cp1 * st0, sp1 * st0, ct0);
                Vector3D v11 = sphere.center() + sphere.radius() * Vector3D(cp1 * st1, sp1 * st1, ct1);

                unsigned int idx = static_cast<unsigned int>(_vertices.size()) / 3;
                if (i == 0) {
                    add(v00, (v00 - sphere.center()).normalized(), color);
                    add(v01, (v01 - sphere.center()).normalized(), color);
                    add(v11, (v11 - sphere.center()).normalized(), color);
                    _indices.push_back(idx);
                    _indices.push_back(idx + 1);
                    _indices.push_back(idx + 2);
                } else if (i == nTheta - 1) {
                    add(v00, (v00 - sphere.center()).normalized(), color);
                    add(v11, (v11 - sphere.center()).normalized(), color);
                    add(v10, (v10 - sphere.center()).normalized(), color);
                    _indices.push_back(idx);
                    _indices.push_back(idx + 1);
                    _indices.push_back(idx + 2);
                } else {
                    add(v00, (v00 - sphere.center()).normalized(), color);            
                    add(v01, (v01 - sphere.center()).normalized(), color);
                    add(v11, (v11 - sphere.center()).normalized(), color);
                    add(v10, (v10 - sphere.center()).normalized(), color);
                    _indices.push_back(idx + 0);
                    _indices.push_back(idx + 1);
                    _indices.push_back(idx + 2);
                    _indices.push_back(idx + 0);
                    _indices.push_back(idx + 2);
                    _indices.push_back(idx + 3);
                }
            }
        }
    }

    void VBO::add(const Trimesh& trimesh) {
        const int numVerts = trimesh.numVerts();
        const int numFaces = trimesh.numFaces();

        std::vector<int> belongCount(numVerts, 0);
        std::vector<Vector3D> normals(numVerts, Vector3D(0.0, 0.0, 0.0));

        std::vector<Triplet> indices = trimesh.getIndices();
        for (int i = 0; i < numFaces; i++) {
            const Vector3D& normal = trimesh.getNormal(i);
            for (int j = 0; j < 3; j++) {
                const int vid = indices[i][j];
                normals[vid] += normal;
                belongCount[vid]++;
            }
        }

        for (int i = 0; i < numVerts; i++) {
            const Vector3D vNormal = normals[i] / belongCount[i];
            add(trimesh.getVertex(i), vNormal, trimesh.getColor(i));
        }

        const int prevSize = static_cast<int>(_indices.size());
        _indices.resize(prevSize + indices.size());

        for (size_t i = 0; i < indices.size(); i++) {
            for (int j = 0; j < 3; j++) {
                _indices[(i + prevSize) * 3 + j] = indices[i][j];
            }
        }
    }

    void VBO::add(const Trimesh& trimesh, const Color& color) {
        for (int i = 0; i < trimesh.numFaces(); i++) {
            Triangle tri = trimesh.getTriangle(i);
            add(tri, color);
        }
    }

    void VBO::add(const Disk& disk, const Color& color) {
        static const int ndiv = 64;

        Vector3D u, v, w;
        w = disk.normal();
        if (std::abs(w.x()) > EPS) {
            u = Vector3D(0.0, 1.0, 0.0).cross(w).normalized();
        } else {
            u = Vector3D(1.0, 0.0, 0.0).cross(w).normalized();
        }
        v = w.cross(u);

        for (int i = 0; i < ndiv; i++) {
            const double t1 = 2.0 * PI * i / ndiv;
            const double t2 = 2.0 * PI * (i + 1) / ndiv;
            const Vector3D v1 = disk.center() + disk.radius() * (u * cos(t1) + v * sin(t1));
            const Vector3D v2 = disk.center() + disk.radius() * (u * cos(t2) + v * sin(t2));
            Triangle tri = Triangle(disk.center(), v1, v2);
            add(tri, color);
        }
    }

}  // namespace spica
