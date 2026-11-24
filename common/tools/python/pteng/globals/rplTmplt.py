"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : rplTmplt.py
Date of creation : 7.8.2018
Author Name/Dept : 
Organization     : 
Description      : Helps to replace marked Strings in Templates.
"""

import sys
import re
import getpass
import time
import os
from stat import *

class TemplateFileReplace:
    
    def __init__(self, dictionary):
        self.substDict = dictionary

    #==============================================================================#
    #  FUNCTION Author()                                                           #
    #  Returns the user string (supposed to be the author)                         #
    #==============================================================================#
    def Author(self):
        return str(getpass.getuser() + " / code generator")

    def AUTHOR(self):
        return str(getpass.getuser())

    #==============================================================================#
    #  FUNCTION Date()                                                             #
    #  Returns a string with current date                                          #
    #==============================================================================#
    def Date(self):
        return time.strftime("%d.%m.%Y  %H:%M:%S", time.localtime())
         
        
    def addReplacement(self, dictionary):
        self.substDict.update(dictionary)
        #self.substDict = dictionary
        
    #======================================================================================#
    #  FUNCTION replaceTemplate(tmplName, outName)                                         #
    #  Takes a content of a template and replaces the substitution variables.              #
    #  The result is written to the output file.                                           #
    #  If a substitution variables is contained in the passed Replacements-Dictionary      #
    #  but if a class method of the identical name is existing then                        #
    #  the variable is replace by the functions return value.                              #
    #  Parameters:                                                                         #
    #     tmplName : Name and path of the template file                                    #
    #     outName  : Name and path of the output file                                      #
    #======================================================================================#
    def replaceString(self, replace_in_me, ignoreUnkonwn=False, silent=False, tmplFolder=''):
        
        # tmplFolder: folder where template is located; if file content variable not in dict: assume that file is in tmplFolder wiht name of FoundFileContentVariables
                
        FileContentVariablePattern = re.compile('<\$\$([^>]+)>')   # Replacement pattern
        
        FoundFileContentVariables = FileContentVariablePattern.findall(replace_in_me) # list that contains doublets
        FoundFileContentVariablesUnique = set(FoundFileContentVariables) # set that does not contain doublets
        
        for variable in FoundFileContentVariablesUnique:
             
            execute_replace = False 
                 
            if variable in self.substDict:
                variable_replacement = self.substDict[variable]
                execute_replace = True
                               
            elif tmplFolder:
                variable_replacement = os.path.join(tmplFolder,variable)
                execute_replace = True
                         
            else:
               if not silent:
                   print ("Info: Could not replace template variable: ", variable)
                    
             
            if execute_replace:
                # Try to open file whose content shall replace template variable
                try:
                    ReplaceFile = open(variable_replacement, "r")
                except:
                    print ('rplTemplt.replaceTemplate: Can not open file "' + variable_replacement + '" for replacing template variable ' + variable + ' by file content. (Not replaced.)')
                    execute_replace = False
            
            
            if execute_replace:  
                variable_replacement = ReplaceFile.read()   
                ReplaceFile.close()  
                replace_in_me = replace_in_me.replace("<$$" + variable + ">", variable_replacement) 
                  
            elif ignoreUnkonwn==False:
                replace_in_me = replace_in_me.replace("<$$" + variable + ">", variable) 

        
        VariablePattern = re.compile('<\$([^>]+)>')   # Replacement pattern
        
        FoundVariables = VariablePattern.findall(replace_in_me) # list that contains doublets
        FoundVariablesUnique = set(FoundVariables) # set that does not contain doublets
        
        for variable in FoundVariablesUnique:
            
            execute_replace = False 
                     
            if variable in self.substDict:               
                variable_replacement = self.substDict[variable]
                execute_replace = True
            
            # if template variable is not in dict: if an member function with this name is existing then replace template variable by return value of this member function
            # attention: these functions may not relay on 
            elif (hasattr(self,variable)):
                variable_replacement = eval("self." + variable + "()")
                execute_replace = True
                    
            else:
                if not silent:
                    print(("Info: Could not replace template variable: ", variable))

            if execute_replace:
                replace_in_me = replace_in_me.replace("<$" + str(variable) + ">", str(variable_replacement)) #works
                #FileContent = re.sub(r"\<\$" + variable + r"\>", variable_replacement, FileContent) #works
                #FileContent = re.sub("\\<\\$" + variable + "\\>", variable_replacement, FileContent) #works
                
            elif ignoreUnkonwn==False:
                replace_in_me = replace_in_me.replace("<$" + str(variable) + ">", str(variable)) 
         
                                      
        return replace_in_me
                
                
        
    def replaceTemplate(self, tmplName, outName, ignoreUnkonwn=False, silent=False, tmplFolder='', overwrite=True):

        if tmplFolder:
            print('rplTemplt.replaceTemplate: Called with unsused pararemter tmplFolder ', tmplFolder)
        
        # Try to open template file
        try:
            TemplateFile = open(tmplName, "r")
        except:
            print(('rplTemplt.replaceTemplate: Can not open template file "' + tmplName + '". (Nothing generated.)'))
            return
            
        # try to open output file, make output file writable
        if os.path.exists(outName):
            os.chmod(outName,S_IWRITE)
            
        ResultFille = None    
        
        if (overwrite): 
            try:
                ResultFille = open(outName, "w")
            except:
                print(('rplTemplt.replaceTemplate: Can not open result file "' + outName + '". (Nothing generated.)'))
                return
        else: 
            if(not os.path.isfile(outName)):
                ResultFille = open(outName, "w")
        
     
        if (ResultFille):
            FileContent = TemplateFile.read()      # Read template file
            TemplateFile.close()
                
            tmplFolder, basename = os.path.split(tmplName)
            FileContent = self.replaceString(FileContent, ignoreUnkonwn, silent, tmplFolder)
               
            ResultFille.write(FileContent)
            
            ResultFille.close()
        


    def replaceTemplate_old(self,tmplName, outName):

        def preFilter(self, line):
            return(line)
        
        # Try to open template file
        try:
            templateStubC = open(tmplName, "r")
        except:
            print ("File could not be open: " + tmplName)
            return
            
        # try to open output file
            # Make ouput file writable
        if os.path.exists(outName):
            os.chmod(outName,S_IWRITE)

        try:
            outFileC = open(outName, "w")
        except:
            print ("File could not be open: " + outName)
            return


        allLines = templateStubC.readlines()      # Read all lines of the template
        templateStubC.close()
        ReReplace = re.compile('(<\$([^>]+)>)')   # Replacement pattern

        #--------------------------------------------------------------------------
        # Do for all lines in template
        #--------------------------------------------------------------------------
        for line in allLines:
            line = self.preFilter(line)

            while 1:
                rep = ReReplace.search(line)
                if rep == None:
                    break
                if (hasattr(self,rep.group(2))):
                    # Try to find a function with name of replacement text
                    subs = eval("self." + rep.group(2) + "()")
                    line = re.sub('<\$[^>]+>', subs, line,1)  # if successful replace the text with function result
                else:
                    try:
                        subs = eval('self.substDict["' +  rep.group(2)+ '"]')
                        line = re.sub('<\$[^>]+>', subs, line,1)  # Try to find a variable with this name
                    except:
                        print ("ould not replace template variable: ", rep.group(1))
                        line = re.sub('<\$[^>]+>', rep.group(2), line,1)
            outFileC.write(line)
        #--------------------------------------------------------------------------
        outFileC.close()
        
		
        

        

        
       
