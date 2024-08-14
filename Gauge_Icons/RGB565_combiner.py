'''
  File: RGB565_combiner.py
  Version: 1
  Author: Abeed Nazar
  Date: 08/12/2024

  Converts multiple RGB565 C array text files from 8 bit to 16 bit
  i.e. 0XC0,0X80,0X60,0XB9,0XC0,0XD9,0XE0,0XE9 becomes
       0x80C0,   0xB960,   0xD9C0,   0xE9E0
  This is needed to use the Adafruit GFX library bitmap drawing functions
  as the Waveshare library uses the 8 bit format

  Changelog:
  8/12/2024 - Authored file
  8/13/2024 - Simplified parsing of input file, corrected output array # of elements, and
              removed unnecessary header comment from output file
'''

# shuffleBytes()
# Shuffles the last two items in a list
def shuffleBytes(inputList):
  lastInd = len(inputList) - 1
  # store the last two values
  val2 = inputList[lastInd]
  val1 = inputList[lastInd - 1]
  # shuffle the last two values
  inputList[lastInd] = val1
  inputList[lastInd - 1] = val2

  return inputList


filesList = ["digit_0.c", "digit_1.c", "digit_2.c", "digit_3.c", "digit_4.c",  "digit_5.c",
"digit_6.c", "digit_7.c", "digit_8.c", "digit_9.c", "icon_Ctemp.c", "icon_block.c", "icon_boost.c", 
"icon_oil.c", "index_8off.c", "index_8on.c", "index_9off.c", "index_9on.c", "index_10off.c",
"index_10on.c", "index_11off.c", "index_11on.c", "index_12off.c",  "index_12on.c", "index_1off.c",
"index_1on.c", "index_2off.c", "index_2on.c", "index_3off.c", "index_3on.c", "index_4off.c",
"index_4on.c", "text_block.c", "unit_PSI.c",  "unit_celcius.c"]

# For testing one file at a time
# filesList = ["digit_9.c"]

for fileInd in range(0, len(filesList)):

  # generate a new filename with "_new" to write to
  lettersFileName = filesList[fileInd]
  outputFileName_temp = lettersFileName.split(".")
  outputFileName_temp.insert(1, "_new")
  outputFileName_temp.insert(2, ".")
  outputFileName = "".join(outputFileName_temp)

  with open(lettersFileName, "r") as lettersFile:
    # outputFile = open("digit_0_adj.c", "w")

    parsedBytes = []

    for lineNum, currLine in enumerate(lettersFile):
      # skip the first line of the file
      if lineNum == 0:
        continue
      # print(str(lineNum) + ": " + line + "\n")
      
      currByte = ""
      for letter in currLine:
        # Store each letter inbetween commas into a string
        if letter != ",":
          currByte = currByte + letter
        # Convert the string into int and store
        else:
          currByte = int(currByte, 16)
          parsedBytes.append(currByte)
          # switch the order of two bytes when a full int is recorded
          if len(parsedBytes) % 2 == 0:
            parsedBytes = shuffleBytes(parsedBytes)
          currByte = ""


  # print(parsedBytes)
  RGB565_Bytes = []
  for i in range(0, len(parsedBytes)-1, 2):
    # print(hex(parsedBytes[i]), hex(parsedBytes[i+1]), end=",")
    curr565 = parsedBytes[i] << 8 | parsedBytes[i+1]
    RGB565_Bytes.append(curr565)
    # print("{0:#0{1}X}".format(curr565,6), end=",")

  newSize = len(RGB565_Bytes)
  # print(newSize)


  with open(lettersFileName, 'r') as lettersFile, open(outputFileName, 'w') as outputFile:
    # Reuse the first line of the file, except the following changes:
    # array size must change
    firstLine = lettersFile.readline()
    sizeStartInd = firstLine.index("[") + 1
    sizeEndInd = firstLine.index("]")
    oldSizeStr = firstLine[sizeStartInd:sizeEndInd]

    # data type must change
    firstLine = firstLine.replace(oldSizeStr, str(newSize))
    firstLine = firstLine.replace("unsigned char", "uint16_t")

    # and the commented header row can be deleted
    headerInd = firstLine.find("{") + 1
    firstLine = firstLine.replace(firstLine[headerInd:], "\n")

    outputFile.write(firstLine)

    i = 0
    for byte in RGB565_Bytes:
      # outputFile.write(hex(byte).upper())
      outputFile.write("{0:#0{1}X}".format(byte,6))
      outputFile.write(",")
      i = i + 1
      if i % 8 == 0:
        outputFile.write("\n")
    outputFile.write("};")

