import cv2
import numpy as np
import pprint
import logging

image_width = 400
image_height = 400
offset = (10,10)
initial_color = 100

def project (v):
  return (int(image_width*v[0] + offset[0]),
          int(image_height*v[1] + offset[1])) 

def init_image ():
  w = image_width+2*offset[0]
  h = image_height+2*offset[1]
  work_image = np.zeros((w, h), np.uint8)
  mask = np.zeros ((w+2, h+2), np.uint8)
  return work_image, mask

def get_vertices_from_segments (segment_list):
  vertices  = []
  for s in segment_list:
    vertices.append (s[0])
    vertices.append (s[1])
  return vertices

def get_vertex_index(vertices):
  vertex_index = {}
  for v in vertices:
    p = project(v)
    if p not in vertex_index:
      vertex_index[p] = [ v ]
    else:
      if v not in vertex_index[p]:
        vertex_index[p].append(v)
  return vertex_index

def draw_segments (segment_list, image):
  for s in segment_list:
    segment_projection = [project(s[0]),project(s[1])]
    logging.debug ('printing segment %s -> %s', pprint.pformat(s), 
                    pprint.pformat(segment_projection)) 
    cv2.line(image, segment_projection[0], 
             segment_projection[1], (255,255,255),1)
#    cv2.rectangle(image, segment_projection[0], 
#             segment_projection[0], (0,0,255),1)

def color_background (image, mask, color):
  cv2.floodFill(image, mask, (0,0), color)

def get_contours (image):
  gray_image = cv2.cvtColor (image, cv2.COLOR_BGR2GRAY)
  c,h = cv2.findContours(gray_image, cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE, )
  return c

def next_color(color):
  n = color[2] + color[1]*256+color[0]*256*256 + 1
  r = n%256
  n -= r
  n /= 256
  g = n%256
  n -= g
  n /= 256
  b = n
  return (b,g,r)

def fill_regions(work_image, mask):
  w = np.argwhere ( work_image == 0 )
  current_color = initial_color
  while  w is not None and len(w)>0:
    p = (w[0][1],w[0][0])
    logging.debug ('Region at %s color: %s',pprint.pformat (p), current_color)
    cv2.floodFill(work_image, mask, p, current_color)
    w = np.argwhere (work_image == 0)
    logging.debug ('Next w: %s',pprint.pformat(w))
    current_color = current_color + 1
  return current_color-initial_color

def closest_vertex(v, vertex_index):
  logging.debug ("Finding closest for %s in %s", str(v), pprint.pformat(vertex_index))
  
  pmin = vertex_index.keys() [0]
  vdif = (v[0] - pmin[0], v[1] - pmin[1])
  dmin = vdif[0]*vdif[0]+ vdif[1]*vdif[1]
  for p in vertex_index.keys()[1:]:
    logging.debug ("Testing %s", str(p))
    vdif = (v[0] - p[0], v[1] - p[1])
    d = vdif[0]*vdif[0]+ vdif[1]*vdif[1]
    if d < dmin:
      logging.debug ("Found minimum : %s", str(p))
      dmin = d
      pmin = p
  logging.debug ("The best found for %s was %s", str(v), str(pmin))
  return vertex_index[pmin]

def get_region_boundary (work_image, current_color, vertex_index):
  image_copy = work_image.copy ()
  image_copy[work_image != current_color] = 0
  image_copy[work_image == current_color] = 255
  c,h = cv2.findContours(image_copy,cv2.RETR_TREE,cv2.CHAIN_APPROX_SIMPLE)
  region = []
  for cv in c[0]:
    v = (cv[0][0],cv[0][1])
    closest = closest_vertex (v, vertex_index)
    logging.info ("Vertex : (%s,%s)", str(cv[0][0]), str(cv[0][1]))
    logging.info ("Closest Vertex : %s", closest)
    region.append(closest)
  
  logging.debug ("region: %s", pprint.pformat(region)) 
  return region

def get_facets (segment_list):
  work_image, mask = init_image ()
  vertices = get_vertices_from_segments(segment_list)
  vertex_index = get_vertex_index(vertices)
  draw_segments (segment_list, work_image)
  logging.debug  ("searching")
  nregions = fill_regions(work_image, mask)
  logging.debug ("Found %d regions", nregions)
  current_color = initial_color
  facets = []
  for i in range(nregions):
    boundary = get_region_boundary (work_image, initial_color+i, vertex_index)
    facets.append (boundary)
  logging.debug ("Facets: %s", pprint.pformat (facets))
  return facets


