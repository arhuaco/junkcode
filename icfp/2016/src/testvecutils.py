from fracvecutils import *
from fractions import Fraction
q  = mirror_p_vw ( point(Fraction(0),Fraction(1)),
                   point(Fraction(1),Fraction(0)),
                   point(Fraction(1,2),Fraction(0)))
print (q.to_frac_str())

print (segments_intersect (0,0,1,1,0,1,2,0))
print (segments_intersect (0,0,0.5,0.5,0,1,2,0))
