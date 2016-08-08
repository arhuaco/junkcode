import facet_finder as ff
import logging
import pprint 
import cv2
import numpy as np

def get_spaced_colors(n):
    max_value = 16581375 #255**3
    interval = int(max_value / n)
    colors = [hex(I)[2:].zfill(6) for I in range(0, max_value, interval)]
    
    return [(int(i[:2], 16), int(i[2:4], 16), int(i[4:], 16)) for i in colors]


FORMAT = '%(asctime)-15s %(module)s %(lineno)d %(message)s'
logging.basicConfig(format=FORMAT, level=logging.DEBUG)

facets = ff.get_facets ([ [(0,0),(0.5,0.5)],
                 [(0,0),(1,0)],
                 [(0,0),(0,1)],
                 [(0.5,0.5),(0,1)],
                 [(0.5,0.5),(1,0)],
                 [(0.5,0.5),(1,1)],
                 [(0,1),(1,1)],
                 [(1,0),(1,1)]
])

w = ff.image_width+ff.offset[0]*2
h = ff.image_height+ff.offset[1]*2 
logging.debug ("w: %d, h: %d", w, h)
image = np.zeros((w, h, 3), np.uint8)
colors = get_spaced_colors (len(facets)+1)
color_index = 1
for f in facets: 
  polygon = np.array ([ff.project(x[0]) for x in f])
  logging.debug ('Polygon %s', pprint.pformat(polygon))
  color = colors[color_index%len(colors)]
  logging.debug ('color %s',pprint.pformat(color))
  cv2.fillPoly(image, [ polygon ], color)
  color_index = color_index + 1
  for i in range (len(f)):
    initial = f[i][0]
    final = f[(i+1)%len(f)][0]
    logging.debug ('initial : %s', pprint.pformat(initial))
    logging.debug ('final : %s', pprint.pformat(final))
    cv2.line (image, ff.project(initial), ff.project(final), (255,255,0), 1)

cv2.imwrite ('facets.jpg', image)
cv2.imshow ('regions', image)
cv2.waitKey (0)
