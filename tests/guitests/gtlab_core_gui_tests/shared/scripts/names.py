import sys
import os

# generate os-compatible path
def _generateImportPath(*path):
    joinedPath = path[0]
    for i in range(1, len(path)):
        subPath = path[i]
        if os.name == 'nt':
            joinedPath += '\\'
        else:
            joinedPath += '/'
        joinedPath += subPath
    return joinedPath

# add path to custom, gloabl object_map
sys.path.append(_generateImportPath('..', '..', 'common', 'globals'))

from objectmaphelper import *
# add custom, global object_map
from object_names import *

# generated by squish
