# -*- coding: utf-8 -*-

import squish
import test_logging as logging

from common import *
from gt_preferencesdialog import gtPreferencesDialog
from gt_shortcut import gtShortCutEdit


@When("the shortcut is changed to '|any|'")
def step(context, keySeq):
    shortcut = context.userData['shortcut']
    shortcut.enterKeySequence(f"<{keySeq}>")
    context.userData['shortcut_new_value'] = keySeq
    
@Then("the shortcut '|any|' exists")
def step(context, shortcutID):
    shortcut = gtShortCutEdit(shortcutID)
    if shortcut.object is None:
        logging.failTest(f"could not find the shortcut edit '{shortcutID}'", stopExecution=False)
    else:
        logging.passTest(f"shortcut edit '{shortcutID}' found!")
    context.userData['shortcut'] = shortcut
    context.userData['shortcut_init_value'] = shortcut.keySequence()

@Then("the shortcut is set to '|any|'")
def step(context, keyseq):
    shortcut = context.userData['shortcut']
    logging.compare(shortcut.keySequence(), keyseq, "is the shortcut set correctly?", stopExecution=False)

@Then("the shortcut is accepted")
def step(context):
    shortcut = context.userData['shortcut']
    newSeq   = context.userData['shortcut_new_value']
    logging.compare(shortcut.keySequence(), newSeq, "is the shortcut set correctly?", stopExecution=False)

@Then("the shortcut is unchanged")
def step(context):
    runStep("Then", "the shortcut is restored")
    
@Then("the shortcut is restored")
def step(context):
    shortcut = context.userData['shortcut']
    initSeq  = context.userData['shortcut_init_value']
    logging.compare(shortcut.keySequence(), initSeq, "is the shortcut set to its init value?", stopExecution=False)

@Then("the shortcut is blank")
def step(context):
    shortcut = context.userData['shortcut']
    logging.compare(shortcut.isBlank(), True, "is the shortcut blank?", stopExecution=False)

