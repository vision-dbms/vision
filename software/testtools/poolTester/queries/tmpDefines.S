########################################################################
###  This script is used to temporarily define any new primitives
###  until they are officially added to the networks by the
###  testdeck administrator/release administrator.
###
###  May 9, 1988	jad
###  Mar 24,2000	jck   committed portion saved in committed/March2000
########################################################################



Object

defineMethod: [| print: fieldWidth with: decimalPlaces | 
  ^self print: fieldWidth
];

Double 

defineMethod: [| print: fieldWidth with: decimalPlaces | 
  !dp <- decimalPlaces absoluteValue asInteger; 
  !fw <- fieldWidth absoluteValue asInteger; 
  fw > 10000 ifTrue: [:fw <- 10000]; 
  dp > 16 ifTrue: [:dp <- 16]; 
  !format <- (dp * 65536 + fw) asInteger; 
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger]; 
  format  printOf: ^self asDouble
].

defineMethod: [| printWithCommas: fieldWidth with: decimalPlaces | 
  !dp <- decimalPlaces absoluteValue asInteger; 
  !fw <- fieldWidth absoluteValue asInteger; 
  fw > 10000 ifTrue: [:fw <- 10000]; 
  dp > 16 ifTrue: [:dp <- 16]; 
  !format <- (dp * 65536 + fw) asInteger; 
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger]; 
  format  printWithCommasOf: ^self asDouble
];

Float 

defineMethod: [| print: fieldWidth with: decimalPlaces | 
  !dp <- decimalPlaces absoluteValue asInteger; 
  !fw <- fieldWidth absoluteValue asInteger; 
  fw > 10000 ifTrue: [:fw <- 10000]; 
  dp > 16 ifTrue: [:dp <- 16]; 
  !format <- (dp * 65536 + fw) asInteger; 
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger]; 
  format  printOf: ^self asFloat
].

defineMethod: [| printWithCommas: fieldWidth with: decimalPlaces | 
  !dp <- decimalPlaces absoluteValue asInteger; 
  !fw <- fieldWidth absoluteValue asInteger; 
  fw > 10000 ifTrue: [:fw <- 10000]; 
  dp > 16 ifTrue: [:dp <- 16]; 
  !format <- (dp * 65536 + fw) asInteger; 
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger]; 
  format  printWithCommasOf: ^self asFloat
];

Utility define:'processId' toBe: 604 asPrimitive;
