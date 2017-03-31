#!/usr/bin/python

import sys
from optparse import OptionParser

class comm:
    def __init__(self, fname1, fname2):
        try:
            #set file1 and file2 to None
            file1 = None
            file2 = None
            #check if either is trying to be read from stdin
            if fname1 == "-" and fname2 != "-":
                file1 = sys.stdin
                file2 = open(fname2, 'r')
            elif fname2 == "-" and fname1 != "-":
                file2 = sys.stdin
                file1 = open(fname1, 'r')
            #now check if neither are from stdin
            elif fname1 != "-" and fname2 != "-":
                file1 = open(fname1, 'r')
                file2 = open(fname2, 'r')
            #if either file1 or file2 are None, then files are passed in improperly
            if (file1 != None and file2 != None):
                #read the files
                self.lines1 = file1.readlines();
                self.lines2 = file2.readlines();
                #format the line arrays to take care of empty lines and such
                self.lines1 = self.format_lines(self.lines1)
                self.lines2 = self.format_lines(self.lines2)
                #close the files
                file1.close()
                file2.close()
            else:
                parser.error("Error: At least one source must be a file")
        except IOError as error:
            parser.error("I/OError({0}): {1}".format(error.errno, error.strerror))
            
    def format_lines(self, lines):
        if not len(lines):
            lines.append("\n")
        for i in range(len(lines)):
            #replace empty string with newlines
            if lines[i] == '':
                lines[i] = '\n'
        #add newlines to end of file
        if "\n" not in lines[-1]:
            lines[-1] = lines[-1] + "\n"
        return lines
                
    #check if list is sorted
    def is_sorted(self, lines):
        for i in range(len(lines)-1):
            #simply checks to make sure a line is never greater than the next
            if lines[i] > lines[i+1]:
                return False
        return True
    
    #replicates check_order from comm.c
    def check_order(self):
        if not self.is_sorted(self.lines1):
            #use sys.stderr.write when errors do not involve the parser
            sys.stderr.write("File 1 not sorted")
            return False
        elif not self.is_sorted(self.lines2):
            sys.stderr.write("File 2 not sorted")
            return False
        return True

    #replicates writeln from comm.c (almost line for line!)
    def output_line(self, line, val, show_col_1, show_col_2, show_col_3):
        #comm.c's "delimiter"
        SPACER = "\t"
        #output string
        output = ""
        #if only python had switch statements...
        if val == 1:
            if not show_col_1:
                return
        elif val == 2:
            if not show_col_2:
                return
            if show_col_1:
                #substitute fputs from comm.c with appending to the output
                output += SPACER
        elif val == 3:
            if not show_col_3:
                return
            if show_col_1:
                output += SPACER
            if show_col_2:
                output += SPACER
        sys.stdout.write(output + line)

    def compare_files(self, unsorted, show_col_1, show_col_2, show_col_3):
        if unsorted:
            line2_copy = self.lines2
            #loop through everything in line1
            for line in self.lines1:
                #if the line isn't in lines2, output in column 1
                if line not in self.lines2:
                    self.output_line(line, 1, show_col_1, show_col_2, show_col_3)
                #otherwise output in column 3
                else:
                    self.output_line(line, 3, show_col_1, show_col_2, show_col_3)
                    #now remove the line from the copy of lines2
                    line2_copy.remove(line)
            #the copy should contain only lines unique to lines2
            #add them all to self.col2
            for line in line2_copy:
                self.output_line(line, 2, show_col_1, show_col_2, show_col_3)
            return
        else:
            #check the order if it's not an unsorted comparison
            if not self.check_order():
                return
            #use bool flag since there are no do...while loops in python
            checked_all = False
            i = 0
            j = 0
            #takes an approach similar to merging two arrays
            #keep looping until both i and j are out of bounds
            while not checked_all:
                #set line vars to the appropriate line or None if out of bounds
                line1 = self.lines1[i] if i < len(self.lines1) else None
                line2 = self.lines2[j] if j < len(self.lines2) else None                
                #if i is out of bounds or line1 > line2, then print line2
                #make sure to check that line2 is also not None
                if line1 == None or (line2 != None and line1 > line2):
                    self.output_line(line2, 2, show_col_1, show_col_2, show_col_3)
                    #advance line1
                    j += 1
                #if j is out of bounds or line1 < line2 then print line1
                elif line2 == None or line1 < line2:
                    self.output_line(line1, 1, show_col_1, show_col_2, show_col_3)
                    #advance line2
                    i += 1
                #otherwise both lines are equal and you can print either one
                else:
                    self.output_line(line1, 3, show_col_1, show_col_2, show_col_3)
                    #advance both i and j
                    i += 1
                    j += 1
                #everything has been checked when both counters have run out of bounds
                checked_all = i >= len(self.lines1) and j >= len(self.lines2)
                
def main():
    #set version_msg and usage_msg
    version_msg = "%prog 1.0"
    usage_msg="""%prog [OPTION]... FILE1 FILE2

    Compare sorted files FILE1 and FILE2 line by line"""
    #add the options to the parser
    parser = OptionParser(version=version_msg, usage=usage_msg)
    parser.add_option("-1", "--nofile1", action="store_false", dest="show_col_1", default=True, help="suppress column 1")
    parser.add_option("-2", "--nofile2", action="store_false", dest="show_col_2", default=True, help="suppress column 2")
    parser.add_option("-3", "--nofile3", action="store_false", dest="show_col_3", default=True, help="suppress column 3")
    parser.add_option("-u", "--unsorted", action="store_true", dest="unsorted", help="accept unsorted input")
    options, args = parser.parse_args(sys.argv[1:])

    #make sure options are bools and store in vars
    try:
        show_col_1 = bool(options.show_col_1)
        show_col_2 = bool(options.show_col_2)
        show_col_3 = bool(options.show_col_3)
        unsorted = bool(options.unsorted)
    except:
        #throw error if the options can't be casted to bools
        parser.error("invalid options")
        
    #make sure there are two args
    if len(args) != 2:
        parser.error("wrong number of operands")
    #set input1 and input2
    input1 = args[0]
    input2 = args[1]
    #try/catch block for running comm
    try:
        comm_obj = comm(input1, input2)
        comm_obj.compare_files(unsorted, show_col_1, show_col_2, show_col_3)
    except IOError as error:
        parser.error("I/O error({0}): {1}".format(error.errno, error.strerror))

if __name__ == "__main__":
    main()
