#pragma once

// The Geometry class specifies the geometry of a given mesh, i.e.,
// the location of its vertices in space, where "in space" could mean
// a variety of things (in 3D, in the plane, on the sphere).  It is
// also used to answer queries about that geometry, e.g., what is the
// area of a given triangle or the length of a given edge.
//
// There are two principal ways to evaluate a given geometric quantity:
//
//    1. via methods that evaluate attributes directly, or
//    2. by caching attributes in a Data vector (Geometry::get*())
//
// The former is useful in scenarios where the geometry is constantly
// changing (e.g., when smoothing or editing a surface), hence cached
// values quickly become stale and there is little gained by storing
// them.  The latter is useful in scenarios where the geometry remains
// fixed throughout the entire algorithm (e.g., flattening or
// remeshing), hence there is no reason to repeatedly recompute values.
// Caching may also simplify syntax and reduce function call overhead.
// Finally, caching only the attributes that are needed (rather than
// all possible attributes) reduces memory usage.
//
// Example usage:
//
//    // compute the total surface area directly
//    double sum = 0.;
//    for(Face f : mesh->faces)
//    {
//       sum += geometry->area(f);
//    }
//
//    // compute the total surface area using cached values
//    FaceData<double> area;
//    geometry->getFaceAreas(area); // fill the cache
//    double sum = 0.;
//    for(Face f : mesh->faces())
//    {
//       sum += area[f];
//    }
//
// Note that the Geometry class also includes convenience methods for
// computing standard quantities like total surface area, etc.
//

// TODO: Right now many parts of Geometry implicitly assume that all real faces
// are triangular.
//       If you want to store non-triangular faces, be careful.


#include "geometrycentral/surface/geometry_cache.h"
#include "geometrycentral/surface/halfedge_mesh.h"
#include "geometrycentral/utilities/unit_vector3.h"
#include "geometrycentral/utilities/vector2.h"
#include "geometrycentral/utilities/vector3.h"

#include <iostream>

namespace geometrycentral {
namespace surface {

// Possible geometry types
typedef Vector2 Planar; // TODO change to Complex
typedef Vector3 Euclidean;
typedef UnitVector3 Spherical;
// TODO Hyperbolic

template <class T>
class Geometry : public surface::VertexData<T> {

public:
  Geometry(surface::HalfedgeMesh& mesh_) : VertexData<T>(mesh_), mesh(mesh_), cache(this), p(*this) {}

  surface::HalfedgeMesh* getMesh(void); // Returns a pointer to the domain

  T position(Vertex p) const;
  double dualArea(Vertex v);
  double volume(Vertex v);          // always equal to 1
  double angleDefect(Vertex v);     // 2π minus sum of incident angles
  Vector3 normal(Vertex v);         // area-weighted average of incident face normals
  Vector3 boundaryNormal(Vertex v); // length-weighted normal vector to the
                                    // two neighboring edges
  Vector3 projectToTangentSpace(Vertex v, const Vector3& inVec);
  Complex tangentVectorToComplexAngle(Vertex v, const Vector3& inVec);
  Vector3 complexAngleToTangentVector(Vertex v, Complex inAngle);
  Complex principalDirection(Vertex v); // the 2-symmetric complex vector aligned with k1

  // Edge attributes
  // --- Primal ---
  T midpoint(Edge e);
  double length(Edge e);
  double cotanWeight(Edge e); // **triangles only**
  double dihedralAngle(Edge e);

  // Face attributes
  // --- Primal ---
  double area(Face f);
  Vector3 normal(Face f);
  Vector3 areaVector(Face f);
  T barycenter(Face f);
  T circumcenter(Face f);

  // Halfedge attributes
  T vector(Halfedge h);
  double angle(Halfedge h);             // **triangles only**
  double angle(Corner c);               // **triangles only**
  double angularCoordinate(Halfedge h); // **triangles only** Measured CCW
                                        // against the tail vertex's
                                        // arbitrary halfedge
  double cotan(Halfedge h);             // **triangles only**

  // Global attributes
  double totalArea(void); // Total surface area (assuming all triangles)
  T center(void);         // Center of mass (assuming constant density)
  void boundingBox(T& bboxMin,
                   T& bboxMax); // Corners of axis-aligned bounding box
  T extent(void);               // Width, height, and depth of the axis-aligned bounding box
  double lengthScale(void);     // A length scale for the geometry

  // Methods for caching current attributes
  void getVertexPositions(VertexData<T>& vertexPosition);
  void getVertexNormals(VertexData<Vector3>& vertexNormal);
  void getVertexAngleDefects(VertexData<double>& vertexAngleDefect);
  void getPrincipalDirections(VertexData<Complex>& principalDirections);
  void getPrincipalDirections(VertexData<Complex>& principalDirections, HalfedgeData<double>& angularCoordinates);

  void getEdgeLengths(EdgeData<double>& edgeLength);
  void getEdgeCotanWeights(EdgeData<double>& edgeCotanWeight);

  void getFaceAreas(FaceData<double>& faceArea);
  void getFaceNormals(FaceData<Vector3>& faceNormal);
  void getFaceBarycenters(FaceData<T>& faceBarycenter);

  void getHalfedgeVectors(HalfedgeData<T>& halfedgeVector);
  void getHalfedgeAngles(HalfedgeData<double>& halfedgeAngle);
  void getCornerAngles(CornerData<double>& cornerAngle);
  void getHalfedgeCotans(HalfedgeData<double>& halfedgeCotan);

  void getAngularCoordinates(HalfedgeData<double>& angularCoordinates);

  void normalize();
  std::vector<T> getVertexPositionList();

  // members
  surface::HalfedgeMesh& mesh;
  GeometryCache<T> cache;

protected:
  VertexData<T>& p; // convenience reference to "this"
};

} // namespace surface
} // namespace geometrycentral

#include "geometrycentral/surface/geometry.ipp"
