
# doc helper
import sys
if 'sphinx' in sys.modules:
    from doc_helper import *

import test_logging as logging
import squish

from common import *
from gt_preferencesdialog import gtPreferencesDialog
from gt_shortcut import gtShortCutEdit


@Uses('shortcut')
@Sets('shortcut_new_value')
@When("the shortcut is changed to '|any|'")
def step_shortcut_changed(context, keySeq):
    shortcut = context.userData['shortcut']
    shortcut.enterKeySequence(f"<{keySeq}>")
    context.userData['shortcut_new_value'] = keySeq
    
@Sets('shortcut', 'shortcut_init_value')
@Then("the shortcut '|any|' exists")
def step_shortcut_exists(context, shortcutID):
    shortcut = gtShortCutEdit(shortcutID)
    if shortcut.object is None:
        logging.failTest(f"could not find the shortcut edit '{shortcutID}'")
    else:
        logging.passTest(f"shortcut edit '{shortcutID}' found!")
    context.userData['shortcut'] = shortcut
    context.userData['shortcut_init_value'] = shortcut.keySequence()

@Uses('shortcut')
@Then("the shortcut is set to '|any|'")
def step_shortcut_set(context, keyseq):
    shortcut = context.userData['shortcut']
    logging.compare(shortcut.keySequence(), keyseq, "is the shortcut set correctly?")

@Uses('shortcut', 'shortcut_new_value')
@Then("the shortcut is accepted")
def step_shortcut_accept(context):
    shortcut = context.userData['shortcut']
    newSeq   = context.userData['shortcut_new_value']
    logging.compare(shortcut.keySequence(), newSeq, "is the shortcut set correctly?")

@Then("the shortcut is unchanged")
def step_shortcut_unchanged(context):
    runStep("Then", "the shortcut is restored")
    
@Uses('shortcut', 'shortcut_init_value')
@Then("the shortcut is restored")
def step_shortcut_restored(context):
    ''' Checks if the inital shortcut is set. '''
    shortcut = context.userData['shortcut']
    initSeq  = context.userData['shortcut_init_value']
    logging.compare(shortcut.keySequence(), initSeq, "is the shortcut set to its init value?")
    
@Uses('shortcut')
@Then("the shortcut is blank")
def step_shortcut_blank(context):
    shortcut = context.userData['shortcut']
    logging.compare(shortcut.isBlank(), True, "is the shortcut blank?")

