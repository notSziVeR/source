#!/usr/bin/env python
#martysama0134's script

def lookahead(iterable):
	"""Pass through all values from the given iterable, augmented by the
	information if there are more values to come after the current one
	(True), or if it is the last value (False).
	"""
	# Get an iterator and pull the first value.
	it = iter(iterable)
	last = next(it)
	# Run the iterator to exhaustion (starting from the second value).
	for val in it:
		# Report the *previous* value (more to come).
		yield last, True
		last = val
	# Report the last value.
	yield last, False

def processKr(line):
	pos = line.find("//")
	if pos == -1:
		return line, False
	comment = line[pos:]
	for c in comment:
		if 0 <= ord(c) <= 127:
			continue
		else:
			return line[:pos], True
	return line, False

def run(fn):
	import sys
	with open(fn, "rb") as f1:
		f1d=f1.read()

	with open(fn, "wb") as f1:
		for line, hasMore in lookahead(f1d.split("\n")):
			line = line.rstrip()
			line, res = processKr(line)
			line = line.rstrip() # strip again
			if res and not line: # skip empty line if they had kr
				continue
			if hasMore:
				line += "\n"
			f1.write(line)

import os
for root, dirs, files in os.walk("."):
	for name in files:
		name2=name.lower()
		if name2.endswith(".cpp") or name2.endswith(".h") or name2.endswith(".hpp") or name2.endswith(".cc") or name2.endswith(".c"):
			run(os.path.join(root, name))
#
