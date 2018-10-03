#pragma once

#include <list>
#include <vector>

#include "geometrycentral/vector3.h"
#include <geometrycentral/utilities.h>
// NOTE: More includes at bottom of file

namespace geometrycentral {

// A HalfedgeMesh encodes the connectivity---but not the geometry---of a
// manifold surface, possibly with boundary.

// Forward declare classes for primary mesh objects
class HalfedgeMesh;
class Halfedge;
class Vertex;
class Edge;
class Face;

// Forward declare classes used for conversion from other mesh types
class PolygonSoupMesh;
template <class T>
class Geometry;

} // namespace geometrycentral

// Order MATTERS for these includes
// 1
#include "geometrycentral/halfedge_pointer_types.h"
// 3
#include "geometrycentral/halfedge_data_types.h"
#include "geometrycentral/halfedge_iterators.h"
// 4
#include "geometrycentral/halfedge_mesh_data_transfer.h"

namespace geometrycentral {


class HalfedgeMesh {

public:
  HalfedgeMesh();
  HalfedgeMesh(const PolygonSoupMesh& soup, Geometry<Vector3>*& geometry);
  ~HalfedgeMesh();

  // Number of mesh elements of each type
  size_t nHalfedges() const;
  size_t nCorners() const;
  size_t nVertices() const;
  size_t nInteriorVertices();
  size_t nEdges() const;
  size_t nFaces() const;
  size_t nBoundaryLoops() const;
  size_t nImaginaryHalfedges() const;

  // Methods for range-based for loops
  // Example: for(VertexPtr v : mesh.vertices()) { ... }
  HalfedgePtrSet realHalfedges();
  HalfedgePtrSet imaginaryHalfedges();
  HalfedgePtrSet allHalfedges();
  CornerPtrSet corners();
  VertexPtrSet vertices();
  EdgePtrSet edges();
  FacePtrSet faces();
  BoundaryPtrSet boundaryLoops();

  // Methods for accessing elements by index
  // Example: VertexPtr v = mesh.vertex(123);
  HalfedgePtr halfedge(size_t index);
  CornerPtr corner(size_t index);
  VertexPtr vertex(size_t index);
  EdgePtr edge(size_t index);
  FacePtr face(size_t index);
  BoundaryLoopPtr boundaryLoop(size_t index);

  // Methods that mutate the mesh. Note that these occasionally trigger a resize, which invaliates
  // any outstanding VertexPtr or MeshData<> objects. See the guide (currently in docs/mutable_mesh_docs.md).
  // TODOs: support adding boundary


  // Flip an edge. Unlike all the other mutation routines, this _does not_ invalidate pointers, though it does break the canonical ordering.
  // Return true if the edge was actually flipped (can't flip boundary or non-triangular edges)
  bool flip(EdgePtr e);

  // Adds a vertex along an edge, increasing degree of faces. Returns ptr along the new edge, with he.vertex() as new
  // vertex and he.edge().halfedge() == he. Preserves canonical direction of edge.halfedge() for both halves of new
  // edge.
  HalfedgePtr insertVertexAlongEdge(EdgePtr e);

  // Split an edge, also splitting adjacent faces. Returns new vertex.
  VertexPtr splitEdge(EdgePtr e);

  // Split an edge, also splitting adjacent faces. Returns new halfedge which points TOWARDS the new vertex, and is the
  // same direction as e.halfedge() on the original edge. The halfedge direction of the other part of the new split edge
  // is also preserved, as in insertVertexAlongEdge().
  HalfedgePtr splitEdgeReturnHalfedge(EdgePtr e);

  // Add vertex inside face and triangulate. Returns new vertex.
  VertexPtr insertVertex(FacePtr f);

  // Add an edge connecting two vertices inside the same face. Returns new halfedge with vA at tail. he.twin().face() is
  // the new face.
  HalfedgePtr connectVertices(VertexPtr vA, VertexPtr vB);

  // Same as above. Faster if you know the face.
  HalfedgePtr connectVertices(FacePtr face, VertexPtr vA, VertexPtr vB);

  // Same as above, but if vertices do not contain shared face or are adajcent returns HalfedgePtr() rather than
  // throwing.
  HalfedgePtr tryConnectVertices(VertexPtr vA, VertexPtr vB);
  
  // Same as above, but you can specify a face to work in
  HalfedgePtr tryConnectVertices(VertexPtr vA, VertexPtr vB, FacePtr face);

  // Collapse an edge. Returns the vertex adjacent to that edge which still exists. Returns VertexPtr() if not
  // collapsible.
  VertexPtr collapseEdge(EdgePtr e);
 

  // Set e.halfedge() == he. he must be adjacent.
  void setEdgeHalfedge(EdgePtr e, HalfedgePtr he);

  // Triangulate in a face, returns all subfaces
  std::vector<FacePtr> triangulate(FacePtr face);


  // Methods for obtaining canonical indices for mesh elements
  // (Note that in some situations, custom indices might instead be needed)
  VertexData<size_t> getVertexIndices();
  VertexData<size_t> getInteriorVertexIndices();
  FaceData<size_t> getFaceIndices();
  EdgeData<size_t> getEdgeIndices();
  HalfedgeData<size_t> getHalfedgeIndices();
  CornerData<size_t> getCornerIndices();

  // Utility functions
  bool isSimplicial();          // returns true if and only if all faces are triangles
  size_t nFacesTriangulation(); // returns the number of triangles in the
                                // triangulation determined by
                                // Face::triangulate()
  size_t longestBoundaryLoop();
  int eulerCharacteristic();
  size_t nConnectedComponents();
  std::vector<std::vector<size_t>> getPolygonSoupFaces();
  HalfedgeMesh* copy();                            // returns a deep copy
  HalfedgeMesh* copy(HalfedgeMeshDataTransfer& t); // returns a deep copy

  // Compress the mesh
  bool isCompressed();
  void compress();
 
  // Canonicalize the element ordering to be the same indexing convention as after construction from polygon soup.
  bool isCanonical(); 
  void canonicalize(); 

  // == Callbacks that will be invoked on mutation to keep containers/iterators/etc valid.
  // Expansion callbacks
  // Argument is the new size of the element list. Elements up to this index (aka offset from base) may now be used (but
  // _might_ not be)
  std::list<std::function<void(size_t)>> vertexExpandCallbackList;
  std::list<std::function<void(size_t)>> faceExpandCallbackList;
  std::list<std::function<void(size_t)>> edgeExpandCallbackList;
  std::list<std::function<void(size_t)>> halfedgeExpandCallbackList;

  // Compression callbacks
  // Argument is a permutation to a apply, such that d_new[i] = d_old[p[i]]. New size may be smaller, in which case
  // capacity may be shrunk to that size.
  std::list<std::function<void(const std::vector<size_t>&)>> vertexPermuteCallbackList;
  std::list<std::function<void(const std::vector<size_t>&)>> facePermuteCallbackList;
  std::list<std::function<void(const std::vector<size_t>&)>> edgePermuteCallbackList;
  std::list<std::function<void(const std::vector<size_t>&)>> halfedgePermuteCallbackList;

  // Mesh delete callbacks
  // (this unfortunately seems to be necessary; objects which have registered their callbacks above
  // need to know not to try to de-register them if the mesh has been deleted)
  std::list<std::function<void()>> meshDeleteCallbackList;

  // Check capacity. Needed when implementing expandable containers for mutable meshes to ensure the contain can
  // hold a sufficient number of elements before the next resize event.
  size_t nHalfedgesCapacity() const;
  size_t nVerticesCapacity() const;
  size_t nEdgesCapacity() const;
  size_t nFacesCapacity() const;

  // Performs a sanity checks on halfedge structure; throws on fail
  void validateConnectivity();


private:
  // The contiguous chunks of memory which hold the actual structs. These may change size and even become sparse as
  // insertions and deletions happen.
  std::vector<Halfedge> rawHalfedges;
  std::vector<Vertex> rawVertices;
  std::vector<Edge> rawEdges;
  std::vector<Face> rawFaces;
  std::vector<Face> rawBoundaryLoops;

  // Track element counts (can't rely on rawVertices.size() after deletions have made the list sparse). These are the
  // actual number of elements, not the size of the buffer that holds them.
  size_t nRealHalfedgesCount = 0;
  size_t nImaginaryHalfedgesCount = 0;
  size_t nVerticesCount = 0;
  size_t nEdgesCount = 0;
  size_t nFacesCount = 0;
  size_t nBoundaryLoopsCount = 0;
  size_t nextElemID = 77777; // used to assign unique ID to elements
  
  bool isCanonicalFlag = true;
  bool isCompressedFlag = true;

  // Hide copy and move constructors, we don't wanna mess with that
  HalfedgeMesh(const HalfedgeMesh& other) = delete;
  HalfedgeMesh& operator=(const HalfedgeMesh& other) = delete;
  HalfedgeMesh(HalfedgeMesh&& other) = delete;
  HalfedgeMesh& operator=(HalfedgeMesh&& other) = delete;

  // Used to resize the halfedge mesh. Expands and shifts vectors as necessary.
  Halfedge* getNewRealHalfedge();
  Halfedge* getNewImaginaryHalfedge();
  Vertex* getNewVertex();
  Edge* getNewEdge();
  Face* getNewFace();

  // Deletes leave tombstones, which can be cleaned up with compress()
  void deleteElement(HalfedgePtr he);
  void deleteElement(EdgePtr e);
  void deleteElement(VertexPtr v);
  void deleteElement(FacePtr f);

  // Compression helpers
  void compressHalfedges();
  void compressEdges();
  void compressFaces();
  void compressVertices();


  size_t indexOf(Halfedge* ptr);
  size_t indexOf(Vertex* ptr);
  size_t indexOf(Edge* ptr);
  size_t indexOf(Face* ptr);

  friend class DynamicHalfedgePtr;
  friend class DynamicVertexPtr;
  friend class DynamicEdgePtr;
  friend class DynamicFacePtr;
};

class Halfedge {
  friend class Edge;
  friend class HalfedgeMesh;
  friend class HalfedgePtr;
  friend struct std::hash<HalfedgePtr>;
  friend struct std::hash<DynamicHalfedgePtr>;

protected:
  Halfedge* twin;
  Halfedge* next;
  Vertex* vertex;
  Edge* edge;
  Face* face;

  bool isReal = true;
  size_t ID; // a unique value useful for hashing (etc). NOT an index

public:
#ifndef NDEBUG
  // The mesh that this is a part of. Should only be used for debugging, so
  // exclude it unless debug is enabled.
  HalfedgeMesh* parentMesh;
#endif

  // Null twin ptr measn this halfedge has been deleted
  void markDead() { twin = nullptr; };
  bool isDead() { return twin == nullptr; };
};

class Vertex {
  friend class Edge;
  friend class HalfedgeMesh;
  friend class VertexPtr;
  friend struct std::hash<VertexPtr>;
  friend struct std::hash<DynamicVertexPtr>;

protected:
  // Data structure
  Halfedge* halfedge; // some halfedge that emanates from this vertex
                      // (guaranteed to be real)
  bool isBoundary = false;
  size_t ID; // a unique value useful for hashing (etc). NOT an index

public:
#ifndef NDEBUG
  // The mesh that this is a part of. Should only be used for debugging, so
  // exclude it unless debug is enabled.
  HalfedgeMesh* parentMesh;
#endif

  // Null halfedge ptr means this vertex has been deleted
  void markDead() { halfedge = nullptr; };
  bool isDead() { return halfedge == nullptr; };
};

class Edge {
  friend class HalfedgeMesh;
  friend class EdgePtr;
  friend struct std::hash<EdgePtr>;
  friend struct std::hash<DynamicEdgePtr>;

protected:
  Halfedge* halfedge;

  bool isBoundary = false;
  size_t ID; // a unique value useful for hashing (etc). NOT an index

public:
#ifndef NDEBUG
  // The mesh that this is a part of. Should only be used for debugging, so
  // exclude it unless debug is enabled.
  HalfedgeMesh* parentMesh;
#endif

  // Null halfedge ptr means this edge has been deleted
  void markDead() { halfedge = nullptr; };
  bool isDead() { return halfedge == nullptr; };
};

class Face {
  friend class Edge;
  friend class HalfedgeMesh;
  friend class FacePtr;
  friend struct std::hash<FacePtr>;
  friend struct std::hash<DynamicFacePtr>;

protected:
  Halfedge* halfedge;

  bool isBoundary = false;
  bool isReal = false;
  size_t ID; // a unique value useful for hashing (etc). NOT an index

public:
#ifndef NDEBUG
  // The mesh that this is a part of. Should only be used for debugging, so
  // exclude it unless debug is enabled.
  HalfedgeMesh* parentMesh;
#endif

  // Null halfedge ptr means this face has been deleted
  void markDead() { halfedge = nullptr; };
  bool isDead() { return halfedge == nullptr; };
};

} // namespace geometrycentral

#include "geometrycentral/halfedge_data_types.ipp"
#include "geometrycentral/halfedge_iterators.ipp"
#include "geometrycentral/halfedge_mesh.ipp"
#include "geometrycentral/halfedge_mesh_data_transfer.ipp"
#include "geometrycentral/halfedge_pointer_types.ipp"
