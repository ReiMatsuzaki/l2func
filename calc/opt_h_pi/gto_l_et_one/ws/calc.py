import sys
import numpy as np
import pandas as pd

sys.path.append("../../../r1basis")
from r1basis import *
from opt_green import *

def et_zeta(num, z0, z1):
    r = (z1/z0)**(1.0/(num-1))
    return [z0*r**n for n in range(num)]

## this parameter is used with fitted GTOs in H2plus calculation.
et_zeta = et_zeta(15, 0.01, 30.0)
basis_info = [(2, 0.004-0.02j, 'o')] + [(2, z, 'f') for z in et_zeta]

opt_main(basis_type = 'GTO',
         basis_info = basis_info,
         w0 = 1.0,
         ws = np.linspace(0.5, 1.5, 11),
         tol = pow(10.0, -5.0),
         target = 'h_pi',
         channel= '1s->kp',
         dipole = 'length',
         print_level = 0,
         outfile = "res_range.out")

