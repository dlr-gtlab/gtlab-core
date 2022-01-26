# -*- coding: utf-8 -*-

from common import *
from qt_pushbutton import qPushButton

import squish
import test_logging as logging


class gtShortCutEdit:
    ''' class fot gtShortCutEdits '''
    
    @logging.logInit
    def __init__(self, id):
        self.id     = id
        self.type   = "GtShortCutEdit"
        self.object = searchObject(type=self.type, objectName=id)
            
    def isBlank(self):
        ''' returns whether a keysequence is set'''
        return self.keySequence().isEmpty() and self.object.text.isEmpty()
    
    def keySequence(self):
        ''' returns the internal shortcut as a string '''
        return self.object.keySequence.toString()
    
    def enterKeySequence(self, keySeq):
        ''' enters the keysequence specified '''
        squish.type(self.object, keySeq)
        # exit input
        squish.type(self.object, '<Esc>')
        