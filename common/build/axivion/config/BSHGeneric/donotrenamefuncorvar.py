"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : donotrenamefuncorvar.py
Date of creation : 06.07.2018
Author Name/Dept : Manaswetha Pai
Organization     : RBEI
Description      : Checks whether a function or variable is renamed using #define.

"""

import generic
from bauhaus import ir
from bauhaus.ir.common.output import unparse


def rename_func_var(ir_graph):
	'''Returns renamed functions or variables in source files'''	
	macros = []
	for node_list in ir_graph.nodes_of_type(ir.Physical, ('Global_Normal_Ordinary_Variable_Definition', 
			'Global_Static_Ordinary_Variable_Definition', 'Static_Nothrow_Function_Definition', 'Nothrow_Function_Definition',
			'Local_Normal_Ordinary_Variable_Definition', 'Local_Static_Ordinary_Variable_Definition','Global_Extern_Variable_Declaration')):
		macros.append(node_list.Logical.Name)		
        
	return (node
			for node in ir_graph.nodes_of_type(ir.Physical, ('Source_Object_Macro_Definition'))
			if node.Replacement in macros)
				
			
			
class donotrenamefuncorvar\
  (generic.GenericRule):
  
    rulename = 'DoNotRenameFuncOrVar'
    ruletext = '''Do not rename function or variable by #define.'''

    languages = ['C', 'C++']

    _guid = '2403594f-29ef-473a-9e4b-385c59705a3b'
    _message_descriptions = {'Source_Object_Macro_Definition' : ('Do not rename function or variable by #define','7b1d52bd-1992-41fc-b85f-4f0f093c6bcd')}
	
    @classmethod
    def get_rulehtml(klass):
        return '''{header}
        The declaration of a non-inline function in a header file violates the One Definition Rule
        if the header file is included in multiple compilation units.

        <h5>Configuration</h5>
        {config}
        '''.format(header=klass.get_rulehtml_header(), config=klass.get_rulehtml_configuration())
			 
    @classmethod
    def execute(klass, ir_graph):
		'''Checks for renamed functions or variables by #define.'''
		nodes = rename_func_var(ir_graph)
		klass.add_messages\
		(nodes, 'Source_Object_Macro_Definition', entity_callback = unparse.entity)        