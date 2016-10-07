Prototypical Double define: 'nicePrintOf:' toBe: 280 asPrimitive;
Prototypical Integer define: 'nicePrintOf:' toBe: 400 asPrimitive;
	
Number defineMethod:[|nicePrint:aFormat|
      aFormat nicePrintOf: ^self asNumber
];
Number defineMethod:[|nicePrintNL:aFormat|
      ^self nicePrint: aFormat; 
      newLine print; 
      ^self
];

Object defineMethod:[|nicePrint:aFormat|
      ^self print: aFormat; ^self
];
Object defineMethod:[|nicePrintNL:aFormat|
      ^self printNL: aFormat; ^self
];

Object defineMethod: [| nicePrint: fieldWidth with: decimalPlaces |
  ^self nicePrint: fieldWidth
];

Double defineMethod: [| nicePrint: fieldWidth with: decimalPlaces |
  !dp <- decimalPlaces absoluteValue asInteger;
  !fw <- fieldWidth absoluteValue asInteger;
  fw > 10000 ifTrue: [:fw <- 10000];
  dp > 16 ifTrue: [:dp <- 16];
  !format <- (dp * 65536 + fw) asInteger;
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger];
  format  nicePrintOf: ^self asDouble
];

Float defineMethod: [| nicePrint: fieldWidth with: decimalPlaces |
  !dp <- decimalPlaces absoluteValue asInteger;
  !fw <- fieldWidth absoluteValue asInteger;
  fw > 10000 ifTrue: [:fw <- 10000];
  dp > 16 ifTrue: [:dp <- 16];
  !format <- (dp * 65536 + fw) asInteger;
  fieldWidth < 0 ifTrue: [:format <- format negated asInteger];
  format  nicePrintOf: ^self asFloat
];

