#######################################################################################################################
# File Name        : VerfiFiles.py
# Date of creation : 05.10.2017
# Author Name/Dept : Manaswetha Pai
# Organization     : RBEI
# Description      : A Tool using Beyond Compare which can verify a set of files in a reference folder with 
#						a recently generated results of a test
#######################################################################################################################

import os as os
import subprocess as subprocess 
import shutil
import LogSupport

path_ref_logfile = "ResultReference"
RefLogClass = LogSupport.LoggerClass("global.soco", path_ref_logfile)      
RefLogger = RefLogClass.Logger

class BCompShell():
	def __init__(self, mode, criteria, filetype, filter=None):
		self.mode = mode
		self.criteria = criteria
		self.filetype = filetype
		self.filter = filter
		self.bcompexe = "M:\SwTools\BC3\BComp.exe"
	
	"""
	Gives the list of files of given extension 
	"""
	def get_files(self, path):
		filename = []
		for subdir, dirs, files in os.walk(path):
			for file in files :
				if file.endswith(self.filetype):
					if type(self.filter) == str:
						if file.startswith(self.filter):
							fname = os.path.join(subdir, file)
							filename.append(fname)
					else:
						fname = os.path.join(subdir, file)
						filename.append(fname)
		return filename
	
	"""
	Executes Beyond compare with quick compare option which returns errorlevel
	"""
	def get_error_level(self, reference_file, result_file):
		command = self.bcompexe +" -qc="+ self.criteria
		errorlevel = subprocess.call(command+ " " + reference_file + " " + result_file)
		return errorlevel
	
	"""
	Lists the files and subdirectories in two seperate lists, recursively.
	"""
	def get_dirs_for_bco(self, bco_dir):
		bco_path_list = list()
		all_dirs_list = os.listdir(bco_dir)
		all_files_list = list()
		
		for dir in all_dirs_list:
			current_dir = bco_dir + "\\" + dir
			if ("." in dir) and not (dir == ".dm"):
				all_files_list.append(current_dir)
			elif dir == ".dm":
				pass
			else:
				bco_path_list.append(current_dir)
				current_pathes, current_tuples = self.get_dirs_for_bco(current_dir)
				bco_path_list.extend(current_pathes)
				all_files_list.extend(current_tuples)
		return bco_path_list, all_files_list
	
	"""
	Copies the result file, that is different to the reference, to the folder .\\Tools\\Logs\\Variant\\Folder_to_make ...
	"""
	def save_res_file_of_comparison_tuple(self, file_tuple):
		if not os.path.isdir(os.path.split(file_tuple[0].replace("DynamicTest_Reference", "Tools\\Logs"))[0]):
			try:
				os.makedirs(os.path.split(file_tuple[0].replace("DynamicTest_Reference", "Tools\\Logs"))[0])
				shutil.copy(file_tuple[1], file_tuple[0].replace("DynamicTest_Reference", "Tools\\Logs"))
			except:
				pass
	
	"""
	Compares the files of two folders, if there are files that are not existence in both files, they will be neglected.
	"""
	def compare_two_pathes(self, reference, result, output=None):
		if self.mode == 'silent':
			
			comparison_file_list = list()
			
			if os.path.isfile(reference) and os.path.isfile(result):
				comparison_file_list.append((reference , result))

			else:
				all_ref_files = self.get_dirs_for_bco(reference)[1]
				all_res_files = self.get_dirs_for_bco(result)[1]
				
				for ref_file in all_ref_files:
					
					fpath, fname = os.path.split(ref_file)
					
					for res_file in all_res_files:
						
						if "Provided" in res_file:
							continue
						
						if res_file.endswith(fname):
							comparison_file_list.append((ref_file, res_file))
			"""		
			try:
				if output == None:
					report = open("Test_report.txt",'w')
				else:
					report = open(output,'w')
			except IOError:
				RefLogger.critical("Cannot open {file_}\n".format(file_ = test_report))
			"""		
			check = [] #List to store status of individual file comparison
			
			for file_tuple in comparison_file_list:
				errorlevel = self.get_error_level(file_tuple[0], file_tuple[1])
				# checks whether the files are same 
				RefLogger.log(70,"Comparing: {ref_file} & {res_file}".format(ref_file=file_tuple[0], res_file=file_tuple[1]))
				
				if (errorlevel == 1):
					check += [True]
					RefLogger.log(70,"Successful : Results are same\n")
					
				elif (errorlevel == 2):
					check += [True]
					RefLogger.log(70,"Successful : Results are rules-based same\n")
					
				# checks whether the files are similar
				elif (errorlevel == 12) :
					check += [True]
					RefLogger.log(70,"Results are similar\n")
					
				# checks whether the files are different
				elif (errorlevel == 11) :
					check += [False]
					self.save_res_file_of_comparison_tuple(file_tuple)
					RefLogger.error("Error : Results are different\n")
					
				elif (errorlevel == 13) :
					check += [False]
					self.save_res_file_of_comparison_tuple(file_tuple)
					RefLogger.log(70,"Successful : Results are rules-based different\n")
					
				else :
					RefLogger.error("Error : File not found\n")
					report.write("ERROR: File {} not found\n".format(file_tuple[1]))
			"""		
			if all(check):
				report.write("")
			else:
				report.write("Build Failed.\n")
			"""
			
			return all(check)		
		else:
			subprocess.call(self.bcompexe+" /filters=*"+self.filetype+" "+reference+" "+result)
			
					
		
	"""
	Selecting the silent or view mode
		- silent mode = with option quick compare (-qc)
		- view mode = Launches the Beyond compare view
	"""
	def compare_files(self, reference, result, output=None):
		if self.mode == 'silent':
			reference_file = self.get_files(reference)
			result_file = self.get_files(result)
			try:
				if output == None:
					report = open("Test_report.txt",'w')
				else:
					report = open(output,'w')
			except IOError:
				RefLogger.critical("Cannot open {file_}\n".format(file_ = test_report))
				
			check = [] #List to store status of individual file comparison
			for i in range(len(reference_file)):
				errorlevel = self.get_error_level(reference_file[i],result_file[i])
				# checks whether the files are same 
				RefLogger.log(70,"Comparing: {ref_file} & {res_file}".format(ref_file=reference_file[i], res_file=result_file[i]))
				
				if (errorlevel == 1):
					check += [True]
					RefLogger.log(70,"Successful : Results are same\n")
					
				elif (errorlevel == 2):
					check += [True]
					RefLogger.log(70,"Successful : Results are rules-based same\n")
					
				# checks whether the files are similar
				elif (errorlevel == 12) :
					check += [True]
					RefLogger.log(70,"Results are similar\n")
					
				# checks whether the files are different
				elif (errorlevel == 11) :
					check += [False]
					RefLogger.critical("Error : Results are different\n")
					
				elif (errorlevel == 13) :
					check += [False]
					RefLogger.log(70,"Successful : Results are rules-based different\n")
					
				else :
					RefLogger.error("Error : File not found\n")
					report.write("ERROR: File {} not found\n".format(result_file))
					
			if all(check):
				report.write("")
			else:
				report.write("Build Failed.\n")
				
		else:
			subprocess.call(self.bcompexe+" /filters=*"+self.filetype+" "+reference+" "+result)
			
			
			
			
			
