# SPDX-FileCopyrightText: 2023 German Aerospace Center (DLR)
#
# SPDX-License-Identifier: MPL-2.0+

# doc helper
import sys
if 'sphinx' in sys.modules:
    from doc_helper import *

from common import Uses
from constants import Separators

import test_logging as logging
import test_modules

def _makeExcludeJsonString(*modules):
    '''
    Helper function to generate a json string for the ``exclude.json`` 
    containing the specified ``modules``.
    
    :param modules: module name
    :type modules: positional arguments
    
    :return: json string
    :rtype: string
    '''
    moduleList = []
    for i in modules:
        moduleList.append({"id" : i})
    json = { "modules" : moduleList }
    logging.debug(json)
    json = f"{json}".replace('\'', "\"")
    return json
    
def _writeExcludeJsonFile(dir, json):
    '''
    Helper function to write the ``exclude.json`` file.
    
    :param json: json string
    :type json: string
    
    :param dir: direcotry for gtlab
    :type dir: string
    '''
    # save json string
    with open(f'{dir}/_exclude.json', 'w+') as f:
        f.seek(0); f.truncate() # delete content
        f.write(json)

       
@Uses('application_context') 
@Given("only the module '|any|' is included at startup")
@Given("only the modules '|any|' are included at startup")
def step_module_include(context, modules):
    modules  = modules.split(Separators.list)
    excludes = test_modules.modules
    for module in modules:
        excludes.remove(module)            
    json = _makeExcludeJsonString(*excludes)
    dir  = context.userData['application_context'].cwd
    _writeExcludeJsonFile(dir, json)
        
         
@Uses('application_context')        
@Given("no modules are included at startup")
def step_module_include_none(context):
    excludes = test_modules.modules       
    json = _makeExcludeJsonString(*excludes)
    dir  = context.userData['application_context'].cwd
    _writeExcludeJsonFile(dir, json)
      
@Uses('application_context')           
@Given("all modules are included at startup")
def step_module_include_all(context):      
    json = _makeExcludeJsonString()
    dir  = context.userData['application_context'].cwd
    _writeExcludeJsonFile(dir, json)