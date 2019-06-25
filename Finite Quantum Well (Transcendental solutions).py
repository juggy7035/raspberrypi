"""
Created by Vince Paul Juguilon
CMPL-Semicon
Energy calculator for quantum well (C1/HH1/LH1
02/02/2018
"""

import numpy as np
from scipy.optimize import brentq

mb = float(input("Enter relative electron mass in barrier: "))
mw = float(input("Enter relative electron mass in well: "))
delE = float(input("Enter the potential height (eV): "))
L = float(input("Enter well width in Angstroms: "))
Lw = L*(10**-10)
hbar = 1.054571628*(10**-34)
e = (1.602176487*(10**-19))
me = 9.10938215*(10**-31)
V = e*delE
alpha = np.sqrt(mb/mw)
radius = ((2*mw*me*V)/hbar**2)*(Lw/2)**2
#print(alpha) for mathematica code
#print(radius) for mathematica code

def function(x, A, R):
    return A*x*np.tan(x) -  np.sqrt(R-x**2)

end = 0
while function(end, alpha, radius) < 0:
    end += 0.1

root = brentq(function, 0, end, args=(alpha, radius))

E = (((2*root/Lw)*(hbar))**2)/(2*mw*me)
print("Energy of 1st eigenstate is", E/e, "ev")


