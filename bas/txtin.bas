  10 REM Custom text input example
  20 REM by Francis G. Loch
  30 
  40 DIM mc% 100
  50 DIM buffer% 255
  60 osasci=&FFE3
  70 osbyte=&FFF4
  80 osrdch=&FFE0
  90 FOR opt%=0 TO 2 STEP 2
 100 P%=mc%
 110[          OPT opt%
 120.getText
 130           LDA #2
 140           LDX #0
 150           JSR osbyte
 160           LDY #0
 170.loop
 180           JSR osrdch
 190           CMP #27
 200           BNE chkReturn
 210           LDA #13
 220           JSR osasci
 230           STA buffer%
 240           LDA #126
 250           JSR osbyte
 260           RTS
 270.chkReturn
 280           CMP #13
 290           BNE chkDelete
 300           JSR osasci
 310           STA buffer%,Y
 320           RTS
 330.chkDelete
 340           CMP #127
 350           BNE chkLimit
 360           CPY #0
 370           BEQ loop
 380           JSR osasci
 390           DEY
 400           JMP loop
 410.chkLimit
 420           CPY #255
 430           BEQ loop
 440           JSR osasci
 450           STA buffer%,Y
 460           INY
 470           JMP loop
 480]
 490 NEXT
 500 
 510 PRINT "Enter text: ";
 520 A$=FNgetText
 530 PRINT A$
 540 END
 550 
 560 DEF FNgetText
 570 CALL getText
 580 =$buffer%