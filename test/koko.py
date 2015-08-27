#! /usr/bin/python

i = -3.0
xfrom = -5
xto = 5
dx = 2.5

while i <= 1.0:
  indx = int((i - xfrom) / dx)
  indx2 = int( (i -dx/2.0 - xfrom) / dx)
  print 'at %+4.2f -> %1i = %+4.2f -> %4.2f' %(i, indx, indx*dx + xfrom, indx2*dx + xfrom)
  i = i + .1
