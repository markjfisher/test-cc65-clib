 10 REM BBC Basic OSFIND/OSBPUT Test Program
 20 REM This program tests file operations to help debug CC65 issues
 30 REM
 40 PRINT "BBC Basic OSFIND/OSBPUT Test"
 50 PRINT "============================="
 60 PRINT
 70 
 80 REM Allocate memory for machine code
 90 DIM MC% 200
100 DIM handle% 1
110 DIM result% 1
120 
130 REM Assemble the OSFIND/OSBPUT routines
140 FOR opt%=0 TO 3 STEP 3
150 P%=MC%
160 [
170     OPT opt%
180 .osfindcreate
190     LDX #(filename MOD 256) \ X = low byte of filename
200     LDY #(filename DIV 256) \ Y = high byte of filename
210     LDA #&80                \ A = &80 (create for output)
220     JSR &FFCE               \ Call OSFIND
230     STY handle%             \ Save file handle from Y register
240     RTS                     \ Return to BASIC
250 .osfindopen
260     LDX #(filename MOD 256) \ X = low byte of filename
270     LDY #(filename DIV 256) \ Y = high byte of filename
280     LDA #&40                \ A = &40 (open for input)
290     JSR &FFCE               \ Call OSFIND
300     STY handle%             \ Save file handle from Y register
310     RTS                     \ Return to BASIC
320 .osfindclose
330     LDX handle%             \ X = file handle
340     LDA #0                  \ A = 0 (close file)
350     JSR &FFCE               \ Call OSFIND
360     STY result%             \ Save result from Y register
370     RTS                     \ Return to BASIC
380 .osbputcall  
390     LDX handle%             \ X = file handle
400     JSR &FFD1               \ Call OSBPUT
410     STY result%             \ Save result from Y register
420     RTS                     \ Return to BASIC
430 .osbgetcall
440     LDX handle%             \ X = file handle
450     JSR &FFD7               \ Call OSBGET
460     STY result%             \ Save result from Y register
470     RTS                     \ Return to BASIC
480 .filename
490     EQUS "XFILE"+CHR$(13)
510 ]
520 NEXT
480 
490 REM Test 1: Create a new file for writing
500 PRINT "Test 1: Creating file 'TESTFILE' for writing"
510 PRINT "Calling OSFIND with A=&80 (create for output)"
520 
530 handle% = FNosfindcreate
540 PRINT "OSFIND returned: ";handle%
550 IF handle% = 0 THEN
560   PRINT "ERROR: Failed to create file"
570   GOTO 1000
580 ENDIF
590 PRINT "SUCCESS: File created, handle = ";handle%
600 PRINT
610 
620 REM Test 2: Write some data to the file
630 PRINT "Test 2: Writing data to file"
640 
650 REM Write a test string
660 testdata$ = "Hello from BBC Basic!"
670 
680 FOR i% = 1 TO LEN(testdata$)
690   char% = ASC(MID$(testdata$,i%,1))
700   PRINT "Writing character '";CHR$(char%);"' (ASCII ";char%;")"
710   
720   REM Set up registers for OSBPUT
730   A% = char% : REM Character to write
740   result% = FNosbputcall
750   
760   PRINT "  OSBPUT returned: ";result%
770   IF result% <> 0 THEN
780     PRINT "  ERROR: OSBPUT failed"
790     GOTO 1000
800   ENDIF
810 NEXT i%
820 
830 REM Write a carriage return
840 PRINT "Writing carriage return"
850 A% = 13 : REM CR
860 result% = FNosbputcall
870 PRINT "OSBPUT returned: ";result%
880 
890 PRINT "SUCCESS: Data written to file"
900 PRINT
910 
920 REM Test 3: Close the file
930 PRINT "Test 3: Closing file"
940 PRINT "Calling OSFIND with A=0 to close file"
950 
960 result% = FNosfindclose
970 PRINT "OSFIND returned: ";result%
980 IF result% <> 0 THEN
990   PRINT "ERROR: Failed to close file"
1000   GOTO 1000
1010 ENDIF
1020 PRINT "SUCCESS: File closed"
1030 PRINT
1040 
1050 REM Test 4: Try to open the file for reading
1060 PRINT "Test 4: Opening file for reading"
1070 PRINT "Calling OSFIND with A=&40 (open for input)"
1080 
1090 readhandle% = FNosfindopen
1100 PRINT "OSFIND returned: ";readhandle%
1110 IF readhandle% = 0 THEN
1120   PRINT "ERROR: Failed to open file for reading"
1130   GOTO 1000
1140 ENDIF
1150 PRINT "SUCCESS: File opened for reading, handle = ";readhandle%
1160 PRINT
1170 
1180 REM Test 5: Read back the data
1190 PRINT "Test 5: Reading data back from file"
1200 
1210 PRINT "Reading characters:"
1220 FOR i% = 1 TO 25 : REM Read up to 25 characters
1230   char% = FNosbgetcall
1240   
1250   IF char% = 0 THEN
1260     PRINT "End of file reached"
1270     GOTO 1300
1280   ENDIF
1290   
1300   PRINT "  Read character '";CHR$(char%);"' (ASCII ";char%;")"
1310 NEXT i%
1320 
1330 REM Close the read file
1340 PRINT "Closing read file"
1350 handle% = readhandle% : REM Set handle for close
1360 result% = FNosfindclose
1370 PRINT "OSFIND returned: ";result%
1380 
1390 PRINT
1400 PRINT "All tests completed successfully!"
1410 PRINT "Check that TESTFILE was created and contains the test data"
1420 END
1430 
1440 REM Function definitions
1450 DEF FNosfindcreate
1460 CALL osfindcreate
1470 =?handle%
1480 
1490 DEF FNosfindopen
1500 CALL osfindopen
1510 =?handle%
1520 
1530 DEF FNosfindclose
1540 CALL osfindclose
1550 =?result%
1560 
1570 DEF FNosbputcall
1580 CALL osbputcall
1590 =?result%
1600 
1610 DEF FNosbgetcall
1620 CALL osbgetcall
1630 =?result%