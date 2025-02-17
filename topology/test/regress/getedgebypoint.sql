set client_min_messages to WARNING;

SELECT public.CreateTopology('schema_topo') > 0;

select public.AddEdge('schema_topo',ST_GeomFromText('LINESTRING(1 4, 4 7)')) = 1;
select public.AddEdge('schema_topo',ST_GeomFromText('LINESTRING(4 7, 6 9)')) = 2;
select public.AddEdge('schema_topo',ST_GeomFromText('LINESTRING(2 2, 4 4)')) = 3;
select public.AddEdge('schema_topo',ST_GeomFromText('LINESTRING(4 4, 5 5, 6 6)')) = 4;
select public.AddEdge('schema_topo',ST_GeomFromText('LINESTRING(6 6, 6 9)')) = 5;

-- ask for a Point with tolerance zero
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(5 5)'), 0::float8)::int = 4;

-- ask for a point on an edge
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(5 5)'), 0.1::float8)::int = 4;

-- Ask for a point outside from an edge with a tolerance sufficient to include one edge
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(5 5.5)'), 0.7::float8)::int = 4;

-- Ask for a point where there isn't an edge
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(5 5.5)'), 0::float8) = 0;

-- Failing cases (should all raise exceptions) -------

-- Ask for Point in a Node
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(4 7)'), 1::float8);

-- Ask for a Point with a tollerance too high
select public.GetEdgeByPoint('schema_topo',ST_GeomFromText('POINT(5 5)'), 2::float8);

SELECT public.DropTopology('schema_topo');
