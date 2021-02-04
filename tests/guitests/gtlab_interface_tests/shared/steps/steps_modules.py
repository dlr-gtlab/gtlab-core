from common_namespace import Separators

import test_logging as logging
import test_modules

def __makeExcludeJsonString(*modules):
    moduleList = []
    for i in modules:
        moduleList.append({"id" : i})
    json = { "modules" : moduleList }
    logging.debug(json)
    json = f"{json}".replace('\'', "\"")
    return json
    
        
@Given("only the module '|any|' is included at startup")
@Given("only the modules '|any|' are included at startup")
def step(context, modules):
    modules  = createTupleFromString(modules, Separators.list)
    excludes = test_modules.modules
    for module in modules:
        excludes.remove(module)            
    json = __makeExcludeJsonString(*excludes)
    cwd  = context.userData['application_context'].cwd
    # save json string
    with open(f'{cwd}/_exclude.json', 'w+') as f:
        f.seek(0); f.truncate() # delete content
        f.write(json)
        
                
@Given("no modules are included at startup")
def step(context):
    excludes = test_modules.modules       
    json = __makeExcludeJsonString(*excludes)
    cwd  = context.userData['application_context'].cwd
    # save json string
    with open(f'{cwd}/_exclude.json', 'w+') as f:
        f.seek(0); f.truncate() # delete content
        f.write(json)
                
@Given("all modules are included at startup")
def step(context):      
    json = __makeExcludeJsonString()
    cwd  = context.userData['application_context'].cwd
    # save json string
    with open(f'{cwd}/_exclude.json', 'w+') as f:
        f.seek(0); f.truncate() # delete content
        f.write(json)