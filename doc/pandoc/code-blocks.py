#! /usr/bin/python

## argv1 = html filename

## need to find multiline html source code blocks of type:
## <pre class="shell"><code> ...
## ...
## ...
## </code></pre>

import sys
from bs4 import BeautifulSoup, Tag

xanthos_d = { 'name': 'Xanthos P',
              'f_name': 'Xanthos Papanikolaou',
              'mail': 'xanthos@mail.ntua.gr' }
mitsos_d  = { 'name': 'Demitris A',
              'f_name': 'Demtris Anastasiou',
              'mail': 'danast@mail.ntua.gr' }

def is_multiline( s ): return True if s.string.rfind('\n') > 1 else False

def match_author(atr):
    for d in [ xanthos_d, mitsos_d ]:
        if d['name'] == atr : return d
    raise RuntimeError

soup = BeautifulSoup( open(sys.argv[1]) )

tags = soup.find_all("code")
for t in tags :
    if is_multiline( t ):
        if t.parent['class'] == ['shell']:
            t.parent['class'] = ['mlsc']
    else:
        t['class'] = ['inline-src']

#tags = soup.find_all("h2")
#author_list = []
#for t in soup.find_all("h2"):
#    try:
#        if t['class'] == ['author']:
#            try:
#                adict  = match_author( t.contents[0] )
#                content= "<a hfref=\"mailto:%s\" target=\"_top\">%s</a>"%(
#                    adict['mail'], adict['f_name'])
#                print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
#                #print 'Author string:', astr
#                print '%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%%'
#                t.contents = [ content ]
#            except:
#                print >> sys.stderr, 'ERROR. failed for author:', t.contents[0]
#                sys.exit(1)
#    except:
#        pass

print (soup)
