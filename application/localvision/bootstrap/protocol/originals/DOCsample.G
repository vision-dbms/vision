####################
#  Printer Controls
####################
Object
   define: 'printerReset' toBe: "E" .
   define: 'size10' toBe: "(s10H" .
   define: 'size11' toBe: "(s11H" .
   define: 'size12' toBe: "(s12H" .
   define: 'boldOn' toBe: "(s6B" .
   define: 'boldOff' toBe: "(s0B" .
   define: 'ulineOn' toBe: "&dD" .
   define: 'ulineOff' toBe: "&d@" .
   define: 'left6' toBe: "&a6L" .
#   define: 'reverseOn' toBe: "*v2T*c45G" .
#   define: 'reverseOff' toBe: "*v0T"
;



Schema CD  defineMethod:
[ | fullReport |

  "@@@CTRL 1" printNL ;
  printerReset print; left6 printNL ;
  "@@@CTRL 0" printNL ; 
  "@@@PAGELEN 52" printNL ;
  "@@@TOPMARGIN 4" printNL ; 
  "@@@WIDTH 500" printNL ; 
  "@@@PGLINE 2" printNL ; 
  "@@@PGFORMAT 1" printNL ; 
  boldOff print ; size10 print ;
  [ "Vision Classes and Messages" print: 30 ;
    code concat: "-#" . print: -40 ;
  ] divertOutput print ;
  size12 printNL ;
  "@@@PGFORMAT 0" printNL ;
  "@@@BLOCKTOP 1" printNL ; 
  "@@@REPORT 1" printNL ;
  boldOn print ; size10 print ; 
  "The Class: " print ; code printNL ;
  newLine print ; newLine print ; 
  boldOff print ; 
  description isntNA 
      ifTrue: [ description printNL ;
                newLine print ; newLine print ; 
              ] ;
  boldOn print ; 
  asSelf isObjectDescriptor 
      ifFalse: [ class showInheritance ] ;
  boldOff print ; size12 printNL ;
  detailReport ;
  alphaXRefReport ;
  functionXRefReport ;
 "@@@REPORT 0" printNL ; 
] ;

#--------------------

Schema CD
defineMethod:
[ | detailReport |
  "@@@NEWPAGE" printNL ; 
  "@@@HEADER 1" printNL ;
  size10 print ;   
  [ boldOn print ; 
    "Message Detail For Class " print ;
    ^self code print ;
    boldOff print ; 
  ] divertOutput centerNL: 80 ;
  size12 printNL ;
  "@@@HEADER 0" printNL ;
  ^self class getMessages
     select: [ isMessageImplementationDescriptor ] .
     select: [ usageLevel isInternal not && usageLevel isReserved not ] .
  do: [ "@@@BLOCK 1" printNL ;
        !name <- fullSelector ;
        boldOn print ;
        name print ; 
        boldOff print ;
        functionType isntNA
        ifTrue: 
          [ " " print: (69 - name count - functionType code count) ;
            "Function: " print: 10 ; functionType code printNL ;
          ]
        ifFalse: [ newLine print ] ;
        newLine print ;
        description isntNA ifTrue: [ description printNL ] ;
        parameterList count > 0 && type isMethod
        ifTrue:
           [ newLine print ; 
             parameterList toList numberElements
             do: [ "Parameter " print; position print: -3 ;
                   "is " print ;
                   code printNL
                ] ;
           ] ;
        ulineOn print ;
        " " fill: 80 . print ;
        ulineOff printNL ;
        "@@@BLOCK 0" printNL ;
      ] ;
] ;


#--------------------

Schema CD
defineMethod:
[ | alphaXRefReport |
  !list <- class getMessagesX ;
  asSelf isObjectDescriptor
     ifFalse: [ :list <- list 
                  select: [messageClassDescriptor isObjectDescriptor not]
              ] ;
  !title <- 
     [ "***  Message Cross Reference For " print ; 
       ^self code print ; 
       "  ***" print ;
     ] divertOutput ;
  ^self alphaXRefFor: list withTitle: title ;
] .

defineMethod:
[ | alphaFullXRefReport | 
  !list <- class getMessagesInInheritancePath 
      select: [ usageLevel isInternal not ] ;
  !title <- 
     "***  Message Cross Reference - All Messages  ***" ;
  ^self functionXRefFor: list withTitle: title ;
] .

defineMethod:
[ | alphaXRefFor: messageList withTitle: title  | 
  !list <- messageList sortUp: [ code ] .
              select: [ usageLevel isReserved not ] ;
  !maxMess  <- (list max: [ message count ] . + 1) asInteger ;
  !maxClass <- 
      (list max: [ messageClassDescriptor code count ] . + 2 ) asInteger ;
  !maxFunction <- (list select: [ functionType isntNA ] . 
                        max: [ functionType code count ] . + 2 ) asInteger ;
  !width <- (maxMess + maxClass + maxFunction + 25 ) asInteger ;

  "@@@HEADER 1" printNL ;
  size10 print ; boldOn print ; 
  title centerNL: 75 ;
  "Sorted By Message" centerNL: 75 ;
  size12 print ;
  newLine print ;
  "Message" print: maxMess ;
  "Class" print: maxClass ;
  "Type" print: 11 ; 
  "Function" print: maxFunction ; 
  "Level" printNL ; 
  ulineOn print ;
  " " fill: width . print ;
  ulineOff print ;
  boldOff printNL ; 
  "@@@HEADER 0" printNL ;
  "@@@NEWPAGE" printNL ; 
  "@@@SKIPLINE 5" printNL ;
  list numberElements
  do: [ position / 2 = (position / 2 ) asInteger 
          ifTrue: [ boldOn print ] 
         ifFalse: [ boldOff print ] ;
        message print: ^my maxMess ; 
        messageClassDescriptor code print: ^my maxClass ;
        type shortName print: 11 ; 
        functionType isNA
           ifTrue: [ " " ] 
          ifFalse: [ functionType code ] . print: ^my maxFunction ;
        usageLevel isAdvanced || usageLevel isInternal
           ifTrue: [ usageLevel code print ] ;
        newLine print ; 
      ] ;
  boldOff printNL ;
  "@@@SKIPLINE 0" printNL ; 
] .

#------------------------------

defineMethod:
[ | functionXRefReport | 
  !list <- class  getMessagesX ;
  asSelf isObjectDescriptor
    ifFalse: [ :list <- list 
                  select: [messageClassDescriptor isObjectDescriptor not] 
             ] ;
  !title <- 
     [ "***  Message Cross Reference For " print ; 
       ^self code print ; 
       "  ***" print ;
     ] divertOutput ;
  ^self functionXRefFor: list withTitle: title ;
] .

defineMethod:
[ | functionFullXRefReport | 
  !list <- class getMessagesInInheritancePath 
      select: [ usageLevel isInternal not ] ;
  !title <- 
     "***  Message Cross Reference - All Messages  ***" ;
  ^self functionXRefFor: list withTitle: title ;
] .

defineMethod:
[ | functionXRefFor: messageList withTitle: title | 
  !list <- messageList
      select: [ functionType isntNA && usageLevel isReserved not ] ;
  !maxMess  <- (list max: [ message count ] . + 1) asInteger ;
  !maxClass <- 
      (list max: [ messageClassDescriptor code count ] . + 2 ) asInteger ;
  !maxFunction <- (list max: [ functionType code count ] . + 2 ) asInteger ;
  !width <- (maxMess + maxClass + maxFunction + 25 ) asInteger ;
  
  "@@@HEADER 1" printNL ;
  size10 print ;
  boldOn print ; 
  title centerNL: 75 ;
  "Sorted By Function" centerNL: 75 ;
  size12 print ;
  newLine print ;
  "Message" print: maxMess ;
  "Class" print: maxClass ;
  "Type" print: 11 ; 
  "Function" print: maxFunction ; 
  "Level" printNL ; 
  ulineOn print ; 
  " " fill: width  . print ;
  ulineOff print ;
  boldOff printNL ; 
  "@@@HEADER 0" printNL ;
  "@@@NEWPAGE" printNL ; 

  list groupedBy: [ functionType ] . sortUp: [ code ] .
  do: [ !maxMess <- ^my maxMess ; 
        !maxClass <- ^my maxClass ; 
        !maxFunction <- ^my maxFunction ;
        "@@@BLOCK 3" printNL ; 
        groupList sortUp: [ message ] . numberElements
        do: [
              position / 2 = (position / 2 ) asInteger
                 ifTrue: [ boldOn print ] ifFalse: [ boldOff print ] ;
              message print: ^my maxMess ; 
              messageClassDescriptor code print: ^my maxClass ;
              type shortName print: 11 ; 
              functionType code print: ^my maxFunction ;
              usageLevel isAdvanced || usageLevel isInternal
                 ifTrue: [ usageLevel code print ] ;
              newLine print ; 
            ] ;
        "@@@BLOCK 0" printNL ;
        boldOff printNL ;
      ] ;
] .

;


#--------------------------------------------------



#---  To invoke   - class by class report (section 2)
#Schema CD instanceList
# select: [ type isBuiltIn || asSelf isObjectDescriptor] .
#do: [ fullReport ] ;
#Dictionary classDescriptor fullReport ;
#Entity classDescriptor fullReport ;

#---  To invoke full cross reference
#Schema CD
#do: [ 
#    !title <- "***  Message Cross Reference - All Messages  ***" ;
#    !list <- MID activeList
#       select: [ usageLevel isInternal not ] .
#       select: [ messageClassDescriptor type isBuiltIn ||
#                 messageClassDescriptor isObjectDescriptor ||
#                 (messageClassDescriptor code inSet: "Entity", "Dictionary")
#               ] ;
#    ^self alphaXRefFor: list withTitle: title ;
#    ^self functionXRefFor: list withTitle: title ;
#    ] ;



