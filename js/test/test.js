var test = require('tape');

var initializeWASM = require('../build/geometrycentral.js');

const OBJ = `# Blender v2.76 (sub 0) OBJ File: ''
# www.blender.org
mtllib tet.mtl
o Solid
v -6.052902 -1.910479 -0.448446
v -5.110093 -3.243812 -0.448446
v -6.524307 -3.243812 -1.264943
v -6.524307 -3.243812 0.368050
vn 0.471400 0.333300 -0.816500
vn -0.942800 0.333300 0.000000
vn 0.471400 0.333300 0.816500
vn 0.000000 -1.000000 0.000000
usemtl None
s off
f 1//1 2//1 3//1
f 1//2 3//2 4//2
f 1//3 4//3 2//3
f 2//4 4//4 3//4
`;

test('geometrycentral', function (t) {
  initializeWASM().then(function (geometrycentral) {
    const {
      HalfedgeMesh,
      VectorSizeT,
      VectorVectorSizeT,
      PolygonSoupMesh,
      vec3dataView
    } = geometrycentral;

    t.test('Basic API', function (t) {
      t.test('constructor', function (t) {
        const m = new HalfedgeMesh();
        m.delete();
        t.end();
      });

      t.test('constructor', function (t) {
        var polygons = new VectorVectorSizeT();

        var p1 = new VectorSizeT();
        p1.push_back(0);
        p1.push_back(1);
        p1.push_back(2);
        polygons.push_back(p1);

        var p2 = new VectorSizeT();
        p2.push_back(2);
        p2.push_back(1);
        p2.push_back(3);
        polygons.push_back(p2);

        const m = HalfedgeMesh.fromPolygons(polygons);

        let halfedgeCount = 0;
        let vertexCount = 0;
        let faceCount = 0;
        let edgeCount = 0;

        //m.forVertices(v => console.log(v.toString()));
        //m.forHalfedges(v => console.log(v.toString()));

        m.forVertices(v => vertexCount++);
        m.forHalfedges(he => halfedgeCount++);
        m.forEdges(e => edgeCount++);
        m.forFaces(f => faceCount++);

        t.equal(vertexCount, 4);
        t.equal(halfedgeCount, 10);
        t.equal(edgeCount, 5);
        t.equal(faceCount, 2);

        t.end();
      });

      t.test('Element counts', function (t) {
        const m = new HalfedgeMesh();
        t.equal(m.nVertices(), 0);
        t.equal(m.nInteriorVertices(), 0);
        t.equal(m.nCorners(), 0);
        t.equal(m.nEdges(), 0);
        t.equal(m.nFaces(), 0);
        t.equal(m.nHalfedges(), 0);
        t.equal(m.nInteriorHalfedges(), 0);
        t.equal(m.nExteriorHalfedges(), 0);
        t.equal(m.nBoundaryLoops(), 0);
        m.delete();
        t.end();
      });

      t.test('Properties', function (t) {
        const m = new HalfedgeMesh();
        t.equal(m.hasBoundary(), false);
        t.equal(m.eulerCharacteristic(), 0);
        t.equal(m.genus(), 1);
        t.equal(m.isTriangular(), true);
        t.equal(m.nConnectedComponents(), 0);
        m.delete();
        t.end();
      });

      t.test('copy', function (t) {
        const m = new HalfedgeMesh();
        const m2 = m.copy();
        m2.delete();
        m.delete();
        t.end();
      });
    });

    t.test("Vertex", function (t) {
      t.test("Traversal", function (t) {
        t.end();
      });
    });

    t.test("Iteration", function (t) {
      t.test("vertices", function (t) {
        const m = new HalfedgeMesh();
        m.delete();

        t.end();
      });
    });

    t.test('read', function (t) {
      const soup = PolygonSoupMesh.fromString(OBJ, "obj")
      const mesh = HalfedgeMesh.fromPolygons(soup.polygons());
      const vertices = soup.vertexCoordinates();

      console.log(vertices.get(0));
      console.log(mesh.printStatistics());

      console.log(vec3dataView(vertices));

      soup.delete();
      mesh.delete();

      t.end();
      
    });
  });
});
