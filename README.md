# make n sigma proton shift hist

`version 2.0`

`author: yghuang`

### Totally New Version!

Based on QA hist v2.3, the shift maker is updated.

Now will not consider the run-ID dependence.

Revision: 27.07.2023

### Introcution

1. Modify settings in `conf.py` and run `python3 MakeDbConf.py`.

2. Using `./prepare.sh PATH` and `star-submit -u ie Csubmit.xml` to submit the jobs.

3. Will create the reduced qa histograms, and all the qa processes can be done based on this. 

### Change log

2023 July 27 by yghuang (2.0):

> New shift maker.

