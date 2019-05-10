#pragma once

#include <cstddef>
#include <functional>
#include <iostream>

namespace geometrycentral {

// === Types and inline methods for the halfedge mesh pointer and datatypes

// === Forward declare iterator set types (pointers may need to return these to
// support range-based for loops)
class VertexIncomingHalfedgeSet;
class VertexIncomingInteriorHalfedgeSet;
class VertexOutgoingHalfedgeSet;
class VertexOutgoingInteriorHalfedgeSet;
class VertexAdjacentVertexSet;
class VertexAdjacentFaceSet;
class VertexAdjacentEdgeSet;
class VertexAdjacentCornerSet;
class FaceAdjacentHalfedgeSet;
class FaceAdjacentVertexSet;
class FaceAdjacentEdgeSet;
class FaceAdjacentFaceSet;
class FaceAdjacentCornerSet;

// === Pointer types for mesh elements
class HalfedgePtr;
class CornerPtr;
class VertexPtr;
class EdgePtr;
class FacePtr;

// The dynamic variants are automatically updated when the mesh is mutated, (the standard variants are invalidated).
class DynamicHalfedgePtr;
class DynamicVertexPtr;
class DynamicEdgePtr;
class DynamicFacePtr;
class DynamicBoundaryLoopPtr;

// Halfedge
class HalfedgePtr {
public:
  HalfedgePtr(); // defaults to nullptr
  HalfedgePtr(Halfedge* ptr);
  HalfedgePtr(DynamicHalfedgePtr he);

  // Connectivity
  HalfedgePtr twin() const;
  HalfedgePtr next() const;
  HalfedgePtr prev() const;
  VertexPtr vertex() const;
  EdgePtr edge() const;
  FacePtr face() const;
  CornerPtr corner() const;


  // Properties
  bool isReal() const;

  // Accessors
  Halfedge operator*();
  Halfedge operator*() const;
  Halfedge* operator->();
  const Halfedge* operator->() const;

  // Comparators
  bool operator==(const HalfedgePtr& other) const;
  bool operator!=(const HalfedgePtr& other) const;
  bool operator>(const HalfedgePtr& other) const;
  bool operator>=(const HalfedgePtr& other) const;
  bool operator<(const HalfedgePtr& other) const;
  bool operator<=(const HalfedgePtr& other) const;

  // Null comparators
  bool operator==(void* n) const;
  bool operator!=(void* n) const;
  bool operator>(void* n) const;
  bool operator>=(void* n) const;
  bool operator<(void* n) const;
  bool operator<=(void* n) const;

  // Arithmetic
  unsigned int operator-(const HalfedgePtr& other) const;
  HalfedgePtr& operator++();
  HalfedgePtr operator++(int);
  HalfedgePtr& operator--();
  HalfedgePtr operator--(int);

  // The dynamic equivalent
  typedef DynamicHalfedgePtr DynamicType;

protected:
  Halfedge* ptr = nullptr;

  friend class HalfedgeMesh;
  friend class DynamicHalfedgePtr;
  friend std::ostream& operator<<(std::ostream& output, const HalfedgePtr& he);
  friend struct std::hash<HalfedgePtr>;
};
std::ostream& operator<<(std::ostream& output, const HalfedgePtr& he);

class DynamicHalfedgePtr {
public:
  DynamicHalfedgePtr(){};
  DynamicHalfedgePtr(Halfedge* ptr, HalfedgeMesh* mesh);
  DynamicHalfedgePtr(HalfedgePtr ptr, HalfedgeMesh* mesh);

  DynamicHalfedgePtr twin() const;
  DynamicHalfedgePtr next() const;
  DynamicVertexPtr vertex() const;
  DynamicEdgePtr edge() const;
  DynamicFacePtr face() const;

  // Despite the name, should not be used as a [0,N) index.
  size_t getInd() const;

  // Comparators
  bool operator==(const DynamicHalfedgePtr& other) const;
  bool operator!=(const DynamicHalfedgePtr& other) const;
  bool operator>(const DynamicHalfedgePtr& other) const;
  bool operator>=(const DynamicHalfedgePtr& other) const;
  bool operator<(const DynamicHalfedgePtr& other) const;
  bool operator<=(const DynamicHalfedgePtr& other) const;

  // The static equivalent
  typedef HalfedgePtr StaticType;

private:
  friend class HalfedgePtr;
  friend class HalfedgeMesh;
  friend struct std::hash<DynamicHalfedgePtr>;

  size_t ind = -1;
  HalfedgeMesh* mesh = nullptr;
  // std::list<DynamicHalfedgePtr*>::iterator listIt;
};

enum class HalfedgeSetType { Real, Imaginary, All };
class HalfedgePtrRangeIterator {
public:
  HalfedgePtrRangeIterator(HalfedgePtr startingHalfedge, HalfedgeSetType type_, HalfedgePtr end_);
  const HalfedgePtrRangeIterator& operator++();
  bool operator==(const HalfedgePtrRangeIterator& other) const;
  bool operator!=(const HalfedgePtrRangeIterator& other) const;
  HalfedgePtr operator*() const;

private:
  HalfedgePtr currHalfedge;
  HalfedgeSetType type;
  HalfedgePtr end; // unfortunately needed to respect type option
};
class HalfedgePtrSet {
public:
  HalfedgePtrSet(HalfedgePtr beginptr_, HalfedgePtr endptr_, HalfedgeSetType type_);
  HalfedgePtrRangeIterator begin();
  HalfedgePtrRangeIterator end();

private:
  HalfedgePtr beginptr, endptr;
  HalfedgeSetType type;
};

// Corner
class CornerPtr {
public:
  CornerPtr(); // defaults to nullptr
  CornerPtr(Halfedge* ptr);

  // Connectivity
  CornerPtr next() const;
  CornerPtr prev() const;
  HalfedgePtr halfedge() const;
  VertexPtr vertex() const;
  FacePtr face() const;

  // Accessors
  Halfedge operator*();
  Halfedge operator*() const;
  Halfedge* operator->();
  const Halfedge* operator->() const;

  // Comparators
  bool operator==(const CornerPtr& other) const;
  bool operator!=(const CornerPtr& other) const;
  bool operator>(const CornerPtr& other) const;
  bool operator>=(const CornerPtr& other) const;
  bool operator<(const CornerPtr& other) const;
  bool operator<=(const CornerPtr& other) const;

  // Null comparators
  bool operator==(void* n) const;
  bool operator!=(void* n) const;
  bool operator>(void* n) const;
  bool operator>=(void* n) const;
  bool operator<(void* n) const;
  bool operator<=(void* n) const;

  // Arithmetic
  unsigned int operator-(const CornerPtr& other) const;
  CornerPtr& operator++();
  CornerPtr operator++(int);
  CornerPtr& operator--();
  CornerPtr operator--(int);

  // The dynamic equivalent
  // TODO
  typedef DynamicHalfedgePtr DynamicType;

protected:
  Halfedge* ptr = nullptr;

  friend class HalfedgeMesh;
};
class CornerPtrRangeIterator {
public:
  CornerPtrRangeIterator(CornerPtr startingCorner, CornerPtr end);
  const CornerPtrRangeIterator& operator++();
  bool operator==(const CornerPtrRangeIterator& other) const;
  bool operator!=(const CornerPtrRangeIterator& other) const;
  CornerPtr operator*() const;

private:
  CornerPtr currCorner;
  CornerPtr end;
};
class CornerPtrSet {
public:
  CornerPtrSet(CornerPtr beginptr_, CornerPtr endptr_);
  CornerPtrRangeIterator begin();
  CornerPtrRangeIterator end();

private:
  CornerPtr beginptr, endptr;
};

// Vertex
class VertexPtr {
public:
  VertexPtr(); // defaults to nullptr
  VertexPtr(Vertex* ptr);
  VertexPtr(DynamicVertexPtr v);

  // Connectivity
  HalfedgePtr halfedge() const;
  CornerPtr corner() const;


  // Properties
  bool isBoundary() const;
  unsigned int degree();

  // Iterators
  VertexIncomingHalfedgeSet incomingHalfedges();
  VertexOutgoingHalfedgeSet outgoingHalfedges();
  VertexIncomingInteriorHalfedgeSet incomingInteriorHalfedges();
  VertexOutgoingInteriorHalfedgeSet outgoingInteriorHalfedges();
  VertexAdjacentVertexSet adjacentVertices();
  VertexAdjacentFaceSet adjacentFaces();
  VertexAdjacentEdgeSet adjacentEdges();
  VertexAdjacentCornerSet adjacentCorners();

  // Accessors
  Vertex operator*();
  Vertex operator*() const;
  Vertex* operator->();
  const Vertex* operator->() const;

  // Comparators
  bool operator==(const VertexPtr& other) const;
  bool operator!=(const VertexPtr& other) const;
  bool operator>(const VertexPtr& other) const;
  bool operator>=(const VertexPtr& other) const;
  bool operator<(const VertexPtr& other) const;
  bool operator<=(const VertexPtr& other) const;

  // Null comparators
  bool operator==(void* n) const;
  bool operator!=(void* n) const;
  bool operator>(void* n) const;
  bool operator>=(void* n) const;
  bool operator<(void* n) const;
  bool operator<=(void* n) const;

  // Arithmetic
  unsigned int operator-(const VertexPtr& other) const;
  VertexPtr& operator++();
  VertexPtr operator++(int);
  VertexPtr& operator--();
  VertexPtr operator--(int);

  // The dynamic equivalent
  typedef DynamicVertexPtr DynamicType;

protected:
  Vertex* ptr = nullptr;

  friend class HalfedgeMesh;
  friend class DynamicVertexPtr;
  friend std::ostream& operator<<(std::ostream& output, const VertexPtr& v);
  friend struct std::hash<VertexPtr>;
};
std::ostream& operator<<(std::ostream& output, const VertexPtr& v);

class DynamicVertexPtr {
public:
  DynamicVertexPtr(){};
  DynamicVertexPtr(Vertex* ptr, HalfedgeMesh* mesh);
  DynamicVertexPtr(VertexPtr ptr, HalfedgeMesh* mesh);

  // Connectivity
  DynamicHalfedgePtr halfedge() const;

  // Despite the name, should not be used as a [0,N) index.
  size_t getInd() const;

  // Comparators
  bool operator==(const DynamicVertexPtr& other) const;
  bool operator!=(const DynamicVertexPtr& other) const;
  bool operator>(const DynamicVertexPtr& other) const;
  bool operator>=(const DynamicVertexPtr& other) const;
  bool operator<(const DynamicVertexPtr& other) const;
  bool operator<=(const DynamicVertexPtr& other) const;

  // The static equivalent
  typedef VertexPtr StaticType;

private:
  friend class VertexPtr;
  friend class HalfedgeMesh;
  friend struct std::hash<DynamicVertexPtr>;

  size_t ind = -1;
  HalfedgeMesh* mesh = nullptr;
  // std::list<DynamicVertexPtr*>::iterator listIt;
};

class VertexPtrRangeIterator {
public:
  VertexPtrRangeIterator(VertexPtr startingVertex, VertexPtr end);
  const VertexPtrRangeIterator& operator++();
  bool operator==(const VertexPtrRangeIterator& other) const;
  bool operator!=(const VertexPtrRangeIterator& other) const;
  VertexPtr operator*() const;

private:
  VertexPtr currVertex;
  VertexPtr end;
};
class VertexPtrSet {
public:
  VertexPtrSet(VertexPtr beginptr_, VertexPtr endptr_);
  VertexPtrRangeIterator begin();
  VertexPtrRangeIterator end();

private:
  VertexPtr beginptr, endptr;
};

// Edge
class EdgePtr {
public:
  EdgePtr(); // defaults to nullptr
  EdgePtr(Edge* ptr);
  EdgePtr(DynamicEdgePtr e);

  // Connectivity
  HalfedgePtr halfedge() const;

  // Properties
  bool isBoundary() const;

  // Accessors
  Edge operator*();
  Edge operator*() const;
  Edge* operator->();
  const Edge* operator->() const;

  // Comparators
  bool operator==(const EdgePtr& other) const;
  bool operator!=(const EdgePtr& other) const;
  bool operator>(const EdgePtr& other) const;
  bool operator>=(const EdgePtr& other) const;
  bool operator<(const EdgePtr& other) const;
  bool operator<=(const EdgePtr& other) const;

  // Null comparators
  bool operator==(void* n) const;
  bool operator!=(void* n) const;
  bool operator>(void* n) const;
  bool operator>=(void* n) const;
  bool operator<(void* n) const;
  bool operator<=(void* n) const;

  // Arithmetic
  unsigned int operator-(const EdgePtr& other) const;
  EdgePtr& operator++();
  EdgePtr operator++(int);
  EdgePtr& operator--();
  EdgePtr operator--(int);

  // The dynamic equivalent
  typedef DynamicEdgePtr DynamicType;

protected:
  Edge* ptr = nullptr;

  friend class HalfedgeMesh;
  friend class DynamicEdgePtr;
  friend std::ostream& operator<<(std::ostream& output, const EdgePtr& e);
  friend struct std::hash<EdgePtr>;
};
std::ostream& operator<<(std::ostream& output, const EdgePtr& e);

class DynamicEdgePtr {
public:
  DynamicEdgePtr(){};
  DynamicEdgePtr(Edge* ptr, HalfedgeMesh* mesh);
  DynamicEdgePtr(EdgePtr ptr, HalfedgeMesh* mesh);

  // Connectivity
  DynamicHalfedgePtr halfedge() const;

  // Despite the name, should not be used as a [0,N) index.
  size_t getInd() const;

  // Comparators
  bool operator==(const DynamicEdgePtr& other) const;
  bool operator!=(const DynamicEdgePtr& other) const;
  bool operator>(const DynamicEdgePtr& other) const;
  bool operator>=(const DynamicEdgePtr& other) const;
  bool operator<(const DynamicEdgePtr& other) const;
  bool operator<=(const DynamicEdgePtr& other) const;

  // The static equivalent
  typedef EdgePtr StaticType;

private:
  friend class EdgePtr;
  friend class HalfedgeMesh;
  friend struct std::hash<DynamicEdgePtr>;

  size_t ind = -1;
  HalfedgeMesh* mesh = nullptr;
  // std::list<DynamicEdgePtr*>::iterator listIt;
};

class EdgePtrRangeIterator {
public:
  EdgePtrRangeIterator(EdgePtr startingEdge, EdgePtr end);
  const EdgePtrRangeIterator& operator++();
  bool operator==(const EdgePtrRangeIterator& other) const;
  bool operator!=(const EdgePtrRangeIterator& other) const;
  EdgePtr operator*() const;

private:
  EdgePtr currEdge;
  EdgePtr end;
};
class EdgePtrSet {
public:
  EdgePtrSet(EdgePtr beginptr_, EdgePtr endptr_);
  EdgePtrRangeIterator begin();
  EdgePtrRangeIterator end();

private:
  EdgePtr beginptr, endptr;
};

// NOTE: Triangle is merely a helper class for the
// method FacePtr::triangulation(); it is NOT the
// standard representation for faces of a HalfedgeMesh.
struct Triangle {
  VertexPtr vertex[3];
  VertexPtr& operator[](size_t index) { return vertex[index]; }
};

// Face
class FacePtr {
public:
  FacePtr(); // defaults to nullptr
  FacePtr(Face* ptr);
  FacePtr(DynamicFacePtr f);

  // Connectivity
  HalfedgePtr halfedge() const;
  CornerPtr corner() const;


  // Utility
  std::vector<Triangle> triangulation();

  // Properties
  unsigned int degree();
  bool isBoundary() const;
  bool isReal() const;

  // Iterators
  FaceAdjacentHalfedgeSet adjacentHalfedges();
  FaceAdjacentVertexSet adjacentVertices();
  FaceAdjacentFaceSet adjacentFaces();
  FaceAdjacentEdgeSet adjacentEdges();
  FaceAdjacentCornerSet adjacentCorners();

  // Accessors
  Face operator*();
  Face operator*() const;
  Face* operator->();
  const Face* operator->() const;

  // Comparators
  bool operator==(const FacePtr& other) const;
  bool operator!=(const FacePtr& other) const;
  bool operator>(const FacePtr& other) const;
  bool operator>=(const FacePtr& other) const;
  bool operator<(const FacePtr& other) const;
  bool operator<=(const FacePtr& other) const;

  // Null comparators
  bool operator==(void* n) const;
  bool operator!=(void* n) const;
  bool operator>(void* n) const;
  bool operator>=(void* n) const;
  bool operator<(void* n) const;
  bool operator<=(void* n) const;

  // Arithmetic
  unsigned int operator-(const FacePtr& other) const;
  FacePtr& operator++();
  FacePtr operator++(int);
  FacePtr& operator--();
  FacePtr operator--(int);

  // The dynamic equivalent
  typedef DynamicFacePtr DynamicType;

protected:
  Face* ptr = nullptr;

  friend class HalfedgeMesh;
  friend class DynamicFacePtr;
  friend std::ostream& operator<<(std::ostream& output, const FacePtr& f);
  friend struct std::hash<FacePtr>;
};
std::ostream& operator<<(std::ostream& output, const FacePtr& f);

class DynamicFacePtr {
public:
  DynamicFacePtr(){};
  DynamicFacePtr(Face* ptr, HalfedgeMesh* mesh);
  DynamicFacePtr(FacePtr ptr, HalfedgeMesh* mesh);

  // Connectivity
  DynamicHalfedgePtr halfedge() const;

  // Despite the name, should not be used as a [0,N) index.
  size_t getInd() const;

  // Comparators
  bool operator==(const DynamicFacePtr& other) const;
  bool operator!=(const DynamicFacePtr& other) const;
  bool operator>(const DynamicFacePtr& other) const;
  bool operator>=(const DynamicFacePtr& other) const;
  bool operator<(const DynamicFacePtr& other) const;
  bool operator<=(const DynamicFacePtr& other) const;

  // The static equivalent
  typedef FacePtr StaticType;

private:
  friend class FacePtr;
  friend class HalfedgeMesh;
  friend struct std::hash<DynamicFacePtr>;

  size_t ind = -1;
  HalfedgeMesh* mesh = nullptr;
  // std::list<DynamicFacePtr*>::iterator listIt;
};

class FacePtrRangeIterator {
public:
  FacePtrRangeIterator(FacePtr startingFace, FacePtr end);
  const FacePtrRangeIterator& operator++();
  bool operator==(const FacePtrRangeIterator& other) const;
  bool operator!=(const FacePtrRangeIterator& other) const;
  FacePtr operator*() const;

private:
  FacePtr currFace;
  FacePtr end;
};
class FacePtrSet {
public:
  FacePtrSet(FacePtr beginptr_, FacePtr endptr_);
  FacePtrRangeIterator begin();
  FacePtrRangeIterator end();

private:
  FacePtr beginptr, endptr;
};

// Boundary (currently just a renaming of Face---if we wanted
// stronger type checking we could instead inherit from Face*)

typedef Face BoundaryLoop;
typedef FacePtr BoundaryLoopPtr;
typedef FacePtrSet BoundaryPtrSet;
typedef FacePtrRangeIterator BoundaryRangeIterator;


} // namespace geometrycentral