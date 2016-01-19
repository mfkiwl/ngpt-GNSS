Βαγγ,

το repository τώρα έχει 3 branches:

```
master  -> stable release (should always compile!)
dev     -> c++ development
pydev   -> python development
```

Με την εντολή (όταν είσαι μέσα στον φάκελο `ngpt`):

```bash
[xanthos@jpmarat ngpt]$ git branch
  dev
* master
  origin/gcc-484
  pydev
```
σου δείχνει όλα τα διαθέσιμα branches. Μπορεί να μήν έχεις όλα τα branches που υπάρχουν στο github (λογικά θα σου λείπει το `pydev`). Με αστεράκι είναι το branch που δουλεύεις τώρα. Δεν χρειάζεται να υπάρχει 1 προς 1 αντιστοιχία μετάξυ των local και remote branches. Μπορεί στο remote (δλδ. github) να υπαρχουν branches που δεν σε ενδιαφέρουν και δεν θες να τα παρακολουθείς· αντίστοιχα, μπορεί να έχεις δικά σου, local branches (π.χ. `pydev-bug01`, `pydev-bug02`, κτλ ...).

Για να αρχίσεις να *παρακολουθείς* ένα branch (που υπάρχει ήδη στο github), κάνεις:

```bash
   ##  track remote branch pydev at local branch pydev
   ##  local and remote branches need not be named the same (!)
   $> git fetch
   $> git branch --track pydev origin/pydev
```
(βλ. [SO](http://stackoverflow.com/questions/11262703/track-a-new-remote-branch-created-on-github)).

Για να επιβεβαιώ[σ|ν]εις ότι δουλεύεις στο σωστό (local) branch, η εντολή `git branch` σου  δείχνει (με αστεράκι) σε ποιό είσαι.

Αφού κάνεις editing, προσθέσεις τα αρχεία `bin/foo.py` και `src/py/baz.py`, κτλ .... θα θες να κάνεις commit. Συνήθως, εγώ πριν κάνω commit, τρέχω πρώτα `git status`. Σε εμένα για παράδειγμα δείχνει αυτό:
```bash
[xanthos@jpmarat ngpt]$ git status
On branch dev                                                 ## <- σε ποιο branch είμαι
Changes not staged for commit:                                ## <- αλλαγές που **ΔΕΝ** έχουν γίνει commit (ακόμη)
(use "git add <file>..." to update what will be committed)
(use "git checkout -- <file>..." to discard changes in working directory)

modified:   src/ionex.hpp

Untracked files:                                              ## <- αρχεία που **ΔΕΝ** έχουν γίνει tracked
(use "git add <file>..." to include in what will be committed)

    .ycm_extra_conf.py
    .ycm_extra_conf.pyc
    bin/atxtr
    bin/koko.ps
    bin/sepchoke_b3e6.dat
    doc/figures/warnme.png
```
Για να κάνεις το commit:

```bash
   $> git commit -am "describe my commit in some way"
```

Αυτό θα κάνει commit για όλα τα αρχεία που βρίσκονται στο κομμάτι `Changes not staged for commit:`. Αν υπάρχει κάποιο αρχείο στα `Untracked files` το οποίο θες να προσθέσεις στο branch, πρέπει να το προσθέσεις manually πριν κάνεις commit, με την εντολή:

```bash
   $> git add bin/foo.py και src/py/baz.py
   ## now commit !
```
