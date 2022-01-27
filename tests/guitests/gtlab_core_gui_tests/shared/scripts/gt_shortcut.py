
from common import *
from qt_pushbutton import qPushButton

import test_logging as logging
import squish


class gtShortCutEdit:
    ''' Helper class to interact with ``GtShortCutEdits``.
    
    :members: 
     - **id** (*string*) -- shortcut id/name
     - **type** (*string*) -- type/classname of widget
     - **object** (*squish object*) -- reference to squish object
     '''
    
    @logging.logInit
    def __init__(self, id):
        ''' 
        :param id: shortcut id
        :type id: string
        '''
        self.id     = id
        self.type   = "GtShortCutEdit"
        self.object = searchObject(type=self.type, objectName=id)
            
    def isBlank(self):
        ''' 
        Returns whether the keysequence is not set (i.e. blank)
        
        :return: whether the keysequence is blank
        :rtype: boolean
        '''
        return self.keySequence().isEmpty() and self.object.text.isEmpty()
    
    def keySequence(self):
        ''' 
        Returns the internal shortcut/keysequence as a string.
        
        :return: keysequence
        :rtype: string
        '''
        return self.object.keySequence.toString()
    
    def enterKeySequence(self, keySeq):
        ''' 
        Enters the keysequence ``keySeq`` specified to set the shortcut. 
        
        :param keySeq: keysequence to enter (must be in form ``<...>``)
        :type keySeq: string
        
        :Example:
        
        >>> edit = gtShortCutEdit('copy')
        >>> edit.enterKeySequence('<Strg+C>')
        '''
        squish.type(self.object, keySeq)
        # exit input
        squish.type(self.object, '<Esc>')
        
