#!/usr/bin/env python3
# see, e.g.: https://sajidnisar.github.io/posts/python_2rp_jacobian
import sympy as sp

l1, l2, l3, a1, a2, a3 = sp.symbols("l1 l2 l3 a1 a2 a3")


def rot(a):
    return sp.rot_axis3(a).transpose()


def tr(x, y):
    return sp.Matrix([[1, 0, x], [0, 1, y], [0, 0, 1]])

print("== 2-links planar arm ==")

t = rot(a1) * tr(l1, 0) * rot(a2) * tr(l2, 0)
t = sp.simplify(t)
print("\nt=")
sp.pprint(t)

ee = t * sp.Matrix([0, 0, 1])
print("\nee=")
sp.pprint(ee)

j = ee.jacobian([a1, a2, a3]).as_mutable()
j[2,2] = 1
j = sp.simplify(j)
print("\nj=")
sp.pprint(j)

jinv = j.inv()
jinv = sp.simplify(jinv)
print("\nj_inv=")
sp.pprint(jinv)

e_x, e_y, k = sp.symbols("e_x e_y k")
e = sp.Matrix([e_x, e_y, 1])
ctrl = k * jinv * e
print("\nctrl=")
sp.pprint(ctrl)

import code; code.interact(local=locals())

print()
print("== 3-links planar arm ==")

t = rot(a1) * tr(l1, 0) * rot(a2) * tr(l2, 0) * rot(a3) * tr(l3, 0)
t = sp.simplify(t)
print("\nt=")
sp.pprint(t)

ee = t * sp.Matrix([0, 0, 1])
ee = ee[0:2,:]
print("\nee=")
sp.pprint(ee)

j = ee.jacobian([a1, a2, a3]).as_mutable()
j = sp.simplify(j)
print("\nj=")
sp.pprint(j)

jtrans = j.transpose()  # (j.H * j) ** -1 * j.H  # j.inv()
jinv = sp.simplify(jtrans)
print("\nj_trans=")
sp.pprint(jtrans)

e_x, e_y, k = sp.symbols("e_x e_y k")
e = sp.Matrix([e_x, e_y])
ctrl = k * jtrans * e
print("\nctrl=")
sp.pprint(ctrl)

# With the Jacobian we have \dot{x} = J \dot{q}, that is: given the change
# of the configuration (q), the related change on the end-effector (x) is
# obtained multiplying q times J.
# If we have an x_g (workspace end-effector goal) and we know the current x
# (end-effector position), we can compute the error by e = (x_g - x), that
# leads to update the configuration by \dot{q} = kJ^{-1}e.

import code; code.interact(local=locals())
