#include <emscripten/bind.h>
#include <emscripten/val.h>

#include <sstream>

#include "geometrycentral/surface/halfedge_mesh.h"
#include "geometrycentral/surface/polygon_soup_mesh.h"
#include "geometrycentral/utilities/vector2.h"
#include "geometrycentral/utilities/vector3.h"

using namespace emscripten;

#define ELEMENT_BOILERPLATE(Type) \
  .function("eq", optional_override([](const Type& self, const Type& other) { return self == other; })) \
  .function("neq", optional_override([](const Type& self, const Type& other) { return self != other; })) \
  .function("gt", optional_override([](const Type& self, const Type& other) { return self > other; })) \
  .function("geq", optional_override([](const Type& self, const Type& other) { return self >= other; })) \
  .function("lt", optional_override([](const Type& self, const Type& other) { return self < other; })) \
  .function("leq", optional_override([](const Type& self, const Type& other) { return self <= other; })) \
  .function("toString", optional_override([](const Type& self) { \
    std::ostringstream stream; \
    stream << self; \
    return stream.str(); \
  })) \
  .function("getMesh", &Type::getMesh, allow_raw_pointers())

template<typename T>
void copyToVector(const val &typedArray, std::vector<T> &vec) {
  unsigned int length = typedArray["length"].as<unsigned int>();
  val memory = val::module_property("buffer");
  vec.reserve(length);
  val memoryView = typedArray["constructor"].new_(memory, reinterpret_cast<uintptr_t>(vec.data()), length);
  memoryView.call<void>("set", typedArray);
}

using namespace geometrycentral;
using namespace geometrycentral::surface;

EMSCRIPTEN_BINDINGS(geometrycentral) {
  value_array<Vector3>("Vector3")
    .element(&Vector3::x)
    .element(&Vector3::y)
    .element(&Vector3::z);

  value_array<Vector2>("Vector2")
    .element(&Vector2::x)
    .element(&Vector2::y);

  register_vector<size_t>("VectorSizeT");
  register_vector<std::vector<size_t> >("VectorVectorSizeT");
  register_vector<Vector3>("VectorVector3");

  function("vec3dataView", optional_override([](const std::vector<Vector3>& data) {
    return val(typed_memory_view(data.size() * 3, (double*)data.data()));
  }));

  class_<HalfedgeMesh>("HalfedgeMesh")
    .constructor()
    .class_function("fromPolygons", optional_override([](std::vector<std::vector<size_t>>& polygons) {
      std::unique_ptr<HalfedgeMesh> mesh (new HalfedgeMesh(polygons));
      return mesh;
    }))
    .function("printStatistics", optional_override([](const HalfedgeMesh& self) {
      std::stringstream stream;
      self.printStatistics(stream);
      return stream.str();
    }))
    .function("nVertices", &HalfedgeMesh::nVertices)
    .function("nInteriorVertices", &HalfedgeMesh::nInteriorVertices)
    .function("nCorners", &HalfedgeMesh::nCorners)
    .function("nEdges", &HalfedgeMesh::nEdges)
    .function("nFaces", &HalfedgeMesh::nFaces)
    .function("nHalfedges", &HalfedgeMesh::nHalfedges)
    .function("nInteriorHalfedges", &HalfedgeMesh::nInteriorHalfedges)
    .function("nExteriorHalfedges", &HalfedgeMesh::nExteriorHalfedges)
    .function("nBoundaryLoops", &HalfedgeMesh::nBoundaryLoops)

    .function("nHalfedgesCapacity", &HalfedgeMesh::nHalfedgesCapacity)
    .function("nVerticesCapacity", &HalfedgeMesh::nVerticesCapacity)
    .function("nEdgesCapacity", &HalfedgeMesh::nEdgesCapacity)
    .function("nFacesCapacity", &HalfedgeMesh::nFacesCapacity)
    .function("nBoundaryLoopsCapacity", &HalfedgeMesh::nBoundaryLoopsCapacity)

    .function("hasBoundary", &HalfedgeMesh::hasBoundary)
    .function("eulerCharacteristic", &HalfedgeMesh::eulerCharacteristic)
    .function("genus", &HalfedgeMesh::genus)
    .function("isTriangular", &HalfedgeMesh::isTriangular)
    .function("nConnectedComponents", &HalfedgeMesh::nConnectedComponents)
    .function("printStatistics", &HalfedgeMesh::printStatistics)

    .function("insertVertexAlongEdge", &HalfedgeMesh::insertVertexAlongEdge)
    .function("splitEdgeTriangular", &HalfedgeMesh::splitEdgeTriangular)
    .function("insertVertex", &HalfedgeMesh::insertVertex)
    .function("connectVertices", &HalfedgeMesh::connectVertices)
    .function("triangulate", &HalfedgeMesh::triangulate)
    .function("flip", &HalfedgeMesh::flip)

    .function("isCompressed", &HalfedgeMesh::isCompressed)
    .function("isCanonical", &HalfedgeMesh::isCanonical)
    .function("validateConnectivity", &HalfedgeMesh::validateConnectivity)

    .function("copy", &HalfedgeMesh::copy)

    // The indexing routines in the section are only valid when the mesh is compressed.
    .function("halfedge", &HalfedgeMesh::halfedge)
    .function("corner", &HalfedgeMesh::corner)
    .function("vertex", &HalfedgeMesh::vertex)
    .function("edge", &HalfedgeMesh::edge)
    .function("face", &HalfedgeMesh::face)
    .function("boundaryLoop", &HalfedgeMesh::boundaryLoop)

    .function("forHalfedges", optional_override([](HalfedgeMesh& self, val cb) { for (const Halfedge& he : self.halfedges()) cb(he); }))
    .function("forVertices", optional_override([](HalfedgeMesh& self, val cb) { for (const Vertex& v : self.vertices()) cb(v); }))
    .function("forEdges", optional_override([](HalfedgeMesh& self, val cb) { for (const Edge& e : self.edges()) cb(e); }))
    .function("forFaces", optional_override([](HalfedgeMesh& self, val cb) { for (const Face& f : self.faces()) cb(f); }));

  class_<Vertex>("Vertex")
    ELEMENT_BOILERPLATE(Vertex)
    .function("halfedge", &Vertex::halfedge)
    .function("corner", &Vertex::corner)
    .function("isBoundary", &Vertex::isBoundary)
    .function("degree", &Vertex::degree)
    .function("faceDegree", &Vertex::faceDegree)
    .function("outgoingHalfedges", &Vertex::outgoingHalfedges)

    .function("forOutgoingHalfedges", optional_override([](Vertex& self, val cb) { for (const Halfedge& he : self.outgoingHalfedges()) cb(he); }))
    .function("forIncomingHalfedges", optional_override([](Vertex& self, val cb) { for (const Halfedge& he : self.incomingHalfedges()) cb(he); }))
    .function("forAdjacentVertices", optional_override([](Vertex& self, val cb) { for (const Vertex& v : self.adjacentVertices()) cb(v); }))
    .function("forAdjacentEdges", optional_override([](Vertex& self, val cb) { for (const Edge& e : self.adjacentEdges()) cb(e); }))
    .function("forAdjacentFaces", optional_override([](Vertex& self, val cb) { for (const Face& e : self.adjacentFaces()) cb(e); }));

  class_<Corner>("Corner")
    ELEMENT_BOILERPLATE(Corner)
    .function("halfedge", &Corner::halfedge)
    .function("vertex", &Corner::vertex)
    .function("face", &Corner::face);

  class_<Edge>("Edge")
    ELEMENT_BOILERPLATE(Edge)
    .function("halfedge", &Edge::halfedge)
    .function("isBoundary", &Edge::isBoundary);

    //.function("forAdjacentHalfedges", optional_override([](Edge& self, val cb) { for (const Halfedge& he : self.adjacentHalfedges()) cb(he); }))
    //.function("forAdjacentFaces", optional_override([](Edge& self, val cb) { for (const Face& e : self.adjacentFaces()) cb(e); }));

  class_<Face>("Face")
    ELEMENT_BOILERPLATE(Face)
    .function("halfedge", &Face::halfedge)
    .function("asBoundaryLoop", &Face::asBoundaryLoop)
    .function("isBoundaryLoop", &Face::isBoundaryLoop)
    .function("isTriangle", &Face::isTriangle)
    .function("degree", &Face::degree)

    .function("forAdjacentHalfedges", optional_override([](Face& self, val cb) { for (const Halfedge& he : self.adjacentHalfedges()) cb(he); }))
    .function("forAdjacentVertices", optional_override([](Face& self, val cb) { for (const Vertex& v : self.adjacentVertices()) cb(v); }))
    .function("forAdjacentFaces", optional_override([](Face& self, val cb) { for (const Face& f : self.adjacentFaces()) cb(f); }))
    .function("forAdjacentEdges", optional_override([](Face& self, val cb) { for (const Edge& e : self.adjacentEdges()) cb(e); }));

  class_<BoundaryLoop>("BoundaryLoop")
    ELEMENT_BOILERPLATE(BoundaryLoop)
    .function("asFace", &BoundaryLoop::asFace)

    .function("forAdjacentHalfedges", optional_override([](BoundaryLoop& self, val cb) { for (const Halfedge& he : self.adjacentHalfedges()) cb(he); }))
    .function("forAdjacentVertices", optional_override([](BoundaryLoop& self, val cb) { for (const Vertex& v : self.adjacentVertices()) cb(v); }))
    .function("forAdjacentEdges", optional_override([](BoundaryLoop& self, val cb) { for (const Edge& e : self.adjacentEdges()) cb(e); }));

  class_<Halfedge>("Halfedge")
    ELEMENT_BOILERPLATE(Halfedge)
    .function("twin", &Halfedge::twin)
    .function("next", &Halfedge::next)
    .function("vertex", &Halfedge::vertex)
    .function("edge", &Halfedge::edge)
    .function("face", &Halfedge::face)
    .function("corner", &Halfedge::corner)
    .function("prevOrbitFace", &Halfedge::prevOrbitFace)
    .function("prevOrbitVertex", &Halfedge::prevOrbitVertex)
    .function("isInterior", &Halfedge::isInterior);

  class_<PolygonSoupMesh>("PolygonSoupMesh")
    .constructor()
    .class_function("fromString", optional_override([](std::string str, std::string type = "") {
      std::stringstream in;
      in << str;
      return std::unique_ptr<PolygonSoupMesh>(new PolygonSoupMesh(in, type));
    }))
    .function("triangulate", &PolygonSoupMesh::triangulate)
    .function("polygons", optional_override([](PolygonSoupMesh& self) {
      return self.polygons;
    }))
    .function("vertexCoordinates", optional_override([](const PolygonSoupMesh& self) {
      return self.vertexCoordinates;
    }))
    .function("vertexCoordinatesDataView", optional_override([](const PolygonSoupMesh& self) {
      return val(typed_memory_view(self.vertexCoordinates.size() * 3, (double*)self.vertexCoordinates.data()));
    }));

  
}
