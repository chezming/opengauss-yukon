set client_min_messages to WARNING;

SELECT public.CreateTopology('tt') > 0;

COPY tt.face(face_id, mbr) FROM STDIN;
1	POLYGON((0 0,0 10,10 10,10 0,0 0))
2	POLYGON((2 2,2 8,8 8,8 2,2 2))
3	POLYGON((12 2,12 8,18 8,18 2,12 2))
\.

COPY tt.node(node_id, geom) FROM STDIN;
1	POINT(2 2)
2	POINT(0 0)
3	POINT(12 2)
\.

COPY tt.edge_data(
	edge_id, start_node, end_node,
	abs_next_left_edge, abs_next_right_edge,
	next_left_edge, next_right_edge,
        left_face, right_face, geom) FROM STDIN;
1	1	1	1	1	1	-1	1	2	LINESTRING(2 2, 2  8,  8  8,  8 2, 2 2)
2	2	2	2	2	2	-2	0	1	LINESTRING(0 0, 0 10, 10 10, 10 0, 0 0)
3	3	3	3	3	3	-3	0	3	LINESTRING(12 2, 12 8, 18 8, 18 2, 12 2)
\.

-- F1 should have an hole !
-- See http://trac.osgeo.org/postgis/ticket/726
SELECT 'f1 (with hole)', ST_asText(public.st_getfacegeometry('tt', 1));
SELECT 'f2 (fill hole)', ST_asText(public.st_getfacegeometry('tt', 2));

-- Universal face has no geometry
-- See http://trac.osgeo.org/postgis/ticket/973
SELECT public.st_getfacegeometry('tt', 0);

-- Null arguments
SELECT public.st_getfacegeometry(null, 1);
SELECT public.st_getfacegeometry('tt', null);

-- Invalid topology names
SELECT public.st_getfacegeometry('NonExistent', 1);
SELECT public.st_getfacegeometry('', 1);

-- Non-existent face
SELECT public.st_getfacegeometry('tt', 666);

-- Face with partial rings
-- See https://trac.osgeo.org/postgis/ticket/4681
COPY tt.face(face_id, mbr) FROM STDIN;
4	POLYGON((0 0,2 2,2 2,0 0))
\.
COPY tt.edge_data(
	edge_id, start_node, end_node,
	abs_next_left_edge, abs_next_right_edge,
	next_left_edge, next_right_edge,
        left_face, right_face, geom) FROM STDIN;
4	2	1	1	2	1	-2	4	4	LINESTRING(0 0, 2 2)
\.
SET client_min_messages TO NOTICE;
SELECT ST_AsText(public.st_getfacegeometry('tt', 4));
SET client_min_messages TO WARNING;

SELECT public.DropTopology('tt');
