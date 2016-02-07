#! /usr/bin/python

import sys, re

time_dict = {}

for tmfile in sys.argv[1:]:
    with open(tmfile, 'r') as fin:
        real_secs   = 0.0e0; real_nr = 0;
        user_secs   = 0.0e0; user_nr = 0;
        sys_secs    = 0.0e0; sys_nr  = 0;
        description = ""
        for line in fin.readlines():
            l = line.split()
            if len(line) >2 and line[0:2] == "++":
                description += line[2:]
            if len(l) <= 1:
                pass
            else:
                m = re.match(r"([0-9.]*)m([0-9.]*)s", l[1])
                if   l[0] == 'real':
                    real_nr   += 1
                    real_secs = real_secs + (
                                (float(m.group(2)) + float(m.group(1))*60.0)-real_secs) / real_nr
                elif l[0] == 'user':
                    user_nr   += 1
                    user_secs = user_secs + (
                                 (float(m.group(2)) + float(m.group(1))*60.0)-user_secs) / user_nr
                elif l[0] == 'sys':
                    sys_nr    += 1
                    sys_secs  = sys_secs + (
                                 (float(m.group(2)) + float(m.group(1))*60.0)-sys_secs) / sys_nr
 
        if not ( real_nr == user_nr and user_nr == sys_nr ):
            print >> sys.stderr, 'WARNING! Different real/user/sys entries in file', tmfile
        time_dict[tmfile] = [ real_secs, user_secs, sys_secs, real_nr, description ]

best_user = ['',sys.float_info.max]
best_real = ['',sys.float_info.max]
best_sys  = ['',sys.float_info.max]
for file, lst in time_dict.iteritems():
    if lst[0] < best_real : best_real = [ file, lst[0] ]
    if lst[1] < best_user : best_user = [ file, lst[1] ]
    if lst[2] < best_sys  : best_sys  = [ file, lst[2] ]
perc_dict = {}
for file, lst in time_dict.iteritems():
    perc_dict[file] = [ (lst[0]-best_real[1])/best_real[1],
                        (lst[1]-best_user[1])/best_user[1],
                        (lst[2]-best_sys[1])/best_sys[1] ]

print ''
print 'File Descriptions (if available):'
for file, lst in time_dict.iteritems():
    print file,':',lst[4].strip()
print ''

print 'Average running time.'
dummy_line = '+ {0:20s}+-{1:10s}+-{2:10s}+-{3:10s}+-{4:6s}+'.format('-'*20, '-'*10, '-'*10, '-'*10, '-'*6)
print dummy_line
print '| {0:20s}| {1:10s}| {2:10s}| {3:10s}| {4:6s}| sec'.format('filename', 'real time', 'user time', 'sys time', 'sample')
print dummy_line
for file, lst in time_dict.iteritems():
    print '| {0:20s}| {1:10.6f}| {2:10.6f}| {3:10.6f}| {4:6d}|'.format(file, lst[0], lst[1], lst[2], lst[3])
print dummy_line

print ''
print 'Percentage (w.r.t. best performing)'
dummy_line = '+ {0:20s}+-{1:5s}+-{2:5s}+-{3:5s}+'.format('-'*20, '-'*5, '-'*5, '-'*5)
print dummy_line
print '| {0:20s}| {1:5s}| {2:5s}| {3:5s}| percent (%)'.format('filename', 'real', 'user', 'sys')
print dummy_line
for file, lst in perc_dict.iteritems():
    print '| {0:20s}| {1:5.2f}| {2:5.2f}| {3:5.2f}|'.format(file, lst[0]*100, lst[1]*100, lst[2]*100)
print dummy_line
