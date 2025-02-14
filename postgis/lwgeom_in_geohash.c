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
 * Copyright 2012 J Smith <dark.panda@gmail.com>
 *
 **********************************************************************/


#include <assert.h>

#include "postgres.h"
#include "utils/builtins.h"

#include "../postgis_config.h"
#include "lwgeom_pg.h"
#include "liblwgeom.h"
#include "liblwgeom_internal.h"/* for decode_geohash_bbox */

extern "C" Datum box2d_from_geohash(PG_FUNCTION_ARGS);
extern "C" Datum point_from_geohash(PG_FUNCTION_ARGS);

static void geohash_lwpgerror(char *msg, __attribute__((__unused__)) int error_code)
{
	POSTGIS_DEBUGF(3, "ST_Box2dFromGeoHash ERROR %i", error_code);
	lwpgerror("%s", msg);
}


static GBOX*
parse_geohash(char *geohash, int precision)
{
	GBOX *box = NULL;
	double lat[2], lon[2];

	POSTGIS_DEBUG(2, "parse_geohash called.");

	if (NULL == geohash)
	{
		geohash_lwpgerror("invalid GeoHash representation", 2);
	}

	decode_geohash_bbox(geohash, lat, lon, precision);

	POSTGIS_DEBUGF(2, "ST_Box2dFromGeoHash sw: %.20f, %.20f", lon[0], lat[0]);
	POSTGIS_DEBUGF(2, "ST_Box2dFromGeoHash ne: %.20f, %.20f", lon[1], lat[1]);

	box = gbox_new(lwflags(0, 0, 1));

	box->xmin = lon[0];
	box->ymin = lat[0];

	box->xmax = lon[1];
	box->ymax = lat[1];

	POSTGIS_DEBUG(2, "parse_geohash finished.");
	return box;
}

PG_FUNCTION_INFO_V1(box2d_from_geohash);
Datum box2d_from_geohash(PG_FUNCTION_ARGS)
{
	GBOX *box = NULL;
	text *geohash_input = NULL;
	char *geohash = NULL;
	int precision = -1;

	if (PG_ARGISNULL(0))
	{
		PG_RETURN_NULL();
	}

	if (!PG_ARGISNULL(1))
	{
		precision = PG_GETARG_INT32(1);
	}

	geohash_input = PG_GETARG_TEXT_P(0);
	geohash = text_to_cstring(geohash_input);

	box = parse_geohash(geohash, precision);

	PG_RETURN_POINTER(box);
}

PG_FUNCTION_INFO_V1(point_from_geohash);
Datum point_from_geohash(PG_FUNCTION_ARGS)
{
	GBOX *box = NULL;
	LWPOINT *point = NULL;
	GSERIALIZED *result = NULL;
	text *geohash_input = NULL;
	char *geohash = NULL;
	double lon, lat;
	int precision = -1;

	if (PG_ARGISNULL(0))
	{
		PG_RETURN_NULL();
	}

	if (!PG_ARGISNULL(1))
	{
		precision = PG_GETARG_INT32(1);
	}

	geohash_input = PG_GETARG_TEXT_P(0);
	geohash = text_to_cstring(geohash_input);

	box = parse_geohash(geohash, precision);

	lon = box->xmin + (box->xmax - box->xmin) / 2;
	lat = box->ymin + (box->ymax - box->ymin) / 2;

	point = lwpoint_make2d(SRID_UNKNOWN, lon, lat);
	result = geometry_serialize((LWGEOM *) point);

	lwfree(box);

	PG_RETURN_POINTER(result);
}
