\set VERBOSITY terse
set client_min_messages to WARNING;

select 'create', createtopology('tt') > 0;

-- Invalid calls
select totopogeom('POINT(0 0)'::geometry, 'unexistent', 1);
select totopogeom('POINT(0 0)'::geometry, 'tt', 1);
select totopogeom(null, 'tt', 1);
select totopogeom('POINT(0 0)'::geometry, '', 1);
select totopogeom('POINT(0 0)'::geometry, null, 1);
select totopogeom('POINT(0 0)'::geometry, 'tt', null::integer);

-- Create simple puntual layer (will be layer 1)
CREATE TABLE tt.f_puntal(id serial);
SELECT 'simple_puntual_layer', AddTopoGeometryColumn('tt', 'tt', 'f_puntal','g','POINT');

-- Create a hierarchical layer (will be layer 2)
CREATE TABLE tt.f_hier(id serial);
SELECT 'hierarchical_layer', AddTopoGeometryColumn('tt', 'tt', 'f_hier','g','COLLECTION', 1);

-- Create a lineal layer (will be layer 3)
CREATE TABLE tt.f_lineal(id serial);
SELECT 'simple_lineal_layer', AddTopoGeometryColumn('tt', 'tt', 'f_lineal','g','LINE');

-- Create an areal layer (will be layer 4)
CREATE TABLE tt.f_areal(id serial);
SELECT 'simple_areal_layer', AddTopoGeometryColumn('tt', 'tt', 'f_areal','g','POLYGON');

-- Create a collection layer (will be layer 5)
CREATE TABLE tt.f_coll(id serial);
SELECT 'simple_collection_layer', AddTopoGeometryColumn('tt', 'tt', 'f_coll','g','COLLECTION');

-- A couple more invalid calls
select totopogeom('POINT(0 0)'::geometry, 'tt', 30); -- non existent layer
select totopogeom('POINT(0 0)'::geometry, 'tt', 2); -- invalid (hierarchical) layer
select totopogeom('LINESTRING(0 0, 10 10)'::geometry, 'tt', 1); -- invalid (puntual) layer
select totopogeom('LINESTRING(0 0, 10 10)'::geometry, 'tt', 4); -- invalid (areal) layer
select totopogeom('MULTIPOINT(0 0, 10 10)'::geometry, 'tt', 3); -- invalid (lineal) layer
select totopogeom('MULTIPOINT(0 0, 10 10)'::geometry, 'tt', 4); -- invalid (areal) layer
select totopogeom('POLYGON((0 0, 10 10, 10 0, 0 0))'::geometry, 'tt', 1); -- invalid (puntal) layer
select totopogeom('POLYGON((0 0, 10 10, 10 0, 0 0))'::geometry, 'tt', 3); -- invalid (lineal) layer
-- Unsupported feature types
select totopogeom('TIN( ((0 0 0, 0 0 1, 0 1 0, 0 0 0)), ((0 0 0, 0 1 0, 1 1 0, 0 0 0)) )'::geometry, 'tt', 4); -- TODO: support !
select totopogeom('TRIANGLE ((0 0, 0 9, 9 0, 0 0))'::geometry, 'tt', 4); -- TODO: support !
select totopogeom('CIRCULARSTRING(0 0, 1 1, 1 0)'::geometry, 'tt', 3); -- Unsupported feature type

-- Convert a point
with inp as ( select 'POINT(0 0)' ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 1)::geometry, g) from inp;

-- Convert a line
with inp as ( select 'LINESTRING(0 10, 10 10)' ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 3)::geometry, g) from inp;

-- Convert a polygon
with inp as ( select
'POLYGON((0 20, 10 20, 5 30, 0 20),(2 22, 8 22, 5 28, 2 22))'
 ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 4)::geometry, g) from inp;

-- Convert a multipoint
with inp as ( select 'MULTIPOINT((0 -10),(5 -10))' ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 1)::geometry, g) from inp;

-- Convert a multiline
with inp as ( select 'MULTILINESTRING((-1 10, -10 10),(-10 8, -2 9))' ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 3)::geometry, g) from inp;

-- Convert a multipolygon
with inp as ( select
'MULTIPOLYGON(((100 20, 110 20, 105 30, 100 20),(102 22, 108 22, 105 28, 102 22)),((80 20, 90 20, 90 60, 80 20)))'
 ::geometry as g)
select St_AsText(g), ST_Equals(totopogeom(g, 'tt', 4)::geometry, g) from inp;

-- Convert a collection
with
inp as ( select
'GEOMETRYCOLLECTION(
 POINT(-100 -100),
 LINESTRING(-100 -90,-90 -90),
 POLYGON((-100 -80,-90 -80,-95 -70,-100 -80),(-98 -78,-92 -78,-95 -72,-98 -78)),
 MULTIPOINT(-100 -110,-95 -110),
 LINESTRING EMPTY,
 MULTILINESTRING((-101 -90,-110 -90),(-110 -92,-102 -91)),
 MULTIPOLYGON(((0 -80,10 -80,5 -70,0 -80),(2 -78,8 -78,5 -72,2 -78)),((-20 -80,-10 -80,-10 -40,-20 -80)))
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select St_AsText(inp.g), st_astext(ST_Normalize(tg.g::geometry)) from inp, tg;

-- Convert some empties
SELECT ST_AsText(toTopoGeom('POINT EMPTY', 'tt', 1)::geometry);
SELECT ST_AsText(toTopoGeom('MULTIPOINT EMPTY', 'tt', 1)::geometry);
SELECT ST_AsText(toTopoGeom('LINESTRING EMPTY', 'tt', 3)::geometry);
SELECT ST_AsText(toTopoGeom('MULTILINESTRING EMPTY', 'tt', 3)::geometry);
SELECT ST_AsText(toTopoGeom('POLYGON EMPTY', 'tt', 4)::geometry);
SELECT ST_AsText(toTopoGeom('MULTIPOLYGON EMPTY', 'tt', 4)::geometry);
SELECT ST_AsText(toTopoGeom('GEOMETRYCOLLECTION EMPTY', 'tt', 5)::geometry);

-- Test with tolerance (expect to snap to POINT(-100 -100)
with inp as ( select
'GEOMETRYCOLLECTION(
 POINT(-100 -100.5),
 LINESTRING(-100 -90,-90 -90.5),
 POLYGON((-100 -80,-89.5 -80,-95 -70,-100 -80),(-98 -78,-92 -78,-95 -72.5,-98 -78))
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5, 1) as g from inp )
select 'tolerance_1', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;

-- Test with tolerance specified in topology record
UPDATE public.topology SET precision=1 WHERE name = 'tt';
with inp as ( select
'GEOMETRYCOLLECTION(
 POINT(-100 -100.5),
 LINESTRING(-100 -90,-90 -90.5),
 POLYGON((-100 -80,-89.5 -80,-95 -70,-100 -80),(-98 -78,-92 -78,-95 -72.5,-98 -78))
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select 'tolerance_topo_1', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;

-- Test without tolerance (expect to remain POINT(-100 -100.5)
UPDATE public.topology SET precision=0 WHERE name = 'tt';
with inp as ( select
'GEOMETRYCOLLECTION(
 POINT(-100 -100.5),
 LINESTRING(-100 -90,-90 -90.5),
 POLYGON((-100 -80,-89.5 -80,-95 -70,-100 -80),(-98 -78,-92 -78,-95 -72.5,-98 -78))
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select 'tolerance_0', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;

-- Test usage with custom search_path (#1763)
set search_path to "public";
with inp as ( select 'POINT(-100 -100.5)'::geometry as g),
tg as ( select public.totopogeom(g, 'tt', 1) as g from inp )
select 'custom_search_path', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;
reset search_path;

-- http://trac.osgeo.org/postgis/ticket/1790
UPDATE public.topology SET precision=0 WHERE name = 'tt';
with inp as ( select
'GEOMETRYCOLLECTION(
 POINT(200 200),
 POINT(200 200)
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select '#1790.1', ST_HausdorffDistance(inp.g, tg.g::geometry), ST_HausdorffDistance(tg.g::geometry, inp.g) FROM inp, tg;
with inp as ( select
'GEOMETRYCOLLECTION(
 LINESTRING(300 300, 310 300),
 LINESTRING(300 300, 310 300)
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select '#1790.2', ST_HausdorffDistance(inp.g, tg.g::geometry), ST_HausdorffDistance(tg.g::geometry, inp.g) FROM inp, tg;
with inp as ( select
'GEOMETRYCOLLECTION(
 POLYGON((400 400, 450 450, 500 400, 400 400)),
 POLYGON((400 400, 450 450, 500 400, 400 400))
)'
 ::geometry as g),
tg as ( select totopogeom(g, 'tt', 5) as g from inp )
select '#1790.3', ST_HausdorffDistance(inp.g, tg.g::geometry), ST_HausdorffDistance(tg.g::geometry, inp.g) FROM inp, tg;

-- http://trac.osgeo.org/postgis/ticket/1968
with inp as ( select
'MULTILINESTRING ((0 0, 10 0),(5 0, 5 5))'
::geometry as g ),
tg as ( select totopogeom(g, 'tt', 3) as g from inp )
SELECT '#1968.1', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;
with inp as ( select
ST_Translate(
'MULTILINESTRING ((0 0, 10 0),(5 0, 5 5),(0 0, 5 0),(5 0, 10 0))'
::geometry, 20, 0) as g ),
tg as ( select totopogeom(g, 'tt', 3) as g from inp )
SELECT '#1968.2', ST_HausdorffDistance(inp.g, tg.g::geometry) FROM inp, tg;

-- Test adding portions to an existing TopoGeometry
INSERT INTO tt.f_areal (id, g)
 SELECT -1, toTopoGeom('POLYGON((0 0, 10 0, 10 10, 0 10, 0 0))', 'tt', 4);
SELECT 'tgup1.1', id(t.g), st_area(t.g), count(r.*)
  FROM tt.f_areal t, tt.relation r
  WHERE t.id = -1 AND r.layer_id = 4 AND r.topogeo_id = id(t.g)
  GROUP BY id(t.g), st_area(t.g);
UPDATE tt.f_areal SET g = toTopoGeom(st_translate(g, st_xmax(g::geometry)+1, 0), g);
SELECT 'tgup1.2', id(t.g), st_area(t.g), count(r.*)
  FROM tt.f_areal t, tt.relation r
  WHERE t.id = -1 AND r.layer_id = 4 AND r.topogeo_id = id(t.g)
  GROUP BY id(t.g), st_area(t.g);
-- now add a smaller area
UPDATE tt.f_areal SET g = toTopoGeom(st_buffer(g, -1), g);
SELECT 'tgup1.3', id(t.g), st_area(t.g), count(r.*)
  FROM tt.f_areal t, tt.relation r
  WHERE t.id = -1 AND r.layer_id = 4 AND r.topogeo_id = id(t.g)
  GROUP BY id(t.g), st_area(t.g);

-- Check GeometryType of TopoGeometries getting
-- other TopoGeometry types added
-- See https://trac.osgeo.org/postgis/ticket/4854
BEGIN;
SELECT '#4854.0.0', GeometryType(
    toTopoGeom(
      'POINT(0 0)', -- insert a point
      toTopoGeom(
        'POINT(10 0)', -- to a point TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.0.1', GeometryType(
    toTopoGeom(
      'POINT(0 0)', -- insert a point
      toTopoGeom(
        'LINESTRING(0 0, 10 0)', -- to a linear TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.0.2', GeometryType(
    toTopoGeom(
      'POINT(0 0)', -- insert a point
      toTopoGeom(
        'POLYGON((0 0,10 0,10 10,0 10,0 0))', -- to a polygonal TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.0.3', GeometryType(
    toTopoGeom(
      'POINT(0 0)', -- insert a point
      toTopoGeom(
        'GEOMETRYCOLLECTION(POLYGON((0 0,10 0,10 10,0 10,0 0)),POINT(5 5))', -- to a collection TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.1.0', GeometryType(
    toTopoGeom(
      'LINESTRING(10 0, 20 0)', -- insert a line
      toTopoGeom(
        'POINT(0 0)', -- to a point TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.1.1', GeometryType(
    toTopoGeom(
      'LINESTRING(10 0, 20 0)', -- insert a line
      toTopoGeom(
        'LINESTRING(0 0, 0 10)', -- to a line TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.1.2', GeometryType(
    toTopoGeom(
      'LINESTRING(10 0, 20 0)', -- insert a line
      toTopoGeom(
        'POLYGON((0 0, 10 0,10 10,0 10,0 0))', -- to a polygonal TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.1.3', GeometryType(
    toTopoGeom(
      'LINESTRING(10 0, 20 0)', -- insert a line
      toTopoGeom(
        'GEOMETRYCOLLECTION(POINT(0 0),LINESTRING(0 0, 10 0))', -- to a collection TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.2.0', GeometryType(
    toTopoGeom(
      'POLYGON((100 0,100 10,110 10,110 0, 100 0))', -- insert a polygon
      toTopoGeom(
        'POINT(100 1)', -- to a puntal TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.2.1', GeometryType(
    toTopoGeom(
      'POLYGON((-1 0,0 -2,1 0,-1 0))', -- insert a polygon
      toTopoGeom(
        'LINESTRING(200 1, 200 2)', -- to a lineal TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.2.2', GeometryType(
    toTopoGeom(
      'POLYGON((-1 0,0 -2,1 0,-1 0))', -- insert a polygon
      toTopoGeom(
        'POLYGON((200 1, 200 5,210 2,200 1))', -- to a polygon TopoGeom
        'tt',
        5
      ),
      5)
  );
SELECT '#4854.2.3', GeometryType(
    toTopoGeom(
      'POLYGON((-1 0,0 -2,1 0,-1 0))', -- insert a polygon
      toTopoGeom(
        'GEOMETRYCOLLECTION(POINT(0 0),LINESTRING(0 0,10 0))', -- to a collection TopoGeom
        'tt',
        5
      ),
      5)
  );
ROLLBACK;

--------------------------------------------------------
-- http://trac.osgeo.org/postgis/ticket/3359
--------------------------------------------------------
-- NOTE: requires identifier of the second edge to be 2
TRUNCATE tt.relation CASCADE;
TRUNCATE tt.edge_data CASCADE;
TRUNCATE tt.node CASCADE;
DELETE FROM tt.face WHERE face_id > 0;
SELECT '#3359.setval',
       setval('tt.edge_data_edge_id_seq', 1, false),
       -- face_id is intentionally set to 2
       setval('tt.face_face_id_seq', 2, false),
       setval('tt.node_node_id_seq', 1, false);
SELECT '#3359.line.1', ST_Length(toTopoGeom('LINESTRING (0 0,1 0)'
::geometry, 'tt', 3));
SELECT '#3359.line.2', ST_Length(toTopoGeom('LINESTRING (0 0,1 0,1 1)'
::geometry, 'tt', 3));
SELECT '#3359.area.1', ST_Area(toTopoGeom('POLYGON ((0 0,1 0,1 1,0 1,0 0))'
::geometry, 'tt', 4));
SELECT '#3359.area.2', ST_Area(toTopoGeom('POLYGON ((0 0,1 0,1 2,0 2,0 0))'
::geometry, 'tt', 4));

--------------------------------------------------------
-- http://trac.osgeo.org/postgis/ticket/4884
--------------------------------------------------------
TRUNCATE tt.relation CASCADE;
TRUNCATE tt.edge_data CASCADE;
TRUNCATE tt.node CASCADE;
DELETE FROM tt.face WHERE face_id > 0;
SELECT '#4884.setval',
       setval('tt.edge_data_edge_id_seq', 1, false),
       -- face_id is intentionally set to 2
       setval('tt.face_face_id_seq', 2, false),
       setval('tt.node_node_id_seq', 1, false);
SELECT '#4884.l2r', ST_AsText(toTopoGeom('LINESTRING (0 0,1 0)' ::geometry, 'tt', 3));
SELECT '#4884.r2l', ST_AsText(toTopoGeom('LINESTRING (1 0,0 0)' ::geometry, 'tt', 3));

--------------------------------------------------------
-- Cleanups
--------------------------------------------------------

DROP TABLE tt.f_coll;
DROP TABLE tt.f_areal;
DROP TABLE tt.f_lineal;
DROP TABLE tt.f_hier;
DROP TABLE tt.f_puntal;
select droptopology('tt');
