--BEGIN;

-- More parcels

INSERT INTO features.land_parcels(feature_name, feature) VALUES ('F3', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    3, -- Topology geometry type (polygon/multipolygon)
    1, -- TG_LAYER_ID for this topology (from public.layer)
    '{{3,3}}') -- face_id:3
    );

INSERT INTO features.land_parcels(feature_name, feature) VALUES ('F6', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    3, -- Topology geometry type (polygon/multipolygon)
    1, -- TG_LAYER_ID for this topology (from public.layer)
    '{{6,3}}') -- face_id:3
    );

INSERT INTO features.land_parcels(feature_name, feature) VALUES ('F3F4', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    3, -- Topology geometry type (polygon/multipolygon)
    1, -- TG_LAYER_ID for this topology (from public.layer)
    '{{3,3},{4,3}}') -- face_id:3 face_id:4
    );

INSERT INTO features.land_parcels(feature_name, feature) VALUES ('F1', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    3, -- Topology geometry type (polygon/multipolygon)
    1, -- TG_LAYER_ID for this topology (from public.layer)
    '{{1,3}}') -- face_id:1
    );

-- More TRAFFIC_SIGNS

INSERT INTO features.traffic_signs(feature_name, feature) VALUES ('N1N2N3', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    1, -- Topology geometry type (point)
    2, -- TG_LAYER_ID for this topology (from public.layer)
    '{{1,1},{2,1},{3,1}}'));

INSERT INTO features.traffic_signs(feature_name, feature) VALUES ('N1N6N14', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    1, -- Topology geometry type (point)
    2, -- TG_LAYER_ID for this topology (from public.layer)
    '{{1,1},{6,1},{14,1}}'));

INSERT INTO features.traffic_signs(feature_name, feature) VALUES ('N3N4', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    1, -- Topology geometry type (point)
    2, -- TG_LAYER_ID for this topology (from public.layer)
    '{{3,1},{4,1}}'));

INSERT INTO features.traffic_signs(feature_name, feature) VALUES ('N4', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    1, -- Topology geometry type (point)
    2, -- TG_LAYER_ID for this topology (from public.layer)
    '{{4,1}}'));

-- More STREETS

INSERT INTO features.city_streets(feature_name, feature) VALUES ('E7E8', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    2, -- Topology geometry type (line string)
    3, -- TG_LAYER_ID for this topology (from public.layer)
    '{{7,2},{8,2}}'));

INSERT INTO features.city_streets(feature_name, feature) VALUES ('E20E19', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    2, -- Topology geometry type (line string)
    3, -- TG_LAYER_ID for this topology (from public.layer)
    '{{20,2},{19,2}}'));

INSERT INTO features.city_streets(feature_name, feature) VALUES ('E25', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    2, -- Topology geometry type (line string)
    3, -- TG_LAYER_ID for this topology (from public.layer)
    '{{-25,2}}'));

INSERT INTO features.city_streets(feature_name, feature) VALUES ('R1a', -- Feature name
  public.CreateTopoGeom(
    'city_data', -- Topology name
    2, -- Topology geometry type (line string)
    3, -- TG_LAYER_ID for this topology (from public.layer)
    '{{10,2},{-9,2}}'));

--END;

