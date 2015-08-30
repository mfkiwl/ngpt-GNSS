#! /usr/bin/python

class Grid:
  
  def __init__(self, f, t, s):
    self.frm  = f
    self.to   = t
    self.step = s
    self.pts  = int((t - f) / s) + 1

  def print_grid(self):
    if self.is_ascending():
      x = self.frm
      while x <= self.to :
        print '-- %+3.1f ' %( x ),
        x += self.step
      print '--'
    else:
      x = self.frm
      while x >= self.to :
        print '-- %+3.1f ' %( x ),
        x += self.step
      print '--'

    for i in range(0, self.pts):
      print '-- %4i ' %( i ),
    print '--'

  def is_ascending(self):
    return bool(self.to > self.frm)

  ## In the sequence [frm, to] return the left node just before
  ## the given value.
  ## In an ascending grid, this means that the function will return the
  ## largest node for which: 
  ## node_value <= x
  ## In an scending grid, the function will return the smallest node, for which
  ## node_value > x
  def left_node(self, x):
    idx = int((x - self.frm) / self.step)
    return idx, idx*self.step + self.frm

  def nearest_node(self, x):
    idx = int((x + (self.step/2) - self.frm) / self.step)
    return idx, idx*self.step + self.frm



## Test ##
x = Grid(-5.0, 5.0, 2.5)
y = Grid(5.0, -5.0, -2.5)

x.print_grid()
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(-6, x.left_node(-6)[1], x.left_node(-6)[0])
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(-4.9, x.left_node(-4.9)[1], x.left_node(-4.9)[0])
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(-2.4, x.left_node(-2.4)[1], x.left_node(-2.4)[0])
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(-2.6, x.left_node(-2.6)[1], x.left_node(-2.6)[0])
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(-.1, x.left_node(-.1)[1], x.left_node(-.1)[0])
print "Grid x: left node for %+3.1f is %+3.1f at index %02i" %(2.6, x.left_node(2.6)[1], x.left_node(2.6)[0])

print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(-6, x.nearest_node(-6)[1], x.nearest_node(-6)[0])
print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(-4.9, x.nearest_node(-4.9)[1], x.nearest_node(-4.9)[0])
print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(-2.4, x.nearest_node(-2.4)[1], x.nearest_node(-2.4)[0])
print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(-2.6, x.nearest_node(-2.6)[1], x.nearest_node(-2.6)[0])
print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(-.1, x.nearest_node(-.1)[1], x.nearest_node(-.1)[0])
print "Grid x: nearest node for %+3.1f is %+3.1f at index %02i" %(2.6, x.nearest_node(2.6)[1], x.nearest_node(2.6)[0])

y.print_grid()
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(-6, y.left_node(-6)[1], y.left_node(-6)[0])
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(-4.9, y.left_node(-4.9)[1], y.left_node(-4.9)[0])
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(-2.4, y.left_node(-2.4)[1], y.left_node(-2.4)[0])
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(-2.6, y.left_node(-2.6)[1], y.left_node(-2.6)[0])
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(-.1, y.left_node(-.1)[1], y.left_node(-.1)[0])
print "Grid y: left node for %+3.1f is %+3.1f at index %02i" %(2.6, y.left_node(2.6)[1], y.left_node(2.6)[0])

print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(-6, y.nearest_node(-6)[1], y.nearest_node(-6)[0])
print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(-4.9, y.nearest_node(-4.9)[1], y.nearest_node(-4.9)[0])
print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(-2.4, y.nearest_node(-2.4)[1], y.nearest_node(-2.4)[0])
print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(-2.6, y.nearest_node(-2.6)[1], y.nearest_node(-2.6)[0])
print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(-.1, y.nearest_node(-.1)[1], y.nearest_node(-.1)[0])
print "Grid y: nearest node for %+3.1f is %+3.1f at index %02i" %(2.6, y.nearest_node(2.6)[1], y.nearest_node(2.6)[0])
