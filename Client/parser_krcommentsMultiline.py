#!/usr/bin/env python
#martysama0134's script

def HasIllegal(line):
	for c in line:
		if IsIllegal(c):
			return True
	return False

def IsIllegal(c):
	return not (0 <= ord(c) <= 127)

def processKrMulti(data):
	newdata = ''
	while len(data):
		posBegin = data.find("/*")
		if posBegin == -1: # skip if there's no multilan comment
			newdata += data
			data = ''
			continue #break

		# check "//" before and skip
		if "//" in data[:posBegin].split("\n")[-1]:
			newdata += data[:posBegin+2]
			data = data[posBegin+2:]
			continue

		# check "//*" and skip
		if posBegin > 0 and data[posBegin-1:posBegin+2] == "//*":
			newdata += data[:posBegin+2]
			data = data[posBegin+2:]
			continue

		# save all data before posBegin
		newdata += data[:posBegin]
		data = data[posBegin:]

		posEnd = data.find("*/")
		if posEnd == -1: # no ending comment
			if HasIllegal(data):
				data = ''
			else:
				newdata += data
				data = ''
			continue #break

		posEnd += 2 # add trailing comment length
		comment = data[:posEnd]
		if HasIllegal(comment):
			# print("illegal %s"%comment)
			data = data[posEnd:]
		else:
			# print("ok %s\n"%comment)
			newdata += comment
			data = data[posEnd:]
	return newdata

def run(fn):
	import sys
	with open(fn, "rb") as f1:
		f1d=f1.read()

	with open(fn, "wb") as f1:
		# print "process %s"%fn
		f1.write(processKrMulti(f1d))

import os
for root, dirs, files in os.walk("."):
	for name in files:
		name2=name.lower()
		if name2.endswith(".cpp") or name2.endswith(".h") or name2.endswith(".hpp") or name2.endswith(".cc") or name2.endswith(".c"):
			run(os.path.join(root, name))
#
