/**********************************************************************
 *
 * PostGIS - Spatial Types for PostgreSQL
 * http://postgis.net
 *
 * PostGIS is free software: you can redistribute it and/or modify
 * it under the terms of the GNU General Public License as published by
 * the Free Software Foundation, either version 2 of the License, or
 * (at your option) any later version.
 *
 * PostGIS is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU General Public License for more details.
 *
 * You should have received a copy of the GNU General Public License
 * along with PostGIS.  If not, see <http://www.gnu.org/licenses/>.
 *
 **********************************************************************
 *
 * ^copyright^
 *
 **********************************************************************/

#include "postgres.h"
#include "utils/geo_decls.h"

#include "../postgis_config.h"

#include "liblwgeom.h"         /* For standard geometry types. */
#include "lwgeom_pg.h"       /* For debugging macros. */


extern "C" Datum  geometry_to_point(PG_FUNCTION_ARGS);
extern "C" Datum  point_to_geometry(PG_FUNCTION_ARGS);
extern "C" Datum  geometry_to_path(PG_FUNCTION_ARGS);
extern "C" Datum  path_to_geometry(PG_FUNCTION_ARGS);
extern "C" Datum  geometry_to_polygon(PG_FUNCTION_ARGS);
extern "C" Datum  polygon_to_geometry(PG_FUNCTION_ARGS);

/**
* Cast a PostgreSQL Point to a PostGIS geometry
*/
PG_FUNCTION_INFO_V1(point_to_geometry);
Datum point_to_geometry(PG_FUNCTION_ARGS)
{
	Point *point;
	LWPOINT *lwpoint;
	GSERIALIZED *geom;

	POSTGIS_DEBUG(2, "point_to_geometry called");

	if ( PG_ARGISNULL(0) )
		PG_RETURN_NULL();

	point = PG_GETARG_POINT_P(0);

	if ( ! point )
		PG_RETURN_NULL();

	lwpoint = lwpoint_make2d(SRID_UNKNOWN, point->x, point->y);
	geom = geometry_serialize(lwpoint_as_lwgeom(lwpoint));
	lwpoint_free(lwpoint);

	PG_RETURN_POINTER(geom);
}

/**
* Cast a PostGIS geometry to a PostgreSQL Point
*/
PG_FUNCTION_INFO_V1(geometry_to_point);
Datum geometry_to_point(PG_FUNCTION_ARGS)
{
	Point *point;
	POINT4D pt;
	GSERIALIZED *geom;

	if (PG_ARGISNULL(0))
		PG_RETURN_NULL();
	geom = PG_GETARG_GSERIALIZED_P(0);

	if (gserialized_get_type(geom) != POINTTYPE)
		elog(ERROR, "geometry_to_point only accepts Points");

	if (gserialized_peek_first_point(geom, &pt) == LW_FAILURE)
		PG_RETURN_NULL();

	point = (Point *)palloc(sizeof(Point));
	point->x = pt.x;
	point->y = pt.y;

	PG_RETURN_POINT_P(point);
}

PG_FUNCTION_INFO_V1(geometry_to_path);
Datum geometry_to_path(PG_FUNCTION_ARGS)
{
	PATH *path;
	LWLINE *lwline;
	LWGEOM *lwgeom;
	GSERIALIZED *geom;
	POINTARRAY *pa;
	uint32_t i;
	const POINT2D *pt;
	size_t size;

	POSTGIS_DEBUG(2, "geometry_to_path called");

	if ( PG_ARGISNULL(0) )
		PG_RETURN_NULL();

	geom = PG_GETARG_GSERIALIZED_P(0);

	if ( gserialized_get_type(geom) != LINETYPE )
		elog(ERROR, "geometry_to_path only accepts LineStrings");

	lwgeom = lwgeom_from_gserialized(geom);
	if ( lwgeom_is_empty(lwgeom) )
		PG_RETURN_NULL();
	lwline = lwgeom_as_lwline(lwgeom);

	pa = lwline->points;
    size = offsetof(PATH, p[0]) + sizeof(path->p[0]) * pa->npoints;
    path = (PATH*)palloc(size);
	SET_VARSIZE(path, size);
	path->npts = pa->npoints;
	path->closed = 0;
	path->dummy = 0;

	for ( i = 0; i < pa->npoints; i++ )
	{
		pt = getPoint2d_cp(pa, i);
		(path->p[i]).x = pt->x;
		(path->p[i]).y = pt->y;
	}

	lwgeom_free(lwgeom);
	PG_FREE_IF_COPY(geom,0);

	PG_RETURN_PATH_P(path);
}


PG_FUNCTION_INFO_V1(path_to_geometry);
Datum path_to_geometry(PG_FUNCTION_ARGS)
{
	PATH *path;
	LWLINE *lwline;
	POINTARRAY *pa;
	GSERIALIZED *geom;
	POINT4D pt;
	Point p;
	int i;

	POSTGIS_DEBUG(2, "path_to_geometry called");

	if ( PG_ARGISNULL(0) )
		PG_RETURN_NULL();

	path = PG_GETARG_PATH_P(0);

	if ( ! path )
		PG_RETURN_NULL();

	pa = ptarray_construct_empty(0, 0, path->npts);
	for ( i = 0; i < path->npts; i++ )
	{
		p = path->p[i];
		pt.x = p.x;
		pt.y = p.y;
		ptarray_append_point(pa, &pt, LW_FALSE);
	}
	lwline = lwline_construct(SRID_UNKNOWN, NULL, pa);
	geom = geometry_serialize(lwline_as_lwgeom(lwline));
	lwline_free(lwline);

	PG_RETURN_POINTER(geom);
}

PG_FUNCTION_INFO_V1(geometry_to_polygon);
Datum geometry_to_polygon(PG_FUNCTION_ARGS)
{
	POLYGON *polygon;
	LWPOLY *lwpoly;
	LWGEOM *lwgeom;
	GSERIALIZED *geom;
	POINTARRAY *pa;
	GBOX gbox;
	uint32_t i;
	size_t size;

	POSTGIS_DEBUG(2, "geometry_to_polygon called");

	if ( PG_ARGISNULL(0) )
		PG_RETURN_NULL();

	geom = PG_GETARG_GSERIALIZED_P(0);

	if ( gserialized_get_type(geom) != POLYGONTYPE )
		elog(ERROR, "geometry_to_polygon only accepts Polygons");

	lwgeom = lwgeom_from_gserialized(geom);
	if ( lwgeom_is_empty(lwgeom) )
		PG_RETURN_NULL();
	lwpoly = lwgeom_as_lwpoly(lwgeom);

	pa = lwpoly->rings[0];

	size = offsetof(POLYGON, p[0]) + sizeof(polygon->p[0]) * pa->npoints;
	polygon = (POLYGON*)palloc0(size); /* zero any holes */
	SET_VARSIZE(polygon, size);

	polygon->npts = pa->npoints;

	lwgeom_calculate_gbox(lwgeom, &gbox);
	polygon->boundbox.low.x = gbox.xmin;
	polygon->boundbox.low.y = gbox.ymin;
	polygon->boundbox.high.x = gbox.xmax;
	polygon->boundbox.high.y = gbox.ymax;

	for ( i = 0; i < pa->npoints; i++ )
	{
		const POINT2D *pt = getPoint2d_cp(pa, i);
		(polygon->p[i]).x = pt->x;
		(polygon->p[i]).y = pt->y;
	}

	lwgeom_free(lwgeom);
	PG_FREE_IF_COPY(geom,0);

	PG_RETURN_POLYGON_P(polygon);
}


PG_FUNCTION_INFO_V1(polygon_to_geometry);
Datum polygon_to_geometry(PG_FUNCTION_ARGS)
{
	POLYGON *polygon;
	LWPOLY *lwpoly;
	POINTARRAY *pa;
	POINTARRAY **ppa;
	GSERIALIZED *geom;
	Point p;
	int i = 0, unclosed = 0;

	POSTGIS_DEBUG(2, "polygon_to_geometry called");

	if ( PG_ARGISNULL(0) )
		PG_RETURN_NULL();

	polygon = PG_GETARG_POLYGON_P(0);

	if ( ! polygon )
		PG_RETURN_NULL();

	/* Are first and last points different? If so we need to close this ring */
	if ( memcmp( polygon->p, polygon->p + polygon->npts - 1, sizeof(Point) ) )
	{
		unclosed = 1;
	}

	pa = ptarray_construct_empty(0, 0, polygon->npts + unclosed);

	for ( i = 0; i < (polygon->npts+unclosed); i++ )
	{
		POINT4D pt;
		p = polygon->p[i % polygon->npts];
		pt.x = p.x;
		pt.y = p.y;
		ptarray_append_point(pa, &pt, LW_FALSE);
	}

	ppa = palloc(sizeof(POINTARRAY*));
	ppa[0] = pa;
	lwpoly = lwpoly_construct(SRID_UNKNOWN, NULL, 1, ppa);
	geom = geometry_serialize(lwpoly_as_lwgeom(lwpoly));
	lwpoly_free(lwpoly);

	PG_RETURN_POINTER(geom);
}

