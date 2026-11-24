from openpyxl.styles import Font, Color, colors, Border, Side, PatternFill
from openpyxl import Workbook

class XLS_Writer():
    
    TEST_DARK_GREEN = '00008000'
    TEST_YELLOW     = '00FFFF00'
    ErrStyle        = Font(color=colors.RED,bold=True,size=12)
    OkStyle         = Font(color=TEST_DARK_GREEN,bold=True,size=12)
    
    thin            = Side(border_style="thin", color="000000")
    double          = Side(border_style="double", color="ff0000")

    def __init__(self, filepath):
        self.filepath = filepath
        self.wb = Workbook()
        self.ws = self.wb.active
        self.ws.title = "Table1"
        self.row = 1
        self.column = 1
        self.start_fields = []
        self.end_fields = []

    def close(self):
        self.wb.save(self.filepath)
        
    def start_exe(self):
        self.start_fields.append(self.row)
          
    def end_exe(self):
        self.end_fields.append(self.row-1)
        
    def apply_styles(self):
        c1 = self.ws.cell(row=self.end_fields[-1], column=1)
        c2 = self.ws.cell(row=self.end_fields[-1], column=2)
        c2.border = Border(top=xls_printer.thin, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.double)
        c1.border = Border(top=xls_printer.thin, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.double)         
        c1 = self.ws.cell(row=self.start_fields[-1], column=1)
        c2 = self.ws.cell(row=self.start_fields[-1], column=2)
        c2.border = Border(top=xls_printer.double, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.thin)
        c1.border = Border(top=xls_printer.double, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.thin)
        
    def print_xls(self, value1, value2, style=None, color=True, bb=True):
        c1 = self.ws.cell(row=self.row, column=self.column, value=str(value1))
        self.column = self.column+1
        c2 = self.ws.cell(row=self.row, column=self.column, value=str(value2))
        self.row = self.row+1
        self.column = 1  
        if style:
            c1.font = style
            c2.font = style 

        if color:
            c1.fill = PatternFill("solid", fgColor=xls_printer.TEST_YELLOW) 
            c2.fill = PatternFill("solid", fgColor=xls_printer.TEST_YELLOW)  

        if bb:
            c2.border = Border(top=xls_printer.thin, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.thin)
            c1.border = Border(top=xls_printer.thin, left=xls_printer.thin, right=xls_printer.thin, bottom=xls_printer.thin) 