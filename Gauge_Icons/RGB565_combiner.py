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


filesList = ["digit_0.c", "digit_1.c", "digit_2.c", "digit_3.c", "digit_4.c", 
"digit_5.c", "digit_6.c", "digit_7.c", "digit_8.c", "digit_9.c", "icon_Ctemp.c", "icon_block.c", "icon_boost.c", 
"icon_oil.c", "index_10off.c", "index_10on.c", "index_11off.c", "index_11on.c", "index_12off.c", 
"index_12on.c", "index_8off.c", "index_8on.c", "index_9off.c", "index_9on.c", "text_block.c", "unit_PSI.c", 
"unit_celcius.c"]

for fileInd in range(0, len(filesList)):

  # generate a new filename with "_new" to write to
  lettersFileName = filesList[fileInd]
  outputFileName_temp = lettersFileName.split(".")
  outputFileName_temp.insert(1, "_new")
  outputFileName_temp.insert(2, ".")
  outputFileName = "".join(outputFileName_temp)

  lettersFile = open(lettersFileName, "r")
  # outputFile = open("digit_0_adj.c", "w")

  parsedBytes = []

  for lineNum, currLine in enumerate(lettersFile):
    # skip the first line of the file
    if lineNum == 0:
      continue
    # print(str(lineNum) + ": " + line + "\n")
    
    # This loop goes through each line and finds each hex value
    # Then each pair of 8-bit chars is joined into a 16-bit int
    for letterNum, currLetter in enumerate(currLine):
      
      # Store the first letter/digit 
      if ((letterNum + 3) % 5) == 0: 
        # print("Letter 1 " + str(currLetter) + ": " + currLetter)
        Letter1 = currLetter

      # Store the second letter/digit
      if ((letterNum + 2) % 5) == 0: 
        # print("Letter 2 " + str(currLetter) + ": " + currLetter)
        Letter2 = currLetter
        # Store the full 8-bit value
        currCharStr = Letter1 + Letter2
        currChar = int(currCharStr, 16)
        # print(currChar)
        parsedBytes.append(currChar)

        # Shuffle every pair of bytes into the correct order for RGB565
        if (len(parsedBytes) % 2 == 0):
          shuffleBytes(parsedBytes)

  lettersFile.close()

  # print(parsedBytes)
  RGB565_Bytes = []
  for i in range(0, len(parsedBytes)-2, 2):
    # print(hex(parsedBytes[i]), hex(parsedBytes[i+1]), end=",")
    curr565 = parsedBytes[i] << 8 | parsedBytes[i+1]
    RGB565_Bytes.append(curr565)
    # print(hex(curr565).upper(), end=",")

  newSize = len(RGB565_Bytes)

  with open(lettersFileName, 'r') as lettersFile, open(outputFileName, 'w') as outputFile:
    # Reuse the first line of the file, except the array size and data type must change
    firstLine = lettersFile.readline()
    sizeStartInd = firstLine.index("[") + 1
    sizeEndInd = firstLine.index("]")
    oldSizeStr = firstLine[sizeStartInd:sizeEndInd]

    firstLine = firstLine.replace(oldSizeStr, str(newSize))
    firstLine = firstLine.replace("unsigned char", "uint16_t")
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

