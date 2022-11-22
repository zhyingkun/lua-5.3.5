local nfd = require "nfd"

print(tostring(nfd, 16))
print(tostring({nfd.openDialog("doc")}, 16))
print(tostring({nfd.openDialogMultiple()}, 16))
print(tostring({nfd.saveDialog()}, 16))
print(tostring({nfd.pickFolder()}, 16))
