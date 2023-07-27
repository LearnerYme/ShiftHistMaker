import os
import datetime
from yLog import yLog

# HERE ARE CONFIGURATIONS

runList = './full.run.list'

triggers = [
    710000, 710010, 710020
]
    
# Now make it

l = yLog('.make_db_conf.log')
l.log('Trying to make database config.')

if os.path.exists('StRoot/StYQAMaker/DbConf.h'):
    l.log('StRoot/StYQAMaker/DbConf.h already exists, deleting it.')
    os.remove('StRoot/StYQAMaker/DbConf.h')

if not os.path.exists(runList):
    l.log(f'Invalid run list ({runList}).')
    raise Exception(f'{runList = } which did not exist!')

runs = open(runList).readlines()
runs = [item.strip() for item in runs]
nRuns = len(runs)
l.log(f'{nRuns} runs found in {runList}.')

with open('StRoot/StYQAMaker/DbConf.h', 'w') as f:
    f.write('#ifndef __DbConf__\n')
    f.write('#define __DbConf__\n')

    f.write('\n\n')
    f.write('\t/*\n\t\tInspired by Fan SI\'s structure.\n')
    f.write('\t\tAutomated generated by Yige HUANG\'s python script.\n')
    f.write(f'\t\tDate of the issue: {datetime.datetime.now().ctime()}\n')
    f.write('\t*/\n') # end of the description
    
    f.write('\n\n')
    f.write('#include <map>\n')
    f.write('#include <vector>\n')

    f.write('\n\n')
    f.write('namespace DbConf {\n')
    f.write('\n')
    f.write('\tstatic const std::vector<UInt_t> mTriggers = {\n')
    for item in triggers:
        f.write(f'\t\t{item},\n')
    f.write('\t};\n')
    f.write('\n')
    f.write('\tstatic const std::map<Int_t, Int_t> mRunIdxMap = {\n')
    for idx, item in enumerate(runs):
        f.write('\t\t{\t')
        f.write(item)
        f.write(',\t')
        f.write(f'{idx:5d}')
        f.write('},\n')
    f.write('\t};\n') # end of the run idx map

    f.write('}\n') # end of the namespace

    f.write('\n\n#endif\n') # end of this header file

l.log('StRoot/StYQAMaker/DbConf.h Generated. This is the end.')
