import sys
import numpy as np
import pandas as pd

sys.path.append("../../../../../r1basis")
from r1basis import *
from opt_green import *

basis_info = [('id',  True,  2, 0.004-0.02j),
              ("geo", False, 2, 15, 0.01, 1.771600539669121)]

opt_main(basis_type = 'GTO',
         basis_info = basis_info,
         w0 = 1.0,
         ws = np.linspace(0.5, 2.0, 16),
         tol = pow(10.0, -5.0),
         target = 'h_pi',
         channel= '1s->kp',
         dipole = 'length',
         print_level = 2,
         outfile = "res.out",
         wf_outfile = "wf.csv",
         wf_rs = np.linspace(0.0, 40.0, 200))
