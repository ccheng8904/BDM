"""
Copyright (c) 2017 BSH Hausgeraete GmbH,
Carl-Wery-Str. 34, 81739 Munich, Germany, www.bsh-group.de

All rights reserved. This program and the accompanying materials
are protected by international copyright laws.
Please contact copyright holder for licensing information.

File Name        : constants.py
Date of creation : 2020-03-04
Author Name/Dept : Nikolai Ehrhardt
Organization     : SDD-4
Description      : Constants for configuration
"""

import re

#not in use
TreeMap = {
    "#1": 0,
    "#2": 1,
    "#3": 2,
    "#4": 3,
    "#5": 4
}

class tree_const:
    
    array_range_regexp = re.compile(r"(\[([\d]+)\-([\d]+)\])")

    tc_prms =  "prms"    
    tc_vars =  "vars" 
    tc_ret  =  "ret" 
    tc_desc =  "desc"    

    tc_iset = "!iset"
    val_pos = "+"
    val_neg = "-"
    
    tuple_pos=(val_pos, )
    tuple_neg=(val_neg, )
    
    rep_dec = 0
    rep_bin = 1
    rep_hex = 2
    combo_reps = ["dec", "bin", "hex", ""]

    tc_key_tuple = (tc_prms, tc_vars, tc_ret, tc_desc)
    
    col_in  = "in"
    col_in_solved = "solved_in"
    col_op  = "op"
    col_exp = "exp"
    col_exp_solved = "solved_exp"
    col_out = "out"
    col_valid = "valid"
    col_rep = "rep"
    col_dc  = "dc"
    tc_header_tuple = (col_in, col_in_solved, col_exp, col_exp_solved, col_op, col_out, col_valid, col_rep, col_dc)
    tc_heading_tuple = ("In", "In-solved", "Exp", "Exp-solved", "Operator", "Out", "Validation", "Rep", "dc")
    
    prm_value_std = ("0", "", "", "", "", "", "", "", "")
    ret_value_std = ("", "", "0", "", "==", "", "", "", "")
    var_value_std = ("0", "", "0", "", "==", "", "", "", "")
    
    tc_entry_cols_dict = { tc_prms: (col_in, col_rep, col_dc),
                            tc_vars: (col_in, col_exp, col_op, col_rep, col_dc),
                            tc_ret:  (col_exp, col_op, col_rep, col_dc), 
                            tc_desc: (col_in,)
                        }
    
    tc_read_cols = (col_in_solved, col_exp_solved, col_out)    
    
    tc_read_cols_dict = { tc_prms: (col_in_solved),
                            tc_vars: tc_read_cols,
                            tc_ret:  (col_exp_solved, col_out),
                            tc_desc: []
                        }
                        
    
                        
    io_cols = (col_in, col_exp)
                        
    tc_index_column_map = {
        "#0": 0,
        "#1": col_in,
        "#2": col_in_solved,
        "#3": col_exp,
        "#4": col_exp_solved,
        "#5": col_op,
        "#6": col_out,
        "#7": col_valid,
        "#8": col_rep,
        "#9": col_dc
    }
    
    tc_text_id_map = {
        tc_prms: "Parameters",
        tc_vars: "Variables",
        tc_ret:  "Return", 
        tc_desc: "Description"          
    }
    
    
    #parent split is: [tc_id, one of tc_key_tuple, ]
    ##2 1__vars__!iset#TEAT_ex_pointer_struct ['1', 'vars', '!iset#TEAT_ex_pointer_struct']
    @classmethod 
    def isEntryField(cls, item_split, col):
        if len(item_split) == 2:
            if item_split[1] == tree_const.tc_desc:
                return True
            else:
                return False
        elif len(item_split) >= 3:
            if item_split[2] == tree_const.tc_iset:
                return False
            elif item_split[2].startswith(tree_const.tc_iset) and tree_const.tc_index_column_map[col] == tree_const.col_in:
                return True
            else:
                return tree_const.tc_index_column_map[col] in tree_const.tc_entry_cols_dict[item_split[1]]
        else:        
            return False
            
    @classmethod 
    def isReadField(cls, item_split, col):
        if len(item_split) >= 3:
            if item_split[2] == tree_const.tc_iset:
                return False
            else:
                return tree_const.tc_index_column_map[col] in tree_const.tc_read_cols_dict[item_split[1]]
        else:        
            return False
    
    col_type  = 'type'
    col_address = 'address'
    col_linkage = 'linkage'
    col_qualifier = 'qualifier'
    soco_header_tuple = (col_type, col_address, col_linkage, col_qualifier)
    

    
    combo_ops = ("==", "!=", ">=", ">", "<=", "<")
    range_ops = ("=..","!=..")
    
    all_ops = list(combo_ops)+list(range_ops)
    
    col_id = "id"
    col_loc = "loc"
    tsuite_header_tuple = (col_id, col_loc, col_valid)
    
    tsuite_tseq =  "ts"    

    tsuite_key_tuple = (tsuite_tseq,)
    
    tsuite_index_column_map = {
        "#0": 0,
        "#1": col_id,
        "#2": col_loc,
        "#3": col_valid
    }
    
    tsuite_entry_cols_dict = {  0: (col_loc,),
                                tsuite_tseq: (col_id, col_loc)
                            }
    
            
    @classmethod
    def getValueTupleForKind(cls, value_kind):
        if value_kind == tree_const.tc_prms:   
            return tree_const.prm_value_std
        if value_kind == tree_const.tc_vars:
            return tree_const.var_value_std
        if value_kind == tree_const.tc_ret: 
            return tree_const.ret_value_std
    

class exec_const:
    
    passed_str      = "Passed"
    failed_str      = "Failed"
    exception_str  = "Exception" 
    resolved_str    = "Resolved"
    dont_care       = "#dc#"
    
    tex_passed_str      = "\\PASSED"
    tex_failed_str      = "\\FAILED"
    tex_exeception_str  = "\\EXCEPTION" 
    
    tex_column_passed   = "\\PAS"
    tex_column_failed   = "\\FAI"
    tex_column_exception   = "\\EXC"

    passed_int      = 0
    failed_int      = 1
    exception_int  = 2    

    validation_int_dict = {
                    passed_str: passed_int,
                    failed_str: failed_int,
                    exception_str: exception_int,
                    "": 2
                    }
                    
    validation_str_dict = {
                    passed_int: passed_str,
                    failed_int: failed_str, 
                    exception_int: exception_str
                    }
                    
    validation_texstr_dict = {
                    passed_int: tex_passed_str,
                    failed_int: tex_failed_str, 
                    exception_int: tex_exeception_str
                    }
                    
    column_validation_texstr_dict = {
                    passed_int: tex_column_passed,
                    failed_int: tex_column_failed, 
                    exception_int: tex_column_exception
                    }
            
    def getValidationEnumeratorFromString(test_passed):
        return exec_const.validation_int_dict.get(test_passed, None)
        
    def getValidationStringFromEnumerator(enum):
        return exec_const.validation_str_dict.get(enum, None)
        
    def getValidationTexStringFromEnumerator(enum):
        return exec_const.validation_texstr_dict.get(enum, None)
        
    def getValidationColumnTexStringFromEnumerator(enum):
        return exec_const.column_validation_texstr_dict.get(enum, None)
    
    
if __name__ == "__main__":
    pass
    
